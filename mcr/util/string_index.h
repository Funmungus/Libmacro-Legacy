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

#include "mcr/util/string_map.h"
#include "mcr/util/string_set.h"

#ifndef MCR_STRINGINDEX_H
#define MCR_STRINGINDEX_H

struct mcr_StringIndex {
	mcr_StringMap map;
	mcr_StringSet set;
};

MCR_API void mcr_StringIndex_init(void *dataPt);
MCR_API void mcr_StringIndex_free(void *dataPt);
#define mcr_StringIndex_free_foreach( indexPt, ignore) \
mcr_StringIndex_free(indexPt)
/* */
/* Allocation control */
/* */
MCR_API int mcr_StringIndex_minused(struct mcr_StringIndex *indexPt,
	size_t minimumUsed);
MCR_API int mcr_StringIndex_minsize(struct mcr_StringIndex *indexPt,
	size_t minimumSize);
MCR_API int mcr_StringIndex_smartsize(struct mcr_StringIndex *indexPt,
	size_t increasingCount);
/*! Minimize allocated space.
 */
MCR_API void mcr_StringIndex_trim(struct mcr_StringIndex *indexPt);
MCR_API int mcr_StringIndex_resize(struct mcr_StringIndex *indexPt,
	size_t newSize);
/*! Remove all mapped key-value pairs.
 */
MCR_API void mcr_StringIndex_clear(struct mcr_StringIndex *indexPt);
/* */
/* Position/Values */
/* */
MCR_API mcr_String *mcr_StringIndex_string(struct mcr_StringIndex *indexPt,
	size_t index);
MCR_API const char *mcr_StringIndex_name(struct mcr_StringIndex *indexPt,
	size_t index);
MCR_API size_t mcr_StringIndex_index(struct mcr_StringIndex *indexPt,
	const char *strKey);
/* */
/* Set/remove mappings */
/* */
MCR_API int mcr_StringIndex_map(struct mcr_StringIndex *indexPt,
	size_t index, const char *strKey, const char **addKeys,
	size_t addCount);
MCR_API int mcr_StringIndex_add(struct mcr_StringIndex *indexPt,
	size_t index, const char **addKeys, size_t addCount);
MCR_API int mcr_StringIndex_reindex(struct mcr_StringIndex *indexPt,
	size_t curIndex, size_t newIndex);
MCR_API int mcr_StringIndex_remap(struct mcr_StringIndex *indexPt,
	const char *strKey, const char *newKey);
MCR_API void mcr_StringIndex_unmap(struct mcr_StringIndex *indexPt,
	size_t remIndex, bool flagRemoveAll);
MCR_API void mcr_StringIndex_unmap_string(struct mcr_StringIndex *indexPt,
	const char *remString, bool flagRemoveAll);

MCR_API void mcr_StringIndex_sort(struct mcr_StringIndex *indexPt);

#define MCR_STRINGINDEX_STRING(index, stringIndex) \
(MCR_STRINGSET_ELEMENT((index).set, stringIndex))

#endif
