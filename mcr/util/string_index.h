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
 *  \brief \ref mcr_StringIndex - Map strings to indices of \ref mcr_StringSet
 */

#ifndef MCR_UTIL_STRING_INDEX_H_
#define MCR_UTIL_STRING_INDEX_H_

#include "mcr/util/string_set.h"
#include "mcr/util/map.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Map strings to indices of \ref mcr_StringSet */
struct mcr_StringIndex {
	/*! Map \ref mcr_String to \ref retind */
	struct mcr_Map map;
	/*! Array of strings */
	mcr_StringSet set;
};

/*! \ref mcr_StringIndex ctor
 *
 *  \return 0
 */
MCR_API int mcr_StringIndex_init(void *indexPt);
/*! \ref mcr_StringIndex_init
 *
 *  \return Empty index
 */
MCR_API struct mcr_StringIndex mcr_StringIndex_new();
/*! \ref mcr_StringIndex dtor
 *
 *  \return 0
 */
MCR_API int mcr_StringIndex_deinit(void *indexPt);

/* Allocation control */
/*! Set a minimum number of used elements and resize
 *  if needed.
 *
 *  \param minUsed Minimum number of used elements
 *  \return \ref reterr
 */
MCR_API int mcr_StringIndex_minused(struct mcr_StringIndex *indexPt,
									size_t minUsed);
/*! Set a minimum size, and resize if needed.
 *
 *  \param minSize Minimum number of allocated elements
 *  \return \ref reterr
 */
MCR_API int mcr_StringIndex_minsize(struct mcr_StringIndex *indexPt,
									size_t minSize);
/*! Apply a smart resizing algorithm for adding a number of
 *  elements.
 *
 *  \param increasingCount Number of elements to add
 *  \return \ref reterr
 */
MCR_API int mcr_StringIndex_smartsize(struct mcr_StringIndex *indexPt,
									  size_t increasingCount);
/*! Minimize allocated space. */
MCR_API void mcr_StringIndex_trim(struct mcr_StringIndex *indexPt);
/*! Reallocate to given size.
 *
 *  \param newSize New allocated number of elements
 *  \return \ref reterr
 */
MCR_API int mcr_StringIndex_resize(struct mcr_StringIndex *indexPt,
								   size_t newSize);
/*! Remove all mapped key-value pairs.
 *
 *  \return \ref reterr
 */
MCR_API void mcr_StringIndex_clear(struct mcr_StringIndex *indexPt);

/* Position/Values */
/*! Get the string of a given index.
 *
 *  \param index Index of the string
 *  \return String of the given index, null if not found
 */
MCR_API mcr_String *mcr_StringIndex_string(struct mcr_StringIndex *indexPt,
		size_t index);
/*! Get the C-string of a given index.
 *
 *  \param indexPt \ref opt
 *  \param index Index of the string
 *  \return C-string for this index, null if not found
 */
MCR_API const char *mcr_StringIndex_name(const struct mcr_StringIndex *indexPt,
		size_t index);
/*! Get the index of a string
 *
 *  \param indexPt \ref opt
 *  \param strKey \ref opt C-string to find index for
 *  \return \ref retind -1 if not found
 */
MCR_API size_t mcr_StringIndex_index(const struct mcr_StringIndex *indexPt,
										const char *strKey);

/* Set/remove mappings */
/*! Map an index to a string, and a set of strings to that index.
 *
 *  \param index Index to map to
 *  \param strKey \ref opt String to map to and from index
 *  \param addKeys \ref opt More strings to map to the index
 *  \param addCount Number of additional strings to map
 *  \return \ref reterr
 */
MCR_API int mcr_StringIndex_map(struct mcr_StringIndex *indexPt,
								size_t index, const char *strKey);
/*! Map a string to an index, without changing the index itself
 *
 *  \param index Index to map to
 *  \param addKeys \ref opt Strings to map to the index
 *  \param addCount Number of strings to map
 *  \return \ref reterr
 */
MCR_API int mcr_StringIndex_add(struct mcr_StringIndex *indexPt,
								size_t index, const char **addKeys, size_t addCount);
/*! Remove mapped index and remap to a different index.
 *
 *  \param curIndex \ref opt Index to remove
 *  \param newIndex \ref opt Index to remap into
 *  \return \ref reterr
 */
MCR_API int mcr_StringIndex_reindex(struct mcr_StringIndex *indexPt,
									size_t curIndex, size_t newIndex);
/*! Remove mapped string and remap to a different index.
 *
 *  \param strKey String to remove
 *  \param newKey String to remap into
 *  \return \ref reterr
 */
MCR_API int mcr_StringIndex_remap(struct mcr_StringIndex *indexPt,
								  const char *strKey, const char *newKey);
/*! Remove a mapped index
 *
 *  \param remIndex Index to remove
 *  \param flagRemoveAll If true this also removes all strings mapped to the
 *  index.
 */
MCR_API void mcr_StringIndex_unmap(struct mcr_StringIndex *indexPt,
								   size_t remIndex, bool flagRemoveAll);
/*! Remove a mapped string
 *
 *  \param remString \ref opt String to remove
 *  \param flagRemoveAll If true this also removes all indices mapped to the
 *  string.
 */
MCR_API void mcr_StringIndex_unmap_string(struct mcr_StringIndex *indexPt,
		const char *remString, bool flagRemoveAll);

/*! See \ref mcr_Map_sort and \ref mcr_StringSet_sort */
MCR_API void mcr_StringIndex_sort(struct mcr_StringIndex *indexPt);

/*! See \ref mcr_StringIndex_string */
#define MCR_STRINGINDEX_STRING(index, stringIndex) \
MCR_STRINGSET_ELEMENT((index).set, stringIndex)

#ifdef __cplusplus
}
#endif

#endif
