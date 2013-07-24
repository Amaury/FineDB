#ifndef __COMMAND_DROP_H__
#define __COMMAND_DROP_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_drop
 *		Process a DROP command.
 * @param	thread		Pointer to the thread's structure.
 * @param	sync		YTRUE if the response must be synchronized.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_drop(tcp_thread_t *thread, ybool_t sync, ydynabin_t *buff);

#endif /* __COMMAND_DROP_H__ */
