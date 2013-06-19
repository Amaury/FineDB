#ifndef __CONNECTION_THREAD_H__
#define __CONNECTION_THREAD_H__

#include <pthread.h>
#include "finedb.h"

/**
 * @typedef	tcp_thread_t
 *		Structure of connection threads.
 * @field	tid	Thread ID.
 * @field	fd	File descriptor to the socket used to communicate with the client.
 * @field	finedb	Pointer to the FineDB structure.
 */
typedef struct tcp_thread_s {
	pthread_t tid;
	int fd;
	finedb_t *finedb;
} tcp_thread_t;

/**
 * @function	connection_thread_new
 *		Create a new connection thread.
 * @param	finedb	Pointer to the FineDB structure.
 * @return	Pointer to the connection thread structure.
 */
tcp_thread_t *connection_thread_new(finedb_t *finedb);

#endif /* __CONNECTION_THREAD_H__ */
