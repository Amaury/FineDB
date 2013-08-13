#ifndef __COMMAND_PING_H__
#define __COMMAND_PING_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_ping
 *		Process a PING command.
 * @param	thread		Pointer to the thread's structure.
 * @return	YENOERR if OK.
 */
yerr_t command_ping(tcp_thread_t *thread);

#endif /* __COMMAND_GET_H__ */
