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

#include "mcr/intercept/win/nintercept.h"
#include "mcr/intercept/intercept.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "mcr/modules.h"

/* IsUserAnAdmin */
#if _WIN32_WINNT < 0x0600
	#include <shlobj.h>
#endif

/*! Print windows error, \ref mcr_errno, and set err to mcr_err
 *
 * Do not change original error of mcr_err */
#define winerr(err, fallbackError) { \
	ddo(fprintf(stderr, "Windows Error: %ld\n", GetLastError())); \
	if (!mcr_err) { \
		mcr_errno(fallbackError) \
		err = mcr_err; \
	} \
}

static bool mcr_is_privileged();
static int enable_impl(struct mcr_Grabber *grabPt, bool enable);
static int grab_hook(struct mcr_Grabber *grabPt);
static int grab_unhook(struct mcr_Grabber *grabPt);
static DWORD WINAPI thread_receive_hooking(LPVOID lpParam);
static DWORD WINAPI thread_disable(LPVOID lpParam);
static int disable_impl(struct mcr_Grabber *grabPt);

int mcr_Grabber_init(void *grabDataPt)
{
	int err = 0;
	struct mcr_Grabber *grabPt = grabDataPt;
	if (grabPt) {
		memset(grabPt, 0, sizeof(struct mcr_Grabber));
		if (!(grabPt->hMutex = CreateMutex(NULL, FALSE, NULL))) {
			winerr(err, EINTR);
			return err;
		}
	}
	return 0;
}

int mcr_Grabber_deinit(void *grabDataPt)
{
	bool locked;
	struct mcr_Grabber *grabPt = grabDataPt;
	int err = 0;
	if (!grabPt)
		return 0;

	locked = WaitForSingleObject(grabPt->hMutex,
								 MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	if (!locked) {
		winerr(err, EINTR);
	}
	disable_impl(grabPt);
	if (!CloseHandle(grabPt->hMutex)) {
		winerr(err, EINTR);
	}
	memset(grabPt, 0, sizeof(struct mcr_Grabber));
	return err;
}

int mcr_Grabber_set_all(struct mcr_Grabber *grabPt, int type, HOOKPROC proc)
{
	int err = 0;
	if (mcr_Grabber_is_enabled(grabPt)) {
		if ((err = mcr_Grabber_set_enabled(grabPt, false)))
			return err;
	}
	grabPt->type = type;
	grabPt->proc = proc;
	return err;
}

int mcr_Grabber_set_enabled(struct mcr_Grabber *grabPt, bool enable)
{
	int err = 0;
	bool locked;
	dassert(grabPt);
	/* No change, no need to lock */
	if (MCR_GRABBER_IS_ENABLED(*grabPt) == enable)
		return 0;
	/* Cannot grab without permissions, so no need to lock */
	if (!mcr_is_privileged()) {
		mset_error(EPERM);
		return EPERM;
	}
	locked = WaitForSingleObject(grabPt->hMutex,
								 MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	if (!locked) {
		winerr(err, EINTR);
	}
	err = enable_impl(grabPt, enable);
	/* Previous error, but not fatal */
	if (!err && !locked)
		err = mcr_err;
	if (locked && !ReleaseMutex(grabPt->hMutex)) {
		winerr(err, EINTR);
	}
	return err;
}

bool mcr_Grabber_is_enabled(struct mcr_Grabber * grabPt)
{
	dassert(grabPt);
	return MCR_GRABBER_IS_ENABLED(*grabPt);
}

static bool mcr_is_privileged()
{
#if _WIN32_WINNT >= 0x0600
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD uiAccess = 0;
		DWORD cbSize = sizeof(DWORD);
		/* Intended use, uiAccess */
		if(GetTokenInformation(hToken, TokenUIAccess, &uiAccess, sizeof(DWORD),
							   &cbSize))
			fRet = !!uiAccess;
		/* Another possibility is Admin */
		if(!fRet && GetTokenInformation(hToken, TokenElevation, &Elevation,
										sizeof(Elevation),
										&cbSize))
			fRet = Elevation.TokenIsElevated;
	}
	if(hToken)
		CloseHandle(hToken);
	return fRet;
#else
	return !!IsUserAnAdmin();
#endif
}

static int enable_impl(struct mcr_Grabber *grabPt, bool enable)
{
	dassert(grabPt);
	int err = 0;
	bool isEnabled = MCR_GRABBER_IS_ENABLED(*grabPt);
	if (enable) {
		/* Not already enabled, can enable, and does not have
		 * an intercepting thread. */
		if (!isEnabled) {
			if (grabPt->hThread) {
				winerr(err, EPERM);
				CloseHandle(grabPt->hThread);
				grabPt->hThread = NULL;
				grabPt->dwThread = 0;
			}
			if (!(grabPt->hThread = CreateThread(NULL, 0,
												 (LPTHREAD_START_ROUTINE)
												 thread_receive_hooking,
												 (LPVOID) grabPt, 0,
												 &grabPt->dwThread))) {
				winerr(err, EINTR);
			}
		}
	} else if (isEnabled) {
		DWORD dwThread = 0;
		/* Disabling from intercepting thread would block */
		HANDLE hThread = CreateThread(NULL, 0,
									  (LPTHREAD_START_ROUTINE) thread_disable,
									  (LPVOID) grabPt, 0, &dwThread);
		if (!hThread) {
			winerr(err, EINTR);
			return err;
		}
		if (!CloseHandle(hThread)) {
			winerr(err, EINTR);
		}
	}
	return err;
}

static int grab_hook(struct mcr_Grabber *grabPt)
{
	int err = 0;
	dassert(grabPt);
	if (!MCR_GRABBER_IS_ENABLED(*grabPt)) {
		if (!grabPt->hThread) {
			mset_error(EPERM);
			return EPERM;
		}
		if (!grabPt->dwThread) {
			mset_error(EPERM);
			return EPERM;
		}
		if (!(grabPt->hModule = GetModuleHandle(NULL))) {
			winerr(err, EINTR);
			return err;
		}
		/* This can return null if invalid values. */
		/* Will also be null if admin permissions are not raised */
		if (!(grabPt->id = SetWindowsHookEx(grabPt->type,
											grabPt->proc, grabPt->hModule,
											0))) {
			winerr(err, EINTR);
			grabPt->hModule = NULL;
		}
	}
	return err;
}

static int grab_unhook(struct mcr_Grabber *grabPt)
{
	int err = 0;
	dassert(grabPt);
	if (MCR_GRABBER_IS_ENABLED(*grabPt)) {
		if (!UnhookWindowsHookEx(grabPt->id)) {
			winerr(err, EINTR);
		}
		grabPt->id = 0;
		grabPt->hModule = NULL;
	}
	return err;
}

static DWORD WINAPI thread_receive_hooking(LPVOID lpParam)
{
	struct mcr_Grabber *me = (struct mcr_Grabber *)lpParam;
	MSG message;
	bool locked = WaitForSingleObject(me->hMutex,
									  MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	int err = grab_hook(me);
	if (locked)
		ReleaseMutex(me->hMutex);
	if (err) {
		enable_impl(me, false);
		return err;
	}
	memset(&message, 0, sizeof(message));
	while (MCR_GRABBER_IS_ENABLED(*me)) {
		if (MsgWaitForMultipleObjects(0, NULL, FALSE,
									  MCR_INTERCEPT_WAIT_MILLIS, QS_ALLINPUT)
			== WAIT_OBJECT_0) {
			while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
	}

	if (!MCR_GRABBER_IS_ENABLED(*me)) {
		locked = WaitForSingleObject(me->hMutex,
									 MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
		if (!locked) {
			winerr(err, EINTR);
		}
		err = grab_unhook(me);
		/* Previous error, but not fatal */
		if (!err && !locked)
			err = mcr_err;
		if (locked && !ReleaseMutex(me->hMutex)) {
			winerr(err, EINTR);
		}
	}
	return err;
}

static DWORD WINAPI thread_disable(LPVOID lpParam)
{
	struct mcr_Grabber *grabPt = (struct mcr_Grabber *)lpParam;
	int err = 0;
	bool locked = WaitForSingleObject(grabPt->hMutex,
									  MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	if (!locked) {
		winerr(err, EINTR);
	}
	err = disable_impl(grabPt);
	/* Previous error, but not fatal */
	if (!err && !locked)
		err = mcr_err;
	if (locked && !ReleaseMutex(grabPt->hMutex)) {
		winerr(err, EINTR);
	}
	return err;
}

static int disable_impl(struct mcr_Grabber *grabPt)
{
	int err = grab_unhook(grabPt);

	if (grabPt->hThread) {
		if (!CloseHandle(grabPt->hThread)) {
			winerr(err, EINTR);
		}
		grabPt->hThread = NULL;
		grabPt->dwThread = 0;
	}
	return err;
}
