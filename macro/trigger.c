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

#include "mcr/macro/macro.h"
#include "mcr/macro/private.h"
#include <string.h>

fixme;
#pragma message("Add function to add trigger to dispatchers")

static const struct mcr_Interface _MCR_TRIGGER_IFACE = {
	.id = ~0,
	.compare = mcr_Trigger_compare,
	.copy = mcr_Trigger_copy,
	.data_size = sizeof(struct mcr_Trigger),
	.init = mcr_Trigger_init,
	.free = mcr_Trigger_free
};

bool mcr_Trigger_receive(void *trigReceiverPt,
	struct mcr_Signal *dispatchSignal, unsigned int mods)
{
	struct mcr_Trigger *trigPt = trigReceiverPt;
	if (trigPt && trigPt->itrig && trigPt->itrig->receive) {
		return trigPt->itrig->receive(trigReceiverPt,
			dispatchSignal, mods);
	}
	return false;
}

const struct mcr_Interface *mcr_Trigger_iface()
{
	return &_MCR_TRIGGER_IFACE;
}

void mcr_Trigger_init(void *trigDataPt)
{
	if (trigDataPt) {
		memset(trigDataPt, 0, sizeof(struct mcr_Trigger));
	}
}

void mcr_Trigger_set_all(struct mcr_Trigger *trigPt,
	mcr_Dispatcher_receive_fnc trigger, void *actor)
{
	dassert(trigPt);
	trigPt->trigger = trigger;
	trigPt->actor = actor;
}

void mcr_Trigger_free(void *trigDataPt)
{
	mcr_inst_free(trigDataPt);
}

int mcr_Trigger_copy(void *dstPt, void *srcPt)
{
	struct mcr_Trigger *dstTrig = dstPt, *srcTrig = srcPt;
	int err = mcr_inst_copy(dstPt, srcPt);
	if (err)
		return err;
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

int mcr_Trigger_compare(const void *lhsTrigPt, const void *rhsTrigPt)
{
	const struct mcr_Trigger *lTrigPt = lhsTrigPt, *rTrigPt = rhsTrigPt;
	int valHolder;
	if (rhsTrigPt) {
		if (lhsTrigPt) {
			if (lhsTrigPt == rhsTrigPt)
				return 0;
			valHolder = mcr_inst_compare(lhsTrigPt, rhsTrigPt);
			if (valHolder)
				return valHolder;
			if ((valHolder = MCR_CMP_INTEGRAL(lTrigPt->trigger,
						rTrigPt->trigger)))
				return valHolder;
			return MCR_CMP_INTEGRAL(lTrigPt->actor, rTrigPt->actor);
		}
		return -1;
	}
	return ! !lhsTrigPt;
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

int mcr_trigger_initialize(struct mcr_context *ctx)
{
	dassert(ctx);
	mcr_reg_init(mcr_ITrigger_reg(ctx));
	return 0;
}

void mcr_trigger_cleanup(struct mcr_context *ctx)
{
	dassert(ctx);
	mcr_reg_free(mcr_ITrigger_reg(ctx));
}
