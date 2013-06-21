#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "nanomsg/nn.h"
#include "nanomsg/fanout.h"
#include "ydefs.h"
#include "ylog.h"
#include "yerror.h"
#include "connection_thread.h"
#include "protocol.h"
#include "command_put.h"

/* Create a new connection thread. */
tcp_thread_t *connection_thread_new(finedb_t *finedb) {
	tcp_thread_t *thread;

	thread = YMALLOC(sizeof(tcp_thread_t));
	thread->fd = -1;
	thread->finedb = finedb;
	if (pthread_create(&(thread->tid), 0, connection_thread_execution,
	    thread)) {
		YLOG_ADD(YLOG_WARN, "Unable to create thread.");
		YFREE(thread);
		return (NULL);
	}
	pthread_detach(thread->tid);
	return (thread);
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
		// waiting for a new connection to handle
		if (nn_recv(incoming_socket, &thread->fd, sizeof(int), 0) < 0)
			continue;
		YLOG_ADD(YLOG_DEBUG, "Process an incoming connection.");
		// loop on incoming requests
		for (; ; ) {
			char buff[4096];
			size_t bufsz, offset;
			unsigned char command;

			YLOG_ADD(YLOG_DEBUG, "Processing a new request.");
			bufsz = read(thread->fd, buff, 4096);
			if (bufsz <= 0) {
				YLOG_ADD(YLOG_DEBUG, "The socket was closed (%d).", bufsz);
				goto end_of_connection;
			}
			// read command
			command = buff[0];
			offset++;
			switch (REQUEST_COMMAND(command)) {
			case PROTO_PUT:
				YLOG_ADD(YLOG_DEBUG, "PUT command");
				if (command_put(thread, &buff[1], bufsz) != YENOERR)
					goto end_of_connection;
				break;
			case PROTO_GET:
				YLOG_ADD(YLOG_DEBUG, "GET command");
				if (connection_send_response(thread->fd, RESP_OK, NULL, 0) != YENOERR)
					goto end_of_connection;
				break;
			case PROTO_DEL:
				YLOG_ADD(YLOG_DEBUG, "DEL COMMAND");
				if (connection_send_response(thread->fd, RESP_OK, NULL, 0) != YENOERR)
					goto end_of_connection;
				break;
			default:
				YLOG_ADD(YLOG_DEBUG, "Bad command");
				connection_send_response(thread->fd, RESP_BAD_CMD, NULL, 0);
				goto end_of_connection;
			}
		}
end_of_connection:
		YLOG_ADD(YLOG_DEBUG, "End of connection.");
		close(thread->fd);
	}
}

/* Send a response. */
yerr_t connection_send_response(int fd, unsigned char code, const void *data, size_t data_len) {
	struct iovec iov[3];
	struct msghdr mh;
	ssize_t expected = 1, rc;
	uint32_t data_nlen;

	mh.msg_name = NULL;
	mh.msg_namelen = 0;
	mh.msg_iov = iov;
	mh.msg_iovlen = 1;
	mh.msg_control = NULL;
	mh.msg_controllen = 0;
	mh.msg_flags = 0;
	if (data != NULL) {
		code = RESPONSE_ADD_DATA(code);
		data_nlen = htonl((uint32_t)data_len);
		iov[1].iov_base = (caddr_t)&data_nlen;
		iov[1].iov_len = sizeof(uint32_t);
		iov[2].iov_base = (caddr_t)data;
		iov[2].iov_len = data_len;
		mh.msg_iovlen = 3;
		expected += sizeof(unsigned int) + data_len;
	}
	iov[0].iov_base = (caddr_t)&code;
	iov[0].iov_len = sizeof(code);
	rc = sendmsg(fd, &mh, 0);
	if (rc < 0) {
		YLOG_ADD(YLOG_WARN, "Unable to send response.");
		return (YEIO);
	} else if (rc < expected) {
		YLOG_ADD(YLOG_WARN, "Unable to send the complete response (%d / %d).", rc, expected);
		return (YEIO);
	}
	YLOG_ADD(YLOG_DEBUG, "Sent %d bytes.", rc);
	return (YENOERR);
}

