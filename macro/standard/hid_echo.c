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

#include "mcr/libmacro.h"

int mcr_HidEcho_send(struct mcr_Signal *signalPt)
{
	struct mcr_HidEcho *echoPt = mcr_HidEcho_data(signalPt);
	return echoPt ? mcr_HidEcho_send_data(echoPt) : 0;
}

size_t mcr_HidEcho_name_echo(struct mcr_context * ctx, const char *eventName)
{
	if (!eventName)
		return MCR_ECHO_ANY;
	return mcr_StringIndex_index(&ctx->standard.echo_name_index, eventName);
}

const char *mcr_HidEcho_name(struct mcr_context *ctx, size_t eventCode)
{
	const char *ret;
	if (eventCode == MCR_ECHO_ANY)
		return ctx->standard.echo_name_any.array;
	ret = mcr_StringIndex_name(&ctx->standard.echo_name_index, eventCode);
	return ret ? ret : ctx->standard.echo_name_any.array;
}

size_t mcr_HidEcho_count(struct mcr_context * ctx)
{
	return ctx->standard.echo_name_index.set.used;
}

int mcr_HidEcho_set_name(struct mcr_context *ctx, size_t eventCode,
						 const char *eventName)
{
	if (eventCode == MCR_ECHO_ANY) {
		if (eventName)
			return mcr_String_replace(&ctx->standard.echo_name_any,
									  eventName);
		mcr_String_deinit(&ctx->standard.echo_name_any);
		return 0;
	}
	if (eventName)
		return mcr_StringIndex_map(&ctx->standard.echo_name_index,
								   eventCode, eventName, NULL, 0);
	mcr_StringIndex_unmap(&ctx->standard.echo_name_index, eventCode, 1);
	return 0;
}

int mcr_HidEcho_add(struct mcr_context *ctx, size_t eventCode,
					const char **addNames, size_t bufferLen)
{
	return mcr_StringIndex_add(&ctx->standard.echo_name_index, eventCode,
							   addNames, bufferLen);
}

int mcr_HidEcho_map(struct mcr_context *ctx, size_t eventCode,
					const char *eventName, const char **addNames, size_t bufferLen)
{
	if (eventCode == MCR_ECHO_ANY) {
		return mcr_HidEcho_set_name(ctx, eventCode, eventName) &&
			   mcr_HidEcho_add(ctx, eventCode, addNames, bufferLen);
	}
	return mcr_StringIndex_map(&ctx->standard.echo_name_index, eventCode,
							   eventName, addNames, bufferLen);
}

int mcr_HidEcho_reecho(struct mcr_context *ctx, size_t eventCode,
					   size_t newCode)
{
	int err;
	if (eventCode == newCode)
		return 0;
	err = mcr_HidEcho_set_name(ctx, newCode,
							   mcr_HidEcho_name(ctx, eventCode));
	if (!err)
		return mcr_HidEcho_set_name(ctx, eventCode, NULL);
	return err;
}

int mcr_HidEcho_rename(struct mcr_context *ctx, const char *oldName,
					   const char *newName)
{
	return mcr_StringIndex_remap(&ctx->standard.echo_name_index, oldName,
								 newName);
}

void mcr_HidEcho_trim(struct mcr_context *ctx)
{
	mcr_StringIndex_trim(&ctx->standard.echo_name_index);
}

void mcr_HidEcho_clear(struct mcr_context *ctx)
{
	mcr_StringIndex_clear(&ctx->standard.echo_name_index);
}

struct mcr_ISignal *mcr_iHidEcho(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.ihid_echo;
}
