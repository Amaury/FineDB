#include "ylog.h"
#include "command_start_commit_rollback.h"
#include "protocol.h"
#include "database.h"

/* Process a START command. */
yerr_t command_start(tcp_thread_t *thread) {
	YLOG_ADD(YLOG_DEBUG, "START command");
	// rollback previous transaction
	if (thread->transaction != NULL)
		database_transaction_rollback(thread->transaction);
	// open transaction
	thread->transaction = database_transaction_start(thread->finedb->database, YFALSE);
	if (thread->transaction == NULL)
		goto error;
	CONNECTION_SEND_OK(thread);
	return (YENOERR);
error:
	YLOG_ADD(YLOG_WARN, "START error");
	CONNECTION_SEND_ERROR(thread, RESP_ERR_TRANSACTION);
	return (YEACCESS);
}

/* Process a COMMIT command. */
yerr_t command_commit(tcp_thread_t *thread) {
	YLOG_ADD(YLOG_DEBUG, "COMMIT command");
	// check running transaction
	if (thread->transaction == NULL)
		goto error;
	// commit transaction
	if (database_transaction_commit(thread->transaction) != YENOERR)
		goto error;
	thread->transaction = NULL;
	CONNECTION_SEND_OK(thread);
	return (YENOERR);
error:
	YLOG_ADD(YLOG_WARN, "COMMIT error");
	CONNECTION_SEND_ERROR(thread, RESP_ERR_TRANSACTION);
	return (YEACCESS);
}

/* Process a ROLLBACK command. */
yerr_t command_rollback(tcp_thread_t *thread) {
	YLOG_ADD(YLOG_DEBUG, "ROLLBACK command");
	// check running transaction
	if (thread->transaction == NULL)
		goto error;
	// rollback transaction
	database_transaction_rollback(thread->transaction);
	thread->transaction = NULL;
	CONNECTION_SEND_OK(thread);
	return (YENOERR);
error:
	YLOG_ADD(YLOG_WARN, "ROLLBACK error");
	CONNECTION_SEND_ERROR(thread, RESP_ERR_TRANSACTION);
	return (YEACCESS);
}
