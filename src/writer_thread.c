#include "nanomsg/nn.h"
#include "nanomsg/fanin.h"
#include "lmdb.h"
#include "ylog.h"
#include "writer_thread.h"
#include "finedb.h"

/* Callback function executed by the writer thread. */
void *writer_loop(void *param) {
	finedb_t *finedb = (finedb_t*)param;
	int socket;

	// create the nanomsg socket for threads communication
	if ((socket = nn_socket(AF_SP, NN_SINK)) < 0 ||
	    nn_bind(socket, ENDPOINT_WRITER_SOCKET) < 0) {
		YLOG_ADD(YLOG_CRIT, "Unable to create socket in writer thread.");
		exit(5);
	}
	// loop to process new connections
	for (; ; ) {
		writer_msg_t *msg;
		MDB_dbi dbi;
		MDB_val key, data;
		MDB_txn *txn;

		// waiting for a new connection to handle
		if (nn_recv(socket, &msg, sizeof(writer_msg_t*), 0) < 0)
			continue;
		/* add data into database */
		// opening transaction
		if (mdb_txn_begin(finedb->database, NULL, 0, &txn) != 0) {
			YLOG_ADD(YLOG_WARN, "Unable to open transaction.");
			goto free_data;
		}
		// opening the database
		if (mdb_open(txn, NULL, 0, &dbi) != 0) {
			YLOG_ADD(YLOG_WARN, "Unable to open database.");
			goto free_data;
		}
		// write data
		key.mv_size = msg->name_len;
		key.mv_data = msg->name;
		data.mv_size = msg->data_len;
		data.mv_data = msg->data;
		if (mdb_put(txn, dbi, &key, &data, 0) != 0)
			YLOG_ADD(YLOG_WARN, "Unable to write data into database.");
free_data:
		// free data
		YFREE(msg->name);
		YFREE(msg->data);
		YFREE(msg);
	}
        return (NULL);
}

