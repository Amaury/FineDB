/**
 * FineDB server
 *
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "ydefs.h"
#include "ylog.h"
#include "finedb.h"
//#include "database.h"
#include "server.h"
//#include "connection_thread.h"
//#include "writer_thread.h"
#include "self_path.h"

/* Global variable used by signal handlers. */
finedb_t *finedb_g = NULL;

/* Declaration of private functions. */
static void usage(void);
static void signal_handler(int signal);

/** Usage function. */
static void usage() {
	printf("Usage: finedb [-t number] [-n number] [-s number] [-p port] [-f path] [-i seconds] [-h] [-d]\n"
	       "\t-t number    Set the number of connection threads.\n"
	       "\t-n number    Set the maximum number of opened databases.\n"
	       "\t-s number    Set the database map size (maximum size on disk).\n"
	       "\t-p port      Listening port number.\n"
	       "\t-f path      Path to the database directory.\n"
	       "\t-i seconds   NUmber of seconds before considering a connection is timing out.\n"
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
	char *optstr = "dht:n:s:f:p:i:";
	int i;
	unsigned int nbr_dbs = 1;
	size_t mapsize = DEFAULT_MAPSIZE;
	unsigned short nbr_threads = DEFAULT_NBR_THREADS;
	unsigned short port = DEFAULT_PORT;
	unsigned short timeout = DEFAULT_TIMEOUT;
	char *db_path = NULL;
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
		case 'n':
			nbr_dbs = (unsigned int)atoi(optarg);
			break;
		case 's':
			mapsize = (size_t)atoi(optarg);
			break;
		case 'p':
			port = (unsigned short)atoi(optarg);
			break;
		case 'f':
			db_path = strdup(optarg);
			break;
		case 'i':
			timeout = atoi(optarg);
			break;
		case 'd':
			YLOG_SET_DEBUG();
			break;
		case 'h':
			usage();
			exit(0);
		}
	}
	YLOG_ADD(YLOG_DEBUG, "Configuration\n\t# threads: %d\n"
	         "\t# dbs: %d\n\tMap size: %d\n\tPort number: %d\n"
	         "\tDatabase path: %s\n\tTimeout: %d\n", nbr_threads, nbr_dbs,
	         mapsize, port, db_path, timeout);
	// FineDB structure init
	finedb = finedb_init(db_path, port, nbr_threads, mapsize, nbr_dbs, timeout);
	finedb_g = finedb;
	// FineDB run
	finedb_start(finedb);

	return (0);
}

