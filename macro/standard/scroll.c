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

#include "mcr/libmacro.h"

int mcr_Scroll_send(struct mcr_Signal *sigPt)
{
	dassert(sigPt);
	struct mcr_Scroll *scrPt = mcr_Scroll_data(sigPt);
	return scrPt ? mcr_Scroll_send_data(scrPt) : 0;
}

struct mcr_ISignal *mcr_iScroll(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.iscroll;
}
