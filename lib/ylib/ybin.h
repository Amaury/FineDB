/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	ybin.h
 * @abstract	Manipulation of binary contents.
 * @discussion	This file contains the definition of structures and functions
 *		useful to manipulate binary data.
 * @version	1.0.0 Jun 15 2013
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YBIN_H__
#define __YBIN_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#include <stdio.h>
#include "ydefs.h"

/*!
 * @struct      ybin_s
 *              Structure used for binary data transmission.
 * @field       data    Pointer to data itselves.
 * @field       len     Size of data.
 */
struct ybin_s {
	void *data;
	size_t len;
};

/*! @typedef ybin_t See struct ybin_s. */
typedef struct ybin_s ybin_t;

/*!
 * @function	ybin_set
 *		Fill a ybin_t structure. The structure to fill could be given,
 *		otherwise it will be allocated.
 * @param	container	Pointer to an existing ybin_t structure. If the
 *				parameter is NULL, a new structure will be
 *				allocated.
 * @param	data		Pointer to the data.
 * @param	len		Data size.
 * @return	A pointer to the allocated ybin_t structure, or to the one given.
 */
ybin_t *ybin_set(ybin_t *container, void *data, size_t len);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YBIN_H__ */

