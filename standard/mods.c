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

#include "mcr/standard/standard.h"
#include "mcr/standard/private.h"
#include MCR_STANDARD_NATIVE_INC
#include "mcr/modules.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mcr_Mods_set_all(struct mcr_Mods *modPt, unsigned int modifiers,
	enum mcr_KeyUpType upType)
{
	modPt->modifiers = modifiers;
	modPt->up_type = upType;
}

int mcr_Mods_send(struct mcr_Signal *sigPt)
{
	struct mcr_Mods *modPt = mcr_Mods_data(sigPt);
	struct mcr_CtxISignal *isigPt = (struct mcr_CtxISignal *)sigPt->isig;
	unsigned int mods;
	if (isigPt && modPt) {
		mods = mcr_modifiers(isigPt->ctx);
		switch (modPt->up_type) {
		case MCR_DOWN:
			mods |= modPt->modifiers;
			break;
		case MCR_UP:
		case MCR_BOTH:
			mods &= (~modPt->modifiers);
			break;
		case MCR_TOGGLE:
			if ((mods & modPt->modifiers) == modPt->modifiers) {
				mods &= (~modPt->modifiers);
			} else {
				mods |= modPt->modifiers;
			}
			break;
		default:
			mset_error(EINVAL);
			return EINVAL;
		}
		mcr_set_modifiers(isigPt->ctx, mods);
	}
	return 0;
}

int mcr_Mods_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs) {
			return lhs == rhs ? 0 :
				memcmp(lhs, rhs, sizeof(struct mcr_Mods));
		}
		return -1;
	}
	return ! !lhs;
}

void mcr_Mods_modify(struct mcr_Mods *modPt,
	unsigned int modifier, enum mcr_KeyUpType modifierKeyUp)
{
	if (!modPt)
		return;
	/* If same side of the MCR_DOWN, then adding. */
	if ((modPt->up_type == MCR_DOWN) == (modifierKeyUp == MCR_DOWN)) {
		modPt->modifiers |= modifier;
	} else {
		modPt->modifiers &= ~modifier;
	}
}

struct mcr_ISignal *mcr_iMods(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.imods.isignal;
}
