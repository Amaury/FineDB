#include <string.h>
#include <stdio.h>
#include "nanomsg/nn.h"
#include "nanomsg/pipeline.h"
#include "server.h"
#include "connection_thread.h"
#include "writer_thread.h"
#include "database.h"
#include "self_path.h"
#include "finedb.h"

/* Initialize a finedb structure. */
finedb_t *finedb_init(char *db_path, unsigned short port,
                      unsigned short nbr_threads, size_t mapsize,
                      unsigned int nbr_dbs, unsigned short timeout) {
	finedb_t *finedb = NULL;
	unsigned short i;

	// structure allocation
	finedb = YMALLOC(sizeof(finedb_t));
	finedb->run = YTRUE;
	//finedb->database = NULL;
	finedb->socket = -1;
	finedb->threads_socket = -1;
	//finedb->writer_tid = 0;
	finedb->tcp_threads = yv_create(YVECT_SIZE_MEDIUM);
	finedb->timeout = timeout;

	// path management
	if (db_path == NULL) {
		const char *base_path = get_self_path();
		db_path = YMALLOC(strlen(base_path) + strlen(DEFAULT_DB_PATH) + 2);
		sprintf(db_path, "%s/%s", base_path, DEFAULT_DB_PATH);
	}
	// open database
	finedb->database = database_open(db_path, mapsize, nbr_threads, nbr_dbs);
	if (finedb->database == NULL) {
		YLOG_ADD(YLOG_CRIT, "Unable to open database.");
		exit(1);
	}
	// create the nanomsg socket for threads communication
	if ((finedb->threads_socket = nn_socket(AF_SP, NN_PUSH)) < 0 ||
	    nn_bind(finedb->threads_socket, ENDPOINT_THREADS_SOCKET) < 0) {
		YLOG_ADD(YLOG_CRIT, "Unable to create threads socket.");
		database_close(finedb->database);
		exit(2);
	}
	// create the writer thread
	if (pthread_create(&finedb->writer_tid, NULL, writer_loop, finedb)) {
		YLOG_ADD(YLOG_ERR, "Unable to create writer thread.");
		database_close(finedb->database);
		exit(3);
	}
	// create connection threads
	for (i = 0; i < nbr_threads; i++) {
		tcp_thread_t *thread;

		if ((thread = connection_thread_new(finedb)) != NULL)
			yv_add(&finedb->tcp_threads, thread);
	}
	// create the listening socket
	if (server_create_listening_socket(&finedb->socket, port) != YENOERR) {
		YLOG_ADD(YLOG_CRIT, "Aborting.");
		exit(4);
	}

	return (finedb);
}

/* Starts a finedb run. */
void finedb_start(finedb_t *finedb) {
	// main server loop
	server_loop(&finedb->run, finedb->socket, finedb->threads_socket);
}

/* Ends a finedb run. */
void finedb_stop(finedb_t *finedb) {
	finedb->run = YFALSE;
	database_close(finedb->database);
}
