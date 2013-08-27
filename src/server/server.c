#include <linux/limits.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include "nanomsg/nn.h"
#include "nanomsg/pipeline.h"
#include "ylog.h"
#include "connection_thread.h"
#include "server.h"

/* Create the socket for incoming connections. */
yerr_t server_create_listening_socket(int *psock, unsigned short port) {
	struct sockaddr_in addr;
	unsigned int addr_size;
	const int on = 1;

	// create the socket
	if ((*psock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		YLOG_ADD(YLOG_CRIT, "Socket error");
		return (YEIO);
	}
	// some options
	if (setsockopt(*psock, SOL_SOCKET, SO_REUSEADDR, (void*)&on,
	               sizeof(on)) < 0)
		YLOG_ADD(YLOG_WARN, "setsockopt(SO_REUSEADDR) failed");
	if (setsockopt(*psock, SOL_SOCKET, SO_KEEPALIVE, (void*)&on,
	               sizeof(on)) < 0)
		YLOG_ADD(YLOG_WARN, "setsockopt(SO_KEEPALIVE) failed");
	// binding to any interface
	addr_size = sizeof(addr);
	memset(&addr, 0, addr_size);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (bind(*psock, (struct sockaddr*)&addr, addr_size) < 0) {
		YLOG_ADD(YLOG_CRIT, "Bind error");
		return (YEBADF);
	}
	if (listen(*psock, SOMAXCONN)) {
		YLOG_ADD(YLOG_CRIT, "Listen error");
		return (YEBADF);
	}
	return (YENOERR);
}

/* Main FineDB server loop. */
void server_loop(ybool_t *run, int socket, int threads_socket) {
	int fd;
	struct sockaddr_in addr;
	unsigned int addr_size;
	const int on = 1;

	addr_size = sizeof(addr);
	memset(&addr, 0, addr_size);
	while (*run) {
		// accept a new connection
		if ((fd = accept(socket, (struct sockaddr*)&addr,
		                 &addr_size)) < 0)
			continue ;
		if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&on,
		               sizeof(on)) < 0)
			YLOG_ADD(YLOG_WARN, "setsockopt(KEEPALIVE) failed");
		// write the file descriptor number into the threads communication socket
		connection_thread_push_socket(threads_socket, fd);
	}
	close(socket);
}

