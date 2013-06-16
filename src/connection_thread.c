#include <unistd.h>
#include "connection_thread.h"
#include "ydefs.h"
#include "ylog.h"

static void *_connection_thread_handle(void *param);

/* Create a new connection thread. */
tcp_thread_t *connection_thread_new() {
	tcp_thread_t *thread;

	thread = YMALLOC(sizeof(tcp_thread_t));
	thread->fd = -1;
	thread->state = TCP_WAIT;
	pthread_mutex_init(&(thread->mut_do), NULL);
	pthread_mutex_lock(&(thread->mut_do));
	if (pthread_create(&(thread->tid), 0, _connection_thread_handle,
	    thread)) {
		YLOG_ADD(YLOG_WARN, "Unable to create thread.");
		YFREE(thread);
		return (NULL);
	}
	return (thread);
}

/* Callback function executed by all server's threads. Loop to check if the
   thread must handle a new connection, or if it must die. */
void *_connection_thread_handle(void *param) {
	tcp_thread_t *thread = (tcp_thread_t*)param;
	int rc;

	YLOG_ADD(YLOG_DEBUG, "Thread loop.");
	for (; ; ) {
		rc = pthread_mutex_lock(&thread->mut_do);
		if (rc) {
			YLOG_ADD(YLOG_WARN, "Error during mutex unlock.");
			continue;
		}
		if (thread->state == TCP_QUIT) {
			YLOG_ADD(YLOG_DEBUG, "End of thread.");
			if (thread->fd != -1)
				close(thread->fd);
			pthread_exit(NULL);
		} else if (thread->state == TCP_RUN) {
			// processing of incoming connection
		}
	}
}

