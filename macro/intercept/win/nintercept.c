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

#include "mcr/intercept/intercept.h"
#include "mcr/modules.h"
#include MCR_INTERCEPT_PLATFORM_INC
#include "mcr/modules.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define WPARAM_UPTYPE(wParam) \
((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) ? \
		MCR_UP : MCR_DOWN)

static struct mcr_context *_hook_context = NULL;
/* TODO: Move these to context */
static struct mcr_HidEcho _echo = { 0 };
static struct mcr_Key _key = { 0 };
static struct mcr_MoveCursor _mc = { 0 };
static struct mcr_Scroll _scr = { 0 };
static struct mcr_Signal _echoSig = { 0 }, _keySig = {
	0
}, _mcSig = {
	0
}, _scrSig = {
	0
};

static struct mcr_Signal *_signal_set[] = { &_echoSig, &_keySig, &_mcSig,
	       &_scrSig
};

static const DWORD _echo_flags[] = {
	MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEDOWN,
	MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP,
	MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP
};

static int verify_key_state(PBYTE keyState, size_t keyState_size);
static LRESULT CALLBACK key_proc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK move_proc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK scroll_proc(int nCode, WPARAM wParam, LPARAM lParam);

bool mcr_intercept_is_enabled(struct mcr_context *ctx)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	for (int i = MCR_GRAB_COUNT; i--;) {
		if (mcr_Grabber_is_enabled(nPt->all_grabbers[i]))
			return true;
	}
	return false;
}

int mcr_intercept_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	int err = 0;
	for (int i = MCR_GRAB_COUNT; i--;) {
		if ((err = mcr_Grabber_set_enabled(nPt->all_grabbers[i],
						   enable)))
			return err;
	}
	return err;
}

unsigned int mcr_intercept_modifiers(struct mcr_context *ctx)
{
	unsigned int values = 0;
	BYTE keyState[0x100];
	int key;
	size_t modlen = mcr_Key_mod_count(ctx);
	size_t i = modlen;
	unsigned int *modBuffer;
	/* If cannot get the mod state, return current */
	if (verify_key_state(keyState, 0xFF))
		return *mcr_modifiers(ctx);

	modBuffer = malloc(sizeof(int) * modlen);
	if (!modBuffer) {
		mset_error(ENOMEM);
		return *mcr_modifiers(ctx);
	}

	mcr_Key_mod_all(ctx, modBuffer, modlen);

	while (i--) {
		key = mcr_Key_mod_key(ctx, modBuffer[i]);
		/* high bit is down, lowest byte is toggled. */
		/* high bit of byte is 0x80, high bit of short is 0x8000 */
		if ((keyState[key] & 0x8080) != 0) {
			values |= modBuffer[i];
		}
	}
	free(modBuffer);
	return values;
}

bool mcr_intercept_key_is_enabled(struct mcr_context * ctx)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	return MCR_GRABBER_IS_ENABLED(*nPt->grab_key);
}

int mcr_intercept_key_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	return mcr_Grabber_set_enabled(nPt->grab_key, enable);
}

bool mcr_intercept_move_is_enabled(struct mcr_context * ctx)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	return MCR_GRABBER_IS_ENABLED(*nPt->grab_move);
}

int mcr_intercept_move_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	return mcr_Grabber_set_enabled(nPt->grab_move, enable);
}

bool mcr_intercept_scroll_is_enabled(struct mcr_context * ctx)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	return MCR_GRABBER_IS_ENABLED(*nPt->grab_scroll);
}

int mcr_intercept_scroll_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	return mcr_Grabber_set_enabled(nPt->grab_scroll, enable);
}

static LRESULT CALLBACK key_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	struct mcr_mod_intercept_platform *nPt =
			_hook_context->intercept.platform;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *) lParam;
		_key.key = p->vkCode;
		_key.scan = p->scanCode;
		_key.up_type = (WPARAM_UPTYPE(wParam));
		if (mcr_dispatch(_hook_context, &_keySig))
			return -1;
	}
	return CallNextHookEx(nPt->grab_key->id, nCode, wParam, lParam);
}

static LRESULT CALLBACK move_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	struct mcr_mod_intercept_platform *nPt =
			_hook_context->intercept.platform;
	if (nCode == HC_ACTION) {
		MSLLHOOKSTRUCT *p = (MSLLHOOKSTRUCT *) lParam;
		DWORD flags = p->flags;
		/* Check for movecursor */
		if ((flags & MOUSEEVENTF_MOVE)) {
			_mc.pos[MCR_X] = p->pt.x;
			_mc.pos[MCR_Y] = p->pt.y;
			/* 0 absolute, 1 justify/relative */
			_mc.is_justify = !(flags & MOUSEEVENTF_ABSOLUTE);
			if (mcr_dispatch(_hook_context, &_mcSig))
				return -1;
		}
		for (unsigned int i = arrlen(_echo_flags); i--;) {
			if ((flags & _echo_flags[i])) {
				_echo.echo = i;
				if (mcr_dispatch(_hook_context, &_echoSig))
					return -1;
			}
		}
	}
	return CallNextHookEx(nPt->grab_move->id, nCode, wParam, lParam);
}

static LRESULT CALLBACK scroll_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	struct mcr_mod_intercept_platform *nPt =
			_hook_context->intercept.platform;
	if (nCode == HC_ACTION) {
		MSLLHOOKSTRUCT *p = (MSLLHOOKSTRUCT *) lParam;
		WORD delta = GET_WHEEL_DELTA_WPARAM
			     ((p->mouseData >> sizeof(WORD)));
		switch (wParam) {
		case WM_MOUSEWHEEL:
			_scr.dm[MCR_X] = 0;
			_scr.dm[MCR_Y] = delta;
			break;
		case WM_MOUSEHWHEEL:
			_scr.dm[MCR_X] = delta;
			_scr.dm[MCR_Y] = 0;
			break;
		}
		if (mcr_dispatch(_hook_context, &_scrSig))
			return -1;
	}
	return CallNextHookEx(nPt->grab_scroll->id, nCode, wParam, lParam);
}

static int verify_key_state(PBYTE keyState, size_t keyState_size)
{
	BYTE key = (BYTE) VK_RETURN;
	/*      short result ; /* Only used to refresh kb state. */
	/* Only get the keyboard state if not done already. The only way to */
	/* check for this is if keyState is in the right format. */
	if (keyState == NULL || keyState_size < 0xFF) {
		mset_error(EINVAL);
		return EINVAL;
	}
	/* sample key state so windows refreshes keyboard state */
	UNUSED(GetKeyState(key));

	/* Get pressed keys, error if returned false */
	if (!GetKeyboardState(keyState)) {
		mset_error(EINTR);
		dprint("Windows Error: %ld\n", GetLastError());
		return EINTR;
	}
	return 0;
}

int mcr_intercept_platform_initialize(struct mcr_context *ctx)
{
	int i;
	struct mcr_mod_intercept_platform *nPt =
		malloc(sizeof(struct mcr_mod_intercept_platform));
	if (!nPt) {
		mset_error(ENOMEM);
		return ENOMEM;
	}
	memset(nPt, 0, sizeof(struct mcr_mod_intercept_platform));
	_hook_context = ctx;
	ctx->intercept.platform = nPt;
	for (i = arrlen(_signal_set); i--;) {
		mcr_Signal_deinit(_signal_set[i]);
		mcr_Signal_init(_signal_set[i]);
		_signal_set[i]->is_dispatch = true;
	}

	mcr_Instance_set_all(&_echoSig, mcr_iHidEcho(ctx), &_echo, NULL);
	mcr_Instance_set_all(&_keySig, mcr_iKey(ctx), &_key, NULL);
	mcr_Instance_set_all(&_mcSig, mcr_iMoveCursor(ctx), &_mc, NULL);
	mcr_Instance_set_all(&_scrSig, mcr_iScroll(ctx), &_scr, NULL);
	/* Grabber implementation assumes it is heap-allocated for
	   threading without errors */
	fixme;
	for (i = 0; i < MCR_GRAB_COUNT; i++) {
		nPt->all_grabbers[i] = malloc(sizeof(struct mcr_Grabber));
		if (!nPt->all_grabbers[i]) {
			mset_error(ENOMEM);
			for (i = 0; i < MCR_GRAB_COUNT; i++) {
				if (nPt->all_grabbers[i]) {
					free(nPt->all_grabbers[i]);
					nPt->all_grabbers[i] = NULL;
				}
			}
			return ENOMEM;
		}
	}
	i = 0;
	nPt->grab_key = nPt->all_grabbers[i++];
	nPt->grab_move = nPt->all_grabbers[i++];
	nPt->grab_scroll = nPt->all_grabbers[i];
	mcr_Grabber_init(nPt->grab_key);
	if ((i = mcr_Grabber_set_all(nPt->grab_key, WH_KEYBOARD_LL, key_proc)))
		return i;
	mcr_Grabber_init(nPt->grab_move);
	if ((i = mcr_Grabber_set_all(nPt->grab_move, WH_MOUSE_LL, move_proc)))
		return i;
	mcr_Grabber_init(nPt->grab_scroll);
	if ((i = mcr_Grabber_set_all(nPt->grab_scroll, WH_MOUSEWHEEL_LL,
				     scroll_proc)))
		return i;

	mcr_HidEcho_init(&_echo);
	mcr_Key_init(&_key);
	mcr_Scroll_init(&_scr);
	mcr_MoveCursor_init(&_mc);
	return 0;
}

int mcr_intercept_platform_deinitialize(struct mcr_context *ctx)
{
	struct mcr_mod_intercept_platform *nPt = ctx->intercept.platform;
	for (int i = MCR_GRAB_COUNT; i--;) {
		/* Will block while waiting to finish. */
		mcr_Grabber_deinit(nPt->all_grabbers[i]);
		free(nPt->all_grabbers[i]);
	}
	memset(nPt, 0, sizeof(struct mcr_mod_intercept_platform));
	free(nPt);
	return 0;
}
