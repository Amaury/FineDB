#ifndef __WRITER_THREAD_H__
#define __WRITER_THREAD_H__

/**
 * typedef	writer_action_t
 *		Type of writer's action.
 * @const	WRITE_PUT	Add or update a key in database.
 * @const	WRITE_DEL	Remove a key from database.
 */
typedef enum writer_action_e {
	WRITE_PUT = 0,
	WRITE_DEL
} writer_action_t;

/**
 * @typedef	writer_msg_t
 *		Structure used to transfer data to the writer thread.
 * @field	type		Type of action (WRITE_PUT, WRITE_DEL).
 * @field	name		Pointer to the name.
 * @field	name_len	Length of the name.
 * @field	data		Pointer to the data.
 * @field	data_len	Length of the data.
 */
typedef struct writer_msg_s {
	writer_action_t type;
	void *name;
	size_t name_len;
	void *data;
	size_t data_len;
} writer_msg_t;

/**
 * @function	writer_loop
 *		Callback function executed by the writer thread.
 * @param	param	Pointer to the main FineDB structure.
 * @return	Always NULL.
 */
void *writer_loop(void *param);

#endif /* __WRITER_THREAD_H__ */
