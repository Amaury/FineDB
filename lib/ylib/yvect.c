#include <string.h>
#include <stdio.h>
#include "ydefs.h"
#include "yvect.h"

/*
** yv_new()
** Create a new yvector of the default size (4K).
*/
yvect_t yv_new() {
	return (yv_create(YVECT_SIZE_DEFAULT));
}

/*
 * yv_create()
 * Creates a new yvector of the given size.
 */
yvect_t yv_create(yv_size_t size) {
	void		**nv;
	yvect_head_t	*y;

	if (!(nv = (void**)YMALLOC((size * sizeof(void*)) +
				   sizeof(yvect_head_t))))
		return (NULL);
	y = (yvect_head_t*)nv;
	nv = (void**)((size_t)nv + sizeof(yvect_head_t));
	y->total = size;
	y->used = 0;
	*nv = NULL;
	return ((yvect_t)nv);
}

/*
** yv_del()
** Delete an existing yvector.
*/
void yv_del(yvect_t *v, void (*f)(void*, void*), void *data)
{
  yvect_head_t *y;
  unsigned int i;

  if (!v || !*v)
    return ;
  y = (yvect_head_t*)((size_t)*v - sizeof(yvect_head_t));
  if (f)
    for (i = 0; i < y->used; ++i)
      f((*v)[i], data);
  YFREE(y);
  *v = NULL;
}

/*
** yv_trunc()
** Truncate an existing yvector. The allocated memory doesn't change.
*/
void yv_trunc(yvect_t v, void (*f)(void*, void*), void *data)
{
  yvect_head_t *y;
  unsigned int i;

  if (!v || !*v)
    return ;
  y = (yvect_head_t*)((size_t)*v - sizeof(yvect_head_t));
  if (f)
    for (i = 0; i < y->used; ++i)
      f(v[i], data);
  y->used = 0;
  *v = NULL;
}

/*
** yv_setsz()
** Set the minimum size of a yvector.
*/
int yv_setsz(yvect_t *v, size_t sz)
{
  yvect_head_t *y, *ny;
  unsigned int totalsz, leap;
  void **nv;

  if (!v || !*v)
    return (0);
  y = (yvect_head_t*)((size_t)**v - sizeof(yvect_head_t));
  if (sz < y->total)
    return (1);
  leap = (sz < YVECT_SIZE_BIG) ? YVECT_SIZE_DEFAULT :
    (sz < YVECT_SIZE_HUGE) ? YVECT_SIZE_BIG : YVECT_SIZE_HUGE;
  totalsz = (((sz + 1) / leap) + 1) * leap;
  if (!(nv = (void**)YMALLOC((totalsz * sizeof(void*)) +
			     sizeof(yvect_head_t))))
    return (0);
  ny = (yvect_head_t*)nv;
  nv = (void**)((size_t)nv + sizeof(yvect_head_t));
  ny->total = totalsz;
  ny->used = y->used;
  memcpy(nv, **v, (y->used + 1) * sizeof(void*));
  YFREE(y);
  *v = nv;
  return (1);
}

/*
** yv_len()
** Return the length of a yvector (its used size).
*/
size_t yv_len(yvect_t v)
{
  if (!v)
    return (0);
  return (((yvect_head_t*)((size_t)v - sizeof(yvect_head_t)))->used);
}

/*
** yv_cat()
** Concatenate a yvector at the end of another one.
*/
int yv_cat(yvect_t *dest, yvect_t src)
{
  unsigned int srcsz, vectsz, totalsz, leap;
  yvect_head_t *y, *ny;
  void **nv;

  if (!src || !dest || !*dest || !(srcsz = yv_len(src)))
    return (1);
  y = (yvect_head_t*)((size_t)*dest - sizeof(yvect_head_t));
  if ((y->used + 1 + srcsz) <= y->total)
    {
      memcpy(*dest + y->used, src, (srcsz + 1) * sizeof(void*));
      y->used += srcsz;
      return (1);
    }
  vectsz = y->used + srcsz;
  leap = (vectsz < YVECT_SIZE_BIG) ? YVECT_SIZE_DEFAULT :
    (vectsz < YVECT_SIZE_HUGE) ? YVECT_SIZE_BIG : YVECT_SIZE_HUGE;
  totalsz = (((vectsz + 1) / leap) + 1) * leap;
  if (!(nv = (void**)YMALLOC((totalsz * sizeof(void*)) +
			     sizeof(yvect_head_t))))
    return (0);
  ny = (yvect_head_t*)nv;
  nv = (void**)((size_t)nv + sizeof(yvect_head_t));
  ny->total = totalsz;
  ny->used = vectsz;
  memcpy(nv, *dest, y->used * sizeof(void*));
  memcpy(nv + y->used, src, (srcsz + 1) * sizeof(void*));
  YFREE(y);
  *dest = nv;
  return (1);
}

/*
** yv_ncat()
** Concatenate a given number of elements from a yvector
** at the end of another.
*/
int yv_ncat(yvect_t *dest, yvect_t src, unsigned int n)
{
  unsigned int vectsz, totalsz, leap;
  yvect_head_t *y, *ny;
  void **nv;

  if (!src || !dest || !*dest || !n)
    return (1);
  y = (yvect_head_t*)((size_t)*dest - sizeof(yvect_head_t));
  if ((y->used + 1 + n) <= y->total)
    {
      memcpy(*dest + y->used, src, n * sizeof(void*));
      y->used += n;
      (*dest)[y->used] = NULL;
      return (1);
    }
  vectsz = y->used + n;
  leap = (vectsz < YVECT_SIZE_BIG) ? YVECT_SIZE_DEFAULT :
    (vectsz < YVECT_SIZE_HUGE) ? YVECT_SIZE_BIG : YVECT_SIZE_HUGE;
  totalsz = (((vectsz + 1) / leap) + 1) * leap;
  if (!(nv = (void**)YMALLOC((totalsz * sizeof(void*)) +
			     sizeof(yvect_head_t))))
    return (0);
  ny = (yvect_head_t*)nv;
  nv = (void**)((size_t)nv + sizeof(yvect_head_t));
  ny->total = totalsz;
  ny->used = vectsz;
  memcpy(nv, *dest, y->used * sizeof(void*));
  memcpy(nv + y->used, src, n * sizeof(void*));
  nv[ny->used] = NULL;
  YFREE(y);
  *dest = nv;
  return (1);
}

/*
** yv_dup()
** Duplicate a yvector.
*/
yvect_t yv_dup(yvect_t v)
{
  yvect_head_t *y, *ny;
  void **nv;

  if (!v)
    return (NULL);
  y = (yvect_head_t*)((size_t)v - sizeof(yvect_head_t));
  if (!(nv = (void**)YMALLOC((y->total * sizeof(void*)) +
			     sizeof(yvect_head_t))))
    return (NULL);
  ny = (yvect_head_t*)nv;
  nv = (void**)((size_t)nv + sizeof(yvect_head_t));
  ny->total = y->total;
  ny->used = y->used;
  memcpy(nv, v, (y->used + 1) * sizeof(void*));
  return (nv);
}

/*
** yv_concat()
** Concatenate 2 yvectors to create a new one.
*/
yvect_t yv_concat(yvect_t v1, yvect_t v2)
{
  void **nv;

  nv = yv_dup(v1);
  yv_cat(&nv, v2);
  return (nv);
}

/*
** yv_put()
** Add an element at the beginning of a yvector.
*/
int yv_put(yvect_t *v, void *e)
{
  unsigned int vectsz, totalsz, leap;
  int i;
  yvect_head_t *y, *ny;
  void **nv;

  if (!v || !*v)
    return (0);
  y = (yvect_head_t*)((size_t)*v - sizeof(yvect_head_t));
  if ((y->used + 2) <= y->total)
    {
      for (i = y->used; i >= 0; --i)
	(*v)[i + 1] = (*v)[i];
      (*v)[0] = e;
      y->used++;
      return (1);
    }
  vectsz = y->used + 1;
  leap = (vectsz < YVECT_SIZE_BIG) ? YVECT_SIZE_DEFAULT :
    (vectsz < YVECT_SIZE_HUGE) ? YVECT_SIZE_BIG : YVECT_SIZE_HUGE;
  totalsz = (((vectsz + 1) / leap) + 1) * leap;
  if (!(nv = (void**)YMALLOC((totalsz * sizeof(void*)) +
			     sizeof(yvect_head_t))))
    return (0);
  ny = (yvect_head_t*)nv;
  nv = (void**)((size_t)nv + sizeof(yvect_head_t));
  ny->total = totalsz;
  ny->used = vectsz;
  nv[0] = e;
  memcpy((void*)((size_t)nv + sizeof(void*)), *v, (y->used + 1) * sizeof(void*));
  YFREE(y);
  *v = nv;
  return (1);
}

/*
** yv_ins()
** Insert an element at the given offset of a yvector. All elements
** placed at this offset and after are shifted.
*/
int yv_ins(yvect_t *v, void *e, size_t i)
{
  yvect_head_t *y;
  size_t j;

  if (!v || !*v)
    return (0);
  y = (yvect_head_t*)((size_t)*v - sizeof(yvect_head_t));
  if (!v || !*v || !yv_setsz(v, y->used + 2))
    return (0);
  if (i > y->used)
    return (0);
  for (j = i; j < (y->used + 1); ++j)
    (*v)[j + 1] = (*v)[j];
  (*v)[i] = e;
  y->used++;
  return (1);
}

/*
** yv_add()
** Add an element at the end of a yvector.
*/
int yv_add(yvect_t *v, void *e)
{
  unsigned int vectsz, totalsz, leap;
  yvect_head_t *y, *ny;
  void **nv;

  if (!v || !*v)
    return (0);
  y = (yvect_head_t*)((size_t)*v - sizeof(yvect_head_t));
  if ((y->used + 2) <= y->total)
    {
      (*v)[y->used] = e;
      (*v)[y->used + 1] = NULL;
      y->used++;
      return (1);
    }
  vectsz = y->used + 1;
  leap = (vectsz < YVECT_SIZE_BIG) ? YVECT_SIZE_DEFAULT :
    (vectsz < YVECT_SIZE_HUGE) ? YVECT_SIZE_BIG : YVECT_SIZE_HUGE;
  totalsz = (((vectsz + 1) / leap) + 1) * leap;
  if (!(nv = (void**)YMALLOC((totalsz * sizeof(void*)) +
			     sizeof(yvect_head_t))))
    return (0);
  ny = (yvect_head_t*)nv;
  nv = (void**)((size_t)nv + sizeof(yvect_head_t));
  ny->total = totalsz;
  ny->used = vectsz;
  memcpy(nv, *v, y->used * sizeof(void*));
  nv[y->used] = e;
  nv[ny->used] = NULL;
  YFREE(y);
  *v = nv;
  return (1);
}

/*
** yv_pop()
** Remove the first element of a yvector and return it.
*/
void *yv_pop(yvect_t v)
{
  yvect_head_t *y;
  void *res;
  unsigned int i;

  if (!v)
    return (NULL);
  y = (yvect_head_t*)((size_t)v - sizeof(yvect_head_t));
  if (!y->used)
    return (NULL);
  res = *v;
  for (i = 0; i < y->used; ++i)
    v[i] = v[i + 1];
  y->used--;
  return (res);
}

/*
** yv_get()
** Remove the last element of a yvector and return it.
*/
void *yv_get(yvect_t v)
{
  yvect_head_t *y;
  void *res;

  if (!v)
    return (NULL);
  y = (yvect_head_t*)((size_t)v - sizeof(yvect_head_t));
  if (!y->used)
    return (NULL);
  res = v[y->used - 1];
  v[y->used - 1] = NULL;
  y->used--;
  return (res);
}

/*
** yv_ext()
** Extract the element placed at the given offset of a yvector.
*/
void *yv_ext(yvect_t v, size_t i)
{
  yvect_head_t *y;
  void *res;
  size_t j;

  if (!v)
    return (NULL);
  y = (yvect_head_t*)((size_t)v - sizeof(yvect_head_t));
  if (i >= y->used)
    return (NULL);
  res = v[i];
  for (j = i; j < y->used; ++j)
    v[j] = v[j + 1];
  y->used--;
  return (res);
}

/*
** yv_uniq()
** Remove all values of a yvector to let only one entry of each value.
*/
void yv_uniq(yvect_t v)
{
  yvect_head_t *y;
  size_t i, j, k;

  if (!v)
    return ;
  y = (yvect_head_t*)((size_t)v - sizeof(yvect_head_t));
  for (i = 0; i < y->used; ++i)
    {
      for (j = i + 1; j < y->used; ++j)
        {
          if (v[i] == v[j])
            {
              for (k = j + 1; k < (y->used - 1); ++k)
                v[k] = v[k + 1];
              y->used--;
            }
        }
    }
}

/*
** yv_sort()
** Do a quick sort of all elements of a yvector.
*/
void yv_sort(yvect_t v, int (*f)(const void*, const void*))
{
  yvect_head_t *y;

  if (!v)
    return ;
  y = (yvect_head_t*)((size_t)v - sizeof(yvect_head_t));
  qsort(v, y->used, sizeof(void*), f);
}

/*
** yv_search()
** Search the offset of an element in a yvector.
** WARNING: the yvector must be sorted (using yv_sort()),
** because this function use dichotomy.
*/
int yv_search(yvect_t v, void *e, int (*f)(const void*, const void*))
{
  yvect_head_t *y;
  int o_start, o_end, o_pivot;
  int cmp_res;

  if (!v || !f)
    return (-1);
  y = (yvect_head_t*)((size_t)v - sizeof(yvect_head_t));
  o_start = 0;
  o_end = y->used - 1;
  for (; ; )
    {
      o_pivot = (o_end + o_start) / 2;
      if (!(cmp_res = f(e, v[o_pivot])))
	return (o_pivot);
      if (cmp_res < 0)
	{
	  if (o_pivot == (o_start + 1))
	    {
	      if (!f(e, v[o_start]))
		return (o_start);
	      else
		return (-1);
	    }
	  o_end = o_pivot;
	}
      else
	{
	  if (o_pivot == (o_end - 1))
	    {
	      if (!f(e, v[o_end]))
		return (o_end);
	      else
		return (-1);
	    }
	  o_start = o_pivot;
	}
    }
}
