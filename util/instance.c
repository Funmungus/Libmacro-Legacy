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
#include <stdlib.h>
#include <string.h>

void mcr_inst_init(void *instDataPt)
{
	memset(instDataPt, 0, sizeof(struct mcr_Instance));
}

void mcr_inst_free(void *instDataPt)
{
	struct mcr_Instance *instPt = instDataPt;
	if (instPt) {
		MCR_INST_FREE(*instPt);
	}
}

void mcr_inst_set_all(void *instDataPt, void *iDataPt, void *dataPt,
	mcr_data_fnc deallocate)
{
	struct mcr_Instance *instPt = instDataPt;
	if (instPt) {
		mcr_inst_free(instDataPt);
		instPt->iface = iDataPt;
		MCR_DATA_SET_ALL(instPt->data, dataPt, deallocate);
	}
}

void mcr_inst_set_iface(void *instDataPt, void *iDataPt)
{
	struct mcr_Instance *instPt = instDataPt;
	if (instPt) {
		mcr_inst_free(instDataPt);
		instPt->iface = iDataPt;
	}
}

void mcr_inst_set_data(void *instDataPt, void *dataPt, mcr_data_fnc deallocate)
{
	struct mcr_Instance *instPt = instDataPt;
	if (instPt) {
		MCR_INST_SET_DATA(*instPt, dataPt, deallocate);
	}
}

void mcr_inst_reset(void *instDataPt)
{
	struct mcr_Instance *instPt = instDataPt;
	if (instPt) {
		MCR_INST_RESET(*instPt);
	}
}

int mcr_inst_compare(const void *lhsPt, const void *rhsPt)
{
	const struct mcr_Instance *lhsInst = lhsPt, *rhsInst = rhsPt;
	if (rhsInst) {
		if (lhsInst) {
			if (lhsInst == rhsInst)
				return 0;
			return MCR_INST_CMP(*lhsInst, *rhsInst);
		}
		return -1;
	}
	return ! !lhsInst;
}

int mcr_inst_copy(void *dstPt, void *srcPt)
{
	struct mcr_Instance *dstInst = dstPt, *srcInst = srcPt;
	dassert(dstInst);
	if (dstInst == srcInst)
		return 0;
	if (!srcInst || !srcInst->iface) {
		MCR_INST_FREE(*dstInst);
		dstInst->iface = NULL;
		return 0;
	}
	if (dstInst->iface != srcInst->iface) {
		MCR_INST_FREE(*dstInst);
		dstInst->iface = srcInst->iface;
	}
	dassert(dstInst->iface == srcInst->iface);
	return mcr_icpy(srcInst->iface, &dstInst->data, &srcInst->data);
}
