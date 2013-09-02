#ifndef __COMMAND_START_COMMIT_ROLLBACK_H__
#define __COMMAND_START_COMMIT_ROLLBACK_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_start
 *		Process a START command.
 * @param	thread		Pointer to the thread's structure.
 * @return	YENOERR if OK.
 */
yerr_t command_start(tcp_thread_t *thread);

/**
 * @function	command_commit
 *		Process a COMMIT command.
 * @param	thread		Pointer to the thread's structure.
 * @return	YENOERR if OK.
 */
yerr_t command_commit(tcp_thread_t *thread);

/**
 * @function	command_ROLLBACK
 *		Process a ROLLBACK command.
 * @param	thread		Pointer to the thread's structure.
 * @return	YENOERR if OK.
 */
yerr_t command_rollback(tcp_thread_t *thread);

#endif /* __COMMAND_START_COMMIT_ROLLBACK_H__ */
