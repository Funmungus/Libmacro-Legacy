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

#include <stdio.h>
#include <string.h>

#include "mcr/libmacro.h"

int mcr_Action_init(void *actPt)
{
	struct mcr_Action *localPt = actPt;
	if (actPt) {
		localPt->modifiers = MCR_MF_NONE;
		localPt->trigger_flags = MCR_TF_ALL;
	}
	return 0;
}

bool mcr_Action_receive(void *trigPt, struct mcr_Signal * sigPt,
						unsigned int mods)
{
	bool isMod = false;
	struct mcr_Trigger *localPt = trigPt;
	struct mcr_Action *actPt = mcr_Action_data(localPt);
	dassert(localPt);
	if (actPt && localPt->trigger) {
		MCR_TF_IS_MOD(actPt->modifiers, mods, actPt->trigger_flags,
					  isMod);
		if (isMod)
			return localPt->trigger(localPt, sigPt, mods);
	}
	return false;
}

struct mcr_ITrigger *mcr_iAction(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.iaction;
}
