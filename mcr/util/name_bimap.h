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

#ifndef MCR_NAMEBIMAP_H
#define MCR_NAMEBIMAP_H

#include "mcr/util/string_map.h"
#include "mcr/util/name_map.h"
#include "mcr/util/bimap.h"

/*!
 * \brief \ref mcr_BiMap where first is \ref mcr_StringMap and the second
 * is \ref mcr_NameMap
 *
 * Used to map multiple names to each value.  The abstractions to map many
 * values to the same name are not given here.
 */
typedef struct mcr_BiMap mcr_NameBiMap;

MCR_API void mcr_NameBiMap_init(void *dataPt);
MCR_API void mcr_NameBiMap_free(void *dataPt);
#define mcr_NameBiMap_free_foreach(mapPt, ignore) \
mcr_NameBiMap_free(mapPt)
MCR_API void mcr_NameBiMap_set_all(mcr_NameBiMap * mapPt,
	size_t valueSize, mcr_compare_fnc valueCmp,
	const struct mcr_Interface *valueIface);
MCR_API int mcr_NameBiMap_map(mcr_NameBiMap * mapPt,
	const char *nameKeyValue, const char **nameKeyArr,
	size_t nameCount, const void *secondKeyValuePt);

MCR_API int mcr_NameBiMap_addname(mcr_NameBiMap * mapPt,
	const char *nameKey, const void *secondValuePt);
MCR_API int mcr_NameBiMap_setvalue(mcr_NameBiMap * mapPt,
	const void *secondKeyPt, const char *nameValue);

MCR_API void mcr_NameBiMap_remname(mcr_NameBiMap * mapPt,
	const char *remName, int removeAllOccurances);
MCR_API void mcr_NameBiMap_remvalue(mcr_NameBiMap * mapPt,
	const void *remValuePt, int removeAllOccurances);

MCR_API mcr_String *mcr_NameBiMap_key(const mcr_NameBiMap * mapPt,
	const void *valueKeyPt);
MCR_API const char *mcr_NameBiMap_name(const mcr_NameBiMap * mapPt,
	const void *valueKeyPt);
MCR_API void *mcr_NameBiMap_value(const mcr_NameBiMap * mapPt,
	const char *nameKey);

#endif
