#ifndef __LIBFINEDB_H__
#define __LIBFINEDB_H__

#include "ydefs.h"
#include "ybin.h"

/**
 * @typedef	finedb_clien_t
 * Structure used by the client to connect to a FineDB server.
 * @field	sock	Connection socket.
 * @field	sync	YTRUE for synchronous mode.
 * @field	debug	YTRUE for debug mode.
 */
typedef struct finedb_client_s {
	int	sock;
	ybool_t sync;
	ybool_t	debug;
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

/**
 * @function	finedb_del
 * Delete a value from database.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @return	0 if OK.
 */
int finedb_del(finedb_client_t *client, ybin_t key);

/**
 * @function	finedb_put
 * Put a key/value in the database.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @param	data	Pointer to the data content.
 * @return	0 if OK.
 */
int finedb_put(finedb_client_t *client, ybin_t key, ybin_t data);

/**
 * @function	finedb_add
 * Add a new key in the database.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @param	data	Pointer to the data content.
 * @return	0 if OK.
 */
int finedb_add(finedb_client_t *client, ybin_t key, ybin_t data);

/**
 * @function	finedb_update
 * Update a key in the database.
 * @param	client	Pointer to the client structure.
 * @param	key	Pointer to the key content.
 * @param	data	Pointer to the data content.
 * @return	0 if OK.
 */
int finedb_update(finedb_client_t *client, ybin_t key, ybin_t data);

/**
 * @function	finedb_inc
 * Increment a value.
 * @param	client		Pointer to the client structure.
 * @param	key		Pointer to the key content.
 * @param	increment	Increment value.
 * @param	new_value	Pointer to the new value. Could be NULL.
 * @return	0 if OK.
 */
int finedb_inc(finedb_client_t *client, ybin_t key, int increment, int *new_value);

/**
 * @function	finedb_dec
 * Decrement a value.
 * @param	client		Pointer to the client structure.
 * @param	key		Pointer to the key content.
 * @param	decrement	Decrement value.
 * @param	new_value	Pointer to the new value. Could be NULL.
 * @return	0 if OK.
 */
int finedb_dec(finedb_client_t *client, ybin_t key, int increment, int *new_value);

/**
 * @function	finedb_start
 * Start a transaction.
 * @param	client	Pointer to the client structure.
 * @return	0 if OK.
 */
int finedb_start(finedb_client_t *client);

/**
 * @function	finedb_commit
 * Commit a transaction.
 * @param	client	Pointer to the client structure.
 * @return	0 if OK.
 */
int finedb_commit(finedb_client_t *client);

/**
 * @function	finedb_rollback
 * Rollback a transaction.
 * @param	client	Pointer to the client structure.
 * @return	0 if OK.
 */
int finedb_rollback(finedb_client_t *client);

#endif /* __LIBFINEDB_H__ */
