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

#include "mcr/macro/macro.h"

#include <string.h>

static const struct mcr_Interface _MCR_TRIGGER_IFACE = {
	.id = ~0,
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
	if (triggerPt) {
		memset(triggerPt, 0, sizeof(struct mcr_Trigger));
	}
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
		triggerPt->trigger = triggerPt->actor = NULL;
	}
}

int mcr_Trigger_copy(void *dstPt, const void *srcPt)
{
	struct mcr_Trigger *dstTrig = dstPt;
	const struct mcr_Trigger *srcTrig = srcPt;
	int err = mcr_Instance_copy(dstPt, srcPt);
	if (err)
		return err;
	if (dstTrig) {
		if (srcTrig) {
			dstTrig->trigger = srcTrig->trigger;
			dstTrig->actor = srcTrig->actor;
		} else {
			dstTrig->trigger = dstTrig->actor = NULL;
		}
	}
	return 0;
}

int mcr_Trigger_compare(const void *lhsTriggerPt, const void *rhsTriggerPt)
{
	const struct mcr_Trigger *lTrigPt = lhsTriggerPt, *rTrigPt =
												rhsTriggerPt;
	int valHolder;
	if (rTrigPt) {
		if (lTrigPt) {
			if (lTrigPt == rTrigPt)
				return 0;
			valHolder = mcr_Instance_compare(lTrigPt, rTrigPt);
			if (valHolder)
				return valHolder;
			if ((valHolder = MCR_CMP(lTrigPt->trigger,
									 rTrigPt->trigger)))
				return valHolder;
			return MCR_CMP(lTrigPt->actor, rTrigPt->actor);
		}
		return -1;
	}
	return ! !lTrigPt;
}

int mcr_Triggerref_compare(const void *lhsPtPt, const void *rhsPtPt)
{
	if (rhsPtPt) {
		if (lhsPtPt) {
			if (lhsPtPt == rhsPtPt)
				return 0;
			return mcr_Trigger_compare(*(const void **)lhsPtPt,
									   *(const void **)rhsPtPt);
		}
		return -1;
	}
	return ! !lhsPtPt;
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
