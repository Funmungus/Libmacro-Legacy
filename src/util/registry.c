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

#include "mcr/util/util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mcr_reg_init(void *regPt)
{
	struct mcr_IRegistry *iRegPt = regPt;
	if (iRegPt) {
		mcr_Array_init(&iRegPt->iset);
		mcr_StringSet_init(&iRegPt->names);
		mcr_Map_init(&iRegPt->name_map);
		iRegPt->iset.element_size = sizeof(void *);
		mcr_Map_set_all(&iRegPt->name_map, sizeof(mcr_String),
						sizeof(void *), NULL, mcr_String_interface(), NULL);
	}
	return 0;
}

struct mcr_IRegistry mcr_reg_new()
{
	struct mcr_IRegistry ret;
	mcr_reg_init(&ret);
	return ret;
}

int mcr_reg_deinit(void *regPt)
{
	struct mcr_IRegistry *iRegPt = regPt;
	if (iRegPt) {
		mcr_Array_deinit(&iRegPt->iset);
		mcr_StringSet_deinit(&iRegPt->names);
		mcr_Map_deinit(&iRegPt->name_map);
	}
	return 0;
}

int mcr_register(struct mcr_IRegistry *iRegPt, void *interfacePt,
				 const char *name, const char **addNames, size_t bufferLen)
{
	size_t id = iRegPt->iset.used;
	dassert(iRegPt);
	dassert(interfacePt);
	if (mcr_Array_append(&iRegPt->iset, &interfacePt, 1))
		return mcr_err;
	/* If successful, we can set the id. */
	((struct mcr_Interface *)interfacePt)->id = id;
	return mcr_reg_set_names(iRegPt, interfacePt, name, addNames,
							 bufferLen);
}

void *mcr_reg_from_id(const struct mcr_IRegistry *iRegPt, size_t typeId)
{
	void **interfacePtPt;
	if (!iRegPt || typeId == (size_t) - 1)
		return NULL;
	if (typeId >= iRegPt->iset.used) {
		mset_error(EFAULT);
		return NULL;
	}
	interfacePtPt = MCR_ARR_ELEMENT(iRegPt->iset, typeId);
	return interfacePtPt ? *interfacePtPt : NULL;
}

void *mcr_reg_from_name(const struct mcr_IRegistry *iRegPt,
						const char *typeName)
{
	void *retPt;
	if (!iRegPt || !typeName)
		return NULL;
	retPt = MCR_MAP_ELEMENT((iRegPt)->name_map, &typeName);
	retPt = MCR_MAP_VALUEOF((iRegPt)->name_map, retPt);
	return retPt ? *(void **)retPt : NULL;
}

const char *mcr_reg_name(const struct mcr_IRegistry *iRegPt, size_t id)
{
	return iRegPt ? iRegPt->names.used ?
		   mcr_StringSet_element(&iRegPt->names, id)->array : NULL : NULL;
}

int mcr_reg_set_name(struct mcr_IRegistry *iRegPt,
					 void *interfacePt, const char *name)
{
	struct mcr_Interface *iPt = interfacePt;
	mcr_String *namePt;
	dassert(iRegPt);
	dassert(interfacePt);
	dassert(iPt->id != (size_t) ~ 0);
	if (!name)
		return 0;
	if (iPt->id >= iRegPt->names.used) {
		if (mcr_StringSet_minused(&iRegPt->names, iPt->id + 1))
			return mcr_err;
	}
	namePt = mcr_StringSet_element(&iRegPt->names, iPt->id);
	if (mcr_String_replace(namePt, name))
		return mcr_err;
	return mcr_reg_add_name(iRegPt, interfacePt, name);
}

int mcr_reg_add_name(struct mcr_IRegistry *iRegPt, void *interfacePt,
					 const char *name)
{
	dassert(iRegPt);
	dassert(interfacePt);
	if (!name)
		return 0;
	return mcr_Map_map(&iRegPt->name_map, (void *)&name, &interfacePt);
}

int mcr_reg_add_names(struct mcr_IRegistry *iRegPt, void *interfacePt,
					  const char **names, size_t bufferLen)
{
	size_t i;
	dassert(iRegPt);
	dassert(interfacePt);
	if (!names || !bufferLen)
		return 0;
	for (i = 0; i < bufferLen; i++) {
		if (mcr_Map_map(&iRegPt->name_map, (void *)(names + i),
						&interfacePt)) {
			return mcr_err;
		}
	}
	return 0;
}

int mcr_reg_set_names(struct mcr_IRegistry *iRegPt,
					  void *interfacePt, const char *name, const char **names,
					  size_t bufferLen)
{
	if (mcr_reg_set_name(iRegPt, interfacePt, name) ||
		mcr_reg_add_names(iRegPt, interfacePt, names, bufferLen))
		return mcr_err;
	return 0;
}

int mcr_reg_rename(struct mcr_IRegistry *iRegPt, const char *oldName,
				   const char *newName)
{
	void *interfacePt;
	dassert(iRegPt);
	if (oldName == newName)
		return 0;
	if (newName) {
		if (!(interfacePt = mcr_reg_from_name(iRegPt, oldName))) {
			interfacePt = mcr_reg_from_name(iRegPt, newName);
			if (interfacePt) {
				return mcr_reg_set_name(iRegPt, interfacePt,
										NULL);
			}
			return 0;
		}
		if (mcr_reg_set_name(iRegPt, interfacePt, newName))
			return mcr_err;
	}
	interfacePt = mcr_reg_from_name(iRegPt, oldName);
	if (interfacePt)
		return mcr_reg_set_name(iRegPt, interfacePt, NULL);
	return 0;
}

size_t mcr_reg_count(const struct mcr_IRegistry * iRegPt)
{
	return iRegPt ? iRegPt->iset.used : 0;
}

void mcr_reg_trim(struct mcr_IRegistry *iRegPt)
{
	dassert(iRegPt);
	mcr_Array_trim(&iRegPt->iset);
	mcr_StringSet_trim(&iRegPt->names);
	mcr_Map_trim(&iRegPt->name_map);
}

void mcr_reg_clear(struct mcr_IRegistry *iRegPt)
{
	dassert(iRegPt);
	mcr_Array_clear(&iRegPt->iset);
	mcr_StringSet_clear(&iRegPt->names);
	mcr_Map_clear(&iRegPt->name_map);
}
