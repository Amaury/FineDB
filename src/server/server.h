#ifndef __SERVER_H__
#define __SERVER_H__

#include "finedb.h"
#include "yerror.h"

/**
 * Create a listening socket.
 * @param	psock	Pointer to the socket.
 * @param	port	Port number to bind to.
 * @return	YENOERR if OK.
 */
yerr_t server_create_listening_socket(int *psock, unsigned short port);

/**
 * Main FineDB server loop.
 * @param	prun		Pointer to the run boolean.
 * @param	socket		Socket to listen to.
 * @param	threads_socket	Socket used to communicate with threads.
 */
void server_loop(ybool_t *run, int socket, int threads_socket);

#endif /* __SERVER_H__ */
