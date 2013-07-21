#include "database.h"

/* Open a LMDB database. */
MDB_env *database_open(const char *path, size_t mapsize, unsigned int nbr_dbs) {
	MDB_env *env;
	int rc;

	YLOG_ADD(YLOG_DEBUG, "Open database.");
	// environment initialization
	rc = mdb_env_create(&env);
	if (rc) {
		YLOG_ADD(YLOG_ERR, "Unable to open database on '%s' (%s).", path, mdb_strerror(rc));
		return (NULL);
	}
	// environment mapsize
	rc = mdb_env_set_mapsize(env, mapsize);
	if (rc) {
		YLOG_ADD(YLOG_ERR, "Unable to set mapsize to %d (%s).", mapsize, mdb_strerror(rc));
		return (NULL);
	}
	// setting the maximum number of opened databases
	if (nbr_dbs > 1) {
		rc = mdb_env_set_maxdbs(env, nbr_dbs);
		if (rc) {
			YLOG_ADD(YLOG_ERR, "Unable to set max dbs (%s).", mdb_strerror(rc));
			return (NULL);
		}
	}
	// opening database
	rc = mdb_env_open(env, path, MDB_WRITEMAP | MDB_NOTLS, 0664);
	if (rc) {
		YLOG_ADD(YLOG_ERR, "Unable to open database environmenti (%s).", mdb_strerror(rc));
		mdb_env_close(env);
		return (NULL);
	}
	YLOG_ADD(YLOG_DEBUG, "Database opened.");
	return (env);
}

/* Close a database and free its structure. */
void database_close(MDB_env *env) {
	YLOG_ADD(YLOG_DEBUG, "Close database.");
	mdb_env_close(env);
	YLOG_ADD(YLOG_DEBUG, "Datbase closed.");
}

/* Add or update a key in database. */
yerr_t database_put(MDB_env *env, const char *name, ybin_t key, ybin_t data) {
	MDB_dbi dbi;
	MDB_txn *txn;
	MDB_val db_key, db_data;
	int rc;

	// transaction init
	rc = mdb_txn_begin(env, NULL, 0, &txn);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to create transaction (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// open database in read-write mode
	rc = mdb_dbi_open(txn, name, MDB_CREATE, &dbi);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to open database handle (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// key and data init
	db_key.mv_size = key.len;
	db_key.mv_data = key.data;
	db_data.mv_size = data.len;
	db_data.mv_data = data.data;
	// put data
	rc = mdb_put(txn, dbi, &db_key, &db_data, 0);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to write data in database (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// transaction commit
	rc = mdb_txn_commit(txn);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to commit transaction (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// close database
	mdb_dbi_close(env, dbi);
	return (YENOERR);
}

/* Remove a key from database. */
yerr_t database_del(MDB_env *env, const char *name, ybin_t key) {
	MDB_dbi dbi;
	MDB_txn *txn;
	MDB_val db_key;
	int rc;

	// transaction init
	rc = mdb_txn_begin(env, NULL, 0, &txn);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to create transaction (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// open database in read-write mode
	rc = mdb_dbi_open(txn, name, 0, &dbi);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to open database handle (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// key and data init
	db_key.mv_size = key.len;
	db_key.mv_data = key.data;
	// put data
	rc = mdb_del(txn, dbi, &db_key, NULL);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to write data in database (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// transaction commit
	rc = mdb_txn_commit(txn);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to commit transaction (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// close database
	mdb_dbi_close(env, dbi);
	return (YENOERR);
}

/* Get a key from database. */
yerr_t database_get(MDB_env *env, const char *name, ybin_t key, ybin_t *data) {
	MDB_dbi dbi;
	MDB_txn *txn;
	MDB_val db_key, db_data;
	int rc;

	// transaction init
	rc = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to create transaction (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// open database in read-write mode
	rc = mdb_dbi_open(txn, name, 0, &dbi);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to open database handle (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// key and data init
	db_key.mv_size = key.len;
	db_key.mv_data = key.data;
	// get data
	rc = mdb_get(txn, dbi, &db_key, &db_data);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to read data in database (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// end of transaction
	mdb_txn_abort(txn);
	// close database
	mdb_dbi_close(env, dbi);
	// return
	data->len = db_data.mv_size;
	data->data = db_data.mv_data;
	return (YENOERR);
}

/* Open a cursor on a database, and send every key/value pair to a callback. */
yerr_t database_list(MDB_env *env, const char *name, database_callback cb, void *cb_data) {
	MDB_dbi dbi;
	MDB_txn *txn;
	MDB_cursor *cursor;
	MDB_val db_key, db_data;
	int rc;

	// transaction init
	rc = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to create transaction (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// open database in read-write mode
	rc = mdb_dbi_open(txn, name, 0, &dbi);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to open database handle (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// open cursor
	rc = mdb_cursor_open(txn, dbi, &cursor);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to open cursor on database (%s).", mdb_strerror(rc));
		return (YEACCESS);
	}
	// loop on cursor
	while ((rc = mdb_cursor_get(cursor, &db_key, &db_data, MDB_NEXT)) == 0) {
		ybin_t key, data;

		ybin_set(&key, db_key.mv_data, db_key.mv_size);
		ybin_set(&data, db_data.mv_data, db_data.mv_size);
		if (cb(cb_data, key, data) != YENOERR)
			break;
	}
	// close cursor
	mdb_cursor_close(cursor);
	// end of transaction
	mdb_txn_abort(txn);
	// close database
	mdb_dbi_close(env, dbi);
	return (YENOERR);
}
