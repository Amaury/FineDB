#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "nanomsg/nn.h"
#include "nanomsg/fanin.h"
#include "nanomsg/fanout.h"
#include "ydefs.h"
#include "ylog.h"
#include "yerror.h"
#include "connection_thread.h"
#include "protocol.h"
#include "database.h"
#include "command_put.h"
#include "command_get.h"
#include "command_del.h"
#include "command_list.h"

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
	if ((thread->write_sock = nn_socket(AF_SP, NN_SOURCE)) < 0 ||
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
		if (nn_recv(incoming_socket, &thread->fd, sizeof(int), 0) < 0 ||
		    thread->fd < 0)
			continue;
		YLOG_ADD(YLOG_DEBUG, "Process an incoming connection.");
		// loop on incoming requests
		for (; ; ) {
			ydynabin_t *buff;
			unsigned char *command;
			ybool_t sync, compress, dbname;

			buff = ydynabin_new(NULL, 0, YFALSE);
			YLOG_ADD(YLOG_DEBUG, "Processing a new request.");
			if (connection_read_data(thread->fd, buff, 1) != YENOERR) {
				YLOG_ADD(YLOG_DEBUG, "The socket was closed.");
				goto end_of_connection;
			}
			// read command
			command = ydynabin_forward(buff, sizeof(unsigned char));
			sync = REQUEST_HAS_SYNC(*command) ? YTRUE : YFALSE;
			compress = REQUEST_HAS_COMPRESS(*command) ? YTRUE : YFALSE;
			dbname = REQUEST_HAS_DBNAME(*command) ? YTRUE : YFALSE;
			YLOG_ADD(YLOG_DEBUG, "---Req: '%x' - sync: %d - comp: %d - dbname: %d\n",
			         REQUEST_COMMAND(*command), (sync ? 1 : 0),
			         (compress ? 1 : 0), (dbname ? 1 : 0));
			switch (REQUEST_COMMAND(*command)) {
			case PROTO_LIST:
				// LIST command
				YLOG_ADD(YLOG_DEBUG, "LIST command");
				if (command_list(thread, dbname, buff) != YENOERR)
					goto end_of_connection;
				break;
			case PROTO_GET:
				// GET command
				YLOG_ADD(YLOG_DEBUG, "GET command");
				if (command_get(thread, dbname, compress, buff) != YENOERR)
					goto end_of_connection;
				break;
			case PROTO_PUT:
				if (REQUEST_HAS_DATA(*command)) {
					// PUT command
					YLOG_ADD(YLOG_DEBUG, "PUT command");
					if (command_put(thread, dbname, sync, compress, buff) != YENOERR)
						goto end_of_connection;
				} else {
					// DEL command
					YLOG_ADD(YLOG_DEBUG, "DEL command");
					if (command_del(thread, dbname, sync, buff) != YENOERR)
						goto end_of_connection;
				}
				break;
			default:
				// bad command
				YLOG_ADD(YLOG_DEBUG, "Bad command '%x'", REQUEST_COMMAND(*command));
				connection_send_response(thread->fd, RESP_PROTO,
				                         YFALSE, NULL, 0);
				goto end_of_connection;
			}
		}
end_of_connection:
		YLOG_ADD(YLOG_DEBUG, "End of connection.");
		close(thread->fd);
	}
	pthread_exit(NULL);
}

/* Fill a dynamic buffer. */
yerr_t connection_read_data(int fd, ydynabin_t *container, size_t size) {
	char buff[8196];
	ssize_t bufsz;
	yerr_t dynaerr;

	if (container->len >= size)
		return (YENOERR);
	while (container->len < size) {
		if ((bufsz = read(fd, buff, 8196)) < 0)
			return (YEACCESS);
		if (bufsz == 0) {
			if (container->len < size)
				return (YECONNRESET);
			break;
		}
		if ((dynaerr = ydynabin_expand(container, buff, (size_t)bufsz)) != YENOERR)
			return (dynaerr);
	}
	return (YENOERR);
}

/* Send a response. */
yerr_t connection_send_response(int fd, unsigned char code, ybool_t compress,
                                const void *data, size_t data_len) {
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
	YLOG_ADD(YLOG_DEBUG, "Sent %d bytes '%s'.", rc, data);
	return (YENOERR);
}

