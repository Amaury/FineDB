#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "ybin.h"
#include "command.h"
#include "protocol.h"
#include "database.h"

/* private functions */
static yerr_t _command_list_loop(void *ptr, ybin_t key, ybin_t data);

/* Process a LIST command. */
yerr_t command_list(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized, ydynabin_t *buff) {
	char last_byte = 0;
	yerr_t result;

	YLOG_ADD(YLOG_DEBUG, "LIST command");
	// send the response to the client
	result = CONNECTION_SEND_OK(thread);
	if (result != YENOERR)
		goto error;
	// send data
	if (database_list(thread->finedb->database, thread->transaction, thread->dbname, _command_list_loop, thread) != YENOERR)
		goto error;
	// send last byte
	if (write(thread->fd, &last_byte, 1) != 1)
		goto error;
	YLOG_ADD(YLOG_DEBUG, "LIST command OK");
	return (result);
error:
	YLOG_ADD(YLOG_WARN, "LIST error");
	CONNECTION_SEND_ERROR(thread, RESP_ERR_SERVER);
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
