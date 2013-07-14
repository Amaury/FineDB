#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "command_put.h"
#include "protocol.h"
#include "writer_thread.h"

/* Process a PUT command. */
yerr_t command_put(tcp_thread_t *thread, ybool_t dbname, ybool_t sync, ybool_t compress, ydynabin_t *buff) {
	uint16_t *pname_len, name_len;
	uint32_t *pdata_len, data_len;
	void *ptr, *name = NULL, *data = NULL;
	writer_msg_t *msg = NULL;
	char answer;
	size_t zip_len;
	char *zip_data = NULL;
	struct snappy_env zip_env;

	YLOG_ADD(YLOG_DEBUG, "PUT command");
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
	// read data length
	if (connection_read_data(thread->fd, buff, sizeof(data_len)) != YENOERR)
		goto error;
	pdata_len = ydynabin_forward(buff, sizeof(data_len));
	data_len = ntohl(*pdata_len);
	// read data
	if (data_len > 0) {
		if (connection_read_data(thread->fd, buff, (size_t)data_len) != YENOERR)
			goto error;
		ptr = ydynabin_forward(buff, (size_t)data_len);
		if ((data = YMALLOC((size_t)data_len)) == NULL)
			goto error;
		memcpy(data, ptr, (size_t)data_len);
	}

	// not synchronized: immediate response
	if (!sync)
		connection_send_response(thread->fd, RESP_OK, YFALSE, NULL, 0);

	// creation of the message
	if ((msg = YMALLOC(sizeof(writer_msg_t))) == NULL)
		goto error;
	msg->name = name;
	msg->name_len = name_len;
	if (compress) {
		msg->data = data;
		msg->data_len = data_len;
	} else {
		// data are not already compressed
		memset(&zip_env, 0, sizeof(struct snappy_env));
		if (snappy_init_env(&zip_env)) {
			YLOG_ADD(YLOG_WARN, "Unable to create Snappy environment.");
			goto error;
		}
		if ((zip_data = YMALLOC(snappy_max_compressed_length(data_len))) == NULL) {
			YLOG_ADD(YLOG_WARN, "Unable to allocate memory.");
			goto error;
		}
		if (snappy_compress(&zip_env, data, data_len, zip_data, &zip_len)) {
			YFREE(zip_data);
			YLOG_ADD(YLOG_WARN, "Unable to compress data.");
			goto error;
		}
		zip_data[zip_len] = '\0';
		YLOG_ADD(YLOG_DEBUG, "Compressed data : '%s' (%d) => '%s' (%d/%d)", data, data_len, zip_data, zip_len, strlen(zip_data));
		snappy_free_env(&zip_env);
		msg->data = zip_data;
		msg->data_len = zip_len;
		YFREE(data);
	}
	// send the message to the writer thread
	if (nn_send(thread->write_sock, &msg, sizeof(msg), 0) < 0) {
		YLOG_ADD(YLOG_WARN, "Unable to send message to writer thread.");
		goto error;
	}
	// wait for the answer
	if (nn_recv(thread->write_sock, &answer, sizeof(answer), 0) < 0) {
		YLOG_ADD(YLOG_WARN, "Unable to get answer from writer thread.");
		goto error;
	}
	YLOG_ADD(YLOG_DEBUG, "PUT command %s", (answer ? "OK" : "failed"));
	if (!sync)
		return (YENOERR);
	return (connection_send_response(thread->fd, (answer ? RESP_OK : RESP_NO_DATA),
	                                 YFALSE, NULL, 0));
error:
	YLOG_ADD(YLOG_WARN, "PUT error");
	YFREE(name);
	YFREE(data);
	YFREE(msg);
	connection_send_response(thread->fd, RESP_SERVER_ERR, YFALSE, NULL, 0);
	return (YEIO);
}
