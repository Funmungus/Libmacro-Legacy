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

#include "mcr/standard/standard.h"
#include "mcr/standard/private.h"
#include MCR_STANDARD_NATIVE_INC
#include "mcr/modules.h"

void mcr_Scroll_dimensions(const struct mcr_Scroll *scrollPt,
	mcr_Dimensions buffer)
{
	dassert(scrollPt);
	MCR_SCROLL_DIMENSIONS(*scrollPt, buffer);
}

void mcr_Scroll_set_dimensions(struct mcr_Scroll *scrollPt,
	const mcr_Dimensions pos)
{
	dassert(scrollPt);
	MCR_SCROLL_SET_DIMENSIONS(*scrollPt, pos);
}

long long mcr_Scroll_coordinate(const struct mcr_Scroll *scrollPt,
	int coordinate)
{
	dassert(scrollPt);
	return MCR_SCROLL_COORDINATE(*scrollPt, coordinate);
}

void mcr_Scroll_set_coordinate(struct mcr_Scroll *scrollPt, int coordinate,
	long long value)
{
	dassert(scrollPt);
	MCR_SCROLL_SET_COORDINATE(*scrollPt, coordinate, value);
}

int mcr_Scroll_send(struct mcr_Signal *signalData)
{
	dassert(signalData);
	struct mcr_Scroll *scrPt = mcr_Scroll_data(signalData);
	return scrPt ? mcr_Scroll_send_data(scrPt) : 0;
}

int mcr_Scroll_compare(const void *lhs, const void *rhs)
{
	long long l, r;
	int ret, i;
	if (rhs) {
		if (lhs) {
			const struct mcr_Scroll *lPt = lhs, *rPt = rhs;
			for (i = MCR_DIMENSION_CNT; i--;) {
				l = MCR_SCROLL_COORDINATE(*lPt, i);
				r = MCR_SCROLL_COORDINATE(*rPt, i);
				if ((ret = MCR_CMP_INTEGRAL(l, r)))
					return ret;
			}
			return ret;
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_Scroll_copy(void *dstPt, void *srcPt)
{
	dassert(dstPt);
	struct mcr_Scroll *dPt = dstPt, *sPt = srcPt;
	int i;
	if (sPt) {
		for (i = MCR_DIMENSION_CNT; i--;) {
			MCR_SCROLL_SET_COORDINATE(*dPt, i,
				MCR_SCROLL_COORDINATE(*sPt, i));
		}
	} else {
		for (i = MCR_DIMENSION_CNT; i--;) {
			MCR_SCROLL_SET_COORDINATE(*dPt, i, 0);
		}
	}
	return 0;
}

struct mcr_ISignal *mcr_iScroll(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.iscroll;
}
