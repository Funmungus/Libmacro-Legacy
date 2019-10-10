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

#include "mcr/signal/signal.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "mcr/libmacro.h"

/* Public access modifiers */
unsigned int *mcr_modifiers(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->signal.internal_modifiers;
}

void mcr_add_modifiers(struct mcr_context *ctx, unsigned int addMods)
{
	dassert(ctx);
	ctx->signal.internal_modifiers |= addMods;
}

void mcr_remove_modifiers(struct mcr_context *ctx, unsigned int remMods)
{
	dassert(ctx);
	ctx->signal.internal_modifiers &= (~remMods);
}
