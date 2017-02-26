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

/* init * send_data native */

void mcr_MoveCursor_set_all(struct mcr_MoveCursor *mcPt,
	const mcr_SpacePosition pos, bool flagJustify)
{
	dassert(mcPt);
	MCR_MOVECURSOR_SET_POSITION(*mcPt, pos);
	MCR_MOVECURSOR_SET_JUSTIFY(*mcPt, flagJustify);
}

void mcr_MoveCursor_position(const struct mcr_MoveCursor *mcPt,
	mcr_SpacePosition buffer)
{
	dassert(mcPt);
	MCR_MOVECURSOR_POSITION(*mcPt, buffer);
}

void mcr_MoveCursor_set_position(struct mcr_MoveCursor *mcPt,
	const mcr_SpacePosition pos)
{
	dassert(mcPt);
	MCR_MOVECURSOR_SET_POSITION(*mcPt, pos);
}

long long mcr_MoveCursor_coordinate(const struct mcr_MoveCursor *mcPt,
	int coordinate)
{
	dassert(mcPt);
	return MCR_MOVECURSOR_COORDINATE(*mcPt, coordinate);
}

void mcr_MoveCursor_set_coordinate(struct mcr_MoveCursor *mcPt, int coordinate,
	long long value)
{
	dassert(mcPt);
	MCR_MOVECURSOR_SET_COORDINATE(*mcPt, coordinate, value);
}

bool mcr_MoveCursor_is_justify(const struct mcr_MoveCursor *mcPt)
{
	dassert(mcPt);
	return MCR_MOVECURSOR_IS_JUSTIFY(*mcPt);
}

void mcr_MoveCursor_set_justify(struct mcr_MoveCursor *mcPt, bool flagJustify)
{
	dassert(mcPt);
	MCR_MOVECURSOR_SET_JUSTIFY(*mcPt, flagJustify);
}

int mcr_MoveCursor_send(struct mcr_Signal *signalData)
{
	dassert(signalData);
	mcr_MC *mcPt = mcr_MC_data(signalData);
	return mcPt ? mcr_MoveCursor_send_data(mcPt) : 0;
}

int mcr_MoveCursor_compare(const void *lhs, const void *rhs)
{
	long long l, r;
	int ret, i;
	if (rhs) {
		if (lhs) {
			const struct mcr_MoveCursor *lPt = lhs, *rPt = rhs;
			l = MCR_MOVECURSOR_IS_JUSTIFY(*lPt);
			r = MCR_MOVECURSOR_IS_JUSTIFY(*rPt);
			if ((ret = MCR_CMP_INTEGRAL(l, r)))
				return ret;
			for (i = MCR_DIMENSION_CNT; i--;) {
				l = MCR_MOVECURSOR_COORDINATE(*lPt, i);
				r = MCR_MOVECURSOR_COORDINATE(*rPt, i);
				if ((ret = MCR_CMP_INTEGRAL(l, r)))
					return ret;
			}
			return ret;
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_MoveCursor_copy(void *dstPt, void *srcPt)
{
	dassert(dstPt);
	struct mcr_MoveCursor *dPt = dstPt, *sPt = srcPt;
	int i;
	if (sPt) {
		MCR_MOVECURSOR_SET_JUSTIFY(*dPt,
			MCR_MOVECURSOR_IS_JUSTIFY(*sPt));
		for (i = MCR_DIMENSION_CNT; i--;) {
			MCR_MOVECURSOR_SET_COORDINATE(*dPt, i,
				MCR_MOVECURSOR_COORDINATE(*sPt, i));
		}
	} else {
		MCR_MOVECURSOR_SET_JUSTIFY(*dPt, 0);
		for (i = MCR_DIMENSION_CNT; i--;) {
			MCR_MOVECURSOR_SET_COORDINATE(*dPt, i, 0);
		}
	}
	return 0;
}

bool mcr_resembles(const struct mcr_MoveCursor * lhs,
	const struct mcr_MoveCursor * rhs, const unsigned int measurementError)
{
	dassert(lhs);
	dassert(rhs);
	if (MCR_MOVECURSOR_IS_JUSTIFY(*lhs) != MCR_MOVECURSOR_IS_JUSTIFY(*rhs))
		return 0;
	mcr_SpacePosition me, them;
	MCR_MOVECURSOR_POSITION(*lhs, me);
	MCR_MOVECURSOR_POSITION(*rhs, them);

	if (MCR_MOVECURSOR_IS_JUSTIFY(*lhs)) {
		return mcr_resembles_justified(me, them);
	} else {
		return mcr_resembles_absolute(me, them, measurementError);
	}
	return 1;
}

struct mcr_ISignal *mcr_iMoveCursor(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.imove_cursor;
}
