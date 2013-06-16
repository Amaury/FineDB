/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	ystr.h
 * @abstract	All definitions about ystrings.
 * @discussion	The ystrings are a type of buffered strings. When you get a 
 *		ystring, you get a pointer to a character string. It is a 
 *		(char*) pointer, as for any C's character string. But, in fact,
 *		there is more allocated memory than used memory. It is usefull
 *		to extend the string without new memory allocation and copy. 
 *		And before the address of the first character of the string,
 *		there is two integers ; the first that contains the total
 *		allocated size (used string memory + '\0' + free memory size) ;
 *		the second that contains the used memory size. The used size 
 *		doesn't count the ending '\0' character of the string.
 * @version	1.0 May 17 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YSTR_H__
#define __YSTR_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*! @define YSTR_SIZE Default size for ystrings. */
#define YSTR_SIZE	4096

/*! @define YSTR_SIZE_BIG Size for big ystrings. */
#define YSTR_SIZE_BIG	65536

/*! @define YSTR_SIZE_HUGE Size for huge ystrings. */
#define YSTR_SIZE_HUGE	1048576

/*!
 * @struct	ystr_head_s
 *		Structure used for the head of ystrings.
 * @field	total	Total size of the ystring.
 * @field	used	Used size of the ystring.
 */
struct ystr_head_s
{
  unsigned int total;
  unsigned int used;
};

/*! @typedef ystr_head_t See ystr_head_s structure. */
typedef struct ystr_head_s ystr_head_t;

/*! @typedef ystr_t Type definition equivalent to the character string part
 * of ystrings. A variable of this type could always be cast as (char*). */
typedef char* ystr_t;

/*!
 * @function	ys_new
 *		Create a new ystring.
 * @param	s	Original string that will be copied in the ystring.
 * @return	A pointer to the created ystring.
 */
ystr_t ys_new(const char *s);

/*!
 * @function	ys_copy
 *		Create a minimal ystring that contains a copy of the given string.
 * @param	s	Original string that will be copied in the ystring.
 * @return	A pointer to the created ystring.
 */
ystr_t ys_copy(const char *s);

/*!
 * @function	ys_del
 *		Delete an existing ystring.
 * @param	s	A pointer to the ystring.
 */
void ys_del(ystr_t *s);

/*!
 * @function	ys_free
 *		Delete an existing ystring.
 * @param	s	The ystring.
 */
void ys_free(ystr_t s);

/*!
 * @function	ys_trunc
 *		Truncate an existing ystring. The allocated memory doesn't
 *		change.
 * @param	s	The ystring.
 */
void ys_trunc(ystr_t s);

/*!
 * @function	ys_setsz
 *		Set the minimum size of a ystring.
 * @param	s	A pointer to the ystring.
 * @param	sz	The minimum size for this ystring.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_setsz(ystr_t *s, unsigned int sz);

/*!
 * @function	ys_len
 *		Return the length of a ystring.
 * @param	s	The ystring.
 * @return	The ystring's length.
 */
unsigned int ys_len(const ystr_t s);

/*!
 * @function	ys_cat
 *		Concatenate a character string at the end of a ystring.
 * @param	dest	A pointer to the ystring.
 * @param	src	A pointer to the character string.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_cat(ystr_t *dest, const char *src);

/*!
 * @function	ys_tac
 *		Concatenate a character string at the begining of a ystring.
 * @param	dest	A pointer to the ystring.
 * @param	src	A pointer to the character string.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_tac(ystr_t *dest, const char *src);

/*!
 * @function	ys_ncat
 *		Concatenate a given number of characters from a
 *		character string to an ystring.
 * @param	dest	A pointer to the ystring.
 * @param	src	A pointer to the character string.
 * @param	n	The number of characters to copy.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_ncat(ystr_t *dest, const char *src, unsigned int n);

/*!
 * @function	ys_ntac
 *		Concatenate a given number of characters from a
 *		character string at thbegining of an ystring.
 * @param	dest	A pointer to the ystring.
 * @param	src	A pointer to the character string.
 * @param	n	The number of characters to copy.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_ntac(ystr_t *dest, const char *src, unsigned int n);

/*!
 * @function	ys_dup
 *		Duplicate a ystring.
 * @param	s	The ystring.
 * @return	The new ystring.
 */
ystr_t ys_dup(const ystr_t s);

/*!
 * @function	ys_string
 *		Create a copy of a ystring. The copy is a simple
 *		(char*) string, not bufferized.
 * @param	s	The ystring.
 * @return	A pointer to the created string, or NULL if an error occurs.
 */
char *ys_string(const ystr_t s);

/*!
 * @function	ys_concat
 *		Concatenate 2 character strings to create a ystring.
 * @param	s1	A pointer to the first character string.
 * @param	s2	A pointer to the second character string.
 * @return	A pointer to the new ystring.
 */
ystr_t ys_concat(const char *s1, const char *s2);

/*!
 * @function	ys_ltrim
 *		Remove all spaces at the beginning of a ystring.
 * @param	s	The ystring.
 */
void ys_ltrim(ystr_t s);

/*!
 * @function	ys_rtrim
 *		Remove all spaces at the end of a ystring.
 * @param	s	The ystring.
 */
void ys_rtrim(ystr_t s);

/*!
 * @function	ys_trim
 *		Remove all spaces at the beginning and the end of a ystring.
 * @param	s	The ytring.
 */
void ys_trim(ystr_t s);

/*!
 * @function	ys_lshift
 *		Remove the first character of a ystring.
 * @param	s	The ystring.
 * @return	The removed character.
 */
char ys_lshift(ystr_t s);

/*!
 * @function	ys_rshift
 *		Remove the last character of a ystring.
 * @param	s	The ystring.
 * @return	The removed character.
 */
char ys_rshift(ystr_t s);

/*!
 * @function	ys_putc
 *		Add a character at the beginning of a ystring.
 * @param	s	A pointer to the ystring.
 * @param	c	The character to add.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_putc(ystr_t *s, char c);

/*!
 * @function	ys_addc
 *		Add a character at the end of a ystring.
 * @param	s	A pointer to the ystring.
 * @param	c	The character to add.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_addc(ystr_t *s, char c);

/*!
 * @function	ys_upcase
 *		Convert all characters of a character string to upper
 *		case.
 * @param	s	A pointer to the ystring.
 */
void ys_upcase(char *s);

/*!
 * @function	ys_lowcase
 *		Convert all characters of a character string to lower
 *		case.
 * @param	s	A pointer to the ystring.
 */
void ys_lowcase(char *s);

/*!
 * @function	ys_printf
 *		Write inside a ystring using formatted arguments. The
 *		ystring must be long enough (use ys_setsz() before),
 *		otherwise the resulting string will be truncate.
 * @param	s	A pointer to the ystring.
 * @param	format	Format string (like in printf()).
 * @param	...	Variable argument list.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_printf(ystr_t *s, char *format, ...);

/*!
 * @function	ys_vprintf
 *		Same as ys_printf(), but the variable arguments are given
 *		trough a va_list.
 * @param	s	A pointer to the ystring.
 * @param	format	Format string (like in printf()).
 * @param	args	Variable argument list.
 * @return	0 if an error occurs, 1 otherwise.
 */
int ys_vprintf(ystr_t *s, char *format, va_list args);

/*!
 * @function	ys_str2hexa
 *		Convert a character string to the hexadecimal representation
 *		of this string.
 * @param	str	Character string that must be converted.
 * @return	A ystring that contains the converted string, or NULL.
 */
ystr_t ys_str2hexa(char *str);

/*!
 * @function    ys_subs
 *              Substitute a string by another, inside a charater string.
 * @param       orig    The original character string.
 * @param       from    The string to substitute.
 * @param       to      The substitution string.
 * @return      A ystring that contains the substituted string, or NULL.
 */
ystr_t ys_subs(const char *orig, const char *from, const char *to);

/*!
 * @function    ys_casesubs
 *              Substitute a string by another, in a case-insensitive manner.
 * @param       orig    The original character string.
 * @param       from    The string to substitute.
 * @param       to      The substitution string.
 * @return      A ystring that contains the substituted string, or NULL.
 */
ystr_t ys_casesubs(const char *orig, const char *from, const char *to);

/*!
 * @function	str2xmlentity
 *		Convert a character string in another one where each XML special
 *		characters are replaced by their corresponding XML entities.
 * @param	str	Character string that must be converted.
 * @return	The converted string, or NULL.
 */
char *str2xmlentity(char *str);

/*!
 * @function	xmlentity2str
 *		Convert a string in another one where XML entities are replaced by
 *		their XML special characters.
 * @param	str	The XML encoded character string.
 * @return	The unconverted string, or NULL.
 */
char *xmlentity2str(char *str);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YSTR_H__ */
