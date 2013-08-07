#include <string.h>
#include "ydynabin.h"

/* Create and fill a ydynbin_t structure. */
ydynabin_t *ydynabin_new(void *data, size_t len, ybool_t must_copy) {
	ydynabin_t *container;
	size_t sz;

	container = YMALLOC(sizeof(ydynabin_t));
	if (container == NULL)
		return (NULL);
	if (data == NULL || len == 0)
		return (container);
	if (must_copy) {
		sz = YDYNABIN_SIZE(len);
		sz = YDYNABIN_RNDSZ(sz);
		container->data = YMALLOC(sz);
		memcpy(container->data, data, len);
		container->free = sz - len;
	} else
		container->data = data;
	container->len = len;
	return (container);
}

/* Delete a ydynabin_t structure and its data. */
void ydynabin_delete(ydynabin_t *container) {
	void *ptr;

	if (container == NULL)
		return;
	if (container->data) {
		ptr = (void*)((size_t)container->data - container->offset);
		YFREE(ptr);
	}
	YFREE(container);
}

/* Expand a ydynabin_t structure. */
yerr_t ydynabin_expand(ydynabin_t *container, void *data, size_t len) {
	size_t sz;
	void *ptr, *pt;

	if (!data || !len)
		return (YENOERR);
	if (container->free < len) {
		// there is not enough room in the buffer
		// create a larger buffer
		sz = container->len + len;
		sz = YDYNABIN_SIZE(len);
		sz = YDYNABIN_RNDSZ(sz);
		if ((ptr = YMALLOC(sz)) == NULL)
			return (YENOMEM);
		// copy the old data
		if (container->len)
			memcpy(ptr, container->data, container->len);
		// free the old buffer
		pt = (void*)((size_t)container->data - container->offset);
		YFREE(pt);
		// update the container
		container->data = ptr;
		container->free = sz - container->len;
		container->offset = 0;
	}
	// copy new data in the buffer
	pt = (void*)((size_t)container->data + container->len);
	memcpy(pt, data, len);
	// update the container
	container->len += len;
	container->free -= len;
	return (YENOERR);
}

/* Forward the data pointer of a ydynabin_t structure. */
void *ydynabin_forward(ydynabin_t *container, size_t forward) {
	void *orig = container->data;

	if (container->len < forward)
		return (NULL);
	container->data = (void*)((size_t)container->data + forward);
	container->len -= forward;
	container->offset += forward;
	return (orig);
}
