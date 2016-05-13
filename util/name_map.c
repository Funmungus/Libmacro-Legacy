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

void mcr_NameMap_init(void *dataPt)
{
	mcr_StringMap *mapPt = dataPt;
	mcr_Map_init(dataPt);
	mcr_Map_set_all(mapPt, 1, sizeof(mcr_String), mcr_char_compare, NULL,
		mcr_String_iface());
}

void mcr_NameMap_set_all(mcr_NameMap * mapPt, size_t keySize,
	mcr_compare_fnc compareFnc, const struct mcr_Interface *keyIface)
{
	dassert(mapPt);
	mcr_Map_set_all(mapPt, keySize, sizeof(mcr_String), compareFnc,
		keyIface, mcr_String_iface());
}

mcr_String *mcr_NameMap_valueof(const mcr_NameMap * mapPt, const void *pairPt)
{
	dassert(mapPt);
	return mcr_Map_valueof(mapPt, pairPt);
}

void mcr_NameMap_set_valueof(const mcr_NameMap * mapPt,
	void *pairPt, const char *strValue)
{
	dassert(mapPt);
	mcr_Map_set_valueof(mapPt, pairPt, strValue ? &strValue : NULL);
}

mcr_String *mcr_NameMap_value(const mcr_NameMap * mapPt, const void *keyPt)
{
	dassert(mapPt);
	return mcr_Map_value(mapPt, keyPt);
}

const char *mcr_NameMap_name(const mcr_NameMap * mapPt, const void *keyPt)
{
	mcr_String *ret = mcr_Map_value(mapPt, keyPt);
	return ret ? ret->array : NULL;
}

int mcr_NameMap_map(mcr_NameMap * mapPt, const void *keyPt,
	const char *valueStr)
{
	dassert(mapPt);
	if (valueStr)
		return mcr_Map_map(mapPt, keyPt, &valueStr);
	mcr_Map_unmap(mapPt, keyPt);
	return 0;
}

int mcr_NameMap_nmap(mcr_NameMap * mapPt, const void *keyPt,
	const char *valueStr, size_t valueLen)
{
	mcr_String mem;
	int ret;
	dassert(mapPt);
	mcr_String_init(&mem);
	if ((ret = mcr_String_nreplace(&mem, valueStr, valueLen)))
		return ret;
	ret = mcr_Map_map(mapPt, keyPt, &mem);
	mcr_String_free(&mem);
	return ret;
}

void mcr_NameMap_unmap_value(mcr_NameMap * mapPt, const char *valueStr)
{
	dassert(mapPt);
	if (valueStr)
		mcr_Map_unmap_value(mapPt, &valueStr);
}

int mcr_NameMap_fill(mcr_NameMap * mapPt,
	const void *keyArray, size_t keyCount, const char *valueStr)
{
	dassert(mapPt);
	return mcr_Map_fill(mapPt, keyArray, keyCount, valueStr ?
		&valueStr : NULL);
}

int mcr_NameMap_graph(mcr_NameMap * mapPt,
	const void *keyArray, const char **valueStrArray, size_t sourceArrayLen)
{
	const char *keyIt = keyArray;
	size_t keyBytes = mapPt->key_size, i;
	int ret;
	dassert(mapPt);
	for (i = 0; i < sourceArrayLen; i++) {
		if ((ret = mcr_Map_map(mapPt, keyIt, valueStrArray + i)))
			return ret;
		keyIt += keyBytes;
	}
	return 0;
}
