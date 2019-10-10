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

struct mcr_Data mcr_Data_new(void *dataPt, void (*deallocate) (void *))
{
	struct mcr_Data ret = { 0 };
	if (dataPt)
		ret.data = dataPt;
	if (deallocate)
		ret.deallocate = deallocate;
	return ret;
}

int mcr_Interface_init(void *interfacePt)
{
	if (interfacePt) {
		memset(interfacePt, 0, sizeof(struct mcr_Interface));
		((struct mcr_Interface *)interfacePt)->id = (size_t)-1;
	}
	return 0;
}

struct mcr_Interface mcr_Interface_new(struct mcr_context *context,
									   size_t dataSize,
									   mcr_data_fnc init, mcr_data_fnc deinit, mcr_compare_fnc compare,
									   mcr_copy_fnc copy)
{
	struct mcr_Interface ret = { 0 };
	ret.context = context;
	ret.data_size = dataSize;
	if (init)
		ret.init = init;
	if (deinit)
		ret.deinit = deinit;
	if (compare)
		ret.compare = compare;
	if (copy)
		ret.copy = copy;
	return ret;
}

void mcr_Interface_set_all(void *interfacePt, struct mcr_context *context,
						   size_t dataSize,
						   mcr_data_fnc init, mcr_data_fnc deinit, mcr_compare_fnc compare,
						   mcr_copy_fnc copy)
{
	struct mcr_Interface *iPt = interfacePt;
	dassert(interfacePt);
	iPt->context = context;
	iPt->data_size = dataSize;
	iPt->init = init;
	iPt->deinit = deinit;
	iPt->compare = compare;
	iPt->copy = copy;
}

int mcr_iinit(const void *interfacePt, struct mcr_Data *dataPt)
{
	const struct mcr_Interface *iPt = interfacePt;
	int err;
	if (!dataPt)
		return 0;
	mcr_err = 0;
	if (!iPt) {
		dataPt->data = NULL;
		dataPt->deallocate = NULL;
		return 0;
	}
	if ((dataPt->data = malloc(iPt->data_size))) {
		dataPt->deallocate = free;
		if (iPt->init) {
			if ((err = iPt->init(dataPt->data)))
				mset_error_return(err);
		} else {
			memset(dataPt->data, 0, iPt->data_size);
		}
	} else {
		mset_error_return(ENOMEM);
	}
	return 0;
}

int mcr_iref_init(const void *const *interfacePtPt, struct mcr_Data *dataPt)
{
	return interfacePtPt ? mcr_iinit(*interfacePtPt, dataPt) :
		   mcr_iinit(NULL, dataPt);
}

int mcr_ideinit(const void *interfacePt, struct mcr_Data *dataPt)
{
	const struct mcr_Interface *iPt = interfacePt;
	int err;
	if (!dataPt)
		return 0;
	mcr_err = 0;
	if (iPt) {
		if (dataPt->data) {
			if (iPt->deinit && (err = iPt->deinit(dataPt->data)))
				mset_error_return(err);
			/* Set data null */
			MCR_DATA_FREE(*dataPt);
		}
	} else {
		MCR_DATA_FREE(*dataPt);
	}
	return 0;
}

int mcr_icmp(const void *interfacePt, const struct mcr_Data *lhs,
			 const struct mcr_Data *rhs)
{
	const struct mcr_Interface *iPt = interfacePt;
	bool ldata = lhs && lhs->data, rdata = rhs && rhs->data;
	dassert(iPt);
	if (ldata && rdata) {
		if (iPt->compare)
			return iPt->compare(lhs->data, rhs->data);
		return memcmp(lhs->data, rhs->data, iPt->data_size);
	}
	return MCR_CMP(ldata, rdata);
}

int mcr_icpy(const void *interfacePt, struct mcr_Data *dstPt,
			 const struct mcr_Data *srcPt)
{
	const struct mcr_Interface *iPt = interfacePt;
	int err;
	dassert(iPt);
	dassert(dstPt);
	/* Same object */
	if (srcPt->data == dstPt->data)
		return 0;
	/* Nothing to copy if data size is 0 */
	if (!iPt->data_size)
		return 0;
	mcr_err = 0;
	if (srcPt && srcPt->data) {
		/* Create an object to copy into, if it does not exist */
		if (!dstPt->data && (err = mcr_iinit(iPt, dstPt)))
			mset_error_return(err);
		if (iPt->copy)
			return iPt->copy(dstPt->data, srcPt->data);
		memcpy(dstPt->data, srcPt->data, iPt->data_size);
		return 0;
	}
	/* No source, only free */
	return mcr_ideinit(iPt, dstPt);
}

int mcr_iset_data(const void *interfacePt, struct mcr_Data *dataPt,
				  void *data, void (*deallocate) (void *))
{
	const struct mcr_Interface *iPt = interfacePt;
	int err;
	dassert(iPt);
	if (!dataPt)
		return 0;
	mcr_err = 0;
	/* Set new data */
	if (dataPt->data != data) {
		/* Free existing date */
		if (dataPt->data && iPt->deinit &&
			(err = iPt->deinit(dataPt->data))) {
			mset_error_return(err);
		}
		MCR_DATA_FREE(*dataPt);
		/* Set new */
		MCR_DATA_SET_ALL(*dataPt, data, deallocate);
	}
	return 0;
}

int mcr_ireset(const void *interfacePt, struct mcr_Data *dataPt)
{
	const struct mcr_Interface *iPt = interfacePt;
	int err;
	dassert(iPt);
	if (!dataPt)
		return 0;
	mcr_err = 0;
	if (dataPt->data) {
		if (iPt->deinit && (err = iPt->deinit(dataPt->data)))
			mset_error_return(err);
		if (iPt->init) {
			if ((err = iPt->init(dataPt->data)))
				mset_error_return(err);
		} else {
			memset(dataPt->data, 0, iPt->data_size);
		}
	} else if ((err = mcr_iinit(interfacePt, dataPt))) {
		mset_error_return(err);
	}
	return 0;
}
