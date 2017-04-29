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
#include "mcr/modules.h"

static int mcr_Key_initialize(struct mcr_context *ctx);
static int mcr_Key_deinitialize(struct mcr_context *ctx);
static int mcr_HidEcho_initialize(struct mcr_context *ctx);
static int mcr_HidEcho_deinitialize(struct mcr_context *ctx);

int mcr_standard_initialize(struct mcr_context *ctx)
{
	int err;
	struct mcr_ISignal *sigs[] = {
		mcr_iEcho(ctx), mcr_iKey(ctx), mcr_iMods(ctx),
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

	isigPt = mcr_iEcho(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_HidEcho),
			      NULL, NULL, NULL, NULL);
	isigPt->send = mcr_HidEcho_send;

	isigPt = mcr_iKey(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_Key), NULL,
			      NULL, NULL, NULL);
	isigPt->send = mcr_Key_send;

	isigPt = mcr_iMods(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_Mods), NULL, NULL,
			      NULL, NULL);
	isigPt->send = mcr_Mods_send;
	((struct mcr_CtxISignal *)isigPt)->ctx = ctx;

	isigPt = mcr_iMC(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_MoveCursor),
			      NULL, NULL, NULL, NULL);
	isigPt->send = mcr_MoveCursor_send;

	isigPt = mcr_iNoOp(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_NoOp), NULL, NULL, NULL,
			      NULL);
	isigPt->send = mcr_NoOp_send;

	isigPt = mcr_iScroll(ctx);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_Scroll),
			      NULL, NULL, NULL, NULL);
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
		err = mcr_standard_platform_initialize(ctx);
	return err;
}

int mcr_standard_deinitialize(struct mcr_context *ctx)
{
	int err = mcr_standard_platform_deinitialize(ctx);
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
		mcr_iEcho(ctx), mcr_iKey(ctx), mcr_iMods(ctx),
		mcr_iMC(ctx), mcr_iNoOp(ctx), mcr_iScroll(ctx)
	};
	struct mcr_ITrigger *trigs[] = {
		mcr_iAction(ctx), mcr_iStaged(ctx)
	};
	/* signals */
	size_t i = arrlen(sigs);
	struct mcr_IRegistry *regPt = mcr_ISignal_reg(ctx);
	const char *names[] = {
		"HidEcho", "Key", "Mods",
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
	return mcr_standard_platform_load_contract(ctx);
}

void mcr_standard_trim(struct mcr_context *ctx)
{
	mcr_HidEcho_trim(ctx);
	mcr_Key_trim(ctx);
	mcr_Key_mod_trim(ctx);
	mcr_Stage_trim(ctx);
}

static int mcr_Key_initialize(struct mcr_context *ctx)
{
	struct mcr_mod_standard *standard = &ctx->standard;
	int err = 0;
	mcr_Dispatcher_set_all(&standard->key_dispatcher.dispatcher,
			       mcr_Key_Dispatcher_add, mcr_Key_Dispatcher_clear,
			       mcr_Key_Dispatcher_dispatch, mcr_Key_Dispatcher_modifier,
			       mcr_Key_Dispatcher_remove, mcr_Key_Dispatcher_trim);
	standard->key_dispatcher.ctx = ctx;
	standard->key_dispatcher_maps[0] = standard->key_dispatcher_maps[1] =
			mcr_Map_new(sizeof(int), 0, mcr_int_compare, NULL,
				    mcr_Array_DispatchPair_interface());
	standard->map_key_modifier = mcr_Map_new(sizeof(int),
				     sizeof(unsigned int), mcr_int_compare, NULL, NULL);
	standard->map_modifier_key = mcr_Map_new(sizeof(unsigned int),
				     sizeof(int), mcr_unsigned_compare, NULL, NULL);
	if ((err = mcr_Dispatcher_register(ctx, &standard->key_dispatcher,
					   mcr_iKey(ctx)->interface.id)))
		return err;
	mcr_StringIndex_init(&standard->key_name_index);
	mcr_String_init(&standard->key_name_any);
	if ((err = mcr_String_replace(&standard->key_name_any, "Any")))
		return err;
	return err;
}

static int mcr_Key_deinitialize(struct mcr_context *ctx)
{
	int err;
	struct mcr_mod_standard *standard = &ctx->standard;
	int keyDispMapCount = arrlen(standard->key_dispatcher_maps);
	mcr_StringIndex_deinit(&standard->key_name_index);
	mcr_String_deinit(&standard->key_name_any);
	while (keyDispMapCount--) {
		mcr_Map_deinit(standard->key_dispatcher_maps + keyDispMapCount);
	}
	if ((err = mcr_Map_deinit(&standard->map_key_modifier)))
		return err;
	return mcr_Map_deinit(&standard->map_modifier_key);
}

static int mcr_HidEcho_initialize(struct mcr_context *ctx)
{
	int err = 0;
	mcr_StringIndex_init(&ctx->standard.echo_name_index);
	mcr_String_init(&ctx->standard.echo_name_any);
	if ((err = mcr_String_replace(&ctx->standard.echo_name_any, "Any")))
		return err;
	return err;
}

static int mcr_HidEcho_deinitialize(struct mcr_context *ctx)
{
	mcr_StringIndex_deinit(&ctx->standard.echo_name_index);
	mcr_String_deinit(&ctx->standard.echo_name_any);
	return 0;
}
