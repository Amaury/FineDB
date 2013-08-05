#ifndef __LIBFINEDB_H__
#define __LIBFINEDB_H__

/**
 * @typedef	finedb_clien_t
 * Structure used by the client to connect to a FineDB server.
 * @field	sock	Connection socket.
 */
typedef struct finedb_client_s {
	int	sock;
} finedb_client_t;

/**
 * @function	finedb_connect
 * Connect to a FineDB server.
 * @param	hostname	Hostname.
 * @param	port		Port number.
 * @return	A pointer to an allocated client structure.
 */
finedb_client_t *finedb_connect(char *hostname, unsigned short port);

/**
 * @function	finedb_disconnect
 * Disconnect and free allocated memory.
 * @param	client	Pointer to the client structure.
 */
void finedb_disconnect(finedb_client_t *client);

/**
 * @function	finedb_setdb
 * Select to a different database.
 * @param	client	Pointer to the client structure.
 * @param	dbname	Name of the database, or NULL for the default database.
 * @return	0 if OK.
 */
int finedb_setdb(finedb_client_t *client, char *dbname);

/**
 * @function	finedb_get
 * Get a value from its key.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @param	data	Pointer to the destination data.
 * @return	0 if OK.
 */
int finedb_get(finedb_client_t *client, ybin_t key, ybin_t *data);

#endif /* __LIBFINEDB_H__ */
