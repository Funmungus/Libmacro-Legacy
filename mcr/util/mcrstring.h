/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*! \file
 *  \brief C-String functions for Libmacro
 */

#ifndef MCR_UTIL_MCRSTRING_H_
#define MCR_UTIL_MCRSTRING_H_

#include "mcr/util/array.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Interface for a sorted array of strings */
MCR_API const struct mcr_Interface *mcr_String_interface();

/*! Convert a C-string into boolean true/false.
 *
 *  A string value of false or true is not case sensitive.
 *  A boolean value will be placed into retVal directly.
 *  \param value \ref opt
 *  \param retVal boolean return value of string value
 *  \return \ref reterr
 */
MCR_API int mcr_atob(const char *value, bool * retVal);

/*! \ref mcr_Array designed for C-strings */
typedef struct mcr_Array mcr_String;

/*! \ref mcr_String ctor
 *
 *  Initialize array for character elements
 *  \ref strPt \ref mcr_String *
 *  \return 0
 */
MCR_API int mcr_String_init(void *strPt);
/*! \ref mcr_String_init
 *
 *  \return Empty string
 */
MCR_API mcr_String mcr_String_new();
/*! ref mcr_String ctor */
#define mcr_String_deinit mcr_Array_deinit
/*! Compare two strings, case insensitive.
 *
 *  \param lhs \ref mcr_String * or char ** Left hand side
 *  \param rhs \ref mcr_String * or char ** Right hand side
 *  \return \ref retcmp
 */
#define mcr_String_compare mcr_name_compare
/*! Copy a string
 *
 *  \ref strcpy
 *  \ref dstPt \ref mcr_String * Destination
 *  \ref srcPt \ref mcr_String * or char ** Source
 *  \return \ref reterr
 */
MCR_API int mcr_String_copy(void *dstPt, const void *srcPt);

/* Allocation control */
/*! Set a minimum character count size, not including null-terminating
 *  character.
 *
 *  \param strPt \ref mcr_String *
 *  \param minSize size_t Minimum number of characters available
 *  \return \ref reterr
 */
#define mcr_String_minlen(strPt, strLen) \
mcr_Array_minsize(strPt, (strLen) + 1)
/*! \ref mcr_Array_smartsize */
#define mcr_String_smartlen mcr_Array_smartsize
/*! \ref mcr_Array_trim */
#define mcr_String_trim mcr_Array_trim
/*! Set a new maximum character count.
 *
 *  \param strPt \ref mcr_String *
 *  \param strLen size_t New maximum character count
 *  \return \ref reterr
 */
#define mcr_String_resize(strPt, strLen) \
mcr_Array_resize(strPt, (strLen) + 1)
/*! Make a string empty
 *
 *  If a string is allocated, it will become empty.  If nothing
 *  is allocated, it will be left not allocated.
 */
MCR_API void mcr_String_clear(mcr_String * strPt);

/* Position and elements */
/*! Get C-String from mcr_String
 *
 *  \param strPt \ref mcr_String * \ref opt
 *  \return const char *
 */
#define mcr_String_str(strPt) \
mcr_castpt(const char, strPt ? \
	(strPt)->array : \
mcr_null)

/* Add/remove */
/*! Insert another string at given index
 *
 *  \param index The index within this string to insert. This cannot be
 *  after the null terminator index
 *  \param str \ref opt Other string to copy from
 *  \param len Number of characters to copy from str, if greater
 *  than the string length, then the entire string will be copied.
 *  \return \ref reterr
 */
MCR_API int mcr_String_insert(mcr_String * strPt, size_t index,
							  const char *str, size_t len);
/*! Remove a substring.
 *
 *  \param index First character to remove
 *  \param count Number of characters to remove.  If greater than what
 *  is available, the string will be emptied starting with the given index.
 */
MCR_API void mcr_String_remove_index(mcr_String * strPt, size_t index,
									 size_t count);
/*! Append a string to the end of this string
 *
 *  \param str \ref opt String to copy
 *  \param len Number of characters to copy, if this is more than the
 *  string's length, the entire string will be copied.
 *  \return \ref reterr
 */
MCR_API int mcr_String_append(mcr_String * strPt, const char *str,
							  size_t len);
/*! Append a character to the end of a string.
 *
 *  \param c Character to push
 *  \return \ref reterr
 */
MCR_API int mcr_String_push(mcr_String * strPt, const char c);
/*! Remove a character from the end of a string.
 *
 *  \return The character that was removed.
 */
MCR_API char mcr_String_pop(mcr_String * strPt);

/* Replace current elements */
/*! Reset this string and copy from another.
 *
 *  \param strPt \ref mcr_String *
 *  \param str const char * \ref opt String to copy from
 *  \return \ref reterr
 */
#define mcr_String_replace(strPt, str) \
mcr_String_nreplace(strPt, str, (size_t)-1)
/*! Reset this string and copy from another.
 *
 *  \param str \ref opt String to copy from
 *  \param len Number of characters to copy, if this is more than the
 *  string's length, the entire string will be copied.
 *  \return \ref reterr
 */
MCR_API int mcr_String_nreplace(mcr_String * strPt,
								const char *str, size_t len);
/*! Replace characters starting from an index.
 *
 *  \param index First index to start replacing from
 *  \param str \ref opt String to copy
 *  \param len Number of characters to copy, if this is more than the
 *  string's length, the entire string will be copied.
 *  \return \ref reterr
 */
MCR_API int mcr_String_copy_str(mcr_String * strPt, size_t index,
								const char *str, size_t len);
/*! Replace a single character
 *
 *  \param index Index to replace
 *  \param c Character to copy
 *  \return \ref reterr
 */
MCR_API int mcr_String_set(mcr_String * strPt, size_t index, char c);
/*! Replace a range of characters with a single character
 *
 *  \param index Index to replace
 *  \param count Number of elements to replace
 *  \param c Character to copy
 *  \return \ref reterr
 */
MCR_API int mcr_String_fill(mcr_String * strPt, size_t index,
							size_t count, char c);
/*! Move characters within a string, or from one to the other.
 *
 *  \param dstPos Destination index to move characters into
 *  \param srcPt \ref opt String to move characters from.  If null, the
 *  destination array is assumed
 *  \param srcPos Source index to move characters from
 *  \param count Number of characters to move
 *  \return \ref reterr
 */
MCR_API int mcr_String_move(mcr_String * dstPt, size_t dstPos,
							mcr_String * srcPt, size_t srcPos, size_t count);

/*! An empty string either has no characters, or the first
 *  character is NULL.
 *
 *  \param str \ref mcr_String
 *  \return bool
 */
#define MCR_STR_IS_EMPTY(str) \
(!(str).used || *mcr_castpt(char, (str).array) == '\0')

/*! \ref mcr_String_clear */
#define MCR_STR_CLEAR(str) \
if ((str).size) { \
	*mcr_castpt(char, (str).array) = '\0'; \
	(str).used = 1; \
}

/*! Number of characters in a string, not including the null-terminating
 *  character.
 *
 *  \param str \ref mcr_String
 *  \return \ref retind
 */
#define MCR_STR_LEN(str) \
(MCR_STR_IS_EMPTY(str) ? 0 : (str).used - 1)

/*! Number of characters in a string, not including the null-terminating
 *  character.
 *
 *  \param strPt \ref mcr_String *
 *  \return \ref retind
 */
#define mcr_String_len(strPt) \
(strPt ? MCR_STR_LEN(*(strPt)) : 0)

#ifdef __cplusplus
}
#endif

#endif
