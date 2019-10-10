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

#include "mcr/standard/standard.h"

#include <string.h>

#include "mcr/libmacro.h"

void mcr_MoveCursor_set_all(struct mcr_MoveCursor *mcPt,
							const mcr_SpacePosition pos, bool flagJustify)
{
	dassert(mcPt);
	memcpy(mcPt->pos, pos, sizeof(mcr_SpacePosition));
	mcPt->is_justify = flagJustify;
}

int mcr_MoveCursor_send(struct mcr_Signal *sigPt)
{
	dassert(sigPt);
	mcr_MC *mcPt = mcr_MC_data(sigPt);
	return mcPt ? mcr_MoveCursor_send_data(mcPt) : 0;
}

bool mcr_resembles(const struct mcr_MoveCursor * lhs,
				   const struct mcr_MoveCursor * rhs, const unsigned int measurementError)
{
	if (!lhs || !rhs)
		return lhs == rhs;
	if (lhs->is_justify != rhs->is_justify)
		return false;
	if (lhs->is_justify)
		return mcr_resembles_justified(lhs->pos, rhs->pos);
	return mcr_resembles_absolute(lhs->pos, rhs->pos, measurementError);
}

struct mcr_ISignal *mcr_iMoveCursor(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.imove_cursor;
}
