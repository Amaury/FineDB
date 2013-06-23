#include "ybin.h"

/* Fill a ybin_t structure. */
ybin_t *ybin_set(ybin_t *container, void *data, size_t len) {
	if (container == NULL)
		container = YMALLOC(sizeof(ybin_t));
	container->data = data;
	container->len = len;
	return (container);
}

