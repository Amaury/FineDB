#ifndef __SERVER_H__
#define __SERVER_H__

#include "finedb.h"
#include "yerror.h"

/**
 * Initialize a finedb structure.
 * @return	A pointer to the allocated structure.
 */
finedb_t *init_finedb(void);

/**
 * Create a listening socket.
 * @param	finedb	Pointer to the FineDB structure.
 * @param	port	Port number to bind to.
 * @return	YENOERR if OK.
 */
yerr_t create_listening_socket(finedb_t *finedb, unsigned short port);

/**
 * Main FineDB server loop.
 * @param	finedb	Pointer to the FineDB structure.
 */
void main_loop(finedb_t *finedb);

/**
 * Find a thread to process an incoming connection.
 * @param	finedb	Pointer to the FineDB structure.
 * @param	fd	Connection socket's file descriptor.
 */
void find_thread_to_process_connection(finedb_t *finedb, int fd);

#endif /* __SERVER_H__ */
