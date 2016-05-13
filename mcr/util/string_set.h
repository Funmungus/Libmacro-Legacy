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
 */

#include "mcr/util/mcrstring.h"

#ifndef MCR_STRINGSET_H
#define MCR_STRINGSET_H

typedef struct mcr_Array mcr_StringSet;

/*! \brief ctor unsorted */
MCR_API void mcr_StringSet_init(void *dataPt);
MCR_API void mcr_StringSet_free(void *dataPt);
#define mcr_StringSet_free_foreach(setPt, ignore) \
mcr_mcr_StringSet_free(setPt)
/*!
 * \brief \ref mcr_StringSet_set_sorting
 *
 * Will free if anything is allocated.
 */
MCR_API void mcr_StringSet_set_all(mcr_StringSet * setPt,
	mcr_compare_fnc compare);
MCR_API void mcr_StringSet_set_sorting(mcr_StringSet * setPt,
	mcr_compare_fnc compare);
/* */
/* Allocation control */
/* */
MCR_API int mcr_StringSet_minused(mcr_StringSet * setPt, size_t minimumUsed);
#define mcr_StringSet_minsize mcr_Array_minsize
#define mcr_StringSet_smartsize mcr_Array_smartsize
#define mcr_StringSet_trim mcr_Array_trim
MCR_API int mcr_StringSet_resize(mcr_StringSet * setPt, size_t newSize);
MCR_API void mcr_StringSet_clear(mcr_StringSet * setPt);
/* */
/* Position */
/* */
#define mcr_StringSet_element(setPt, pos) \
((mcr_String *)mcr_Array_element (setPt, pos))
#define mcr_StringSet_next(setPt, posPt) \
((mcr_String *)mcr_Array_next (setPt, ((const void *) posPt)))
#define mcr_StringSet_prev(setPt, posPt) \
((mcr_String *)mcr_Array_prev (setPt, ((const void *) posPt)))
#define mcr_StringSet_end(setPt) \
((mcr_String *)mcr_Array_end (setPt))
#define mcr_StringSet_last(setPt) \
((mcr_String *)mcr_Array_last (setPt))
#define mcr_StringSet_last_index(setPt) \
mcr_Array_last_index (setPt)
#define mcr_StringSet_indexof mcr_Array_indexof
#define mcr_StringSet_for_each mcr_Array_for_each
/* */
/* Add/remove */
/* */
MCR_API int mcr_StringSet_insert(mcr_StringSet * setPt, size_t pos,
	const char **strArr, size_t count);
MCR_API void mcr_StringSet_remove_index(mcr_StringSet * setPt, size_t pos,
	size_t count);
MCR_API int mcr_StringSet_append(mcr_StringSet * setPt, const char **strArr,
	size_t count, bool flagUnique);
MCR_API int mcr_StringSet_push(mcr_StringSet * setPt, const char *copyStr,
	bool flagUnique);
MCR_API void mcr_StringSet_pop(mcr_StringSet * setPt);
/* */
/* Replace current elements */
/* */
MCR_API int mcr_StringSet_replace(mcr_StringSet * setPt,
	const char **arraySource, size_t count);
MCR_API int mcr_StringSet_copy(mcr_StringSet * dstPt, size_t dstPos,
	const char **srcArray, size_t count);
MCR_API int mcr_StringSet_set(mcr_StringSet * setPt, size_t pos,
	const char *copyStr);
MCR_API int mcr_StringSet_fill(mcr_StringSet * setPt, size_t pos,
	size_t count, const char *copyStr);
/* */
/* Sortable functions: If no compare function available, memcmp will be */
/* used on an unsorted array, with a slow iteration. */
/* */
MCR_API void mcr_StringSet_sort(mcr_StringSet * setPt);
MCR_API mcr_String *mcr_StringSet_find(const mcr_StringSet * setPt,
	const char *strKey);
MCR_API int mcr_StringSet_add(mcr_StringSet * setPt,
	const char **copyStr, size_t count, bool flagUnique);
MCR_API void mcr_StringSet_remove(mcr_StringSet * setPt, const char *removeStr);
/* */
/* Macro utils */
/* */
#define MCR_STRINGSET_ELEMENT(set, pos) \
((mcr_String *)MCR_ARR_ELEMENT (set, pos))
#define MCR_STRINGSET_NEXT(set, curPt) \
((mcr_String *)MCR_ARR_NEXT (set, curPt))
#define MCR_STRINGSET_PREV(set, curPt) \
((mcr_String *)MCR_ARR_PREV (set, curPt))
#define MCR_STRINGSET_END(set) \
((mcr_String *)MCR_ARR_END (set))
#define MCR_STRINGSET_LAST(set) \
((mcr_String *)MCR_ARR_LAST (set))
#define MCR_STRINGSET_LAST_INDEX(set) \
MCR_ARR_LAST_INDEX (set)
#define MCR_STRINGSET_INDEXOF(set, elementPt) \
MCR_ARR_INDEX (set, elementPt)
#define MCR_STRINGSET_ITER(set, itHolder, endHolder, bytesHolder) \
MCR_ARR_ITER (set, itHolder, endHolder, bytesHolder)
#define MCR_STRINGSET_ITER_RANGE(set, itHolder, endHolder, \
		bytesHolder, firstIndex, lastIndex) \
MCR_ARR_ITER_RANGE (set, itHolder, endHolder, bytesHolder, \
		firstIndex, lastIndex)
#define MCR_STRINGSET_FOR_EACH(set, iterateFnc, ...) \
MCR_ARR_FOR_EACH (set, iterateFnc, __VA_ARGS__)
#define MCR_STRINGSET_RANGE(set, firstIndex, lastIndex, iterateFnc, ...) \
MCR_ARR_RANGE (set, firstIndex, lastIndex, iterateFnc, __VA_ARGS__)
/* */
/* Sortable array macros; If no compare function available, then array */
/* functions will be used. */
/* */
#define MCR_STRINGSET_SORT(set) \
if ((set).used) \
{ \
	if ((set).compare) \
		qsort ((set).array, (set).used, (set).element_size, \
				(set).compare); \
	else \
		qsort ((set).array, (set).used, (set).element_size, \
				mcr_String_compare); \
}
#define MCR_STRINGSET_FIND(set, strKey) \
((set).used ? \
	(set).compare ? \
		bsearch (&strKey, (set).array, (set).used, \
				(set).element_size, (set).compare) : \
	mcr_StringSet_find (&(set), strKey) : \
NULL)

#define MCR_STRINGSET_MINSIZE(set, minimumSize) \
(minimumSize > (set).size ? \
	mcr_StringSet_resize (&(set), minimumSize) : \
0)

#define mcr_StringSet_bytes(setPt) \
mcr_Array_bytes (setPt)
#define MCR_STRINGSET_BYTES(set) \
MCR_ARR_BYTES (set)

#endif
