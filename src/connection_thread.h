#ifndef __CONNECTION_THREAD_H__
#define __CONNECTION_THREAD_H__

#include <pthread.h>
#include "yerror.h"
#include "finedb.h"

/**
 * @typedef	tcp_thread_t
 *		Structure of connection threads.
 * @field	tid		Thread ID.
 * @field	finedb		Pointer to the FineDB structure.
 * @field	fd		File descriptor to the socket used to communicate with the client.
 * @field	write_sock	Nanomsg socket to communicate with the writer thread.
 */
typedef struct tcp_thread_s {
	pthread_t tid;
	finedb_t *finedb;
	int fd;
	int write_sock;
} tcp_thread_t;

/**
 * @typedef	tcp_state_t
 *		Each steps of a finite state machine used to parse commands.
 * @constant	STATE_READY	Nothing parsed yet.
 * @constant	STATE_COMMAND	Command was read.
 * @constant	STATE_NAMESIZE	Filename's size was read.
 * @constant	STATE_FILENAME	Filename was read.
 */
typedef enum tcp_state_e {
	STATE_READY = 0,
	STATE_COMMAND,
	STATE_NAMESIZE,
	STATE_FILENAME
} tcp_state_t;

/**
 * @function	connection_thread_new
 *		Create a new connection thread.
 * @param	finedb	Pointer to the FineDB structure.
 * @return	Pointer to the connection thread structure.
 */
tcp_thread_t *connection_thread_new(finedb_t *finedb);

/**
 * @function	connection_thread_execution
 *		Callback function executed by all server's threads. Loop to
 *		check if the thread must handle a new connection.
 * @param	param	Pointer to the thread's structure.
 * @return	Always NULL.
 */
void *connection_thread_execution(void *param);

/**
 * @function	connection_send_response
 *		Send a response to the client.
 * @param	fd		Socket descriptor.
 * @param	code		Response code.
 * @param	data		Pointer to the data to send, or NULL if there is no data.
 * @param	data_len	Date size. Unused if data is NULL.
 * @return	YENOERR if OK.
 */
yerr_t connection_send_response(int fd, unsigned char code, const void *data, size_t data_len);

#endif /* __CONNECTION_THREAD_H__ */
