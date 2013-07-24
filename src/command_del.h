#ifndef __COMMAND_DEL_H__
#define __COMMAND_DEL_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_del
 *		Process a DEL command.
 * @param	thread	Pointer to the thread's structure.
 * @param	sync	YTRUE if the answer must be synchronized.
 * @param	buff	Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_del(tcp_thread_t *thread, ybool_t sync, ydynabin_t *buff);

#endif /* __COMMAND_DEL_H__ */
