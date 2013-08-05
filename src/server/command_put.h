#ifndef __COMMAND_PUT_H__
#define __COMMAND_PUT_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @function	command_put
 *		Process a PUT command.
 * @param	thread		Pointer to the thread's structure.
 * @param	sync		YTRUE if the response must be synchronized.
 * @param	compress	YTRUE if the given data is already compressed.
 * @param	create_only	YTRUE if the key must not exist already.
 * @param	update_only	YTRUE if the key must exist already.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_put(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t create_only, ybool_t update_only, ydynabin_t *buff);

#endif /* __COMMAND_PUT_H__ */
