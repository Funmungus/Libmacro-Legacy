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

#include "mcr/macro/macro.h"

#include <string.h>

static const struct mcr_Interface _MCR_TRIGGER_IFACE = {
	.id = (size_t)-1,
	.compare = mcr_Trigger_compare,
	.copy = mcr_Trigger_copy,
	.data_size = sizeof(struct mcr_Trigger),
	.init = mcr_Trigger_init,
	.deinit = mcr_Trigger_deinit
};

bool mcr_Trigger_receive(void *triggerPt,
						 struct mcr_Signal *dispatchSignal, unsigned int mods)
{
	struct mcr_Trigger *localPt = triggerPt;
	if (localPt && localPt->itrigger && localPt->itrigger->receive
		&& localPt->trigger) {
		return localPt->itrigger->receive(localPt, dispatchSignal,
										  mods);
	}
	return false;
}

const struct mcr_Interface *mcr_Trigger_interface()
{
	return &_MCR_TRIGGER_IFACE;
}

int mcr_Trigger_init(void *triggerPt)
{
	if (triggerPt)
		memset(triggerPt, 0, sizeof(struct mcr_Trigger));
	return 0;
}

struct mcr_Trigger mcr_Trigger_new(mcr_Dispatcher_receive_fnc trigger,
								   void *actor)
{
	struct mcr_Trigger ret;
	mcr_Trigger_init(&ret);
	ret.trigger = trigger;
	ret.actor = actor;
	return ret;
}

int mcr_Trigger_deinit(void *trigPt)
{
	return mcr_Instance_deinit(trigPt);
}

void mcr_Trigger_set_all(struct mcr_Trigger *triggerPt,
						 mcr_Dispatcher_receive_fnc trigger, void *actor)
{
	dassert(triggerPt);
	triggerPt->trigger = trigger;
	triggerPt->actor = actor;
}

void mcr_Trigger_set_macro(struct mcr_Trigger *triggerPt,
						   struct mcr_Macro *mcrPt)
{
	dassert(triggerPt);
	if (mcrPt) {
		triggerPt->trigger = mcr_Macro_trigger;
		triggerPt->actor = mcrPt;
	} else {
		triggerPt->trigger = NULL;
		triggerPt->actor = NULL;
	}
}

int mcr_Trigger_copy(void *dstPt, const void *srcPt)
{
	struct mcr_Trigger *dstTrig = dstPt;
	const struct mcr_Trigger *srcTrig = srcPt;
	if (mcr_Instance_copy(dstPt, srcPt))
		return mcr_err;
	if (dstTrig) {
		if (srcTrig) {
			dstTrig->trigger = srcTrig->trigger;
			dstTrig->actor = srcTrig->actor;
		} else {
			dstTrig->trigger = NULL;
			dstTrig->actor = NULL;
		}
	}
	return 0;
}

int mcr_Trigger_compare(const void *lhsTriggerPt, const void *rhsTriggerPt)
{
	const struct mcr_Trigger *lTrigPt = lhsTriggerPt, *rTrigPt =
												rhsTriggerPt;
	int cmp;
	if (lTrigPt && rTrigPt) {
		if ((cmp = mcr_Instance_compare(lTrigPt, rTrigPt)))
			return cmp;
		if ((cmp = MCR_CMP_CAST((void *), lTrigPt->trigger,
								rTrigPt->trigger))) {
			return cmp;
		}
		return MCR_CMP(lTrigPt->actor, rTrigPt->actor);
	}
	return MCR_CMP(lTrigPt, rTrigPt);
}

int mcr_Triggerref_compare(const void *lhsPtPt, const void *rhsPtPt)
{
	if (lhsPtPt && rhsPtPt)
		return mcr_Trigger_compare(*(const void *const *)lhsPtPt,
								   *(const void *const *)rhsPtPt);
	return MCR_CMP(lhsPtPt, rhsPtPt);
}

int mcr_Trigger_add_dispatch(struct mcr_context *ctx,
							 struct mcr_Trigger *trigPt, struct mcr_Signal *interceptPt)
{
	return mcr_Dispatcher_add(ctx, interceptPt, trigPt,
							  mcr_Trigger_receive);
}

int mcr_Trigger_remove_dispatch(struct mcr_context *ctx,
								struct mcr_Trigger *trigPt, struct mcr_ISignal *isigPt)
{
	return mcr_Dispatcher_remove(ctx, isigPt, trigPt);
}
