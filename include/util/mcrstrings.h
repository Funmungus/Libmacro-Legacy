/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
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

/*! \file util/mcrstrings.h
 * \brief String functions for macrolibrary
 * */

# ifndef MCR_STRINGS_H
# define MCR_STRINGS_H

# include "util/array.h"

/*! \brief Convert a C-string into boolean true/false.
 *
 * A string value of false or true is not case sensitive.
 * An integer value will be placed into retVal directly.
 * \param retVal boolean return value of string value
 * \return 0 for failure, otherwise success
 */
MCR_API int mcr_atob ( const char * value, int * retVal ) ;

//
// Array string helper
//
//! \brief Initialize an array to be a char string.
# define mcr_String_init( strPt ) \
mcr_Array_init ( strPt, sizeof ( char ) )
/*! \brief Insert another string at given index. This string will be
 * resized, if needed.
 *
 * \param index The index within this string to insert
 * \param str Other string to copy from
 * \param len Number of characters to copy from str, if greater than the
 * string length, then the entire string will be copied.
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_String_insert ( mcr_Array * strPt, size_t index,
		const char * str, size_t len ) ;
/*! \brief Insert a character inside of a string. This string will be
 * resized, if needed.
 *
 * \param index Index of this string to insert a character
 * \param c Character to insert
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_String_insert_char ( mcr_Array * strPt, size_t index,
		const char c ) ;
/*! \brief Remove a substring.
 *
 * \param index First character to remove
 * \param count Number of characters to remove
 * */
MCR_API void mcr_String_remove ( mcr_Array * strPt, size_t index,
		size_t count ) ;
/*! \brief Append a character to the end of a string. This string will be
 * resized, if needed.
 *
 * \param c Character to push
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_String_push ( mcr_Array * strPt, const char c ) ;
/*! \brief Remove a character from the end of a string.
 *
 * \return The character that was removed.
 * */
MCR_API char mcr_String_pop ( mcr_Array * strPt ) ;
/*! \brief Reset this string and copy from another. This string will be
 * resized, if needed.
 *
 * \param str String to copy from
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_String_from_string ( mcr_Array * strPt,
		const char * str ) ;
/*! \brief Reset this string and copy from another. This string will be
 * resized, if needed.
 *
 * \param str String to copy from
 * \param len Number of characters to copy, if this is more than the
 * string's length, the entire string will be copied.
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_String_nfrom_string ( mcr_Array * strPt,
		const char * str, size_t len ) ;
/*! \brief Append a string to the end of this string. This string will be
 * resized, if needed.
 *
 * \param str String to copy
 * \param len Number of characters to copy, if this is more than the
 * string's length, the entire string will be copied.
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_String_append ( mcr_Array * strPt,
		const char * str, size_t len ) ;
/*! \brief Compare to strings from pointers to either \ref mcr_Array
 * or const char *. i.e. const mcrArray ** or const char **
 *
 * \ref strcasecmp
 * \ref stricmp \ref _stricmp
 * \param lhs Left hand side
 * \param rhs Right hand side
 * \return Negative if lhs < rhs, 0 if equal, and positive if lhs > rhs.
 * */
MCR_API int mcr_String_compare ( const void * lhs, const void * rhs ) ;

/*! \brief An empty string either has no characters, or the first
 * character is NULL.
 *
 * \param arr mcr_Array
 * \return boolean logical expression
 * */
# define MCR_STR_EMPTY( arr ) \
( ! ( arr ).used || ( arr ).array [ 0 ] == '\0' )

# endif
