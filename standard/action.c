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
#include "mcr/modules.h"
#include <stdio.h>
#include <string.h>

void mcr_Action_init(void *actDataPt)
{
	struct mcr_Action *actPt = actDataPt;
	if (actPt) {
		actPt->modifiers = MCR_MF_NONE;
		actPt->trigger_flags = MCR_TF_ALL;
	}
}

bool mcr_Action_receive(void *actTrigPt, struct mcr_Signal *sigPt,
	unsigned int mods)
{
	bool isMod = false;
	struct mcr_Trigger *trigPt = actTrigPt;
	struct mcr_Action *actPt = mcr_Action_data(trigPt);
	dassert(actTrigPt);
	if (actPt && trigPt->trigger) {
		MCR_TF_IS_MOD(actPt->modifiers, mods, actPt->trigger_flags,
			isMod);
		if (isMod) {
			return trigPt->trigger(actTrigPt, sigPt, mods);
		}
	}
	return false;
}

struct mcr_ITrigger *mcr_iAction(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.iaction;
}
