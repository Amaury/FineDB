#include "ydefs.h"
#include "ylist.h"

/* Creates a new list. */
ylist_t *ylist_new() {
	ylist_t *list;

	list = (ylist_t*)YMALLOC(sizeof(ylist_t));
	return (list);
}

/* Destroy a list and all its elements. */
void ylist_delete(ylist_t *list, void (*delete_function)(void*, void*),
		  void *delete_data) {
	ylist_elem_t *elem, *elem_ptr;

	elem = list->first;
	while (elem) {
		if (delete_function)
			delete_function(elem->data, delete_data);
		elem_ptr = elem;
		elem = elem->next;
		YFREE(elem_ptr);
	}
	YFREE(list);
}

/* Add an element at the end of a list. */
ylist_elem_t *ylist_add(ylist_t *list, void *data) {
	ylist_elem_t *elem;

	elem = YMALLOC(sizeof(ylist_elem_t));
	elem->data = data;
	elem->prev = list->last;
	elem->list = list;
	if (list->last)
		list->last->next = elem;
	else
		list->last = list->first = elem;
	list->last = elem;
	return (elem);
}

/* Add an element at the beginnin of a list. */
ylist_elem_t *ylist_push(ylist_t *list, void *data) {
	ylist_elem_t *elem;

	elem = YMALLOC(sizeof(ylist_elem_t));
	elem->data = data;
	elem->next = list->first;
	elem->list = list;
	if (list->first)
		list->first->prev = elem;
	else
		list->first = list->last = elem;
	list->first = elem;
	return (elem);
}

/* Remove the last element of a list. */
void *ylist_remove(ylist_t *list) {
	ylist_elem_t *elem;
	void *data;

	if (list->last == NULL)
		return (NULL);
	elem = list->last;
	list->last = elem->prev;
	if (elem->prev)
		elem->prev->next = NULL;
	else
		list->first = NULL;
	data = elem->data;
	YFREE(elem);
	return (data);
}

/** Remove the first element of a list. */
void *ylist_pop(ylist_t *list) {
	ylist_elem_t *elem;
	void *data;

	if (list->first == NULL)
		return (NULL);
	elem = list->first;
	list->first = elem->next;
	if (elem->next)
		elem->next->prev = NULL;
	else
		list->last = NULL;
	data = elem->data;
	YFREE(elem);
	return (data);
}

/* Extract a list element from one list, and add it at the end of another list. */
void ylist_swap(ylist_elem_t *elem, ylist_t *dest) {
	ylist_t *src;

	// extract from source list
	src = elem->list;
	if (src->first == elem)
		src->first = elem->next;
	else if (src->last == elem)
		src->last = elem->prev;
	if (elem->next)
		elem->next->prev = elem->prev;
	if (elem->prev)
		elem->prev->next = elem->next;
	// add to destination list
	elem->list = dest;
	if (dest->last) {
		elem->prev = dest->last;
		dest->last->next = elem;
	} else {
		elem->prev = elem->next = NULL;
		dest->first = dest->last = elem;
	}
}
