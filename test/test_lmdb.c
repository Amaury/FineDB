#include <string.h>
#include "lmdb.h"
#include "ydefs.h"
#include "yerror.h"
#include "ybin.h"

yerr_t database_put(MDB_env *env, const char *name, ybin_t key, ybin_t data);
yerr_t database_get(MDB_env *env, const char *name, ybin_t key, ybin_t *data);
void database_close(MDB_env *env);
MDB_env *database_open(void);

int main() {
	MDB_env *env;
	ybin_t key, data;

	env = database_open();
	ybin_set(&key, "cle_de_test", strlen("cle_de_test") + 1);
	//ybin_set(&data, "XYZ", 4);
	//database_put(env, NULL, key, data);
	database_get(env, NULL, key, &data);
	printf("RES : '%s' => '%s'\n", (char*)key.data, (char*)data.data);
	database_close(env);
}

yerr_t database_put(MDB_env *env, const char *name, ybin_t key, ybin_t data) {
	MDB_dbi dbi;
	MDB_txn *txn;
	MDB_val db_key, db_data;
	int rc;

	// transaction init
	rc = mdb_txn_begin(env, NULL, 0, &txn);
	if (rc) {
		printf("Unable to create transaction (%s).\n", mdb_strerror(rc));
		return (YEACCESS);
	}
	// open database in read-write mode
	rc = mdb_dbi_open(txn, name, MDB_CREATE, &dbi);
	if (rc) {
		printf("Unable to open database handle (%s).\n", mdb_strerror(rc));
		return (YEACCESS);
	}
	// key and data init
	db_key.mv_size = key.len;
	db_key.mv_data = key.data;
	db_data.mv_size = data.len;
	db_data.mv_data = NULL;
	// put data
	rc = mdb_put(txn, dbi, &db_key, &db_data, MDB_RESERVE);
	if (rc) {
		printf("Unable to write data in database (%s).\n", mdb_strerror(rc));
		return (YEACCESS);
	}
	printf("après le put : %lx\n", (size_t)db_data.mv_data);
	memcpy(db_data.mv_data, data.data, data.len);
	// close database
	mdb_dbi_close(env, dbi);
	// transaction commit
	rc = mdb_txn_commit(txn);
	if (rc) {
		printf("Unable to commit transaction (%s).\n", mdb_strerror(rc));
		return (YEACCESS);
	}
	return (YENOERR);
}

yerr_t database_get(MDB_env *env, const char *name, ybin_t key, ybin_t *data) {
	MDB_dbi dbi;
	MDB_txn *txn;
	MDB_val db_key, db_data;
	int rc;

	// transaction init
	rc = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
	if (rc) {
		printf("Unable to create transaction (%s).\n", mdb_strerror(rc));
		return (YEACCESS);
	}
	// open database in read-write mode
	rc = mdb_dbi_open(txn, name, 0, &dbi);
	if (rc) {
		printf("Unable to open database handle (%s).\n", mdb_strerror(rc));
		return (YEACCESS);
	}
	// key and data init
	db_key.mv_size = key.len;
	db_key.mv_data = key.data;
	// get data
	rc = mdb_get(txn, dbi, &db_key, &db_data);
	if (rc) {
		printf("Unable to read data in database (%s).\n", mdb_strerror(rc));
		return (YEACCESS);
	}
	// close database
	mdb_dbi_close(env, dbi);
	// end of transaction
	mdb_txn_abort(txn);
	// return
	data->len = db_data.mv_size;
	data->data = db_data.mv_data;
	return (YENOERR);
}

MDB_env *database_open() {
	MDB_env *env;
	int rc;

	if ((rc = mdb_env_create(&env))) {
		printf("Unable to create env (%s).\n", mdb_strerror(rc));
		exit(1);
	}
	//mdb_env_set_maxdbs(env, 5);
	if ((rc = mdb_env_open(env, "../var/database", MDB_WRITEMAP | MDB_NOTLS, 0664))) {
		printf("Unable to open database (%s).\n", mdb_strerror(rc));
		exit(2);
	}
	return (env);
}

void database_close(MDB_env *env) {
	mdb_env_close(env);
}
