#ifndef __CONNECTION_THREAD_H__
#define __CONNECTION_THREAD_H__

#include <pthread.h>
#include <time.h>
#include "ydefs.h"
#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "protocol.h"

/**
 * @typedef	tcp_thread_t
 *		Structure of connection threads.
 * @field	tid		Thread ID.
 * @field	finedb		Pointer to the FineDB structure.
 * @field	fd		File descriptor to the socket used to communicate
 *				with the client.
 * @field	write_sock	Nanomsg socket to send data to the writer thread.
 * @field	dbname		Name of the selected database (NULL = default).
 * @field	transaction	Pointer to the running transaction. Default to NULL.
 */
typedef struct tcp_thread_s {
	pthread_t tid;
	finedb_t *finedb;
	int fd;
	int write_sock;
	char *dbname;
	MDB_txn *transaction;
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

/** @define CONNECTION_SEND_OK Send a simple OK response to the client. */
#define CONNECTION_SEND_OK(thread)	connection_send_response(thread, RESP_OK, YFALSE, YFALSE, NULL, 0)

/** @define CONNECTION_SEND_ERROR Send an error response to the client. */
#define CONNECTION_SEND_ERROR(thread, err)	connection_send_response(thread, err, YFALSE, YFALSE, NULL, 0)

/**
 * @function	connection_thread_new
 *		Create a new connection thread.
 * @param	finedb	Pointer to the FineDB structure.
 * @return	Pointer to the connection thread structure.
 */
tcp_thread_t *connection_thread_new(finedb_t *finedb);

/**
 * @function	connection_thread_disconnect
 *		Disconnect a running connection and reset the thread.
 * @param	thread	Pointer to the thread structure.
 */
void connection_thread_disconnect(tcp_thread_t *thread);

/**
 * @function	connection_thread_push_socket
 *		Add a connection socket in the feed of waiting connections.
 * @param	threads_socket	Socket of internal communication.
 * @param	fd		File descriptor of the waiting connection's socket.
 */
void connection_thread_push_socket(int threads_socket, int socket);

/**
 * @function	connection_thread_execution
 *		Callback function executed by all server's threads. Loop to
 *		check if the thread must handle a new connection.
 * @param	param	Pointer to the thread's structure.
 * @return	Always NULL.
 */
void *connection_thread_execution(void *param);

/**
 * @function	connection_read_data
 *		Ensures that a dynamic binary buffer contains the given number
 *		of characters. If not, the needed data is read from socket.
 * @param	thread		Pointer to the thread structure.
 * @param	container	Pointer to ydynabin_t structure.
 * @param	size		Minimal size of the buffer.
 * @return	YENOERR if OK.
 */
yerr_t connection_read_data(tcp_thread_t *thread, ydynabin_t *container, size_t size);

/**
 * @function	connection_send_response
 *		Send a response to the client.
 * @param	thread		Pointer to the thread structure.
 * @param	code		Response code.
 * @param	serialized	YTRUE if the data is serialized.
 * @param	compressed	YTRUE if the data is compressed.
 * @param	data		Pointer to the data to send, or NULL if there is no data.
 * @param	data_len	Date size. Unused if data is NULL.
 * @return	YENOERR if OK.
 */
yerr_t connection_send_response(tcp_thread_t *thread, protocol_response_t code,
                                ybool_t serialized, ybool_t compressed,
                                const void *data, size_t data_len);

#endif /* __CONNECTION_THREAD_H__ */
