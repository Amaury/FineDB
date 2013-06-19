#include <unistd.h>
#include "nanomsg/nn.h"
#include "nanomsg/fanout.h"
#include "connection_thread.h"
#include "ydefs.h"
#include "ylog.h"

/* Private function */
static void *_connection_thread_execution(void *param);

/* Create a new connection thread. */
tcp_thread_t *connection_thread_new(finedb_t *finedb) {
	tcp_thread_t *thread;

	thread = YMALLOC(sizeof(tcp_thread_t));
	thread->fd = -1;
	thread->finedb = finedb;
	if (pthread_create(&(thread->tid), 0, _connection_thread_execution,
	    thread)) {
		YLOG_ADD(YLOG_WARN, "Unable to create thread.");
		YFREE(thread);
		return (NULL);
	}
	pthread_detach(thread->tid);
	return (thread);
}

/* Callback function executed by all server's threads. Loop to check if the
   thread must handle a new connection, or if it must die. */
void *_connection_thread_execution(void *param) {
	tcp_thread_t *thread;
	finedb_t *finedb;
	int incoming_socket;

	YLOG_ADD(YLOG_DEBUG, "Thread loop.");
	thread = (tcp_thread_t*)param;
	finedb = thread->finedb;
	// opening a connection to the main thread
	if ((incoming_socket = nn_socket(AF_SP, NN_PULL)) < 0 ||
	    nn_connect(incoming_socket, "inproc://threads_socket") < 0) {
		YLOG_ADD(YLOG_WARN, "Unable to connect to main thread's socket.");
		pthread_exit(NULL);
	}
	for (; ; ) {
		void *buffer;
		int fd;

		// waiting for a new connection to handle
		if (nn_recv(incoming_socket, &fd, sizeof(fd), 0) < 0)
			continue;
		_connection_thread_process(thread, fd);
		close(fd);
	}
}

/* Process an incoming transmission. */
void _connection_thread_process(tcp_thread_t *thread, int fd) {
	char buff[4096];
	size_t bufsz, offset;
	unsigned char command;

	for (; ; ) {
		bufsz = read(fd, buff, 4096);
		if (bufsz <= 0)
			return;
		// read command
		command = buff[0];
	}
}
