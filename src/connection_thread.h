#ifndef __CONNECTION_THREAD_H__
#define __CONNECTION_THREAD_H__

#include <pthread.h>
#include "ylist.h"

/**
 * @typedef	tcp_state_t
 *		State of a connection thread.
 * @constant	TCP_WAIT	For for an incoming connection.
 * @constant	TCP_RUN		A connection must be processed.
 * @constant	TCP_QUIT	The thread must be ended.
 */
typedef enum tcp_state_e {
	TCP_WAIT = 0,
	TCP_RUN,
	TCP_QUIT
} tcp_state_t;

/**
 * @typedef	tcp_thread_t
 *		Structure of connection threads.
 */
typedef struct tcp_thread_s {
	pthread_t tid;
	int fd;
	pthread_mutex_t mut_do;
	tcp_state_t state;
} tcp_thread_t;

/**
 * @function	connection_thread_new
 *		Create a new connection thread.
 * @return	Pointer to the connection thread structure.
 */
tcp_thread_t *connection_thread_new(void);

#endif /* __CONNECTION_THREAD_H__ */
