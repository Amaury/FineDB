#ifndef __FINEDB_H__
#define __FINEDB_H__

#include <pthread.h>
#include "lmdb.h"
#include "ydefs.h"
#include "ylist.h"

/** @const DEFAULT_NBR_THREADS Default number of connection threads. */
#define DEFAULT_NBR_THREADS	15
/** @const DEFAULT_NBR_MAX_THREADS Default maximum number of connection threads. */
#define DEFAULT_NBR_MAX_THREADS	300
/** @const DEFAULT_DB_PATH Default path to the database. */
#define DEFAULT_DB_PATH		"../var/database"
/** @const DEFAULT_PORT Default port number. */
#define DEFAULT_PORT		11138
/** @const DEFAULT_PURGE_COUNTER Number of connections before thread purge. */
#define DEFAULT_PURGE_COUNTER	300

/**
 * @typedef	Main structure of the FineDB application.
 * @field	run			YTRUE while the server must be running.
 * @field	database		LMDB environment.
 * @field	socket			Socket descriptor for incoming connections.
 * @field	writer_tid		ID of the writer thread.
 * @field	tcp_threads		List of connection threads.
 * @field	nbr_threads_mini	Minimum number of connection threads.
 * @field	nbr_threads_maxi	Maximum number of connection threads
 * @field	first_waiting_thread	Pointer to the first waiting thread.
 * @field	purge_counter		Countdown to threads purge.
 */
typedef struct finedb_s {
	ybool_t run;
	MDB_env	*database;
	int socket;
	pthread_t writer_tid;
	ylist_t *tcp_threads;
	unsigned short nbr_threads_mini;
	unsigned short nbr_threads_maxi;
	ylist_elem_t *first_waiting_thread;
	unsigned short purge_counter;
} finedb_t;

#endif /* __FINEDB_H__ */
