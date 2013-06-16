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
 * @return	A pointer to the allocated environment, or NULL.
 */
MDB_env *database_open(const char *path);

/**
 * Close a database and free its structure.
 * @param	env	A pointer to the database environment.
 */
void database_close(MDB_env *env);

/**
 * Add or update a key in database.
 * @param	env	Database environment.
 * @param	key	Key binary data.
 * @param	data	Binary data.
 * @return	YENOERR if OK.
 */
yerr_t database_put(MDB_env *env, ybin_t key, ybin_t data);

/**
 * Get a key from database.
 * @param	key	Key binary data.
 * @param	data	Pointer to an allocated data space.
 * @return	YENOERR if OK.
 */
yerr_t database_get(MDB_env *env, ybin_t key, ybin_t *data);

#endif /* __DATABASE_H__ */
