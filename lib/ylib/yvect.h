/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	yvect.h
 * @abstract	All definitions about yvectors.
 * @discussion	Vectors are bufferised arrays of pointers.
 * @version	1.0 Jun 13 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YVECT_H__
#define __YVECT_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#include <stdlib.h>

/* ************** TYPE DEFINITIONS ************* */

/*!
 * @typdef	yv_size_t
 *		Enum used to define the size of a yvector.
 * @constant	YVECT_SIZE_NANO		Size of ultra-light yvectors (4).
 * @constant	YVECT_SIZE_MINI		Minimal size of yvectors (32).
 * @constant	YVECT_SIZE_MEDIUM	Medium size of yvectors (256).
 * @constant	YVECT_SIZE_DEFAULT	Default size of yvectors (4K).
 * @constant	YVECT_SIZE_BIG		Size of big yvectors (65K).
 * @constant	YVECT_SIZE_HUGE		Size of huge yvectors (1M).
 */
typedef enum yv_size_e {
	YVECT_SIZE_NANO		= 4,
	YVECT_SIZE_MINI		= 32,
	YVECT_SIZE_MEDIUM	= 256,
	YVECT_SIZE_DEFAULT	= 4096,
	YVECT_SIZE_BIG		= 65536,
	YVECT_SIZE_HUGE		= 1048576
} yv_size_t;

/*!
 * @struct	yvect_head_s
 *		Structure used for the head of yvectors.
 * @field	total	Total size of the yvector.
 * @field	used	Used size of the yvector.
 */
struct yvect_head_s
{
  size_t total;
  size_t used;
};

/*! @typedef yvect_head_t See yvect_head_s structure. */
typedef struct yvect_head_s yvect_head_t;

/*! @typedef yvect_t Vector type definition. Always equivalent to (void**). */
typedef void** yvect_t;

/* *********************** FUNCTIONS ******************* */

/*!
 * @function	yv_new
 *		Create a new yvector of the default size (4K).
 * @return	The created vector.
 */
yvect_t yv_new(void);

/*!
 * @function	yv_create
 *		Creates a new yvector of the given size.
 * @param	size	Size of the new yvector.
 * @return	The created yvector.
 */
yvect_t yv_create(yv_size_t size);

/*!
 * @function	yv_del
 *		Delete an existing yvector.
 * @param	v	A pointer to the yvector.
 * @param	f	Pointer to a function that is called to delete each
 *			element of the vector. Could be NULL ; otherwise, must
 *			have this prototype : void f(void *elem, void *data);
 * @param	data	Pointer to data that could be given to the delete
 *			callback (see previous parameter). Could be NULL.
 */
void yv_del(yvect_t *v, void (*f)(void*, void*), void *data);

/*!
 * @function	yv_trunc
 *		Truncate an existing yvector. The allocated memory doesn't
 *		change.
 * @param	v	The yvector.
 * @param	f	Pointer to a function that is called to delete each
 *			element of the vector. Could be NULL ; otherwise, must
 *			have this prototype : void f(void *elem, void *data);
 * @param	data	Pointer to data that could be given to the delete
 *			callback (see previous parameter). Could be NULL.
 */
void yv_trunc(yvect_t v, void (*f)(void*, void*), void *data);

/*!
 * @function	yv_setsz
 *		Set the minimum size of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	sz	The minimum size for this yvector.
 * @return	0 if an error occurs, 1 otherwise.
 */
int yv_setsz(yvect_t *v, size_t sz);

/*!
 * @function	yv_len
 *		Return the length of a yvector (its used size).
 * @param	v	The yvector.
 * @return	The yvector's length.
 */
size_t yv_len(yvect_t v);

/*!
 * @function	yv_cat
 *		Concatenate a yvector at the end of another one.
 * @param	dest	A pointer to the yvector to extend.
 * @param	src	A yvector.
 * @return	0 if an error occurs, 1 otherwise.
 */
int yv_cat(yvect_t *dest, yvect_t src);

/*!
 * @function	yv_ncat
 *		Concatenate a given number of elements from a yvector
 *		at the end of another.
 * @param	dest	A pointer to the yvector to extend.
 * @param	src	A yvector.
 * @param	n	The number of elements to copy.
 * @return	0 if an error occurs, 1 otherwise.
 */
int yv_ncat(yvect_t *dest, yvect_t src, unsigned int n);

/*!
 * @function	yv_dup
 *		Duplicate a yvector.
 * @param	v	The yvector.
 * @result	The new yvector.
 */
yvect_t yv_dup(yvect_t v);

/*!
 * @function	yv_concat
 *		Concatenate 2 yvectors to create a new one.
 * @param	v1	The first yvector.
 * @param	v2	The second yvector.
 * @return	The new ystring.
 */
yvect_t yv_concat(yvect_t v1, yvect_t v2);

/*!
 * @function	yv_put
 *		Add an element at the beginning of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @return	0 if an error occurs, 1 otherwise.
 */
int yv_put(yvect_t *v, void *e);

/*!
 * @function	yv_add
 *		Add an element at the end of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @return	0 if an error occurs, 1 otherwise.
 */
int yv_add(yvect_t *v, void *e);

/*!
 * @function	yv_ins
 *		Insert an element at the given offset of a yvector. All
 *		elements placed at this offset and after are shifted to the
 *		right.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @param	i	Offset of the element in the vector. Must be less or
 *			equal to the vector's used size.
 * @return	0 if an error occurs, 1 otherwise.
 */
int yv_ins(yvect_t *v, void *e, size_t i);

/*!
 * @function	yv_pop
 *		Remove the first element of a yvector and return it.
 * @param	v	The yvector.
 * @return	A pointer to the removed element.
 */
void *yv_pop(yvect_t v);

/*!
 * @function	yv_get
 *		Remove the last element of a yvector and return it.
 * @param	v	The yvector.
 * @return	A pointer to the removed element.
 */
void *yv_get(yvect_t v);

/*!
 * @function	yv_ext
 *		Extract the element placed at the given offset of a yvector.
 *		All elements placed after the offset are shifted to the left.
 * @param	v	The yvector.
 * @param	i	Offset of the element in the vector. Must be less than
 *			the vector's used size.
 * @return	A pointer to the removed element.
 */
void *yv_ext(yvect_t v, size_t i);

/*!
 * @function	yv_uniq
 *		Remove all values of a yvector to let only one entry of
 *		each value.
 * @param	v	The yvector.
 */
void yv_uniq(yvect_t v);

/*!
 * @function	yv_sort
 *		Do a quick sort of all elements of a yvector. See qsort(3).
 * @param	v	The yvector.
 * @param	f	A pointer to the function used to compare elements.
 */
void yv_sort(yvect_t v, int (*f)(const void*, const void*));

/*!
 * @function	yv_search
 *		Search the offset of an element in a yvector. WARNING: the
 *		yvector must be sorted (using yv_sort()) because this
 *		function uses dichotomy.
 * @param	v	The yvector.
 * @param	e	The element to compare.
 * @param	f	A pointer to the function used to compare elements.
 * @return	The offset of the elment in the yvector, or (-1) if the
 *		element doesn't be finded.
 */
int yv_search(yvect_t v, void *e, int (*f)(const void*, const void*));

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YVECT_H__ */
