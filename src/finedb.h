#ifndef __FINEDB_H__
#define __FINEDB_H__

#include <pthread.h>
#include "lmdb.h"
#include "ylist.h"

/** @const DEFAULT_NBR_THREADS Default number of connection threads. */
#define DEFAULT_NBR_THREADS	15
/** @const DEFAULT_NBR_MAX_THREADS Default maximum number of connection threads. */
#define DEFAULT_NBR_MAX_THREADS	300
/** @const DEFAULT_DB_PATH Default path to the database. */
#define DEFAULT_DB_PATH		"../var/database"

#ifndef __FINEDB_MAIN__
extern struct finedb_s finedb_g;
#endif /* __FINEDB_MAIN__ */

/**
 * @typedef	Main structure of the FineDB application.
 * @field	database		LMDB environment.
 * @field	writer_tid		ID of the writer thread.
 * @field	tcp_free_threads	List of free connection threads.
 * @field	tcp_used_threads	List of used connection threads.
 * @field	nbr_threads_mini	Minimum number of connection threads.
 * @field	nbr_threads_maxi	Maximum number of connection threads
 */
typedef struct finedb_s {
	MDB_env	*database;
	pthread_t writer_tid;
	ylist_t *tcp_free_threads;
	ylist_t *tcp_used_threads;
	unsigned short nbr_threads_mini;
	unsigned short nbr_threads_maxi;
} finedb_t;

#endif /* __FINEDB_H__ */
