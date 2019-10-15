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

#include "mcr/intercept/intercept.h"

#include "mcr/libmacro.h"
#include "mcr/private.h"

void mcr_intercept_reset_modifiers(struct mcr_context *ctx)
{
	*mcr_modifiers(ctx) = mcr_intercept_modifiers(ctx);
}

bool mcr_intercept_is_blockable(struct mcr_context *ctx)
{
	return ctx->intercept.blockable;
}

void mcr_intercept_set_blockable(struct mcr_context *ctx, bool enable)
{
	ctx->intercept.blockable = enable;
}

int mcr_intercept_reset(struct mcr_context *ctx)
{
	if (mcr_intercept_is_enabled(ctx)) {
		if (mcr_intercept_set_enabled(ctx, false))
			return mcr_err;
		return mcr_intercept_set_enabled(ctx, true);
	}
	return 0;
}

int mcr_intercept_initialize(struct mcr_context *ctx)
{
	/* blocking default false, assume structure is 0'd on init */
	dassert(!ctx->intercept.blockable);
	return mcr_intercept_platform_initialize(ctx);
}

int mcr_intercept_deinitialize(struct mcr_context *ctx)
{
	return mcr_intercept_platform_deinitialize(ctx);
}
