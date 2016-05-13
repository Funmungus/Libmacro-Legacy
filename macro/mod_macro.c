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

#include "mcr/macro/macro.h"
#include "mcr/macro/private.h"
#include "mcr/modules.h"

int mcr_macro_initialize(struct mcr_context *ctx)
{
	return mcr_trigger_initialize(ctx);
}

void mcr_macro_cleanup(struct mcr_context *ctx)
{
	mcr_trigger_cleanup(ctx);
}

void mcr_macro_trim(struct mcr_context *ctx)
{
	mcr_reg_trim(mcr_ITrigger_reg(ctx));
}
