#ifndef __COMMAND_GET_H__
#define __COMMAND_GET_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_get
 *		Process a GET command.
 * @param	thread		Pointer to the thread's structure.
 * @param	compress	YTRUE if the returned data could be compressed.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_get(tcp_thread_t *thread, ybool_t compress, ydynabin_t *buff);

#endif /* __COMMAND_GET_H__ */
