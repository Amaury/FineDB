#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "lmdb.h"
#include "ydefs.h"
#include "ybin.h"
#include "yerror.h"
#include "ylog.h"

/**
 * Open a LMDB database.
 * @param	path	Path to the database data directory.
 * @param	mapsize	Database map size.
 * @param	nbr_dbs	Maximum number of opened databases.
 * @return	A pointer to the allocated environment, or NULL.
 */
MDB_env *database_open(const char *path, size_t mapsize, unsigned int nbr_dbs);

/**
 * Close a database and free its structure.
 * @param	env	A pointer to the database environment.
 */
void database_close(MDB_env *env);

/**
 * Add or update a key in database.
 * @param	env	Database environment.
 * @param	name	Database name. NULL for the default DB.
 * @param	key	Key binary data.
 * @param	data	Binary data.
 * @return	YENOERR if OK.
 */
yerr_t database_put(MDB_env *env, const char *name, ybin_t key, ybin_t data);

/**
 * Remove a key from database.
 * @param	env	Database environment.
 * @param	name	Database name. NULL for the default DB.
 * @param	key	Key binary data.
 * @return	YENOERR if OK.
 */
yerr_t database_del(MDB_env *env, const char *name, ybin_t key);

/**
 * Get a key from database.
 * @param	env	Database environment.
 * @param	name	Database name. NULL for the default DB.
 * @param	key	Key binary data.
 * @param	data	Pointer to an allocated data space.
 * @return	YENOERR if OK.
 */
yerr_t database_get(MDB_env *env, const char *name, ybin_t key, ybin_t *data);

#endif /* __DATABASE_H__ */
