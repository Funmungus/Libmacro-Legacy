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

#include "mcr/libmacro.h"
#include "mcr/modules.h"
#include "mcr/signal/private.h"
#include "mcr/macro/private.h"
#include "mcr/standard/private.h"
#include "mcr/intercept/private.h"
#include "mcr/extras/private.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma message("TODO: Optimize for intercept, dispatch, and send." \
	" SIMPLIFY(function instead of inline) everywhere else.")

struct mcr_context *mcr_initialize(bool flagLoadContracts, bool flagTrimFinish)
{
	struct mcr_context *ctx = malloc(sizeof(struct mcr_context));
	if (!ctx) {
		mset_error(ENOMEM);
		return NULL;
	}
	memset(ctx, 0, sizeof(struct mcr_context));
	dprint("Loading signal module\n");
	if (mcr_signal_initialize(ctx))
		goto onError;
	dprint("Loading trigger module\n");
	if (mcr_macro_initialize(ctx))
		goto onError;
	dprint("Loading intercept module\n");
	if (mcr_intercept_initialize(ctx))
		goto onError;
#ifdef MCR_EXTRAS
	dprint("Loading extras module\n");
	if (mcr_extras_initialize(ctx))
		goto onError;
#endif
	if (flagLoadContracts) {
		dprint("Loading contracts\n");
		if (mcr_load_contracts(ctx))
			goto onError;
	}
	if (flagTrimFinish) {
		dprint("Trimming allocation\n");
		mcr_trim(ctx);
	}
	dprint("Initialization complete\n");
 onError:
	mcr_cleanup(ctx);
	free(ctx);
	return NULL;
}

void mcr_cleanup(struct mcr_context *ctx)
{
#ifdef MCR_EXTRAS
	dprint("Cleaning extras module\n");
	mcr_extras_cleanup(ctx);
#endif
	dprint("Cleaning intercept module\n");
	mcr_intercept_cleanup(ctx);
	dprint("Cleaning standard module\n");
	mcr_standard_cleanup(ctx);
	dprint("Cleaning macro module\n");
	mcr_macro_cleanup(ctx);
	dprint("Cleaning signal module\n");
	mcr_signal_cleanup(ctx);
	dprint("Cleaning complete\n");
}

int mcr_load_contracts(struct mcr_context *ctx)
{
	int err = mcr_signal_load_contract(ctx);
	if (err)
		return err;
	if ((err = mcr_standard_load_contract(ctx)))
		return err;
	if ((err = mcr_extras_load_contract(ctx)))
		return err;
	return err;
}

void mcr_trim(struct mcr_context *ctx)
{
	mcr_signal_trim(ctx);
	mcr_macro_trim(ctx);
	mcr_standard_trim(ctx);
}
