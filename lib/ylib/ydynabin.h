/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	ydynabin.h
 * @abstract	Manipulation of dynamic binary contents.
 * @discussion	This file contains the definition of structures and functions
 *		useful to manipulate dynamic binary data.
 * @version	1.0.0 Jun 22 2013
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YDYNABIN_H__
#define __YDYNABIN_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#include <stdio.h>
#include "ydefs.h"
#include "yerror.h"

/** @define YDYNABIN_RNDSZ Round a size number to 8KB bound. */
#define YDYNABIN_RNDSZ(s)	(s + (s % 8192))

/** @define YDYNABIN_SIZE Set the size of a new buffer. */
#define YDYNABIN_SIZE(s)	(!s ? 0 : \
				 (s < (100 * MB) ? (s * 2) : (s + (100 * MB))))

/*!
 * @struct      ydynabin_s
 *              Structure used for dynamic binary data transmission.
 * @field       data    Pointer to data itselves.
 * @field       len     Size of data.
 * @field	offset	Current reading offset.
 * @field	free	Free size in the buffer.
 */
struct ydynabin_s {
	void *data;
	size_t len;
	size_t offset;
	size_t free;
};

/*! @typedef ydynabin_t See struct ydynabin_s. */
typedef struct ydynabin_s ydynabin_t;

/*!
 * @function	ydynabin_new
 *		Fill a ydynabin_t structure. The structure to fill will be
 *		allocated, and the given data will be copied.
 * @param	data		Pointer to the initial data (will be copied).
 * @param	len		Data size.
 * @param	must_copy	YTRUE if the data must be copied.
 * @return	A pointer to the allocated ydynabin_t structure.
 */
ydynabin_t *ydynabin_new(void *data, size_t len, ybool_t must_copy);

/**
 * @function	ydynabin_delete
 *		Delete a ydynabin_t structure and its data.
 * @param	container	Pointer to the ydynabin_t structure.
 */
void ydynabin_delete(ydynabin_t *container);

/**
 * @function	ydynabin_expand
 *		Expends a ydynabin_t structure.
 * @param	container	Pointer to the ydynabin_t structure.
 * @param	data		Pointer to the data to add.
 * @param	len		Data size.
 * @return	YENOERR if OK.
 */
yerr_t ydynabin_expand(ydynabin_t *container, void *data, size_t len);

/**
 * @function	ydynabin_forward
 *		Forward the data pointer of a ydynabin_t structure.
 * @param	container	Pointer to the ydynabin_t structure.
 * @param	forward		Size of the forward.
 * @return	A pointer to the data before being forwarded.
 */
void *ydynabin_forward(ydynabin_t *container, size_t forward);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YDYNABIN_H__ */

