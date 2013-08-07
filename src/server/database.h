#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "lmdb.h"
#include "ydefs.h"
#include "ybin.h"
#include "yerror.h"
#include "ylog.h"

/** Callback function for DB list. */
typedef yerr_t (*database_callback)(void *ptr, ybin_t key, ybin_t data);

/**
 * Open a LMDB database.
 * @param	path		Path to the database data directory.
 * @param	mapsize		Database map size.
 * @param	nbr_readers	Maximum number of reader threads.
 * @param	nbr_dbs		Maximum number of opened databases.
 * @return	A pointer to the allocated environment, or NULL.
 */
MDB_env *database_open(const char *path, size_t mapsize, unsigned int nbr_readers, unsigned int nbr_dbs);

/**
 * Close a database and free its structure.
 * @param	env	A pointer to the database environment.
 */
void database_close(MDB_env *env);

/**
 * Open a transaction.
 * @param	env		A pointer to the database environment.
 * @param	readonly	YTRUE if the transaction is read-only.
 * @return	A pointer to the created transaction, or NULL if an error occurs.
 */
MDB_txn *database_transaction_start(MDB_env *env, ybool_t readonly);

/**
 * Commit a transaction.
 * @param	transaction	Pointer to the opened transaction.
 * @return	YENOERR if OK.
 */
yerr_t database_transaction_commit(MDB_txn *transaction);

/**
 * Rollback a transaction.
 * @param	transaction	Pointer to the opened transaction.
 */
void database_transaction_rollback(MDB_txn *transaction);

/**
 * Add or update a key in database.
 * @param	env		Database environment.
 * @param	transaction	Pointer to the transaction. NULL for standalone transaction.
 * @param	create_only	YTRUE if the key must not already exist.
 * @param	name		Database name. NULL for the default DB.
 * @param	key		Key binary data.
 * @param	data		Binary data.
 * @return	YENOERR 	if OK.
 */
yerr_t database_put(MDB_env *env, MDB_txn *transaction, ybool_t create_only, const char *name, ybin_t key, ybin_t data);

/**
 * Remove a key from database.
 * @param	env		Database environment.
 * @param	transaction	Pointer to the transaction. NULL for standalone transaction.
 * @param	name		Database name. NULL for the default DB.
 * @param	key		Key binary data.
 * @return	YENOERR if OK.
 */
yerr_t database_del(MDB_env *env, MDB_txn *transaction, const char *name, ybin_t key);

/**
 * Get a key from database.
 * @param	env		Database environment.
 * @param	transaction	Pointer to the transaction. NULL for standalone transaction.
 * @param	name		Database name. NULL for the default DB.
 * @param	key		Key binary data.
 * @param	data		Pointer to an allocated data space.
 * @return	YENOERR if OK, YENODATA if the key doesn't exists. YEACCESS if an error occurs.
 */
yerr_t database_get(MDB_env *env, MDB_txn *transaction, const char *name, ybin_t key, ybin_t *data);

/**
 * Loop through the key/value pairs of a database.
 * @param	env		Database environment.
 * @param	transaction	Pointer to the transaction. NULL for standalone transaction.
 * @param	name		Database name. NULL for the default DB.
 * @param	cb		Callback function, used on every key/value.
 * @param	cb_data		Pointer to private data for the callback function.
 * @return	YENOERR if OK.
 */
yerr_t database_list(MDB_env *env, MDB_txn *transaction, const char *name, database_callback cb, void *cb_data);

/**
 * Remove a database and its keys.
 * @param	env		Database environment.
 * @param	transaction	Pointer to the transaction. NULL for standalone transaction.
 * @param	name		Database name.
 * @return 	YENOERR if OK.
 */
yerr_t database_drop(MDB_env *env, MDB_txn *transaction, const char *name);

#endif /* __DATABASE_H__ */
