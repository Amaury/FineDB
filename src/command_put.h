#ifndef __COMMAND_PUT_H__
#define __COMMAND_PUT_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_put
 *		Process a PUT command.
 * @param	thread	Pointer to the thread's structure.
 * @param	buff	Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_put(tcp_thread_t *thread, ydynabin_t *buff);

#endif /* __COMMAND_PUT_H__ */
