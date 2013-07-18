#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "ybin.h"
#include "command_get.h"
#include "protocol.h"
#include "database.h"

/* Process a GET command. */
yerr_t command_get(tcp_thread_t *thread, ybool_t has_dbname, ybool_t compress, ydynabin_t *buff) {
	char *pdbname_len, dbname_len, *dbname = NULL;
	uint16_t *pname_len, name_len;
	void *ptr, *name = NULL;
	ybin_t bin_key, bin_data;
	yerr_t result;

	YLOG_ADD(YLOG_DEBUG, "GET command");
	// read dbname if defined
	if (dbname) {
		// read dbname length
		if (connection_read_data(thread->fd, buff, sizeof(dbname_len)) != YENOERR)
			goto error;
		pdbname_len = ydynabin_forward(buff, sizeof(dbname_len));
		dbname_len = *pdbname_len;
		// read dbname
		if (connection_read_data(thread->fd, buff, (size_t)dbname_len) != YENOERR)
			goto error;
		ptr = ydynabin_forward(buff, (size_t)dbname_len);
		if ((dbname = YMALLOC((size_t)dbname_len + 1)) == NULL)
			goto error;
		memcpy(dbname, ptr, (size_t)dbname_len);
	}
	// read name length
	if (connection_read_data(thread->fd, buff, sizeof(name_len)) != YENOERR)
		goto error;
	pname_len = ydynabin_forward(buff, sizeof(name_len));
	name_len = ntohs(*pname_len);
	// read name
	if (connection_read_data(thread->fd, buff, (size_t)name_len) != YENOERR)
		goto error;
	ptr = ydynabin_forward(buff, (size_t)name_len);
	if ((name = YMALLOC((size_t)name_len)) == NULL)
		goto error;
	memcpy(name, ptr, (size_t)name_len);
	// creation of the message
	bin_key.len = (size_t)name_len;
	bin_key.data = name;
	// get data
	if (database_get(thread->finedb->database, dbname, bin_key, &bin_data) != YENOERR)
		goto error;
	if (bin_data.len && !compress) {
		// uncompress data before sending them
		size_t unzip_len;
		char *unzip_data;

		YLOG_ADD(YLOG_DEBUG, "Uncompress data.");
		snappy_uncompressed_length(bin_data.data, bin_data.len, &unzip_len);
		unzip_data = YMALLOC(unzip_len);
		if (snappy_uncompress(bin_data.data, bin_data.len, unzip_data)) {
			YLOG_ADD(YLOG_WARN, "Unable to uncompress data.");
			connection_send_response(thread->fd, RESP_SERVER_ERR, YFALSE, NULL, 0);
		}
		bin_data.data = unzip_data;
		bin_data.len = unzip_len;
	}
	// send the response to the client
	YLOG_ADD(YLOG_DEBUG, "GET command OK");
	YFREE(dbname);
	YFREE(name);
	result = connection_send_response(thread->fd, RESP_OK, compress, bin_data.data, bin_data.len);
	return (result);
error:
	YLOG_ADD(YLOG_WARN, "GET error");
	YFREE(dbname);
	YFREE(name);
	connection_send_response(thread->fd, RESP_SERVER_ERR, YFALSE, NULL, 0);
	return (YEIO);
}
