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
	if (mcr_initialize(ctx, flagLoadContracts, flagTrimFinish))
		return NULL;
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

#define local_err_stack_size 5
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
			mcr_intercept_deinitialize, mcr_extras_deinitialize};
	if (!ctx) {
		mset_error(EINVAL);
		return EINVAL;
	}
	memset(ctx, 0, sizeof(struct mcr_context));
	dprint("Loading signal module\n");
	/* No error functions yet on stack */
	if ((err = mcr_signal_initialize(ctx)))
		return err;
	++stackCount;
	dprint("Loading trigger module\n");
	if ((err = mcr_macro_initialize(ctx)))
		return local_err(ctx, err, errStack, stackCount);
	++stackCount;
	dprint("Loading standard types\n");
	if ((err = mcr_standard_initialize(ctx)))
		return local_err(ctx, err, errStack, stackCount);
	++stackCount;
	dprint("Loading intercept module\n");
	if ((err = mcr_intercept_initialize(ctx)))
		return local_err(ctx, err, errStack, stackCount);
	++stackCount;
#ifdef MCR_EXTRAS
	dprint("Loading extras module\n");
	if ((err = mcr_extras_initialize(ctx)))
		return local_err(ctx, err, errStack, stackCount);
	++stackCount;
#endif
	if (flagLoadContracts) {
		dprint("Loading contracts\n");
		if ((err = mcr_load_contracts(ctx)))
			return local_err(ctx, err, errStack, stackCount);
	}
	if (flagTrimFinish) {
		dprint("Trimming allocation\n");
		mcr_trim(ctx);
	}
	dprint("Initialization complete\n");
	return 0;
}

int mcr_deinitialize(struct mcr_context *ctx)
{
	mcr_set_error(0);
#ifdef MCR_EXTRAS
	dprint("Cleaning extras module\n");
	mcr_extras_deinitialize(ctx);
#endif
	dprint("Cleaning intercept module\n");
	mcr_intercept_deinitialize(ctx);
	dprint("Cleaning standard module\n");
	mcr_standard_deinitialize(ctx);
	dprint("Cleaning macro module\n");
	mcr_macro_deinitialize(ctx);
	dprint("Cleaning signal module\n");
	mcr_signal_deinitialize(ctx);
	dprint("Cleaning complete\n");
	return mcr_error();
}

int mcr_load_contracts(struct mcr_context *ctx)
{
	int err = mcr_signal_load_contract(ctx);
	if (err)
		return err;
	if ((err = mcr_standard_load_contract(ctx)))
		return err;
#ifdef MCR_EXTRAS
	if ((err = mcr_extras_load_contract(ctx)))
		return err;
#endif
	return err;
}

void mcr_trim(struct mcr_context *ctx)
{
	mcr_signal_trim(ctx);
	mcr_macro_trim(ctx);
	mcr_standard_trim(ctx);
#ifdef MCR_EXTRAS
	/* TODO: Add trim function */
#endif
}
