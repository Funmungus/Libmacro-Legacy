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

void mcr_iinit(void *ifaceDataPt)
{
	if (ifaceDataPt) {
		memset(ifaceDataPt, 0, sizeof(struct mcr_Interface));
		((struct mcr_Interface *)ifaceDataPt)->id = ~0;
	}
}

void mcr_iset_all(void *ifaceDataPt,
	mcr_compare_fnc compare,
	mcr_copy_fnc copy,
	size_t dataSize, mcr_data_fnc init, mcr_data_fnc free)
{
	struct mcr_Interface *iPt = ifaceDataPt;
	dassert(ifaceDataPt);
	iPt->compare = compare;
	iPt->copy = copy;
	iPt->data_size = dataSize;
	iPt->init = init;
	iPt->free = free;
}

struct mcr_Data mcr_imkdata(const void *ifaceDataPt)
{
	const struct mcr_Interface *iPt = ifaceDataPt;
	struct mcr_Data data = { 0 };
	if (iPt) {
		MCR_IMKDATA(*iPt, data);
	}
	return data;
}

struct mcr_Data mcr_iref_mkdata(const void *const *ifaceDataPtPt)
{
	return ifaceDataPtPt ? mcr_imkdata(*ifaceDataPtPt) : mcr_imkdata(NULL);
}

void mcr_ifree(const void *ifaceDataPt, struct mcr_Data *dataPt)
{
	const struct mcr_Interface *iPt = ifaceDataPt;
	if (iPt && dataPt) {
		MCR_IFREE(*iPt, *dataPt);
		dataPt->data = NULL;
	}
}

int mcr_icpy(const void *ifaceDataPt, struct mcr_Data *dstPt,
	struct mcr_Data *srcPt)
{
	const struct mcr_Interface *iPt = ifaceDataPt;
	dassert(iPt);
	dassert(dstPt);
	if (srcPt->data == dstPt->data)
		return 0;
	if (srcPt && srcPt->data) {
		if (!dstPt->data) {
			MCR_IMKDATA(*iPt, *dstPt);
			if (!dstPt->data) {
				mset_error(ENOMEM);
				return -ENOMEM;
			}
		}
		if (iPt->copy)
			return iPt->copy(dstPt->data, srcPt->data);
		else
			memcpy(dstPt->data, srcPt->data, iPt->data_size);
	} else {
		MCR_IFREE(*iPt, *dstPt);
		dstPt->data = NULL;
	}
	return 0;
}

int mcr_icmp(const void *ifaceDataPt, const struct mcr_Data *lhs,
	const struct mcr_Data *rhs)
{
	const struct mcr_Interface *iPt = ifaceDataPt;
	dassert(iPt);
	if (rhs) {
		if (lhs) {
			return MCR_ICMP(*iPt, *lhs, *rhs);
		}
		return -1;
	}
	return ! !lhs;
}

void mcr_iset_data(const void *ifaceDataPt, struct mcr_Data *dataPt,
	void *data, mcr_data_fnc deallocate)
{
	const struct mcr_Interface *iPt = ifaceDataPt;
	dassert(iPt);
	if (dataPt) {
		MCR_ISET_DATA(*iPt, *dataPt, data, deallocate);
	}
}
