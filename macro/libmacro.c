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

#include "mcr/libmacro.h"
#include "mcr/modules.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma message("TODO: Optimize for intercept, dispatch, and send." \
	" SIMPLIFY(function instead of inline) everywhere else.")

struct mcr_context *mcr_allocate(bool flagLoadContracts, bool flagTrimFinish)
{
	struct mcr_context *ctx = malloc(sizeof(struct mcr_context));
	if (!ctx) {
		mset_error(ENOMEM);
		return NULL;
	}
	if (mcr_initialize(ctx, flagLoadContracts, flagTrimFinish)) {
		free(ctx);
		return NULL;
	}
	return ctx;
}

int mcr_deallocate(struct mcr_context *ctx)
{
	int err;
	if (!ctx) {
		mset_error(EINVAL);
		return EINVAL;
	}
	err = mcr_deinitialize(ctx);
	free(ctx);
	return err;
}

#define local_err_stack_size 4
static int local_err(struct mcr_context *ctx, int errorOut,
		     int (**errStack) (struct mcr_context *), int stackCount)
{
	dassert(stackCount <= local_err_stack_size);
	/* Traverse backwards through stack */
	while (stackCount--) {
		errStack[stackCount] (ctx);
	}
	return errorOut;
}

int mcr_initialize(struct mcr_context *ctx,
		   bool flagLoadContracts, bool flagTrimFinish)
{
	int err = 0, stackCount = 0;
	int (*errStack[local_err_stack_size]) (struct mcr_context *) = {
		mcr_signal_deinitialize, mcr_macro_deinitialize,
		mcr_standard_deinitialize,
		mcr_intercept_deinitialize
	};
	if (!ctx) {
		mset_error(EINVAL);
		return EINVAL;
	}
	mcr_set_error(0);
	memset(ctx, 0, sizeof(struct mcr_context));
	/* No error functions yet on stack */
	if ((err = mcr_signal_initialize(ctx)))
		return err;
	++stackCount;
	if ((err = mcr_macro_initialize(ctx)))
		return local_err(ctx, err, errStack, stackCount);
	++stackCount;
	if ((err = mcr_standard_initialize(ctx)))
		return local_err(ctx, err, errStack, stackCount);
	++stackCount;
	if ((err = mcr_intercept_initialize(ctx)))
		return local_err(ctx, err, errStack, stackCount);
	++stackCount;
	if (flagLoadContracts) {
		if ((err = mcr_load_contracts(ctx)))
			return local_err(ctx, err, errStack, stackCount);
	}
	if (flagTrimFinish) {
		mcr_trim(ctx);
	}
	return 0;
}

int mcr_deinitialize(struct mcr_context *ctx)
{
	mcr_set_error(0);
	mcr_intercept_deinitialize(ctx);
	mcr_standard_deinitialize(ctx);
	mcr_macro_deinitialize(ctx);
	mcr_signal_deinitialize(ctx);
	return mcr_error();
}

int mcr_load_contracts(struct mcr_context *ctx)
{
	int err = mcr_signal_load_contract(ctx);
	if (err)
		return err;
	if ((err = mcr_standard_load_contract(ctx)))
		return err;
	return err;
}

void mcr_trim(struct mcr_context *ctx)
{
	mcr_signal_trim(ctx);
	mcr_macro_trim(ctx);
	mcr_standard_trim(ctx);
}
