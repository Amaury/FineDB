/**
 * FineDB server
 *
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */
#define __FINEDB_MAIN__
#include <unistd.h>
#include "ydefs.h"
#include "ylog.h"
#include "finedb.h"
#include "database.h"
#include "server.h"
#include "connection_thread.h"

/** Usage function. */
static void usage() {
	printf("Usage: finedb [-s number] [-m number] [-p port] [-f path] [-h] [-d]\n"
		"\t-s number	Set the number of connection threads at startup.\n"
		"\t-m number	Set the maximum number of connection threads.\n"
		"\t-p port	Listening port number.\n"
		"\t-f path	Path to the database directory.\n"
		"\t-h		Shows this help and exits.\n"
		"\t-d		Debug mode. Error messages are more verbose.\n"
		"\n");
}

void *writer_loop(void *param) {
	return (NULL);
}

/**
 * Main function of the program.
 */
int main(int argc, char *argv[]) {
	char *optstr = "dhr:m:f:p:";
	int i;
	unsigned short port = DEFAULT_PORT;
	char *db_path = DEFAULT_DB_PATH;
	finedb_t *finedb;

	finedb = init_finedb();
	// log init
	YLOG_INIT_STDERR();
	YLOG_SET_NOTE();
	// parse command line parameters
	while ((i = getopt(argc, argv, optstr)) != -1) {
		switch (i) {
		case 's':
			finedb->nbr_threads_mini = atoi(optarg);
			break;
		case 'm':
			finedb->nbr_threads_maxi = atoi(optarg);
			break;
		case 'p':
			port = (unsigned short)atoi(optarg);
		case 'f':
			db_path = optarg;
			break;
		case 'd':
			YLOG_SET_DEBUG();
			break;
		case 'h':
			usage();
			exit(0);
		}
	}
	// open database
	finedb->database = database_open(db_path);
	if (finedb->database == NULL) {
		YLOG_ADD(YLOG_ERR, "Unable to open database.");
		exit(1);
	}
	// create writer thread
	if (pthread_create(&finedb->writer_tid, NULL, writer_loop, finedb)) {
        	YLOG_ADD(YLOG_ERR, "Unable to create writer thread.");
		database_close(finedb->database);
		exit(2);
	}
	// create connection threads
	finedb->tcp_threads = ylist_new();
	for (i = 0; (unsigned short)i < finedb->nbr_threads_mini; i++) {
		tcp_thread_t *thread;
		ylist_elem_t *elem;

		thread = connection_thread_new();
		elem = ylist_add(finedb->tcp_threads, thread);
	}
	finedb->first_waiting_thread = finedb->tcp_threads->first;
	// create the listening socket
	if (create_listening_socket(finedb, port) != YENOERR) {
		YLOG_ADD(YLOG_CRIT, "Aborting.");
		exit(3);
	}
	// main server loop
	main_loop(finedb);

	return (0);
}

