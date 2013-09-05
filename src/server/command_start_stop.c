#include "ylog.h"
#include "command.h"
#include "protocol.h"
#include "database.h"

/* Process a START command. */
yerr_t command_start(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized, ydynabin_t *buff) {
	YLOG_ADD(YLOG_DEBUG, "START command");
	// rollback previous transaction
	if (thread->transaction != NULL)
		database_transaction_rollback(thread->transaction);
	// open transaction
	thread->transaction = database_transaction_start(thread->finedb->database, YTRUE);
	if (thread->transaction == NULL)
		goto error;
	CONNECTION_SEND_OK(thread);
	return (YENOERR);
error:
	YLOG_ADD(YLOG_WARN, "START error");
	CONNECTION_SEND_ERROR(thread, RESP_ERR_TRANSACTION);
	return (YEACCESS);
}

/* Process a STOP command. */
yerr_t command_stop(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized, ydynabin_t *buff) {
	YLOG_ADD(YLOG_DEBUG, "STOP command");
	// check running transaction
	if (thread->transaction == NULL)
		goto error;
	// rollback transaction
	database_transaction_rollback(thread->transaction);
	thread->transaction = NULL;
	CONNECTION_SEND_OK(thread);
	return (YENOERR);
error:
	YLOG_ADD(YLOG_WARN, "STOP error");
	CONNECTION_SEND_ERROR(thread, RESP_ERR_TRANSACTION);
	return (YEACCESS);
}
