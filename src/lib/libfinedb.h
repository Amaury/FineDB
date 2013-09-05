#ifndef __LIBFINEDB_H__
#define __LIBFINEDB_H__

#include "ydefs.h"
#include "ybin.h"

/**
 * @typedef	finedb_result_t
 * Return values of FineDB client library functions.
 * @const	FINEDB_OK		The request was executed correctly.
 * @const	FINEDB_ERR_NETWORK	Network error.
 * @const	FINEDB_ERR_SERVER	Server error.
 * @const	FINEDB_ERR_FILE		File exists or doesn't exist.
 * @const	FINEDB_ERR_MEMORY	Unable to allocate memory.
 * @const	FINEDB_ERR_ZIP		Compression/decompression error.
 */
typedef enum finedb_result_e {
	FINEDB_OK = 0,
	FINEDB_ERR_NETWORK = 1,
	FINEDB_ERR_SERVER = 2,
	FINEDB_ERR_FILE = 3,
	FINEDB_ERR_MEMORY = 4,
	FINEDB_ERR_ZIP = 5
} finedb_result_t;

/**
 * @typedef	finedb_clien_t
 * Structure used by the client to connect to a FineDB server.
 * @field	hostname	Server hostname.
 * @field	port		Port number.
 * @field	sock		Connection socket.
 * @field	sync		YTRUE for synchronous mode.
 * @field	debug		YTRUE for debug mode.
 */
typedef struct finedb_client_s {
	char *hostname;
	unsigned short port;
	int sock;
	ybool_t sync;
	ybool_t debug;
} finedb_client_t;

/**
 * @function	finedb_create
 * Create a FineDB connection client.
 * @param	hostname	Hostname.
 * @param	port		Port number.
 * @return	A pointer to an allocated client structure.
 */
finedb_client_t *finedb_create(const char *hostname, unsigned short port);

/**
 * @function	finedb_delete
 * @Destroy a FineDB connection client.
 * @param	client	Pointer to the client structure.
 */
void finedb_delete(finedb_client_t *client);

/**
 * @function	finedb_connect
 * Connect to the configured server.
 * @param	client	Pointer to the client structure.
 * @return	FINEDB_OK if OK.
 */
int finedb_connect(finedb_client_t *client);

/**
 * @function	finedb_disconnect
 * Disconnect and free allocated memory.
 * @param	client	Pointer to the client structure.
 */
void finedb_disconnect(finedb_client_t *client);

/**
 * @function	findb_reconnect
 * Reconnect to a previously connected server.
 * @param	client	Pointer to the client structure.
 * @return	FINEDB_OK if OK.
 */
int finedb_reconnect(finedb_client_t *client);

/**
 * @function	finedb_sync
 * Set synchronous mode.
 * @param	client	Pointer to the client structure.
 */
void finedb_sync(finedb_client_t *client);

/**
 * @function	finedb_async
 * Set asynchronous mode.
 * @param	client	Pointer to the client structure.
 */
void finedb_async(finedb_client_t *client);

/**
 * @function	finedb_setdb
 * Select to a different database.
 * @param	client	Pointer to the client structure.
 * @param	dbname	Name of the database, or NULL for the default database.
 * @return	FINEDB_OK if OK.
 */
int finedb_setdb(finedb_client_t *client, char *dbname);

/**
 * @function	finedb_get
 * Get a value from its key.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @param	data	Pointer to the destination data.
 * @return	FINEDB_OK if OK.
 */
int finedb_get(finedb_client_t *client, ybin_t key, ybin_t *data);

/**
 * @function	finedb_del
 * Delete a value from database.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @return	FINEDB_OK if OK.
 */
int finedb_del(finedb_client_t *client, ybin_t key);

/**
 * @function	finedb_put
 * Put a key/value in the database.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @param	data	Pointer to the data content.
 * @return	FINEDB_OK if OK.
 */
int finedb_put(finedb_client_t *client, ybin_t key, ybin_t data);

/**
 * @function	finedb_add
 * Add a new key in the database.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @param	data	Pointer to the data content.
 * @return	FINEDB_OK if OK.
 */
int finedb_add(finedb_client_t *client, ybin_t key, ybin_t data);

/**
 * @function	finedb_update
 * Update a key in the database.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @param	data	Pointer to the data content.
 * @return	FINEDB if OK.
 */
int finedb_update(finedb_client_t *client, ybin_t key, ybin_t data);

#if 0
/**
 * @function	finedb_inc
 * Increment a value.
 * @param	client		Pointer to the client structure.
 * @param	key		Pointer to the key content.
 * @param	increment	Increment value.
 * @param	new_value	Pointer to the new value. Could be NULL.
 * @return	FINEDB_OK if OK.
 */
int finedb_inc(finedb_client_t *client, ybin_t key, int increment, int *new_value);

/**
 * @function	finedb_dec
 * Decrement a value.
 * @param	client		Pointer to the client structure.
 * @param	key		Pointer to the key content.
 * @param	decrement	Decrement value.
 * @param	new_value	Pointer to the new value. Could be NULL.
 * @return	FINEDB_OK if OK.
 */
int finedb_dec(finedb_client_t *client, ybin_t key, int increment, int *new_value);
#endif /* 0 */

/**
 * @function	finedb_start
 * Start a transaction.
 * @param	client	Pointer to the client structure.
 * @return	FINEDB_OK if OK.
 */
int finedb_start(finedb_client_t *client);

/**
 * @function	finedb_stop
 * Stop a transaction.
 * @param	client	Pointer to the client structure.
 * @return	FINEDB_OK if OK.
 */
int finedb_stop(finedb_client_t *client);

/**
 * @function	finedb_ping
 * Test a running connection.
 * @param	client	Pointer to the client structure.
 * @return	FINEDB_OK if OK.
 */
int finedb_ping(finedb_client_t *client);

#endif /* __LIBFINEDB_H__ */
