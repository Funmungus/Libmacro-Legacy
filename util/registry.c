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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mcr_reg_init(void *dataPt)
{
	struct mcr_IRegistry *iRegPt = dataPt;
	dassert(iRegPt);
	mcr_Array_init(&iRegPt->iset);
	mcr_Array_init(&iRegPt->names);
	mcr_StringMap_init(&iRegPt->name_map);
	iRegPt->iset.element_size = sizeof(void *);
	iRegPt->names.element_size = sizeof(mcr_String);
	iRegPt->name_map.value_size = sizeof(void *);
}

void mcr_reg_free(void *dataPt)
{
	struct mcr_IRegistry *iRegPt = dataPt;
	dassert(iRegPt);
	mcr_Array_free(&iRegPt->iset);
	MCR_ARR_FOR_EACH(iRegPt->names, MCR_EXP(mcr_Array_free_foreach),);
	mcr_Array_free(&iRegPt->names);
	mcr_StringMap_free(&iRegPt->name_map);
}

int mcr_register(struct mcr_IRegistry *iRegPt, void *ifacePt,
	const char *name, const char **addNames, size_t bufferLen)
{
	size_t id = iRegPt->iset.used;
	int err;
	dassert(iRegPt);
	dassert(ifacePt);
	if ((err = mcr_Array_append(&iRegPt->iset, &ifacePt, 1, 0)))
		return err;
	/* If successful, we can set the id. */
	((struct mcr_Interface *)ifacePt)->id = id;
	if ((err = mcr_reg_set_names(iRegPt, ifacePt, name, addNames,
				bufferLen)))
		return err;
	return 0;
}

void *mcr_reg_from_id(const struct mcr_IRegistry *iRegPt, size_t typeId)
{
	void **ifacePtPt;
	dassert(iRegPt);
	if (typeId >= iRegPt->iset.used) {
		mset_error(EFAULT);
		return NULL;
	}
	ifacePtPt = MCR_ARR_ELEMENT(iRegPt->iset, typeId);
	return ifacePtPt ? *ifacePtPt : NULL;
}

void *mcr_reg_from_name(const struct mcr_IRegistry *iRegPt,
	const char *typeName)
{
	void *retPt;
	dassert(iRegPt);
	if (!typeName) {
		mset_error(EINVAL);
		return NULL;
	}
	retPt = MCR_STRINGMAP_ELEMENT((iRegPt)->name_map, typeName);
	retPt = MCR_STRINGMAP_VALUEOF((iRegPt)->name_map, retPt);
	return retPt ? *(void **)retPt : NULL;
}

const char *mcr_reg_name(const struct mcr_IRegistry *iRegPt, size_t id)
{
	return iRegPt ? iRegPt->names.used ?
		((mcr_String *) MCR_ARR_ELEMENT(iRegPt->names, id))->array :
		NULL : NULL;
}

int mcr_reg_set_name(struct mcr_IRegistry *iRegPt,
	const void *ifacePt, const char *name)
{
	const struct mcr_Interface *iPt = ifacePt;
	int ret;
	struct mcr_Array initial, *namePt;
	dassert(iRegPt);
	dassert(ifacePt);
	dassert(iPt->id != (size_t) ~ 0);
	if (iPt->id >= iRegPt->names.used) {
		mcr_String_init(&initial);
		if ((ret = mcr_Array_minfill(&iRegPt->names, iPt->id + 1,
					&initial)))
			return ret;
	}
	namePt = MCR_ARR_ELEMENT(iRegPt->names, iPt->id);
	if ((ret = mcr_String_replace(namePt, name)))
		return ret;
	return mcr_reg_add_name(iRegPt, ifacePt, name);
}

int mcr_reg_add_name(struct mcr_IRegistry *iRegPt, const void *ifacePt,
	const char *name)
{
	int ret;
	dassert(iRegPt);
	dassert(ifacePt);
	if (!name)
		return 0;
	if ((ret = mcr_StringMap_map(&iRegPt->name_map, name, &ifacePt)))
		return ret;
	return 0;
}

int mcr_reg_add_names(struct mcr_IRegistry *iRegPt, const void *ifacePt,
	const char **names, size_t bufferLen)
{
	dassert(iRegPt);
	dassert(ifacePt);
	if (!names || !bufferLen)
		return 0;
	return mcr_StringMap_fill(&iRegPt->name_map, names, bufferLen,
		&ifacePt);
}

int mcr_reg_set_names(struct mcr_IRegistry *iRegPt,
	const void *ifacePt, const char *name, const char **names,
	size_t bufferLen)
{
	int ret = mcr_reg_set_name(iRegPt, ifacePt, name);
	if (!ret)
		ret = mcr_reg_add_names(iRegPt, ifacePt, names, bufferLen);
	return ret;
}

int mcr_reg_rename(struct mcr_IRegistry *iRegPt, const char *oldName,
	const char *newName)
{
	void *ifacePt = mcr_reg_from_name(iRegPt, oldName);
	dassert(iRegPt);
	dassert(oldName);
	/* Not an error, just nothing to rename */
	if (!ifacePt)
		return 0;
	/* Remove old name, set new. */
	mcr_StringMap_unmap(&iRegPt->name_map, oldName);
	return mcr_reg_set_name(iRegPt, ifacePt, newName);
}

size_t mcr_reg_count(const struct mcr_IRegistry * iRegPt)
{
	return iRegPt ? iRegPt->iset.used : 0;
}

void mcr_reg_all(struct mcr_IRegistry *iRegPt, void **ifacePtBuffer,
	size_t bufferLength)
{
	size_t avail = iRegPt->iset.used;
	dassert(iRegPt);
	dassert(ifacePtBuffer);
	if (avail)
		memcpy(ifacePtBuffer, iRegPt->iset.array,
			bufferLength < avail ? bufferLength * sizeof(void *) :
			avail * sizeof(void *));
}

void mcr_reg_trim(struct mcr_IRegistry *iRegPt)
{
	dassert(iRegPt);
	mcr_Array_trim(&iRegPt->iset);
	mcr_Array_trim(&iRegPt->names);
	mcr_StringMap_trim(&iRegPt->name_map);
}

void mcr_reg_clear(struct mcr_IRegistry *iRegPt)
{
	dassert(iRegPt);
	mcr_Array_clear(&iRegPt->iset);
	MCR_ARR_FOR_EACH(iRegPt->names, mcr_String_free_foreach,);
	mcr_Array_clear(&iRegPt->names);
	mcr_StringMap_clear(&iRegPt->name_map);
}
