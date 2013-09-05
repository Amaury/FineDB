#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "nanomsg/nn.h"
#include "nanomsg/pipeline.h"
#include "ydefs.h"
#include "ylog.h"
#include "yerror.h"
#include "connection_thread.h"
#include "database.h"
#include "command.h"

/* Array of command handlers. */
static command_handler_t _commands[] = {
	command_ping,
	command_get,
	command_del,
	command_put,
	command_setdb,
	command_start,
	command_stop,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, //command_admin,
	NULL  //command_extra
};

/* Create a new connection thread. */
tcp_thread_t *connection_thread_new(finedb_t *finedb) {
	tcp_thread_t *thread;

	// thread init
	thread = YMALLOC(sizeof(tcp_thread_t));
	thread->fd = -1;
	thread->finedb = finedb;
	// thread creation
	if (pthread_create(&(thread->tid), 0, connection_thread_execution,
	    thread)) {
		YLOG_ADD(YLOG_WARN, "Unable to create thread.");
		YFREE(thread);
		return (NULL);
	}
	pthread_detach(thread->tid);
	return (thread);
}

/* Disconnect a running connection and reset the thread. */
void connection_thread_disconnect(tcp_thread_t *thread) {
	if (thread->fd == -1)
		return;
	if (thread->transaction) {
		database_transaction_rollback(thread->transaction);
		thread->transaction = NULL;
	}
	shutdown(thread->fd, SHUT_RDWR);
	thread->fd = -1;
	YFREE(thread->dbname);
}

/* Add a connection socket in the feed of waiting connections. */
void connection_thread_push_socket(int threads_socket, int fd) {
	// write the file descriptor number into the threads communication socket
	nn_send(threads_socket, &fd, sizeof(fd), 0);
}

/* Callback function executed by all server's threads. Loop to check if the
   thread must handle a new connection. */
void *connection_thread_execution(void *param) {
	tcp_thread_t *thread;
	int incoming_socket;

	YLOG_ADD(YLOG_DEBUG, "Thread loop.");
	thread = (tcp_thread_t*)param;
	// opening a connection to the writer thread
	if ((thread->write_sock = nn_socket(AF_SP, NN_PUSH)) < 0 ||
	    nn_connect(thread->write_sock, ENDPOINT_WRITER_SOCKET) < 0) {
		YLOG_ADD(YLOG_WARN, "Unable to connect to writer's socket.");
		pthread_exit(NULL);
	}
	// opening a connection to the main thread
	if ((incoming_socket = nn_socket(AF_SP, NN_PULL)) < 0 ||
	    nn_connect(incoming_socket, ENDPOINT_THREADS_SOCKET) < 0) {
		YLOG_ADD(YLOG_WARN, "Unable to connect to main thread's socket.");
		pthread_exit(NULL);
	}
	// loop to process new connections
	for (; ; ) {
		ydynabin_t *buff = NULL;

		// waiting for a new connection to handle
		if (nn_recv(incoming_socket, &thread->fd, sizeof(int), 0) < 0 ||
		    thread->fd < 0)
			continue;
		YLOG_ADD(YLOG_DEBUG, "Process an incoming connection.");
		// create a dynamic buffer
		buff = ydynabin_new(NULL, 0, YFALSE);
		// loop on incoming requests
		for (; ; ) {
			unsigned char *request, command;
			ybool_t sync, compress, serialized;
			command_handler_t func;

			YLOG_ADD(YLOG_DEBUG, "Processing a new request.");
			if (connection_read_data(thread, buff, 1) != YENOERR) {
				YLOG_ADD(YLOG_DEBUG, "The socket was closed.");
				goto end_of_connection;
			}
			// read request
			request = ydynabin_forward(buff, sizeof(unsigned char));
			command = REQUEST_COMMAND(*request);
			sync = (thread->transaction || REQUEST_HAS_SYNC(*request)) ? YTRUE : YFALSE;
			compress = REQUEST_HAS_COMPRESSED(*request) ? YTRUE : YFALSE;
			serialized = REQUEST_HAS_SERIALIZED(*request) ? YTRUE : YFALSE;
			YLOG_ADD(YLOG_DEBUG, "---Req: '%x' - txn: %d - sync: %d - comp: %d\n",
			         command, (thread->transaction ? 1 : 0), (sync ? 1 : 0),
			         (compress ? 1 : 0));
			// execute the command
			if (!(func = _commands[command])) {
				YLOG_ADD(YLOG_DEBUG, "Bad command '%x'", command);
				CONNECTION_SEND_ERROR(thread, RESP_ERR_PROTOCOL);
				goto end_of_connection;
			}
			YLOG_ADD(YLOG_DEBUG, "Command %x", command);
			if (func(thread, sync, compress, serialized, buff) != YENOERR)
				goto end_of_connection;
		}
end_of_connection:
		YLOG_ADD(YLOG_DEBUG, "End of connection.");
		connection_thread_disconnect(thread);
		ydynabin_delete(buff);
	}
	pthread_exit(NULL);
}

/* Fill a dynamic buffer. */
yerr_t connection_read_data(tcp_thread_t *thread, ydynabin_t *container, size_t size) {
	char buff[8196];
	ssize_t bufsz;
	yerr_t dynaerr;

	if (thread->fd < 0)
		return (YECONNRESET);
	if (container->len >= size)
		return (YENOERR);
	while (container->len < size) {
		// define timeout on the socket
		struct timeval tv;
		tv.tv_sec = thread->finedb->timeout;
		tv.tv_usec = 0;
		if (setsockopt(thread->fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(tv)) < 0)
			YLOG_ADD(YLOG_WARN, "Unable to set RCVTIMEO on socket.");
		// try to read from socket
		if ((bufsz = recv(thread->fd, buff, 8196, 0)) < 0) {
			YLOG_ADD(YLOG_DEBUG, "Socket error");
			return (YEACCESS);
		}
		if (bufsz == 0) {
			YLOG_ADD(YLOG_DEBUG, "Socket closed");
			if (container->len < size)
				return (YECONNRESET);
			break;
		}
		// remove timeout from the socket
		tv.tv_sec = 0;
		if (setsockopt(thread->fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(tv)) < 0)
			YLOG_ADD(YLOG_WARN, "Unable to remove RCVTIMEO from socket.");
		// expand the buffer
		if ((dynaerr = ydynabin_expand(container, buff, (size_t)bufsz)) != YENOERR)
			return (dynaerr);
	}
	return (YENOERR);
}

/* Send a response. */
yerr_t connection_send_response(tcp_thread_t *thread, protocol_response_t code,
                                ybool_t serialized, ybool_t compressed,
                                const void *data, size_t data_len) {
	unsigned char code_byte;
	struct iovec iov[3];
	struct msghdr mh;
	ssize_t expected = 1, rc;
	uint32_t data_nlen;
	struct timeval tv;

	YLOG_ADD(YLOG_DEBUG, "Send response (%d).", code);
	mh.msg_name = NULL;
	mh.msg_namelen = 0;
	mh.msg_iov = iov;
	mh.msg_iovlen = 1;
	mh.msg_control = NULL;
	mh.msg_controllen = 0;
	mh.msg_flags = 0;
	// code
	code_byte = (unsigned char)code;
	if (serialized)
		code_byte = RESPONSE_ADD_SERIALIZED(code_byte);
	if (compressed)
		code_byte = RESPONSE_ADD_COMPRESSED(code_byte);
	iov[0].iov_base = (caddr_t)&code_byte;
	iov[0].iov_len = sizeof(code_byte);
	// data
	if (data != NULL) {
		data_nlen = htonl((uint32_t)data_len);
		iov[1].iov_base = (caddr_t)&data_nlen;
		iov[1].iov_len = sizeof(uint32_t);
		iov[2].iov_base = (caddr_t)data;
		iov[2].iov_len = data_len;
		mh.msg_iovlen = 3;
		expected += sizeof(unsigned int) + data_len;
	}
	// define timeout on the socket
	tv.tv_sec = thread->finedb->timeout;
	tv.tv_usec = 0;
	if (setsockopt(thread->fd, SOL_SOCKET, SO_SNDTIMEO, (void*)&tv, sizeof(tv)) < 0)
		YLOG_ADD(YLOG_WARN, "Unable to set SNDTIMEO on socket.");
	// send the message
	rc = sendmsg(thread->fd, &mh, 0);
	if (rc < 0) {
		YLOG_ADD(YLOG_WARN, "Unable to send response.");
		return (YEIO);
	} else if (rc < expected) {
		YLOG_ADD(YLOG_WARN, "Unable to send the complete response (%d / %d).", rc, expected);
		return (YEIO);
	} else if (rc > expected) {
		YLOG_ADD(YLOG_WARN, "Too much data were sent (%d / %d).", rc, expected);
		return (YEIO);
	}
	// remove timeout from the socket
	tv.tv_sec = 0;
	if (setsockopt(thread->fd, SOL_SOCKET, SO_SNDTIMEO, (void*)&tv, sizeof(tv)) < 0)
		YLOG_ADD(YLOG_WARN, "Unable to remove SNDTIMEO from socket.");
	// return
	YLOG_ADD(YLOG_DEBUG, "Sent %d bytes '%s'.", rc, data);
	return (YENOERR);
}
