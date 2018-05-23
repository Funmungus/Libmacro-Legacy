/* Libmacro - A multi-platform, extendable macro and hotkey C library
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

int mcr_Instance_init(void *instPt)
{
	if (instPt)
		memset(instPt, 0, sizeof(struct mcr_Instance));
	return 0;
}

struct mcr_Instance mcr_Instance_new(void *iPt, void *dataPt,
									 void (*deallocate) (void *))
{
	struct mcr_Instance ret;
	mcr_Instance_init(&ret);
	mcr_Instance_set_all(&ret, iPt, dataPt, deallocate);
	return ret;
}

int mcr_Instance_deinit(void *instPt)
{
	struct mcr_Instance *localPt = instPt;
	if (localPt)
		return mcr_ideinit(localPt->interface, &localPt->data);
	return 0;
}

int mcr_Instance_set_all(void *instPt, void *iPt, void *dataPt,
						 void (*deallocate) (void *))
{
	struct mcr_Instance *localPt = instPt;
	int err;
	dassert(localPt);
	if ((err = mcr_Instance_set_interface(localPt, iPt)))
		return err;
	return mcr_Instance_set_data(instPt, dataPt, deallocate);
}

int mcr_Instance_set_interface(void *instPt, void *iPt)
{
	struct mcr_Instance *localPt = instPt;
	int err;
	dassert(localPt);
	if ((err = mcr_Instance_deinit(localPt)))
		return err;
	localPt->interface = iPt;
	return 0;
}

int mcr_Instance_set_data(void *instPt, void *dataPt,
						  void (*deallocate) (void *))
{
	struct mcr_Instance *localPt = instPt;
	dassert(localPt);
	if (localPt->interface) {
		return mcr_iset_data(localPt->interface, &localPt->data, dataPt,
							 deallocate);
	} else {
		if (mcr_Instance_data(localPt)) {
			MCR_DATA_FREE(localPt->data);
		}
		MCR_DATA_SET_ALL(localPt->data, dataPt, deallocate);
	}
	return 0;
}

int mcr_Instance_compare(const void *lhsPt, const void *rhsPt)
{
	const struct mcr_Instance *lhsInst = lhsPt, *rhsInst = rhsPt;
	if (rhsInst && rhsInst->interface) {
		if (lhsInst && lhsInst->interface) {
			if (lhsInst->interface == rhsInst->interface)
				return mcr_icmp(lhsInst->interface, lhsPt,
								rhsPt);
			return MCR_CMP(mcr_iid(lhsInst->interface),
						   mcr_iid(rhsInst->interface));
		}
		return -1;
	}
	return lhsInst && lhsInst->interface;
}

int mcr_Instance_copy(void *dstPt, const void *srcPt)
{
	struct mcr_Instance *dstInst = dstPt;
	const struct mcr_Instance *srcInst = srcPt;
	int err;
	dassert(dstInst);
	if (dstInst == srcInst)
		return 0;
	if (!srcInst || !srcInst->interface)
		return mcr_Instance_deinit(dstPt);
	/* Interface exists, make sure it is the same one */
	if ((err = mcr_Instance_set_interface(dstPt, srcInst->interface)))
		return err;
	dassert(dstInst->interface == srcInst->interface);
	return mcr_icpy(srcInst->interface, &dstInst->data, &srcInst->data);
}

int mcr_Instance_reset(void *instPt)
{
	struct mcr_Instance *localPt = instPt;
	dassert(localPt);
	if (localPt->interface)
		return mcr_ireset(localPt->interface, &localPt->data);
	MCR_DATA_FREE(localPt->data);
	return 0;
}
