#ifndef __WRITER_THREAD_H__
#define __WRITER_THREAD_H__

#include "ybin.h"

/**
 * typedef	writer_action_t
 *		Type of writer's action.
 * @const	WRITE_PUT	Add or update a key in database.
 * @const	WRITE_DEL	Remove a key from database.
 * @const	WRITE_DROP	Remove a database and its keys.
 */
typedef enum writer_action_e {
	WRITE_PUT = 0,
	WRITE_DEL,
	WRITE_DROP
} writer_action_t;

/**
 * @typedef	writer_msg_t
 *		Structure used to transfer data to the writer thread.
 * @field	type		Type of action (WRITE_PUT, WRITE_DEL).
 * @field	dbname		Name of the database. NULL by default.
 * @field	name		Key.
 * @field	data		Data.
 * @field	create_only	YTRUE if the key must not exist already.
 */
typedef struct writer_msg_s {
	writer_action_t type;
	char *dbname;
	ybin_t name;
	ybin_t data;
	ybool_t create_only;
} writer_msg_t;

/**
 * @function	writer_loop
 *		Callback function executed by the writer thread.
 * @param	param	Pointer to the main FineDB structure.
 * @return	Always NULL.
 */
void *writer_loop(void *param);

#endif /* __WRITER_THREAD_H__ */
