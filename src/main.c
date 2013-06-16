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
#include "connection_thread.h"

/** @var finedb_g	Global program structure. */
finedb_t finedb_g = {
	.database		= NULL,
	.writer_tid		= 0,
	.tcp_threads		= NULL,
	.nbr_threads_mini	= DEFAULT_NBR_THREADS,
	.nbr_threads_maxi	= DEFAULT_NBR_MAX_THREADS
};

/** Usage function. */
static void usage() {
	printf("Usage: finedb [-s number] [-m number] [-p path] [-h] [-d]\n"
		"\t-s number	Set the number of connection threads at startup.\n"
		"\t-m number	Set the maximum number of connection threads.\n"
		"\t-p path	Path to the database directory.\n"
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
	char *optstr = "dhr:m:p:";
	int i;
	char *db_path = DEFAULT_DB_PATH;

	// log init
	YLOG_INIT_STDERR();
	YLOG_SET_NOTE();
	// parse command line parameters
	while ((i = getopt(argc, argv, optstr)) != -1) {
		switch (i) {
		case 's':
			finedb_g.nbr_threads_mini = atoi(optarg);
			break;
		case 'm':
			finedb_g.nbr_threads_maxi = atoi(optarg);
			break;
		case 'p':
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
	finedb_g.database = database_open(db_path);
	if (finedb_g.database == NULL) {
		YLOG_ADD(YLOG_ERR, "Unable to open database.");
		exit(1);
	}
	// create writer thread
	if (pthread_create(&finedb_g.writer_tid, NULL, writer_loop, &finedb_g)) {
        	YLOG_ADD(YLOG_ERR, "Unable to create writer thread.");
		database_close(finedb_g.database);
		exit(2);
	}
	// create connection threads
	finedb_g.tcp_free_threads = ylist_new();
	finedb_g.tcp_used_threads = ylist_new();
	for (i = 0; i < (unsigned short)finedb_g.nbr_threads_mini; i++) {
		tcp_thread_t *thread;
		ylist_elem_t *elem;

		thread = connection_thread_new();
		elem = ylist_add(finedb_g.tcp_threads, thread);
	}

	return (0);
}
