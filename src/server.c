#include <linux/limits.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "ylog.h"
#include "server.h"

/* Initialize a finedb structure. */
finedb_t *init_finedb() {
	finedb_t *finedb = YMALLOC(sizeof(finedb_t));

	finedb->run = YTRUE;
	//finedb->database = NULL;
	finedb->socket = -1;
	//finedb->writer_tid = 0;
	//finedb->tcp_threads = NULL;
	finedb->nbr_threads_mini = DEFAULT_NBR_THREADS;
	finedb->nbr_threads_maxi = DEFAULT_NBR_MAX_THREADS;
	//finedb->first_waiting_thread = NULL;
	finedb->purge_counter = DEFAULT_PURGE_COUNTER;
	return (finedb);
}

/* Create the socket for incoming connections. */
yerr_t create_listening_socket(finedb_t *finedb, unsigned short port) {
	struct sockaddr_in addr;
	unsigned int addr_size;
	const int on = 1;

	// create the socket
	if ((finedb->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		YLOG_ADD(YLOG_CRIT, "Socket error");
		return (YEIO);
	}
	// some options
	if (setsockopt(finedb->socket, SOL_SOCKET, SO_REUSEADDR, (void*)&on,
	               sizeof(on)) < 0)
		YLOG_ADD(YLOG_WARN, "setsockopt(SO_REUSEADDR) failed");
	if (setsockopt(finedb->socket, SOL_SOCKET, SO_KEEPALIVE, (void*)&on,
	               sizeof(on)) < 0)
		YLOG_ADD(YLOG_WARN, "setsockopt(SO_KEEPALIVE) failed");
	// binding to any 
	addr_size = sizeof(addr);
	memset(&addr, 0, addr_size);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (bind(finedb->socket, (struct sockaddr*)&addr, addr_size) < 0) {
		YLOG_ADD(YLOG_CRIT, "Bind error");
		return (YEBADF);
	}
	if (listen(finedb->socket, SOMAXCONN)) {
		YLOG_ADD(YLOG_CRIT, "Listen error");
		return (YEBADF);
	}
	return (YENOERR);
}

/* Main FineDB server loop. */
void main_loop(finedb_t *finedb) {
	int fd;
	struct sockaddr_in addr;
	unsigned int addr_size;
	const int on = 1;

	addr_size = sizeof(addr);
	memset(&addr, 0, addr_size);
	while (finedb->run) {
		// accept a new connection
		if ((fd = accept(finedb->socket, (struct sockaddr*)&addr,
		                 &addr_size)) < 0)
			continue ;
		if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&on,
		               sizeof(on)) < 0)
			YLOG_ADD(YLOG_WARN, "setsockopt(KEEPALIVE) failed");
		// choose a thread to process the connection
		find_thread_to_process_connection(finedb, fd);
	}
	close(finedb->socket);
}

/* Find a thread to process an incoming connection. */
void find_thread_to_process_connection(finedb_t *finedb, int fd) {
	tcp_thread_t *thread;

	// search a waiting thread
	if (finedb->first_waiting_thread == NULL) {
		ylist_elem_t *elem;

		for (elem = finedb->tcp_threads->first; elem; elem = elem->next) {
			thread = (tcp_thread_t*)elem->data;
			if (thread->state == TCP_WAIT)
				break;
		}
		finedb->first_waiting_thread = elem;
	}
	// no free thread, create a new one

	// use the first free thread to process the incoming connection
	if (finedb->first_waiting_thread != NULL) {
		thread = (tcp_thread_t*)finedb->first_waiting_thread->data;
		thread->fd = fd;
		thread->state = TCP_RUN;
		pthread_mutex_unlock(&(thread->mut_do));
		
		for (i = 0; i < server->nbr_threads; ++i) {
			thread = (ytcp_thread_t*)(server->vect_threads[i]);
			if (thread->state == YTCP_WAIT) {
				server->first_waiting = i;
				break;
			}
		}
		if (i == server->nbr_threads)
			server->first_waiting = -1;
		YLOG_MOD("ytcp", YLOG_DEBUG, "Exiting");
		return (YENOERR);
	}
}
