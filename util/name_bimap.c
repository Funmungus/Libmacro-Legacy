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

#include "mcr/util/util.h"

void mcr_NameBiMap_init(void *dataPt)
{
	mcr_NameBiMap *mapPt = dataPt;
	dassert(dataPt);
	mcr_StringMap_init(&mapPt->first);
	mcr_NameMap_init(&mapPt->second);
}

void mcr_NameBiMap_free(void *dataPt)
{
	mcr_NameBiMap *mapPt = dataPt;
	dassert(mapPt);
	mcr_StringMap_free(&mapPt->first);
	mcr_NameMap_free(&mapPt->second);
}

void mcr_NameBiMap_set_all(mcr_NameBiMap * mapPt, size_t valueSize,
	mcr_compare_fnc valueCmp, const struct mcr_Interface *valueIface)
{
	dassert(mapPt);
	mcr_StringMap_set_all(&mapPt->first, valueSize, valueIface);
	mcr_NameMap_set_all(&mapPt->second, valueSize, valueCmp, valueIface);
}

int mcr_NameBiMap_map(mcr_NameBiMap * mapPt,
	const char *nameKeyValue, const char **nameKeyArr,
	size_t nameCount, const void *secondKeyValuePt)
{
	dassert(mapPt);
	return mcr_BiMap_map(mapPt, nameKeyValue ? &nameKeyValue : NULL,
		nameKeyArr, nameCount, secondKeyValuePt, NULL, 0);
}

int mcr_NameBiMap_addname(mcr_NameBiMap * mapPt,
	const char *nameKey, const void *secondValuePt)
{
	dassert(mapPt);
	return mcr_StringMap_map(&mapPt->first, nameKey, secondValuePt);
}

int mcr_NameBiMap_setvalue(mcr_NameBiMap * mapPt,
	const void *secondKeyPt, const char *nameValue)
{
	int ret;
	dassert(mapPt);
	if ((ret = mcr_NameMap_map(&mapPt->second, secondKeyPt, nameValue)))
		return ret;
	if (nameValue)
		return mcr_StringMap_map(&mapPt->first, nameValue, secondKeyPt);
	return 0;
}

void mcr_NameBiMap_remname(mcr_NameBiMap * mapPt,
	const char *remName, int removeAllOccurances)
{
	dassert(mapPt);
	dassert(remName);
	mcr_BiMap_remfirst(mapPt, &remName, removeAllOccurances);
}

void mcr_NameBiMap_remvalue(mcr_NameBiMap * mapPt,
	const void *remValuePt, int removeAllOccurances)
{
	dassert(mapPt);
	mcr_BiMap_remsecond(mapPt, remValuePt, removeAllOccurances);
}

mcr_String *mcr_NameBiMap_key(const mcr_NameBiMap * mapPt,
	const void *valueKeyPt)
{
	dassert(mapPt);
	return mcr_Map_value(&mapPt->second, valueKeyPt);
}

const char *mcr_NameBiMap_name(const mcr_NameBiMap * mapPt,
	const void *valueKeyPt)
{
	dassert(mapPt);
	mcr_String *ret = mcr_Map_value(&mapPt->second, valueKeyPt);
	return ret ? ret->array : NULL;
}

void *mcr_NameBiMap_value(const mcr_NameBiMap * mapPt, const char *nameKey)
{
	dassert(mapPt);
	return nameKey ? mcr_Map_value(&mapPt->first, &nameKey) : NULL;
}
