#ifndef __WRITER_THREAD_H__
#define __WRITER_THREAD_H__

/**
 * @typedef	writer_msg_t
 *		Structure used to transfer data to the writer thread.
 * @field	name		Pointer to the name.
 * @field	name_len	Length of the name.
 * @field	data		Pointer to the data.
 * @field	data_len	Length od the data.
 */
typedef struct writer_msg_s {
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
