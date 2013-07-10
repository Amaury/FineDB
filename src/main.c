/**
 * FineDB server
 *
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */
#include <unistd.h>
#include <string.h>
#include <signal.h>
//#include "nanomsg/nn.h"
//#include "nanomsg/fanout.h"
#include "ydefs.h"
#include "ylog.h"
#include "finedb.h"
//#include "database.h"
#include "server.h"
//#include "connection_thread.h"
//#include "writer_thread.h"

/* Global variable used by signal handlers. */
finedb_t *finedb_g = NULL;

/* Declaration of private functions. */
static void usage(void);
static void signal_handler(int signal);

/** Usage function. */
static void usage() {
	printf("Usage: finedb [-t number] [-p port] [-f path] [-h] [-d]\n"
		"\t-t number    Set the number of connection threads.\n"
		"\t-p port      Listening port number.\n"
		"\t-f path      Path to the database directory.\n"
		"\t-h           Shows this help and exits.\n"
		"\t-d           Debug mode. Error messages are more verbose.\n"
		"\n");
}

/** Signal handler. */
static void signal_handler(int sig) {
	YLOG_ADD(YLOG_DEBUG, "Interruption signal catched.");
	signal(sig, SIG_IGN);
	if (finedb_g)
		finedb_stop(finedb_g);
	// exit program
	exit(0);
}

/**
 * Main function of the program.
 */
int main(int argc, char *argv[]) {
	char *optstr = "dht:f:p:";
	int i;
	unsigned short nbr_threads = DEFAULT_NBR_THREADS;
	unsigned short port = DEFAULT_PORT;
	char *db_path = DEFAULT_DB_PATH;
	finedb_t *finedb;

	// signal handlers
	signal(SIGINT, signal_handler);
	// log init
	YLOG_INIT_STDERR();
	YLOG_SET_NOTE();
	// parse command line parameters
	while ((i = getopt(argc, argv, optstr)) != -1) {
		switch (i) {
		case 't':
			nbr_threads = (unsigned short)atoi(optarg);
			break;
		case 'p':
			port = (unsigned short)atoi(optarg);
			break;
		case 'f':
			db_path = strdup(optarg);
			break;
		case 'd':
			YLOG_SET_DEBUG();
			break;
		case 'h':
			usage();
			exit(0);
		}
	}
	YLOG_ADD(YLOG_DEBUG, "Configuration\n\tNumber of threads: %d\n"
	         "\tPort number: %d\n\tDatabase path: %s", nbr_threads, port,
	         db_path);
	// FineDB structure init
	finedb = finedb_init(db_path, port, nbr_threads);
	finedb_g = finedb;
	// FineDB run
	finedb_start(finedb);

	return (0);
}

