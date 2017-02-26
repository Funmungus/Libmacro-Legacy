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

#include "mcr/standard/standard.h"
#include "mcr/standard/private.h"
#include MCR_STANDARD_NATIVE_INC
#include "mcr/modules.h"

int mcr_standard_initialize(struct mcr_context *ctx)
{
	int err;
	struct mcr_ISignal *sigs[] = {
		mcr_iAlarm(ctx), mcr_iEcho(ctx),
		mcr_iKey(ctx), mcr_iMods(ctx),
		mcr_iMC(ctx), mcr_iNoOp(ctx), mcr_iScroll(ctx)
	};
	struct mcr_ITrigger *trigs[] = {
		mcr_iAction(ctx), mcr_iStaged(ctx)
	};
	struct mcr_ISignal *isigPt;
	struct mcr_ITrigger *itrigPt;
	/* signals */
	size_t i = arrlen(sigs), count = arrlen(sigs);
	struct mcr_IRegistry *regPt = mcr_ISignal_reg(ctx);
	while (i--)
		mcr_ISignal_init(sigs[i]);
	isigPt = mcr_iAlarm(ctx);
	mcr_Interface_set_all(isigPt, sizeof(mcr_Alarm), mcr_Alarm_init, NULL,
		mcr_Alarm_compare, NULL);
	isigPt->send = mcr_Alarm_send;

	isigPt = mcr_iEcho(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_HidEcho),
		mcr_HidEcho_init, NULL, mcr_HidEcho_compare, mcr_HidEcho_copy);
	isigPt->send = mcr_HidEcho_send;

	isigPt = mcr_iKey(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_Key), mcr_Key_init,
		NULL, mcr_Key_compare, mcr_Key_copy);
	isigPt->send = mcr_Key_send;

	isigPt = mcr_iMods(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_Mods), NULL, NULL,
		mcr_Mods_compare, NULL);
	isigPt->send = mcr_Mods_send;
	((struct mcr_CtxISignal *)isigPt)->ctx = ctx;

	isigPt = mcr_iMC(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_MoveCursor),
		mcr_MoveCursor_init, NULL, mcr_MoveCursor_compare,
		mcr_MoveCursor_copy);
	isigPt->send = mcr_MoveCursor_send;

	isigPt = mcr_iNoOp(ctx);
	mcr_Interface_set_all(isigPt, sizeof(mcr_NoOp), NULL, NULL, NULL, NULL);
	isigPt->send = mcr_NoOp_send;

	isigPt = mcr_iScroll(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_Scroll),
		mcr_Scroll_init, NULL, mcr_Scroll_compare, mcr_Scroll_copy);
	isigPt->send = mcr_Scroll_send;

	count = arrlen(sigs);
	for (i = 0; i < count; i++) {
		if ((err = mcr_register(regPt, sigs[i], NULL, NULL, 0)))
			return err;
	}

	/* triggers */
	i = arrlen(trigs);
	while (i--)
		mcr_ITrigger_init(trigs[i]);
	itrigPt = mcr_iAction(ctx);
	mcr_Interface_set_all(itrigPt, sizeof(struct mcr_Action),
		mcr_Action_init, NULL, NULL, NULL);
	itrigPt->receive = mcr_Action_receive;

	itrigPt = mcr_iStaged(ctx);
	mcr_Interface_set_all(itrigPt, sizeof(struct mcr_Staged),
		mcr_Staged_init, mcr_Staged_deinit, mcr_Staged_compare,
		mcr_Staged_copy);
	itrigPt->receive = mcr_Staged_receive;

	count = arrlen(trigs);
	regPt = mcr_ITrigger_reg(ctx);
	for (i = 0; i < count; i++) {
		if ((err = mcr_register(regPt, trigs[i], NULL, NULL, 0)))
			return err;
	}
	err = mcr_Key_initialize(ctx);
	if (!err)
		err = mcr_HidEcho_initialize(ctx);
	if (!err)
		err = mcr_standard_native_initialize(ctx);
	return err;
}

int mcr_standard_deinitialize(struct mcr_context *ctx)
{
	int err = mcr_standard_native_deinitialize(ctx);
	if (err)
		return err;
	if ((err = mcr_HidEcho_deinitialize(ctx)))
		return err;
	return mcr_Key_deinitialize(ctx);
}

int mcr_standard_load_contract(struct mcr_context *ctx)
{
	int err;
	struct mcr_ISignal *sigs[] = {
		mcr_iAlarm(ctx), mcr_iEcho(ctx),
		mcr_iKey(ctx), mcr_iMods(ctx),
		mcr_iMC(ctx), mcr_iNoOp(ctx), mcr_iScroll(ctx)
	};
	struct mcr_ITrigger *trigs[] = {
		mcr_iAction(ctx), mcr_iStaged(ctx)
	};
	/* signals */
	size_t i = arrlen(sigs);
	struct mcr_IRegistry *regPt = mcr_ISignal_reg(ctx);
	const char *names[] = {
		"Alarm", "HidEcho", "Key", "Mods",
		"MoveCursor", "NoOp", "Scroll"
	};
	const char *echoAdd[] = {
		"Echo", "HidEcho", "HID Echo", "hid_echo"
	};
	const char *mcAdd[] = {
		"MC", "Move Cursor", "move_cursor"
	};
	const char *trigNames[] = { "Action", "Staged" };
	while (i--) {
		if ((err = mcr_reg_set_name(regPt, sigs[i], names[i])))
			return err;
	}
	if ((err = mcr_reg_add_names(regPt, mcr_iEcho(ctx), echoAdd,
				arrlen(echoAdd))))
		return err;
	if ((err = mcr_reg_add_names(regPt, mcr_iMC(ctx), mcAdd,
				arrlen(mcAdd))))
		return err;
	i = arrlen(trigs);
	regPt = mcr_ITrigger_reg(ctx);
	while (i--) {
		if ((err = mcr_reg_set_name(regPt, trigs[i], trigNames[i])))
			return err;
	}
	if ((err = mcr_Key_load_contract(ctx)))
		return err;
	return mcr_HidEcho_load_contract(ctx);
}

void mcr_standard_trim(struct mcr_context *ctx)
{
	mcr_HidEcho_trim(ctx);
	mcr_Key_trim(ctx);
	mcr_Key_mod_trim(ctx);
	mcr_Stage_trim(ctx);
}
