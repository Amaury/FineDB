/**
 * @header	finedb.h
 *
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */
#ifndef __FINEDB_H__
#define __FINEDB_H__

#include <pthread.h>
#include "lmdb.h"
#include "ydefs.h"
#include "yvect.h"

/** @const DEFAULT_NBR_THREADS Default number of connection threads. */
#define DEFAULT_NBR_THREADS	15
/** @const DEFAULT_DB_PATH Default path to the database. */
#define DEFAULT_DB_PATH		"../var/database"
/** @const DEFAULT_PORT Default port number. */
#define DEFAULT_PORT		11138
/** @const DEFAULT_MAPSIZE Default map size (10 MB). */
#define DEFAULT_MAPSIZE		10485760
/** @const DEFAULT_TIMEOUT Default connection timeout (30 seconds). */
#define DEFAULT_TIMEOUT		30

/** @const ENDPOINT_THREADS_SOCKET Threads' connection endpoint. */
#define ENDPOINT_THREADS_SOCKET	"inproc://threads_socket"
/** @const ENDPOINT_WRITER_SOCKET Writer thread's connection endpoint. */
#define ENDPOINT_WRITER_SOCKET	"inproc://writer_socket"

/**
 * @typedef	Main structure of the FineDB application.
 * @field	run		YTRUE while the server must be running.
 * @field	database	Pointer to the database environment.
 * @field	socket		Socket descriptor for incoming connections.
 * @field	threads_socket	Nanomsg socket for threads communication.
 * @field	writer_tid	ID of the writer thread.
 * @field	tcp_threads	List of connection threads.
 * @field	timeout		Time before a connection should be ended.
 */
typedef struct finedb_s {
	ybool_t run;
	MDB_env *database;
	int socket;
	int threads_socket;
	pthread_t writer_tid;
	yvect_t tcp_threads;
	unsigned short timeout;
} finedb_t;

/**
 * Initialize a finedb structure.
 * @param	db_path		Path to the database directory.
 * @param	port		Port number to listen to.
 * @param	nbr_threads	Number of connection threads.
 * @param	mapsize		Maximum size of the database.
 * @param	nbr_dbs		Maximum number of opened databases.
 * @param	timeout		Time before a connection should be ended.
 * @return	A pointer to the allocated structure.
 */
finedb_t *finedb_init(char *db_path, unsigned short port,
                      unsigned short nbr_threads, size_t mapsize,
                      unsigned int nbr_dbs, unsigned short timeout);

/**
 * Starts a finedb run.
 * @param	finedb	Pointer to the finedb structure.
 */
void finedb_start(finedb_t *finedb);

/**
 * Ends a finedb run.
 * @param	finedb	Pointer to a finedb structure.
 */
void finedb_stop(finedb_t *finedb);

#endif /* __FINEDB_H__ */
