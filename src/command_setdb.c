#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "ybin.h"
#include "command_setdb.h"
#include "protocol.h"
#include "database.h"

/* Process a SETDB command. */
yerr_t command_setdb(tcp_thread_t *thread, ydynabin_t *buff) {
	unsigned char *pdbname_len, dbname_len;
	char *dbname = NULL;
	void *ptr;
	yerr_t result;

	YLOG_ADD(YLOG_DEBUG, "SETDB command");
	// read dbname length
	if (connection_read_data(thread->fd, buff, sizeof(dbname_len)) != YENOERR)
		goto error;
	pdbname_len = ydynabin_forward(buff, sizeof(dbname_len));
	dbname_len = *pdbname_len;
	YFREE(thread->dbname);
	if (dbname_len > 0) {
		// read dbname
		if (connection_read_data(thread->fd, buff, (size_t)dbname_len) != YENOERR)
			goto error;
		ptr = ydynabin_forward(buff, (size_t)dbname_len);
		if ((dbname = YMALLOC((size_t)dbname_len + 1)) == NULL)
			goto error;
		memcpy(dbname, ptr, (size_t)dbname_len);
		thread->dbname = dbname;
	}
	// send the response to the client
	YLOG_ADD(YLOG_DEBUG, "SETDB command OK");
	result = connection_send_response(thread->fd, RESP_OK, YFALSE, NULL, 0);
	return (result);
error:
	YLOG_ADD(YLOG_WARN, "SETDB error");
	connection_send_response(thread->fd, RESP_SERVER_ERR, YFALSE, NULL, 0);
	return (YEIO);
}
