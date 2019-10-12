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
 *  \brief \ref mcr_StringSet - An array of \ref mcr_String
 */

#ifndef MCR_UTIL_STRING_SET_H_
#define MCR_UTIL_STRING_SET_H_

#include "mcr/util/mcrstring.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! An array of \ref mcr_String */
typedef struct mcr_Array mcr_StringSet;

/*! \ref mcr_StringSet ctor
 *
 *  Comparison will be set to mcr_String_compare by default.
 *  \return 0
 */
MCR_API int mcr_StringSet_init(void *setPt);
/*! \ref mcr_StringSet_init
 *
 *  \return Empty string set
 */
MCR_API mcr_StringSet mcr_StringSet_new();
/*! \ref mcr_StringSet dtor
 *
 *  \return 0
 */
MCR_API int mcr_StringSet_deinit(void *setPt);
/*! Set initial values
 *
 *  Will deinit if anything is allocated
 *  \param compare \ref mcr_Array.compare
 */
MCR_API void mcr_StringSet_set_all(mcr_StringSet * setPt,
								   mcr_compare_fnc compare);

/* Allocation control */
/*! \ref mcr_Array_minfill with empty strings */
MCR_API int mcr_StringSet_minused(mcr_StringSet * setPt, size_t minUsed);
/*! \ref mcr_Array_minsize */
#define mcr_StringSet_minsize mcr_Array_minsize
/*! \ref mcr_Array_smartsize */
#define mcr_StringSet_smartsize mcr_Array_smartsize
/*! \ref mcr_Array_trim */
#define mcr_StringSet_trim mcr_Array_trim
/*! \ref mcr_Array_resize which also frees strings that are removed */
MCR_API int mcr_StringSet_resize(mcr_StringSet * setPt, size_t newSize);
/*! \ref mcr_Array_clear which also frees strings that are removed */
MCR_API void mcr_StringSet_clear(mcr_StringSet * setPt);

/* Position */
/*! \ref mcr_Array_element
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_element(setPt, pos) \
mcr_castpt(mcr_String, mcr_Array_element(setPt, pos))
/*! \ref mcr_Array_first
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_first(setPt) \
mcr_castpt(mcr_String, mcr_Array_first(setPt))
/*! \ref mcr_Array_last
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_last(setPt) \
mcr_castpt(mcr_String, mcr_Array_last(setPt))
/*! \ref mcr_Array_end
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_end(setPt) \
mcr_castpt(mcr_String, mcr_Array_end(setPt))
/*! \ref mcr_Array_next
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_next(setPt, posPt) \
mcr_castpt(mcr_String, mcr_Array_next(setPt, mcr_castpt(void, posPt)))
/*! \ref mcr_Array_prev
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_prev(setPt, posPt) \
mcr_castpt(mcr_String, mcr_Array_prev(setPt, mcr_castpt(void, posPt)))
/*! \ref mcr_Array_index
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_index mcr_Array_index
/*! \ref mcr_Array_last_index
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_last_index mcr_Array_last_index
/*! \ref mcr_Array_iter
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_iter mcr_Array_iter
/*! \ref mcr_Array_iter_range
 *
 *  \return \ref mcr_String *
 */
#define mcr_StringSet_iter_range mcr_Array_iter_range

/* Add/remove */
/*! \ref mcr_Array_insert with an array of strings */
MCR_API int mcr_StringSet_insert(mcr_StringSet * setPt, size_t pos,
								 const char **strArr, size_t count);
/*! \ref mcr_Array_remove_index and deinit strings that are removed */
MCR_API void mcr_StringSet_remove_index(mcr_StringSet * setPt, size_t pos,
										size_t count);
/*! \ref mcr_Array_append with an array of strings */
MCR_API int mcr_StringSet_append(mcr_StringSet * setPt, const char **strArr,
								 size_t count);
/*! \ref mcr_Array_push with a string */
MCR_API int mcr_StringSet_push(mcr_StringSet * setPt, const char *copyStr);
/*! \ref mcr_Array_pop and deinit the removed string */
MCR_API void mcr_StringSet_pop(mcr_StringSet * setPt);

/* Replace current elements */
/*! \ref mcr_Array_replace with an array of strings */
MCR_API int mcr_StringSet_replace(mcr_StringSet * setPt,
								  const char **arraySource, size_t count);
/*! \ref mcr_Array_copy with an array of strings */
MCR_API int mcr_StringSet_copy(mcr_StringSet * dstPt, size_t dstPos,
							   const char **srcArray, size_t count);
/*! \ref mcr_Array_set with a string */
MCR_API int mcr_StringSet_set(mcr_StringSet * setPt, size_t pos,
							  const char *copyStr);
/*! \ref mcr_Array_fill with a string */
MCR_API int mcr_StringSet_fill(mcr_StringSet * setPt, size_t pos,
							   size_t count, const char *copyStr);
/// \todo mcr_StringSet_move ?

/* Sorted functions: If no compare function is available, mcr_String_compare
 * will be used.
 */
/*! \pre Set may or may not be sorted
 *  \brief \ref mcr_Array_sort
 */
MCR_API void mcr_StringSet_sort(mcr_StringSet * setPt);
/*! \pre Set is sorted
 *  \brief \ref mcr_Array_find
 */
MCR_API mcr_String *mcr_StringSet_find(const mcr_StringSet * setPt,
									   const char *strKey);
/*! \pre Set is sorted
 *  \brief \ref mcr_Array_add
 */
MCR_API int mcr_StringSet_add(mcr_StringSet * setPt,
							  const char **copyStr, size_t count, bool flagUnique);
/*! \pre Set is sorted
 *  \brief \ref mcr_Array_remove
 */
MCR_API void mcr_StringSet_remove(mcr_StringSet * setPt, const char *removeStr);

/* Macro utils */
/*! See \ref mcr_StringSet_element */
#define MCR_STRINGSET_ELEMENT(set, pos) \
mcr_castpt(mcr_String, MCR_ARR_ELEMENT(set, pos))
/*! See \ref mcr_StringSet_first */
#define MCR_STRINGSET_FIRST(set) \
mcr_castpt(mcr_String, MCR_ARR_FIRST(set))
/*! See \ref mcr_StringSet_end */
#define MCR_STRINGSET_END(set) \
mcr_castpt(mcr_String, MCR_ARR_END(set))
/*! See \ref mcr_StringSet_last */
#define MCR_STRINGSET_LAST(set) \
mcr_castpt(mcr_String, MCR_ARR_LAST(set))
/*! See \ref mcr_StringSet_next */
#define MCR_STRINGSET_NEXT(set, curPt) \
mcr_castpt(mcr_String, MCR_ARR_NEXT(set, curPt))
/*! See \ref mcr_StringSet_prev */
#define MCR_STRINGSET_PREV(set, curPt) \
mcr_castpt(mcr_String, MCR_ARR_PREV(set, curPt))
/*! See \ref mcr_StringSet_index */
#define MCR_STRINGSET_INDEX(set, posPt) \
MCR_ARR_INDEX(set, posPt)
/*! See \ref mcr_StringSet_last_index */
#define MCR_STRINGSET_LAST_INDEX(set) \
MCR_ARR_LAST_INDEX(set)
/*! See \ref MCR_ARR_FOR_EACH */
#define MCR_STRINGSET_FOR_EACH(set, iterateFnc) \
MCR_ARR_FOR_EACH(set, iterateFnc)

#ifdef __cplusplus
}
#endif

#endif
