/* Libmacro - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

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
 * \brief String functions for libmacro
 */

#ifndef MCR_STRING_H
#define MCR_STRING_H

#include "mcr/util/array.h"

MCR_API const struct mcr_Interface *mcr_String_iface();

/*!
 * \brief Convert a C-string into boolean true/false.
 *
 * A string value of false or true is not case sensitive.
 * An integer value will be placed into retVal directly.
 * \param retVal boolean return value of string value
 * \return \ref reterr
*/
MCR_API int mcr_atob(const char *value, bool * retVal);

/*! \brief \ref mcr_Array designed for C-strings */
typedef struct mcr_Array mcr_String;
/* Array string helper */
MCR_API void mcr_String_init(void *dataPt);
#define mcr_String_free mcr_Array_free
/*! \ref mcr_Array_free_foreach */
#define mcr_String_free_foreach(strPt, ignore) mcr_String_free(strPt)
MCR_API int mcr_String_copy(void *dstPt, void *srcPt);
/*!
 * \brief Compare to strings from pointers to either \ref mcr_String
 * or const char *. i.e. const mcr_Array * or const char **
 *
 * \ref strcasecmp
 * \ref stricmp \ref _stricmp
 * \param lhs Left hand side
 * \param rhs Right hand side
 * \return \ref retcmp
 */
#define mcr_String_compare mcr_name_compare
/* Allocation control */
#define mcr_String_minlen(strPt, strLen) \
mcr_Array_minsize(strPt, (strLen) + 1)
#define mcr_String_smartlen mcr_Array_smartsize
#define mcr_String_trim mcr_Array_trim
#define mcr_String_resize(strPt, strLen) \
mcr_Array_resize(strPt, (strLen) + 1)
MCR_API void mcr_String_clear(mcr_String * strPt);
/* */
/* Position, all same as array */
/* */
#define mcr_String_str(strPt) \
((const char *)(strPt ? \
	(strPt)->array : \
NULL))
/* */
/* Add/remove */
/* */
/*!
 * \brief Insert another string at given index. This string will be
 * resized, if needed.
 *
 * \param index The index within this string to insert
 * \param str (opt) Other string to copy from
 * \param len Number of characters to copy from str, if greater
 * than the string length, then the entire string will be copied.
 * \return \ref reterr
 */
MCR_API int mcr_String_insert(mcr_String * strPt, size_t index,
	const char *str, size_t len);
/*!
 * \brief Remove a substring.
 *
 * \param index First character to remove
 * \param count Number of characters to remove
 */
MCR_API void mcr_String_remove_index(mcr_String * strPt, size_t index,
	size_t count);
/*!
 * \brief Append a string to the end of this string. This string will be
 * resized, if needed.
 *
 * \param str (opt) String to copy
 * \param len Number of characters to copy, if this is more than the
 * string's length, the entire string will be copied.
 * \return \ref reterr
 */
MCR_API int mcr_String_append(mcr_String * strPt, const char *str, size_t len);
/*!
 * \brief Append a character to the end of a string. This string will be
 * resized, if needed.
 *
 * \param c Character to push
 * \return \ref reterr
 */
MCR_API int mcr_String_push(mcr_String * strPt, const char c);
/*!
 * \brief Remove a character from the end of a string.
 *
 * \return The character that was removed.
 */
MCR_API char mcr_String_pop(mcr_String * strPt);
/*!
 * \brief Reset this string and copy from another. This string will be
 * resized, if needed.
 *
 * \param strPt \ref mcr_String *
 * \param str const char * (opt) String to copy from
 * \return \ref reterr
 */
#define mcr_String_replace(strPt, str) \
mcr_String_nreplace(strPt, str, ~0)
/*!
 * \brief Reset this string and copy from another. This string will be
 * resized, if needed.
 *
 * \param str (opt) String to copy from
 * \param len Number of characters to copy, if this is more than the
 * string's length, the entire string will be copied.
 * \return \ref reterr
 */
MCR_API int mcr_String_nreplace(mcr_String * strPt,
	const char *str, size_t len);
MCR_API int mcr_String_copy_str(mcr_String * strPt, size_t index,
	const char *str, size_t len);
MCR_API int mcr_String_set(mcr_String * strPt, size_t index, char c);
MCR_API int mcr_String_fill(mcr_String * strPt, size_t index,
	size_t count, char c);
MCR_API int mcr_String_move(mcr_String * dstPt, size_t dstPos,
	mcr_String * srcPt, size_t srcPos, size_t count);
/*!
 * \brief An empty string either has no characters, or the first
 * character is NULL.
 *
 * \param arr mcr_String
 * \return bool
 */
#define MCR_STR_IS_EMPTY(str) \
(!(str).used || (str).array[0] == '\0')
#define MCR_STR_SET_EMPTY(str) \
if ((str).size) { \
	(str).array[0] = '\0'; \
	(str).used = 1; \
}
#define MCR_STR_LEN(str) \
(MCR_STR_IS_EMPTY (str) ? 0 : (str).used - 1)
#define mcr_String_len(strPt) \
((strPt) ? MCR_STR_LEN (*(strPt)) : 0)
/*!
 * \brief On a \ref mcr_String do a function
 * only if the string contains characters, not including '\0'.
 *
 * Usage: MCR_STRING_DO (mcrString, fnc, mcrString.array) ;
 * \param mcrString \ref mcr_String
 * \param void (*)(...), or similar function.
 * All variadic args are optional.
 */
#define MCR_STR_DO(mcrString, ...) \
if (!MCR_STR_IS_EMPTY (mcrString)) \
{ __VA_ARGS__ }
/*!
 * \brief On a \ref mcr_Array do a function with a numeric
 * return value.  If the string is empty the return value is 0.
 *
 * Usage: MCR_STRING_IS (mcrString, isFnc, mcrString.array, str2) ;
 * \param mcrString \ref mcr_String
 * \param void (*)(...), or similar function.
 * All variadic args are optional.
 * \return bool 0 or return value of isFnc
 */
#define MCR_STR_IS(mcrString, isFnc, ...) \
(MCR_STR_IS_EMPTY(mcrString) ? false : isFnc(__VA_ARGS__))

#endif
