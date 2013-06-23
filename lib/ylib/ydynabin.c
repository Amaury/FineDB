#include <string.h>
#include "ydynabin.h"

/* Create and fill a ydynbin_t structure. */
ydynabin_t *ydynabin_new(void *data, size_t len, ybool_t must_copy) {
	ydynabin_t *container;

	container = YMALLOC(sizeof(ydynabin_t));
	if (container == NULL)
		return (NULL);
	if (data == NULL || len == 0) {
		container->data = NULL;
		container->len = 0;
	} else if (must_copy) {
		container->data = YMALLOC(len);
		memcpy(container->data, data, len);
	} else
		container->data = data;
	container->len = len;
	container->offset = 0;
	return (container);
}

/* Delete a ydynabin_t structure and its data. */
void ydynabin_delete(ydynabin_t *container) {
	void *ptr;

	if (container->data) {
		ptr = (void*)((size_t)container->data - container->offset);
		YFREE(ptr);
	}
	YFREE(container);
}

/* Expand a ydynabin_t structure. */
yerr_t ydynabin_expand(ydynabin_t *container, void *data, size_t len) {
	void *ptr;

	if ((ptr = YMALLOC(container->len + len)) == NULL)
		return (YENOMEM);
	if (container->len)
		memcpy(ptr, container->data, container->len);
	memcpy((void*)((size_t)ptr + container->len), data, len);
	YFREE(container->data);
	container->data = ptr;
	container->len += len;
	container->offset = 0;
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
