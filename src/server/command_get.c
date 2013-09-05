#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "ybin.h"
#include "command.h"
#include "protocol.h"
#include "database.h"

/* Process a GET command. */
yerr_t command_get(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized, ydynabin_t *buff) {
	uint16_t *pname_len, name_len;
	void *ptr, *name = NULL;
	ybin_t bin_key, bin_data;
	yerr_t result;

	YLOG_ADD(YLOG_DEBUG, "GET command");
	// read name length
	if (connection_read_data(thread, buff, sizeof(name_len)) != YENOERR)
		goto error;
	pname_len = ydynabin_forward(buff, sizeof(name_len));
	name_len = ntohs(*pname_len);
	// read name
	if (connection_read_data(thread, buff, (size_t)name_len) != YENOERR)
		goto error;
	ptr = ydynabin_forward(buff, (size_t)name_len);
	if ((name = YMALLOC((size_t)name_len)) == NULL)
		goto error;
	memcpy(name, ptr, (size_t)name_len);
	// creation of the message
	bin_key.len = (size_t)name_len;
	bin_key.data = name;
	// get data
	result = database_get(thread->finedb->database, thread->transaction, thread->dbname, bin_key, &bin_data);
	if (result == YENODATA)
		goto no_data;
	if (result != YENOERR)
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
			goto error;
		}
		bin_data.data = unzip_data;
		bin_data.len = unzip_len;
	}
	// send the response to the client
	YLOG_ADD(YLOG_DEBUG, "GET command OK");
	YFREE(name);
	result = connection_send_response(thread, RESP_OK, serialized, compress,
	                                  bin_data.data, bin_data.len);
	return (result);
no_data:
	YLOG_ADD(YLOG_DEBUG, "GET no data");
	YFREE(name);
	CONNECTION_SEND_ERROR(thread, RESP_ERR_BAD_NAME);
	return (YENOERR);
error:
	YLOG_ADD(YLOG_WARN, "GET error");
	YFREE(name);
	CONNECTION_SEND_ERROR(thread, RESP_ERR_SERVER);
	return (YEIO);
}
