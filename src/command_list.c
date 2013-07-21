#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "ybin.h"
#include "command_list.h"
#include "protocol.h"
#include "database.h"

/* private functions */
static yerr_t _command_list_loop(void *ptr, ybin_t key, ybin_t data);

/* Process a LIST command. */
yerr_t command_list(tcp_thread_t *thread, ybool_t has_dbname, ydynabin_t *buff) {
	char *pdbname_len, dbname_len, *dbname = NULL, last_byte = 0;
	void *ptr;
	yerr_t result;

	YLOG_ADD(YLOG_DEBUG, "LIST command");
	// read dbname if defined
	if (has_dbname) {
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
	// send the response to the client
	result = connection_send_response(thread->fd, RESP_OK, YFALSE, NULL, 0);
	if (result != YENOERR)
		goto error;
	// send data
	if (database_list(thread->finedb->database, dbname, _command_list_loop, thread) != YENOERR)
		goto error;
	// send last byte
	if (write(thread->fd, &last_byte, 1) != 1)
		goto error;
	YLOG_ADD(YLOG_DEBUG, "LIST command OK");
	YFREE(dbname);
	return (result);
error:
	YLOG_ADD(YLOG_WARN, "LIST error");
	YFREE(dbname);
	connection_send_response(thread->fd, RESP_SERVER_ERR, YFALSE, NULL, 0);
	return (YEIO);
}

// send one element of list
static yerr_t _command_list_loop(void *ptr, ybin_t key, ybin_t data) {
	tcp_thread_t *thread = (tcp_thread_t*)ptr;
	uint16_t length16;
	struct iovec iov[2];
	struct msghdr mh;
	ssize_t expected, rc;

	YLOG_ADD(YLOG_DEBUG, "list element: '%s'.", (char*)key.data);
	mh.msg_name = NULL;
	mh.msg_namelen = 0;
	mh.msg_iov = iov;
	mh.msg_iovlen = 2;
	mh.msg_control = NULL;
	mh.msg_controllen = 0;
	mh.msg_flags = 0;
	length16 = htons((uint16_t)key.len);
	iov[0].iov_base = (caddr_t)&length16;
	iov[0].iov_len = sizeof(uint16_t);
	iov[1].iov_base = (caddr_t)key.data;
	iov[1].iov_len = key.len;
	expected = iov[0].iov_len + iov[1].iov_len;
	rc = sendmsg(thread->fd, &mh, 0);
	if (rc < 0) {
		YLOG_ADD(YLOG_WARN, "Unable to send response.");
		return (YEIO);
	} else if (rc < expected) {
		YLOG_ADD(YLOG_WARN, "Unable to send the complete response (%d / %d).", rc, expected);
		return (YEIO);
	}
	return (YENOERR);
}
