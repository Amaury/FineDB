#include "database.h"

/* Open a LMDB database. */
MDB_env *database_open(const char *path) {
	MDB_env *env;
	int rc;

	YLOG_ADD(YLOG_DEBUG, "Open database.");
	// environment initialization
	rc = mdb_env_create(&env);
	if (rc) {
		YLOG_ADD(YLOG_ERR, "Unable to open database on '%s'.", path);
		return (NULL);
	}
	// opening database
	rc = mdb_env_open(env, path, MDB_WRITEMAP | MDB_NOTLS, 0664);
	if (rc) {
		YLOG_ADD(YLOG_ERR, "Unable to open database environment.");
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
yerr_t database_put(MDB_env *env, ybin_t key, ybin_t data) {
	MDB_dbi dbi;
	MDB_txn *txn;
	MDB_val db_key, db_data;
	int rc;

	// transaction init
	rc = mdb_txn_begin(env, NULL, 0, &txn);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to create transaction.", mdb_strerror(rc));
		return (YEACCESS);
	}
	// open database in read-write mode
	rc = mdb_open(txn, NULL, 0, &dbi);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to open database handle.", mdb_strerror(rc));
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
		YLOG_ADD(YLOG_WARN, "Unable to write data in database.", mdb_strerror(rc));
		return (YEACCESS);
	}
	// transaction commit
	rc = mdb_txn_commit(txn);
	if (rc) {
		YLOG_ADD(YLOG_WARN, "Unable to commit transaction.", mdb_strerror(rc));
		return (YEACCESS);
	}
	// close database
	mdb_close(env, dbi);
	return (YENOERR);
}

/* Get a key from database. */
yerr_t database_get(MDB_env *env, ybin_t key, ybin_t *data) {
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
	rc = mdb_open(txn, NULL, 0, &dbi);
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
	mdb_close(env, dbi);
	// return
	data->len = db_data.mv_size;
	data->data = db_data.mv_data;
	return (YENOERR);
}
