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

#include "mcr/extras/extras.h"
#include "mcr/modules.h"
#include "mcr/modules.h"

static int mcr_safe_string_initialize(struct mcr_context *ctx);
static int mcr_safe_string_deinitialize(struct mcr_context *ctx);
/* TODO: Start with size 0xFF, add trim function */
static int mcr_signal_extras_initialize(struct mcr_context *ctx);
static int mcr_signal_extras_deinitialize(struct mcr_context *ctx);

int mcr_extras_initialize(struct mcr_context *ctx)
{
	int err = mcr_safe_string_initialize(ctx);
	if (err)
		return err;
	if ((err = mcr_signal_extras_initialize(ctx)))
		return err;
	return mcr_extras_platform_initialize(ctx);
}

int mcr_extras_deinitialize(struct mcr_context *ctx)
{
	int err = mcr_extras_platform_deinitialize(ctx);
	if (err)
		return err;
	if ((err = mcr_signal_extras_deinitialize(ctx)))
		return err;
	return mcr_safe_string_deinitialize(ctx);
}

int mcr_extras_load_contract(struct mcr_context *ctx)
{
	char *cmdNames[] = {
		"Command", "Cmd"
	};
	char *skNames[] = {
		"StringKey", "SK", "string_key"
	};
	struct mcr_IRegistry *isigReg = mcr_ISignal_reg(ctx);
	int err = mcr_reg_set_names(isigReg, mcr_iCommand(ctx),
		(const char *)cmdNames[0], (const char **)cmdNames,
		arrlen(cmdNames));
	if (!err)
		err = mcr_reg_set_names(isigReg, mcr_iStringKey(ctx),
			(const char *)skNames[0], (const char **)skNames,
			arrlen(skNames));
	return err ? err : mcr_StringKey_load_contract(ctx);
}

int mcr_safe_string_initialize(struct mcr_context *ctx)
{
	char pass[MCR_AES_BLOCK_SIZE + 1] = { 0 };
	int err = mcr_randomize((unsigned char *)pass, MCR_AES_BLOCK_SIZE);
	UNUSED(ctx);
	pass[MCR_AES_BLOCK_SIZE] = '\0';
	mcr_SafeString_set_password(pass);
	return err;
}

int mcr_safe_string_deinitialize(struct mcr_context *ctx)
{
	UNUSED(ctx);
	return 0;
}

static int mcr_signal_extras_initialize(struct mcr_context *ctx)
{
	struct mcr_ISignal *isigPt = mcr_iStringKey(ctx);
	int err = 0;
	mcr_ISignal_init(isigPt);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_StringKey),
		mcr_StringKey_init, mcr_StringKey_deinit, mcr_StringKey_compare,
		mcr_StringKey_copy);
	isigPt->send = mcr_StringKey_send;
	((struct mcr_CtxISignal *)isigPt)->ctx = ctx;
	isigPt = mcr_iCommand(ctx);
	mcr_ISignal_init(isigPt);
	mcr_Interface_set_all(isigPt, sizeof(struct mcr_Command),
		mcr_Command_init, mcr_Command_deinit, mcr_Command_compare,
		mcr_Command_copy);
	isigPt->send = mcr_Command_send;
	mcr_Array_init(&ctx->extras.key_chars);
	mcr_Array_set_all(&ctx->extras.key_chars, NULL,
		sizeof(struct mcr_Array));

	err = mcr_register(mcr_ISignal_reg(ctx), mcr_iStringKey(ctx), NULL,
		NULL, 0);
	if (!err)
		err = mcr_register(mcr_ISignal_reg(ctx), mcr_iCommand(ctx),
			NULL, NULL, 0);
	return err;
}

static int mcr_signal_extras_deinitialize(struct mcr_context *ctx)
{
	mcr_StringKey_char_clear(ctx);
	mcr_Array_deinit(&ctx->extras.key_chars);
	return 0;
}
