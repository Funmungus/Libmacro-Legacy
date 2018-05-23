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

#define SIZE(s) sizeof(struct s)
#define SEND(s) s ## _send
int mcr_standard_initialize(struct mcr_context *ctx)
{
	int err;
	struct mcr_ISignal *sigs[] = {
		mcr_iEcho(ctx), mcr_iKey(ctx), mcr_iMods(ctx),
		mcr_iMC(ctx), mcr_iNoOp(ctx), mcr_iScroll(ctx)
	};
	size_t sigSizes[] = {
		SIZE(mcr_HidEcho), SIZE(mcr_Key),
		SIZE(mcr_Mods), SIZE(mcr_MoveCursor),
		SIZE(mcr_NoOp), SIZE(mcr_Scroll)
	};
	mcr_signal_fnc sendFns[] = {
		SEND(mcr_HidEcho), SEND(mcr_Key), SEND(mcr_Mods),
		SEND(mcr_MoveCursor), SEND(mcr_NoOp), SEND(mcr_Scroll)
	};
	struct mcr_ITrigger *trigs[] = {
		mcr_iAction(ctx), mcr_iStaged(ctx)
	};
	/* signals */
	size_t i = arrlen(sigs), count = arrlen(sigs);
	while (i--) {
		mcr_ISignal_init(sigs[i]);
		mcr_Interface_set_all(sigs[i], sigSizes[i], NULL, NULL, NULL,
							  NULL);
		sigs[i]->send = sendFns[i];
		if ((err = mcr_register(mcr_ISignal_reg(ctx), sigs[i], NULL, NULL, 0)))
			return err;
	}

	((struct mcr_CtxISignal *)sigs[2])->ctx = ctx;

	/* triggers */
	mcr_ITrigger_init(trigs[0]);
	mcr_Interface_set_all(trigs[0], sizeof(struct mcr_Action),
						  mcr_Action_init, NULL, NULL, NULL);
	trigs[0]->receive = mcr_Action_receive;

	mcr_ITrigger_init(trigs[1]);
	mcr_Interface_set_all(trigs[1], sizeof(struct mcr_Staged),
						  mcr_Staged_init, mcr_Staged_deinit, mcr_Staged_compare,
						  mcr_Staged_copy);
	trigs[1]->receive = mcr_Staged_receive;

	count = arrlen(trigs);
	for (i = 0; i < count; i++) {
		if ((err = mcr_register(mcr_ITrigger_reg(ctx), trigs[i], NULL, NULL, 0)))
			return err;
	}

	err = mcr_Key_initialize(ctx);
	if (!err)
		err = mcr_HidEcho_initialize(ctx);
	if (!err)
		err = mcr_standard_platform_initialize(ctx);
	return err;
}
#undef SIZE
#undef SEND

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
