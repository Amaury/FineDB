#include "nanomsg/nn.h"
#include "nanomsg/fanin.h"
#include "lmdb.h"
#include "ylog.h"
#include "ybin.h"
#include "writer_thread.h"
#include "finedb.h"
#include "database.h"

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
		ybin_t key_bin, data_bin;

		// waiting for a new connection to handle
		if (nn_recv(socket, &msg, sizeof(writer_msg_t*), 0) < 0)
			continue;
		/* add data into database */
		ybin_set(&key_bin, msg->name, msg->name_len);
		ybin_set(&data_bin, msg->data, msg->data_len);
		YLOG_ADD(YLOG_DEBUG, "WRITE '%s' => '%s'", msg->name, msg->data);
		if (database_put(finedb->database, key_bin, data_bin) != YENOERR) {
			YLOG_ADD(YLOG_WARN, "Unable to write data into database.");
			goto free_data;
		}
		YLOG_ADD(YLOG_DEBUG, "Data written to database");
free_data:
		// free data
		YFREE(msg->name);
		YFREE(msg->data);
		YFREE(msg);
	}
        return (NULL);
}

