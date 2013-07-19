#ifndef __COMMAND_LIST_H__
#define __COMMAND_LIST_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_list
 *		Process a LIST command.
 * @param	thread		Pointer to the thread's structure.
 * @param	has_dbname	YTRUE if the request contains a database name.
 * @param	compress	YTRUE if the returned data could be compressed.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_list(tcp_thread_t *thread, ybool_t has_dbname, ybool_t compress, ydynabin_t *buff);

#endif /* __COMMAND_LIST_H__ */
