#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "yerror.h"
#include "ydynabin.h"
#include "finedb.h"
#include "connection_thread.h"

/**
 * @typedef	command_handler_t
 *		Function pointer used for command handlers.
 * @param	thread		Pointer to the current thread structure.
 * @param	sync		YTRUE if the request is synchronous.
 * @param	compress	YTRUE for compression.
 * @param	serialized	YTRUE if data is serialized.
 * @param	buff		Pointer to the request's dynamic buffer.
 * @return	YENOERR if OK.
 */
typedef yerr_t (*command_handler_t)(tcp_thread_t *thread, ybool_t sync,
                                    ybool_t compress, ybool_t serialized,
                                    ydynabin_t *buff);

/**
 * @function	command_del
 *		Process a DEL command.
 * @param	thread	Pointer to the thread's structure.
 * @param	sync	YTRUE if the answer must be synchronized.
 * @param	buff	Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_del(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                   ydynabin_t *buff);

/**
 * @function	command_drop
 *		Process a DROP command.
 * @param	thread		Pointer to the thread's structure.
 * @param	sync		YTRUE if the response must be synchronized.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_drop(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                    ydynabin_t *buff);

/**
 * @function	command_get
 *		Process a GET command.
 * @param	thread		Pointer to the thread's structure.
 * @param	compress	YTRUE if the returned data could be compressed.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_get(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                   ydynabin_t *buff);

/**
 * @function	command_list
 *		Process a LIST command.
 * @param	thread		Pointer to the thread's structure.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_list(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                    ydynabin_t *buff);

/**
 * @function	command_ping
 *		Process a PING command.
 * @param	thread		Pointer to the thread's structure.
 * @return	YENOERR if OK.
 */
yerr_t command_ping(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                    ydynabin_t *buff);

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
yerr_t command_put(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                   ydynabin_t *buff);

/**
 * @function	command_setdb
 *		Process a SETDB command.
 * @param	thread		Pointer to the thread's structure.
 * @param	buff		Pointer to the dynamic buffer.
 * @return	YENOERR if OK.
 */
yerr_t command_setdb(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                     ydynabin_t *buff);

/**
 * @function	command_start
 *		Process a START command.
 * @param	thread		Pointer to the thread's structure.
 * @return	YENOERR if OK.
 */
yerr_t command_start(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                     ydynabin_t *buff);

/**
 * @function	command_stop
 *		Process a STOP command.
 * @param	thread		Pointer to the thread's structure.
 * @return	YENOERR if OK.
 */
yerr_t command_stop(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized,
                    ydynabin_t *buff);

#endif /* __COMMAND_H__ */
