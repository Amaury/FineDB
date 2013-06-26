#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "ylog.h"
#include "ybin.h"
#include "command_get.h"
#include "protocol.h"
#include "database.h"

/* Process a GET command. */
yerr_t command_get(tcp_thread_t *thread, ybool_t compress, ydynabin_t *buff) {
	uint16_t *pname_len, name_len;
	void *ptr, *name = NULL;
	ybin_t bin_key, bin_data;
	yerr_t result;

	YLOG_ADD(YLOG_DEBUG, "GET command");
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
	if (database_get(thread->finedb->database, bin_key, &bin_data) != YENOERR)
		goto error;
	// send the response to the client
	YLOG_ADD(YLOG_DEBUG, "GET command OK");
	YFREE(name);
	result = connection_send_response(thread->fd, RESP_OK, compress, bin_data.data, bin_data.len);
	//if (bin_data.data && bin_data.len)
	//	free(bin_data.data);
	return (result);
error:
	YLOG_ADD(YLOG_WARN, "GET error");
	YFREE(name);
	connection_send_response(thread->fd, RESP_SERVER_ERR, YFALSE, NULL, 0);
	return (YEIO);
}
