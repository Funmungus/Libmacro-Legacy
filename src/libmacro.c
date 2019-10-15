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

#include "mcr/libmacro.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcr/private.h"

/*! \todo Optimize for intercept, dispatch, and send.
 * Simplify with function instead of inline everywhere else.
 */

struct mcr_context *mcr_allocate()
{
	int err;
	struct mcr_context *ctx = malloc(sizeof(struct mcr_context));
	if (!ctx) {
		mcr_errno(ENOMEM);
		return NULL;
	}
	if (mcr_initialize(ctx)) {
		free(ctx);
		return NULL;
	}
	if ((err = mcr_load_contracts(ctx))) {
		mcr_deinitialize(ctx);
		/* Recover load error */
		mcr_err = err;
		return NULL;
	}
	mcr_trim(ctx);
	return ctx;
}

int mcr_deallocate(struct mcr_context *ctx)
{
	if (!ctx)
		return (mcr_err = EINVAL);
	mcr_deinitialize(ctx);
	free(ctx);
	return mcr_err;
}

#define local_err_stack_size 4
static int local_err(struct mcr_context *ctx,
					 int (**errStack) (struct mcr_context *), int stackCount)
{
	dassert(stackCount <= local_err_stack_size);
	/* Traverse backwards through stack */
	while (stackCount--) {
		errStack[stackCount] (ctx);
	}
	return mcr_err;
}

int mcr_initialize(struct mcr_context *ctx)
{
	int stackCount = 0;
	int (*errStack[local_err_stack_size]) (struct mcr_context *) = {
		mcr_signal_deinitialize, mcr_macro_deinitialize,
		mcr_standard_deinitialize
	};
	if (!ctx)
		return (mcr_err = EINVAL);
	mcr_err = 0;
	memset(ctx, 0, sizeof(struct mcr_context));
	/* No error functions yet on stack */
	if (mcr_signal_initialize(ctx))
		return mcr_err;
	++stackCount;
	if (mcr_macro_initialize(ctx))
		return local_err(ctx, errStack, stackCount);
	++stackCount;
	if (mcr_standard_initialize(ctx))
		return local_err(ctx, errStack, stackCount);
	++stackCount;
	if (mcr_intercept_initialize(ctx))
		return local_err(ctx, errStack, stackCount);
	return 0;
}

int mcr_deinitialize(struct mcr_context *ctx)
{
	mcr_err = 0;
	mcr_intercept_deinitialize(ctx);
	mcr_standard_deinitialize(ctx);
	mcr_macro_deinitialize(ctx);
	mcr_signal_deinitialize(ctx);
	return mcr_err;
}

int mcr_load_contracts(struct mcr_context *ctx)
{
	mcr_err = 0;
	if (mcr_signal_load_contract(ctx))
		return mcr_err;
	if (mcr_standard_load_contract(ctx))
		return mcr_err;
	return 0;
}

void mcr_trim(struct mcr_context *ctx)
{
	mcr_signal_trim(ctx);
	mcr_macro_trim(ctx);
	mcr_standard_trim(ctx);
}
