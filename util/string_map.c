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

void mcr_StringMap_init(void *dataPt)
{
	if (!dataPt)
		return;
	mcr_Map_init(dataPt);
	mcr_Map_set_all(dataPt, sizeof(mcr_String), 1, mcr_name_compare,
		mcr_String_iface(), NULL);
}

void mcr_StringMap_set_all(mcr_StringMap * mapPt, size_t valueSize,
	const struct mcr_Interface *valueIface)
{
	mcr_Map_set_all(mapPt, sizeof(mcr_String), valueSize,
		mcr_name_compare, mcr_String_iface(), valueIface);
}

int mcr_StringMap_set_key(const mcr_StringMap * mapPt,
	void *pairPt, const char *keyStr)
{
	dassert(mapPt);
	dassert(pairPt);
	return mcr_Map_set_key(mapPt, pairPt, keyStr ? &keyStr : NULL);
}

mcr_String *mcr_StringMap_element(const mcr_StringMap * mapPt,
	const char *keyStr)
{
	dassert(mapPt);
	return keyStr ? mcr_Map_element(mapPt, &keyStr) : NULL;
}

void *mcr_StringMap_value(const mcr_StringMap * mapPt, const char *keyStr)
{
	dassert(mapPt);
	return keyStr ? mcr_Map_value(mapPt, &keyStr) : NULL;
}

mcr_String *mcr_StringMap_element_ensured(mcr_StringMap * mapPt,
	const char *keyStr)
{
	dassert(mapPt);
	dassert(keyStr);
	return mcr_Map_element_ensured(mapPt, &keyStr);
}

size_t mcr_StringMap_index(const mcr_StringMap * mapPt, const char *keyStr)
{
	dassert(mapPt);
	return keyStr ? mcr_Map_index(mapPt, &keyStr) : (size_t) ~ 0;
}

int mcr_StringMap_map(mcr_StringMap * mapPt, const char *strKey,
	const void *valuePt)
{
	dassert(mapPt);
	dassert(strKey);
	return mcr_Map_map(mapPt, &strKey, valuePt);
}

int mcr_StringMap_nmap(mcr_StringMap * mapPt, const char *strKey,
	size_t strLen, const void *valuePt)
{
	mcr_String key;
	int ret;
	dassert(mapPt);
	dassert(strKey);
	mcr_String_init(&key);
	if ((ret = mcr_String_nreplace(&key, strKey, strLen)))
		return ret;
	ret = mcr_Map_map(mapPt, &key, valuePt);
	mcr_String_free(&key);
	return ret;
}

int mcr_StringMap_remap(mcr_StringMap * mapPt, const char *key,
	const char *newKey)
{
	dassert(mapPt);
	dassert(key);
	if (newKey)
		return mcr_Map_remap(mapPt, &key, &newKey);
	mcr_Map_unmap(mapPt, &key);
	return 0;
}

void mcr_StringMap_unmap(mcr_StringMap * mapPt, const char *key)
{
	dassert(mapPt);
	dassert(key);
	mcr_Map_unmap(mapPt, &key);
}

int mcr_StringMap_fill(mcr_StringMap * mapPt,
	const char **keyStrArray, size_t keyCount, const void *valuePt)
{
	int ret;
	size_t i;
	dassert(mapPt);
	if (!keyStrArray)
		return 0;
	for (i = 0; i < keyCount; i++) {
		if ((ret = mcr_Map_map(mapPt, keyStrArray + i, valuePt)))
			return ret;
	}
	return 0;
}

int mcr_StringMap_graph(mcr_StringMap * mapPt,
	const char **keyStrArray, const void *valueArray, size_t sourceArrayLen)
{
	int ret;
	const char *valIt;
	size_t valBytes, i;
	dassert(mapPt);
	if (!keyStrArray || !valueArray)
		return 0;
	valIt = valueArray;
	valBytes = mapPt->value_size;
	for (i = 0; i < sourceArrayLen; i++) {
		if ((ret = mcr_Map_map(mapPt, keyStrArray + i, valIt)))
			return ret;
		valIt += valBytes;
	}
	return 0;
}
