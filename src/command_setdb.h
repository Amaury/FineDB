#ifndef __COMMAND_SETDB_H__
#define __COMMAND_SETDB_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_setdb
 *		Process a SETDB command.
 * @param	thread		Pointer to the thread's structure.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_setdb(tcp_thread_t *thread, ydynabin_t *buff);

#endif /* __COMMAND_SETDB_H__ */
