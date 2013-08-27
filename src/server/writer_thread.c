#include "nanomsg/nn.h"
#include "nanomsg/pipeline.h"
#include "lmdb.h"
#include "ylog.h"
#include "writer_thread.h"
#include "finedb.h"
#include "database.h"

/* Callback function executed by the writer thread. */
void *writer_loop(void *param) {
	finedb_t *finedb = (finedb_t*)param;
	int socket;

	// create the nanomsg socket for threads communication
	if ((socket = nn_socket(AF_SP, NN_PULL)) < 0 ||
	    nn_bind(socket, ENDPOINT_WRITER_SOCKET) < 0) {
		YLOG_ADD(YLOG_CRIT, "Unable to create socket in writer thread.");
		exit(6);
	}
	// loop to process new connections
	for (; ; ) {
		writer_msg_t *msg;

		// waiting for a new connection to handle
		if (nn_recv(socket, &msg, sizeof(writer_msg_t*), 0) < 0)
			continue;
		if (msg->type == WRITE_PUT) {
			// add data in database
			YLOG_ADD(YLOG_DEBUG, "WRITE '%s' => '%s'", msg->name.data, msg->data.data);
			if (database_put(finedb->database, NULL, msg->create_only, msg->dbname, msg->name, msg->data) == YENOERR)
				YLOG_ADD(YLOG_DEBUG, "Data written to database.");
			else
				YLOG_ADD(YLOG_WARN, "Unable to write data into database.");
			YFREE(msg->data.data);
		} else if (msg->type == WRITE_DEL) {
			// remove data from database
			YLOG_ADD(YLOG_DEBUG, "DELETE '%s'", msg->name.data);
			if (database_del(finedb->database, NULL, msg->dbname, msg->name) == YENOERR)
				YLOG_ADD(YLOG_DEBUG, "Data removed from database.");
			else
				YLOG_ADD(YLOG_WARN, "Unable to delete data into database.");
		}
		// free data
		YFREE(msg->dbname);
		YFREE(msg->name.data);
		YFREE(msg);
	}
        return (NULL);
}

