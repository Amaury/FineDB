#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "ydefs.h"
#include "ystr.h"

/*
** ys_new()
** Create a new ystring.
*/
ystr_t ys_new(const char *s)
{
  char *res;
  unsigned int strsz, totalsz, leap;
  ystr_head_t *y;

  strsz = (!s) ? 0 : strlen(s);
  if (strsz < YSTR_SIZE)
    totalsz = YSTR_SIZE;
  else
    {
      leap = (strsz < YSTR_SIZE_BIG) ? YSTR_SIZE :
	(strsz < YSTR_SIZE_HUGE) ? YSTR_SIZE_BIG : YSTR_SIZE_HUGE;
      totalsz = (((strsz + 1) / leap) + 1) * leap;
    }
  if (!(res = (char*)YMALLOC(totalsz + sizeof(ystr_head_t))))
    return (res);
  y = (ystr_head_t*)res;
  res += sizeof(ystr_head_t);
  y->total = totalsz;
  y->used = strsz;
  if (!strsz)
    *res = '\0';
  else
    strcpy(res, s);
  return ((ystr_t)res);
}

/*
 * ys_copy()
 * Create a minimal ystring that contains a copy of the given string.
 */
ystr_t ys_copy(const char *s) {
	char 		*res;
	unsigned int	strsz, totalsz;
	ystr_head_t	*y;

	strsz = (!s) ? 0 : strlen(s);
	totalsz = strsz + 1;
	if (!(res = (char*)YMALLOC(totalsz + sizeof(ystr_head_t))))
		return (res);
	y = (ystr_head_t*)res;
	res += sizeof(ystr_head_t);
	y->total = totalsz;
	y->used = strsz;
	if (!strsz)
		*res = '\0';
	else
		strcpy(res, s);
	return ((ystr_t)res);
}

/*
** ys_del()
** Delete an existing ystring.
*/
void ys_del(ystr_t *s)
{
  ystr_head_t *y;

  if (!s || !*s)
    return ;
  y = (ystr_head_t*)(*s - sizeof(ystr_head_t));
  YFREE(y);
  *s = NULL;
}

/*
 * ys_free()
 * Delete an existing ystring.
 */
void ys_free(ystr_t s) {
	ystr_head_t	*y;

	if (!s)
		return;
	y = (ystr_head_t*)(s - sizeof(ystr_head_t));
	YFREE(y);
}

/*
** ys_trunc()
** Truncate an existing ystring. The allocated memory size doesn't change.
*/
void ys_trunc(ystr_t s)
{
  ystr_head_t *y;

  if (!s)
    return ;
  y = (ystr_head_t*)(s - sizeof(ystr_head_t));
  *s = '\0';
  y->used = 0;
}

/*
** ys_setsz()
** Set the minimum size of a ystring.
*/
int ys_setsz(ystr_t *s, unsigned int sz)
{
  ystr_head_t *y, *ny;
  unsigned int totalsz, leap;
  char *ns;

  if (!s)
    return (0);
  y = (ystr_head_t*)(*s - sizeof(ystr_head_t));
  if (sz <= y->total)
    return (1);
  leap = (sz < YSTR_SIZE_BIG) ? YSTR_SIZE :
    (sz < YSTR_SIZE_HUGE) ? YSTR_SIZE_BIG : YSTR_SIZE_HUGE;
  totalsz = (((sz + 1) / leap) + 1) * leap;
  if (!(ns = (char*)YMALLOC(totalsz + sizeof(ystr_head_t))))
    return (0);
  ny = (ystr_head_t*)ns;
  ns += sizeof(ystr_head_t);
  ny->total = totalsz;
  ny->used = y->used;
  memcpy(ns, s, y->used + 1);
  YFREE(y);
  *s = ns;
  return (1);
}

/*
** ys_len()
** Return the length of an ystring.
*/
unsigned int ys_len(const ystr_t s)
{
  if (!s)
    return (0);
  return (((ystr_head_t*)(s - sizeof(ystr_head_t)))->used);
}

/*
** ys_cat()
** Concatenate a character string at the end of an ystring.
*/
int ys_cat(ystr_t *dest, const char *src)
{
  unsigned int srcsz, strsz, totalsz, leap;
  ystr_head_t *y, *ny;
  char *ns;

  if (!src || !(srcsz = strlen(src)))
    return (1);
  if (!*dest)
    {
      *dest = ys_new(src);
      return (1);
    }
  y = (ystr_head_t*)(*dest - sizeof(ystr_head_t));
  if ((y->used + 1 + srcsz) <= y->total)
    {
      memcpy(*dest + y->used, src, srcsz + 1);
      y->used += srcsz;
      return (1);
    }
  strsz = y->used + srcsz;
  leap = (strsz < YSTR_SIZE_BIG) ? YSTR_SIZE :
    (strsz < YSTR_SIZE_HUGE) ? YSTR_SIZE_BIG : YSTR_SIZE_HUGE;
  totalsz = (((strsz + 1) / leap) + 1) * leap;
  if (!(ns = (char*)YMALLOC(totalsz + sizeof(ystr_head_t))))
    return (0);
  ny = (ystr_head_t*)ns;
  ns += sizeof(ystr_head_t);
  ny->total = totalsz;
  ny->used = strsz;
  memcpy(ns, *dest, y->used);
  memcpy(ns + y->used, src, srcsz + 1);
  YFREE(y);
  *dest = ns;
  return (1);
}

/*
** ys_tac()
** Concatenate a character string at the begining of an ystring.
*/
int ys_tac(ystr_t *dest, const char *src)
{
  unsigned int srcsz, strsz, totalsz, leap;
  ystr_head_t *y, *ny;
  char *ns;

  if (!src || !(srcsz = strlen(src)))
    return (1);
  if (!*dest)
    {
      *dest = ys_new(src);
      return (1);
    }
  y = (ystr_head_t*)(*dest - sizeof(ystr_head_t));
  if ((y->used + 1 + srcsz) <= y->total)
    {
      char *pt1, *pt2;
      for (pt1 = *dest + y->used, pt2 = pt1 + srcsz;
	   pt1 >= *dest; --pt1, --pt2)
	*pt2 = *pt1;
      memcpy(*dest, src, srcsz);
      y->used += srcsz;
      return (1);
    }
  strsz = y->used + srcsz;
  leap = (strsz < YSTR_SIZE_BIG) ? YSTR_SIZE :
    (strsz < YSTR_SIZE_HUGE) ? YSTR_SIZE_BIG : YSTR_SIZE_HUGE;
  totalsz = (((strsz + 1) / leap) + 1) * leap;
  if (!(ns = (char*)YMALLOC(totalsz + sizeof(ystr_head_t))))
    return (0);
  ny = (ystr_head_t*)ns;
  ns += sizeof(ystr_head_t);
  ny->total = totalsz;
  ny->used = strsz;
  memcpy(ns, src, srcsz);
  memcpy(ns + srcsz, *dest, y->used + 1);
  YFREE(y);
  *dest = ns;
  return (1);
}

/*
** ys_ncat()
** See strncat().
*/
int ys_ncat(ystr_t *dest, const char *src, unsigned int n)
{
  unsigned int strsz, totalsz, leap;
  ystr_head_t *y, *ny;
  char *ns;

  if (!src || !n)
    return (1);
  if (!*dest)
    {
      *dest = ys_new(src);
      return (1);
    }
  y = (ystr_head_t*)(*dest - sizeof(ystr_head_t));
  if ((y->used + 1 + n) <= y->total)
    {
      strncpy(*dest + y->used, src, n);
      y->used += n;
      (*dest)[y->used] = '\0';
      return (1);
    }
  strsz = y->used + n;
  leap = (strsz < YSTR_SIZE_BIG) ? YSTR_SIZE :
    (strsz < YSTR_SIZE_HUGE) ? YSTR_SIZE_BIG : YSTR_SIZE_HUGE;
  totalsz = (((strsz + 1) / leap) + 1) * leap;
  if (!(ns = (char*)YMALLOC(totalsz + sizeof(ystr_head_t))))
    return (0);
  ny = (ystr_head_t*)ns;
  ns += sizeof(ystr_head_t);
  ny->total = totalsz;
  ny->used = strsz;
  strcpy(ns, *dest);
  strncpy(ns + y->used, src, n);
  ns[ny->used] = '\0';
  YFREE(y);
  *dest = ns;
  return (1);
}

/*
** ys_ntac()
** Same as ystr_tac() but at the begining of a ystring.
*/
int ys_ntac(ystr_t *dest, const char *src, unsigned int n)
{
  unsigned int strsz, totalsz, leap;
  ystr_head_t *y, *ny;
  char *ns;

  if (!src || !n)
    return (1);
  if (!*dest)
    {
      *dest = ys_new(src);
      return (1);
    }
  n = (strlen(src) < n) ? strlen(src) : n;
  y = (ystr_head_t*)(*dest - sizeof(ystr_head_t));
  if ((y->used + 1 + n) <= y->total)
    {
      char *pt1, *pt2;
      for (pt1 = *dest + y->used, pt2 = pt1 + n;
	   pt1 >= *dest; --pt1, --pt2)
	*pt2 = *pt1;
      memcpy(*dest, src, n);
      y->used += n;
      return (1);
    }
  strsz = y->used + n;
  leap = (strsz < YSTR_SIZE_BIG) ? YSTR_SIZE :
    (strsz < YSTR_SIZE_HUGE) ? YSTR_SIZE_BIG : YSTR_SIZE_HUGE;
  totalsz = (((strsz + 1) / leap) + 1) * leap;
  if (!(ns = (char*)YMALLOC(totalsz + sizeof(ystr_head_t))))
    return (0);
  ny = (ystr_head_t*)ns;
  ns += sizeof(ystr_head_t);
  ny->total = totalsz;
  ny->used = strsz;
  memcpy(ns, src, n);
  memcpy(ns + n, *dest, y->used + 1);
  YFREE(y);
  *dest = ns;
  return (1);
}

/*
** ys_dup()
** Duplicate an ystring.
*/
ystr_t ys_dup(const ystr_t s)
{
  ystr_head_t *y, *ny;
  char *ns;

  if (!s)
    return (ys_new(""));
  y = (ystr_head_t*)(s - sizeof(ystr_head_t));
  if (!(ns = (char*)YMALLOC(y->total + sizeof(ystr_head_t))))
    return (ns);
  ny = (ystr_head_t*)ns;
  ns += sizeof(ystr_head_t);
  ny->total = y->total;
  ny->used = y->used;
  memcpy(ns, s, y->used);
  ns[y->used] = '\0';
  return ((ystr_t)ns);
}

/*
** ys_string()
** Create a copy of a ystring. The copy is a simple
** (char*) string, not bufferized.
*/
char *ys_string(const ystr_t s)
{
  ystr_head_t *y;
  char *res;

  if (!s)
    return (NULL);
  y = (ystr_head_t*)(s - sizeof(ystr_head_t));
  if (!(res = (char*)YMALLOC(y->used + 1)))
    return (NULL);
  return (memcpy(res, s, y->used + 1));
}

/*
** ys_concat()
** Create a new ystring that is the concatenation of 2 ystrings.
*/
ystr_t ys_concat(const char *s1, const char *s2)
{
  char *ns;

  ns = ys_new(s1);
  ys_cat(&ns, s2);
  return ((ystr_t)ns);
}

/*
** ys_ltrim()
** Remove all spaces at the beginning of an ystring.
*/
void ys_ltrim(ystr_t s)
{
  ystr_head_t *y;
  char *pt;

  y = (ystr_head_t*)(s - sizeof(ystr_head_t));
  for (pt = s;
       *pt == ' ' || *pt == '\t' || *pt == '\n' || *pt == '\r';
       ++pt, y->used--)
    ;
  if (pt == s)
    return ;
  for (; *pt; ++pt, ++s)
    *s = *pt;
  *s = '\0';
}

/*
** ys_rtrim()
** Remove all spaces at the end of an ystring.
*/
void ys_rtrim(ystr_t s)
{
  ystr_head_t *y;
  char *pt;
  unsigned int initsz;

  y = (ystr_head_t*)(s - sizeof(ystr_head_t));
  initsz = y->used;
  for (pt = s + y->used - 1;
       *pt == ' ' || *pt == '\t' || *pt == '\n' || *pt == '\r';
       --pt)
    {
      if (pt == s)
	{
	  *pt = '\0';
	  y->used = 0;
	  return ;
	}
      y->used--;
    }
  if (initsz != y->used)
    *(pt + 1) = '\0';
}

/*
** ys_trim()
** Remove all spaces at the beginning and the end of an ystring.
*/
void ys_trim(ystr_t s)
{
  ys_ltrim(s);
  ys_rtrim(s);
}

/*
** ys_lshift()
** Remove the first character of a ystring and return it.
*/
char ys_lshift(ystr_t s)
{
  char c;

  if (!s || !*s)
    return ('\0');
  c = *s;
  *s = ' ';
  ys_ltrim(s);
  return (c);
}

/*
** ys_rshift()
** Remove the last character of a ystring and return it.
*/
char ys_rshift(ystr_t s)
{
  ystr_head_t *y;
  char c;

  if (!s || !*s)
    return ('\0');
  y = (ystr_head_t*)(s - sizeof(ystr_head_t));
  c = *(s + y->used - 1);
  *(s + y->used - 1) = '\0';
  y->used--;
  return (c);
}

/*
** ys_putc()
** Add a character at the beginning of a ystring.
*/
int ys_putc(ystr_t *s, char c)
{
  ystr_head_t *y, *ny;
  char *pt1, *pt2, *ns;
  unsigned int totalsz, leap;

  if (c == '\0')
    return (1);
  if (!*s)
    {
      char tc[2] = {'\0', '\0'};
      tc[0] = c;
      *s = ys_new(tc);
      return (1);
    }
  y = (ystr_head_t*)(*s - sizeof(ystr_head_t));
  if (y->total >= (y->used + 2))
    {
      for (pt1 = *s + y->used, pt2 = pt1 + 1; pt1 >= *s; --pt1, --pt2)
	*pt2 = *pt1;
      **s = c;
      y->used++;
      return (1);
    }
  leap = (y->used < YSTR_SIZE_BIG) ? YSTR_SIZE :
    (y->used < YSTR_SIZE_HUGE) ? YSTR_SIZE_BIG : YSTR_SIZE_HUGE;
  totalsz = (((y->used + 2) / leap) + 1) * leap;
  if (!(ns = (char*)YMALLOC(totalsz + sizeof(ystr_head_t))))
    return (0);
  ny = (ystr_head_t*)ns;
  ns += sizeof(ystr_head_t);
  ny->total = totalsz;
  ny->used = y->used + 1;
  *ns = c;
  memcpy(ns + 1, *s, y->used + 1);
  YFREE(y);
  *s = ns;
  return (1);
}

/*
** ys_addc()
** Add a character at the end of a ystring.
*/
int ys_addc(ystr_t *s, char c)
{
  char tc[2] = {'\0', '\0'};

  tc[0] = c;
  return (ys_cat(s, tc));
}

/*
** ys_upcase()
** Convert all characters of a character string to upper case.
*/
void ys_upcase(char *s)
{
  if (!s)
    return ;
  for (; *s; ++s)
    if (*s >= 'a' && *s <= 'z')
      *s = 'A' + (*s - 'a');
}

/*
** ys_lowcase()
** Convert all characters of a character string to lower case.
*/
void ys_lowcase(char *s)
{
  if (!s)
    return ;
  for (; *s; ++s)
    if (*s >= 'A' && *s <= 'Z')
      *s = 'a' + (*s - 'A');
}

/*
** ys_printf()
** Write inside a ystring using formatted arguments. The
** ystring must be long enough (use ys_setsz() before),
** otherwise the resulting string will be truncate.
*/
int ys_printf(ystr_t *s, char *format, ...)
{
  va_list p_list;
  ystr_head_t *y, *ny;
  int i;
  char *ns;

  y = (ystr_head_t*)(*s - sizeof(ystr_head_t));
  if (!(ns = (char*)YMALLOC(y->total + sizeof(ystr_head_t))))
    return (0);
  ny = (ystr_head_t*)ns;
  ns += sizeof(ystr_head_t);
  ny->total = y->total;
  va_start(p_list, format);
  if ((i = vsnprintf(ns, y->total, format, p_list)) == -1)
    {
      YFREE(ny);
      return (0);
    }
  ny->used = i;
  YFREE(y);
  *s = ns;
  va_end(p_list);
  return (1);
}

/*
** ys_vprintf()
** Same as ys_printf(), but the variable arguments are given
** trough a va_list.
*/
int ys_vprintf(ystr_t *s, char *format, va_list args)
{
  ystr_head_t *y;
  int i;

  y = (ystr_head_t*)(*s - sizeof(ystr_head_t));
  if ((i = vsnprintf(*s, y->total, format, args)) == -1)
    **s = '\0';
  y->used = i == -1 ? 0 : 1;
  return (i == -1 ? 0 : 1);
}

/*
** ys_str2hexa()
** Convert a character string in an hexadecimal ystring.
*/
ystr_t ys_str2hexa(char *str)
{
  char h[3] = {'\0', '\0', '\0'}, *pt;
  ystr_t ys;

  if (!str || !(ys = ys_new("")))
    return (NULL);
  for (pt = str; *pt; ++pt)
    {
      snprintf(h, 3, "%x", *pt);
      ys_cat(&ys, h);
    }
  return (ys);
}

/*
** ys_subs()
** Substitute a string by another, inside a character string.
*/
ystr_t ys_subs(const char *orig, const char *from, const char *to)
{
  ystr_t ys;
  const char *pt;
  unsigned int from_len;

  if (!orig || !(ys = ys_new("")))
    return (NULL);
  from_len = (from) ? strlen(from) : 0;
  for (pt = orig; *pt; ++pt)
    {
      if (from_len && !strncmp(from, pt, from_len))
        {
          ys_cat(&ys, to);
          pt = pt + from_len - 1;
        }
      else
        ys_addc(&ys, *pt);
    }
  return (ys);
}

/*
** ys_casesubs()
** Substitute a string by another in a case-insensitive manner.
*/
ystr_t ys_casesubs(const char *orig, const char *from, const char *to)
{
  ystr_t ys;
  const char *pt;
  unsigned int from_len;

  if (!orig || !(ys = ys_new("")))
    return (NULL);
  from_len = (from) ? strlen(from) : 0;
  for (pt = orig; *pt; ++pt)
    {
      if (from_len && !strncasecmp(from, pt, from_len))
        {
          ys_cat(&ys, to);
          pt = pt + from_len - 1;
        }
      else
        ys_addc(&ys, *pt);
    }
  return (ys);
}

/*
** str2xmlentity()
** Convert a character string in another one where each XML special
** characters are replaced by their corresponding XML entities.
*/
char *str2xmlentity(char *str)
{
  char *pt, *result;
  ystr_t res;

  if (!str || !(res = ys_new("")))
    return (NULL);
  for (pt = str; *pt; ++pt)
    {
      if (*pt == LT)
	ys_cat(&res, "&lt;");
      else if (*pt == GT)
	ys_cat(&res, "&gt;");
      else if (*pt == DQUOTE)
	ys_cat(&res, "&quot;");
      else if (*pt == QUOTE)
	ys_cat(&res, "&apos;");
      else if (*pt == AMP)
	ys_cat(&res, "&amp;");
      else
	ys_addc(&res, *pt);
    }
  result = ys_string(res);
  ys_del(&res);
  return (result);
}

/*
** xmlentity2str()
** Convert a string in another one where XML entities are replaced
** by their XML special characters.
*/
char *xmlentity2str(char *str)
{
  ystr_t res;
  char *pt, *result, *pt2;
  int i;
  
  if (!str || !(res = ys_new("")))
    return (NULL);
  for (pt = str; *pt; pt += i)
    {
      if (*pt != AMP)
	{
	  ys_addc(&res, *pt);
	  i = 1;
	}
      else if (!strncmp(pt, "&amp;", (i = strlen("&amp;"))))
	ys_addc(&res, AMP);
      else if (!strncmp(pt, "&lt;", (i = strlen("&lt;"))))
	ys_addc(&res, LT);
      else if (!strncmp(pt, "&gt;", (i = strlen("&gt;"))))
	ys_addc(&res, GT);
      else if (!strncmp(pt, "&quot;", (i = strlen("&quot;"))))
	ys_addc(&res, DQUOTE);
      else if (!strncmp(pt, "&apos;", (i = strlen("&apos;"))))
	ys_addc(&res, QUOTE);
      else if (*(pt + 1) == SHARP && (pt2 = strchr(pt + 2, ';')))
	{
	  ys_addc(&res, atoi(pt + 2));
	  i = (pt2 - pt) + 1;
	}
      else
	{
	  ys_addc(&res, *pt);
	  i = 1;
	}
    }
  result = ys_string(res);
  ys_del(&res);
  return (result);
}
