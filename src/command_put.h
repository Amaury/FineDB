#ifndef __COMMAND_PUT_H__
#define __COMMAND_PUT_H__

#include "yerror.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_put
 *		Process a PUT command.
 * @param	thread	Pointer to the thread's structure.
 * @param	buff	First data buffer.
 * @param	buffsz	Data buffer's size.
 * @return	YENOERR if OK.
 */
yerr_t command_put(tcp_thread_t *thread, char *buff, size_t buffsz);

#endif /* __COMMAND_PUT_H__ */
