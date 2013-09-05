#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "command.h"
#include "protocol.h"
#include "writer_thread.h"
#include "database.h"

/* Process a DROP command. */
yerr_t command_drop(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized, ydynabin_t *buff) {
	writer_msg_t *msg = NULL;
	char answer;

	YLOG_ADD(YLOG_DEBUG, "PUT command");
	// check dbname
	if (thread->dbname == NULL)
		goto error;
	// not synchronized: immediate response
	if (!sync)
		CONNECTION_SEND_OK(thread);

	// creation of the message
	if ((msg = YMALLOC(sizeof(writer_msg_t))) == NULL)
		goto error;
	msg->type = WRITE_DROP;
	if (!sync) {
		// not synchronized, send the message to the writer thread
		msg->dbname = thread->dbname ? strdup(thread->dbname) : NULL;
		if (nn_send(thread->write_sock, &msg, sizeof(msg), 0) < 0) {
			YLOG_ADD(YLOG_WARN, "Unable to send message to writer thread.");
			goto error;
		}
		return (YENOERR);
	}
	// synchronized
	if (database_drop(thread->finedb->database, thread->transaction, thread->dbname) == YENOERR) {
		YLOG_ADD(YLOG_DEBUG, "Database dropped.");
		answer = 1;
	} else {
		YLOG_ADD(YLOG_WARN, "Unable to drop database.");
		answer = 0;
	}
	YLOG_ADD(YLOG_DEBUG, "DROP command %s", (answer ? "OK" : "failed"));
	return (connection_send_response(thread, (answer ? RESP_OK : RESP_ERR_BAD_NAME),
	                                 YFALSE, YFALSE, NULL, 0));
error:
	YLOG_ADD(YLOG_WARN, "DROP error");
	YFREE(msg);
	CONNECTION_SEND_ERROR(thread, RESP_ERR_SERVER);
	return (YEIO);
}
