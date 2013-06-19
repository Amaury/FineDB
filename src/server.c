#include <linux/limits.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include "nanomsg/nn.h"
#include "nanomsg/fanout.h"
#include "ylog.h"
#include "connection_thread.h"
#include "server.h"

/* Initialize a finedb structure. */
finedb_t *init_finedb() {
	finedb_t *finedb = YMALLOC(sizeof(finedb_t));

	finedb->run = YTRUE;
	//finedb->database = NULL;
	finedb->socket = -1;
	finedb->threads_socket = -1;
	//finedb->writer_tid = 0;
	finedb->tcp_threads = yv_create(YVECT_SIZE_MEDIUM);
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
	// binding to any interface
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
	char buff[16];

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
		// write the file descriptor number into the threads communication socket
		nn_send(finedb->threads_socket, &fd, sizeof(fd), 0);
	}
	close(finedb->socket);
}

