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

#include "mcr/intercept/windows/p_intercept.h"
#include "mcr/intercept/intercept.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "mcr/libmacro.h"

#define WPARAM_APPLY_TYPE(wParam) \
((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) ? \
		MCR_UNSET : MCR_SET)

static struct mcr_context *_hook_context = NULL;
/// \todo Move these to context
static struct mcr_HidEcho _echo = { 0 };
static struct mcr_Key _key = { 0 };
static struct mcr_MoveCursor _mc = { .is_justify = false };
static struct mcr_Scroll _scr = { 0 };
static struct mcr_Signal _echoSig = {
	.is_dispatch = true,
	.instance.data.data = &_echo
}, _keySig = {
	.is_dispatch = true,
	.instance.data.data = &_key
}, _mcSig = {
	.is_dispatch = true,
	.instance.data.data = &_mc
}, _scrSig = {
	.is_dispatch = true,
	.instance.data.data = &_scr
};

extern LONG _lastX;
LONG _lastX = 0;
extern LONG _lastY;
LONG _lastY = 0;

static struct mcr_Signal *_signal_set[] = { &_echoSig, &_keySig, &_mcSig,
		   &_scrSig
};

static const WPARAM _echo_WM[] = {
	WM_LBUTTONDOWN, WM_LBUTTONUP, WM_MBUTTONDOWN,
	WM_MBUTTONUP, WM_RBUTTONDOWN, WM_RBUTTONUP,
	WM_XBUTTONDOWN, WM_XBUTTONUP
};
//static const DWORD _echo_flags[] = {
//	MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEDOWN,
//	MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP,
//	MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP
//};

static int verify_key_state(PBYTE keyState, size_t keyState_size);
static LRESULT CALLBACK key_proc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK mouse_proc(int nCode, WPARAM wParam, LPARAM lParam);

bool mcr_intercept_is_enabled(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	for (int i = MCR_GRAB_COUNT; i--;) {
		if (mcr_Grabber_is_enabled(nPt->all_grabbers[i]))
			return true;
	}
	return false;
}

int mcr_intercept_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	for (int i = MCR_GRAB_COUNT; i--;) {
		if (mcr_Grabber_set_enabled(nPt->all_grabbers[i],
									enable)) {
			return mcr_err;
		}
	}
	return 0;
}

unsigned int mcr_intercept_modifiers(struct mcr_context *ctx)
{
	unsigned int values = 0;
	BYTE keyState[0x100];
	int key;
	size_t modlen = mcr_Key_modifier_count(ctx), i = modlen;
	unsigned int *modBuffer;
	/* If cannot get the mod state, return current */
	if (verify_key_state(keyState, 0xFF))
		return *mcr_modifiers(ctx);

	modBuffer = malloc(sizeof(int) * modlen);
	if (!modBuffer) {
		mset_error(ENOMEM);
		return *mcr_modifiers(ctx);
	}

	mcr_Key_modifier_all(ctx, modBuffer, modlen);

	while (i--) {
		key = mcr_Key_modifier_key(ctx, modBuffer[i]);
		/* high bit is down, lowest byte is toggled. */
		/* high bit of byte is 0x80, high bit of short is 0x8000 */
		if ((keyState[key] & 0x8080) != 0)
			values |= modBuffer[i];
	}
	free(modBuffer);
	return values;
}

bool mcr_intercept_key_is_enabled(struct mcr_context * ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	return MCR_GRABBER_IS_ENABLED(*nPt->grab_key);
}

int mcr_intercept_key_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	return mcr_Grabber_set_enabled(nPt->grab_key, enable);
}

bool mcr_intercept_mouse_is_enabled(struct mcr_context * ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	return MCR_GRABBER_IS_ENABLED(*nPt->grab_mouse);
}

int mcr_intercept_move_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	return mcr_Grabber_set_enabled(nPt->grab_mouse, enable);
}

static LRESULT __stdcall key_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	struct mcr_intercept_platform *nPt =
			_hook_context->intercept.platform;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *) lParam;
		_key.key = (int)p->vkCode;
		_key.apply = (WPARAM_APPLY_TYPE(wParam));
		if (mcr_dispatch(_hook_context, &_keySig) && _hook_context->intercept.blockable)
			return -1;
	}
	return CallNextHookEx(nPt->grab_key->id, nCode, wParam, lParam);
}

static LRESULT __stdcall mouse_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
	struct mcr_intercept_platform *nPt =
			_hook_context->intercept.platform;
	bool blockable = _hook_context->intercept.blockable;
	if (nCode == HC_ACTION) {
		MSLLHOOKSTRUCT *p = (MSLLHOOKSTRUCT *) lParam;
		DWORD flags = p->flags;
		short delta = GET_WHEEL_DELTA_WPARAM(p->mouseData);
		POINT current;
		switch (wParam) {
		case WM_MOUSEMOVE:
			if (GetCursorPos(&current)) {
				_mc.is_justify = !(flags & MOUSE_MOVE_ABSOLUTE);
				if (_mc.is_justify) {
					_mc.pos[MCR_X] = current.x - _lastX;
					_mc.pos[MCR_Y] = current.y - _lastY;
				} else {
					_mc.pos[MCR_X] = current.x;
					_mc.pos[MCR_Y] = current.y;
				}
				_lastX = current.x;
				_lastY = current.y;
				if (mcr_dispatch(_hook_context, &_mcSig) && blockable)
					return -1;
			}
			break;
		case WM_MOUSEWHEEL:
			_scr.dm[MCR_X] = 0;
			_scr.dm[MCR_Y] = delta;
			if (mcr_dispatch(_hook_context, &_scrSig) && blockable)
				return -1;
			break;
		case WM_MOUSEHWHEEL:
			_scr.dm[MCR_X] = delta;
			_scr.dm[MCR_Y] = 0;
			if (mcr_dispatch(_hook_context, &_scrSig) && blockable)
				return -1;
			break;
		default:
			if (wParam >= WM_MOUSEFIRST && wParam <= WM_MOUSELAST) {
				for (unsigned int i = arrlen(_echo_WM); i--;) {
					if (wParam == _echo_WM[i]) {
						_echo.echo = i;
						if (mcr_dispatch(_hook_context, &_echoSig) && blockable)
							return -1;
						break;
					}
				}
			}
			break;
		}
	}
	return CallNextHookEx(nPt->grab_mouse->id, nCode, wParam, lParam);
}

static int verify_key_state(PBYTE keyState, size_t keyState_size)
{
	BYTE key = (BYTE) VK_RETURN;
	/* Only get the keyboard state if not done already. The only way to */
	/* check for this is if keyState is in the right format. */
	if (keyState == NULL || keyState_size < 0xFF)
		mset_error_return(EINVAL);
	/* sample key state so windows refreshes keyboard state */
	UNUSED(GetKeyState(key));

	/* Get pressed keys, error if returned false */
	if (!GetKeyboardState(keyState)) {
		dprint("Windows Error: %ld\n", GetLastError());
		mset_error_return(EINTR);
	}
	return 0;
}

int mcr_intercept_platform_initialize(struct mcr_context *ctx)
{
	int i;
	struct mcr_intercept_platform *nPt =
		malloc(sizeof(struct mcr_intercept_platform));
	if (!nPt)
		mset_error_return(ENOMEM);
	memset(nPt, 0, sizeof(struct mcr_intercept_platform));
	_hook_context = ctx;
	ctx->intercept.platform = nPt;

	_echoSig.isignal = mcr_iHidEcho(ctx);
	_keySig.isignal = mcr_iKey(ctx);
	_mcSig.isignal = mcr_iMoveCursor(ctx);
	_scrSig.isignal = mcr_iScroll(ctx);
	/* Grabber implementation assumes it is heap-allocated for
	   threading without errors */
	/// \bug Grabber implementation assumes it is heap-allocated.
	for (i = 0; i < MCR_GRAB_COUNT; i++) {
		nPt->all_grabbers[i] = malloc(sizeof(struct mcr_Grabber));
		if (!nPt->all_grabbers[i]) {
			mset_error(ENOMEM);
			/* An error, clear previous successful */
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
	nPt->grab_mouse = nPt->all_grabbers[i++];
	mcr_Grabber_init(nPt->grab_key);
	if ((i = mcr_Grabber_set_all(nPt->grab_key, WH_KEYBOARD_LL, key_proc)))
		return i;
	mcr_Grabber_init(nPt->grab_mouse);
	if ((i = mcr_Grabber_set_all(nPt->grab_mouse, WH_MOUSE_LL, mouse_proc)))
		return i;
	return 0;
}

int mcr_intercept_platform_deinitialize(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	int i;
	for (i = MCR_GRAB_COUNT; i--;) {
		mcr_Grabber_set_enabled(nPt->all_grabbers[i], false);
	}
	Sleep(500);
	for (i = MCR_GRAB_COUNT; i--;) {
		/* Will block while waiting to finish. */
		mcr_Grabber_deinit(nPt->all_grabbers[i]);
		free(nPt->all_grabbers[i]);
	}
	memset(nPt, 0, sizeof(struct mcr_intercept_platform));
	free(nPt);
	return 0;
}
