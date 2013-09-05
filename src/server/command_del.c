#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "ylog.h"
#include "command.h"
#include "protocol.h"
#include "database.h"
#include "writer_thread.h"

/* Process a DEL command. */
yerr_t command_del(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized, ydynabin_t *buff) {
	uint16_t *pkey_len, key_len;
	void *ptr, *key = NULL;
	writer_msg_t *msg = NULL;
	char answer;

	YLOG_ADD(YLOG_DEBUG, "DEL command");
	// read key length
	if (connection_read_data(thread, buff, sizeof(key_len)) != YENOERR)
		goto error;
	pkey_len = ydynabin_forward(buff, sizeof(key_len));
	key_len = ntohs(*pkey_len);
	// read key
	if (connection_read_data(thread, buff, (size_t)key_len) != YENOERR)
		goto error;
	ptr = ydynabin_forward(buff, (size_t)key_len);
	if ((key = YMALLOC((size_t)key_len)) == NULL)
		goto error;
	memcpy(key, ptr, (size_t)key_len);

	if (!sync) {
		// send the response
		connection_send_response(thread, RESP_OK, YFALSE, YFALSE, NULL, 0);
	}

	// creation of the message
	if ((msg = YMALLOC(sizeof(writer_msg_t))) == NULL)
		goto error;
	msg->type = WRITE_DEL;
	ybin_set(&msg->name, key, key_len);
	if (!sync) {
		msg->dbname = thread->dbname ? strdup(thread->dbname) : NULL;
		// send the message to the writer thread
		if (nn_send(thread->write_sock, &msg, sizeof(msg), 0) < 0) {
			YLOG_ADD(YLOG_WARN, "Unable to send message to writer thread.");
			goto error;
		}
		return (YENOERR);
	}
	// synchronized
	if (database_del(thread->finedb->database, thread->transaction, thread->dbname, msg->name) == YENOERR) {
		YLOG_ADD(YLOG_DEBUG, "Deletion done on database.");
		answer = 1;
	} else {
		YLOG_ADD(YLOG_WARN, "Unable to delete data on database.");
		answer = 0;
	}
	YFREE(key);
	YFREE(msg);
	YLOG_ADD(YLOG_DEBUG, "DEL command %s", (answer ? "OK" : "failed"));
	if (!sync)
		return (YENOERR);
	return (connection_send_response(thread, (answer ? RESP_OK : RESP_ERR_BAD_NAME),
	                                 YFALSE, YFALSE, NULL, 0));
error:
	YLOG_ADD(YLOG_WARN, "PUT error");
	YFREE(key);
	YFREE(msg);
	CONNECTION_SEND_ERROR(thread, RESP_ERR_SERVER);
	return (YEIO);
}
