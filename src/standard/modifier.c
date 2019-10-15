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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcr/libmacro.h"

void mcr_Modifier_set_all(struct mcr_Modifier *modPt, unsigned int modifiers,
						  enum mcr_ApplyType applyType)
{
	modPt->modifiers = modifiers;
	modPt->apply = applyType;
}

int mcr_Modifier_send(struct mcr_Signal *sigPt)
{
	struct mcr_Modifier *modPt = mcr_Modifier_data(sigPt);
	struct mcr_ISignal *isigPt = sigPt->isignal;
	struct mcr_context *context;
	if (isigPt && modPt) {
		context = isigPt->interface.context;
		switch (modPt->apply) {
		case MCR_SET:
			context->signal.internal_modifiers |= modPt->modifiers;
			break;
		case MCR_UNSET:
		case MCR_BOTH:
			context->signal.internal_modifiers &=
				(~modPt->modifiers);
			break;
		case MCR_TOGGLE:
			if ((context->signal.
				 internal_modifiers & modPt->modifiers)
				== modPt->modifiers) {
				context->signal.internal_modifiers &=
					(~modPt->modifiers);
			} else {
				context->signal.internal_modifiers |=
					modPt->modifiers;
			}
			break;
		}
	}
	return 0;
}

void mcr_Modifier_modify(struct mcr_Modifier *modPt,
						 unsigned int modifier, enum mcr_ApplyType applyModifier)
{
	dassert(modPt);
	/* Both set or release then adding */
	if (modPt->apply == applyModifier) {
		modPt->modifiers |= modifier;
	} else {
		modPt->modifiers &= ~modifier;
	}
}

struct mcr_ISignal *mcr_iModifier(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.imodifier;
}
