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

/* IsUserAnAdmin */
#if _WIN32_WINNT < 0x0600
	#include <shlobj.h>
#endif

/*! Print windows error, \ref mcr_errno, and set err to mcr_err
 *
 *  Do not change original error of mcr_err */
#define winerr(fallbackError) { \
	ddo(fprintf(stderr, "Windows Error: %ld\n", GetLastError())); \
	mcr_errno(fallbackError); \
}

static bool mcr_is_privileged(void);
static int enable_impl(struct mcr_Grabber *grabPt, bool enable);
static int grab_hook(struct mcr_Grabber *grabPt);
static int grab_unhook(struct mcr_Grabber *grabPt);
static DWORD WINAPI thread_receive_hooking(LPVOID lpParam);
static DWORD WINAPI thread_disable(LPVOID lpParam);
static int disable_impl(struct mcr_Grabber *grabPt);

int mcr_Grabber_init(void *grabDataPt)
{
	struct mcr_Grabber *grabPt = grabDataPt;
	if (grabPt) {
		memset(grabPt, 0, sizeof(struct mcr_Grabber));
		if (!(grabPt->hMutex = CreateMutex(NULL, FALSE, NULL))) {
			winerr(EINTR);
			return mcr_err;
		}
		mcr_err = 0;
	}
	return 0;
}

int mcr_Grabber_deinit(void *grabDataPt)
{
	bool locked;
	struct mcr_Grabber *grabPt = grabDataPt;
	if (!grabPt)
		return 0;

	locked = WaitForSingleObject(grabPt->hMutex,
								 MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	mcr_err = 0;
	if (!locked)
		winerr(EINTR);
	disable_impl(grabPt);
	if (!CloseHandle(grabPt->hMutex))
		winerr(EINTR);
	memset(grabPt, 0, sizeof(struct mcr_Grabber));
	return mcr_err;
}

int mcr_Grabber_set_all(struct mcr_Grabber *grabPt, int type, HOOKPROC proc)
{
	if (mcr_Grabber_is_enabled(grabPt)) {
		if (mcr_Grabber_set_enabled(grabPt, false))
			return mcr_err;
	}
	grabPt->type = type;
	grabPt->proc = proc;
	return 0;
}

int mcr_Grabber_set_enabled(struct mcr_Grabber *grabPt, bool enable)
{
	bool locked;
	dassert(grabPt);
	/* No change, no need to lock */
	if (MCR_GRABBER_IS_ENABLED(*grabPt) == enable)
		return 0;
	/* Cannot grab without permissions, so no need to lock */
	if (!mcr_is_privileged())
		mset_error_return(EPERM);
	locked = WaitForSingleObject(grabPt->hMutex,
								 MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	mcr_err = 0;
	if (!locked)
		winerr(EINTR);
	enable_impl(grabPt, enable);
	if (locked && !ReleaseMutex(grabPt->hMutex))
		winerr(EINTR);
	return mcr_err;
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
							   &cbSize)) {
			fRet = !!uiAccess;
		}
		/* Another possibility is Admin */
		if(!fRet && GetTokenInformation(hToken, TokenElevation, &Elevation,
										sizeof(Elevation),
										&cbSize)) {
			fRet = !!Elevation.TokenIsElevated;
		}
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
	bool isEnabled = MCR_GRABBER_IS_ENABLED(*grabPt);
	mcr_err = 0;
	if (enable) {
		/* Not already enabled, can enable, and does not have
		 * an intercepting thread. */
		if (!isEnabled) {
			if (grabPt->hThread) {
				winerr(EPERM);
				CloseHandle(grabPt->hThread);
				grabPt->hThread = NULL;
				grabPt->dwThread = 0;
			}
			if (!(grabPt->hThread = CreateThread(NULL, 0,
												 (LPTHREAD_START_ROUTINE)
												 thread_receive_hooking,
												 (LPVOID) grabPt, 0,
												 &grabPt->dwThread))) {
				winerr(EINTR);
			}
		}
	} else if (isEnabled) {
		DWORD dwThread = 0;
		/* Disabling from intercepting thread would block */
		HANDLE hThread = CreateThread(NULL, 0,
									  (LPTHREAD_START_ROUTINE) thread_disable,
									  (LPVOID) grabPt, 0, &dwThread);
		if (!hThread) {
			winerr(EINTR);
			return mcr_err;
		}
		if (!CloseHandle(hThread))
			winerr(EINTR);
	}
	return mcr_err;
}

static int grab_hook(struct mcr_Grabber *grabPt)
{
	dassert(grabPt);
	mcr_err = 0;
	if (!MCR_GRABBER_IS_ENABLED(*grabPt)) {
		if (!grabPt->hThread)
			mset_error_return(EPERM);
		if (!grabPt->dwThread)
			mset_error_return(EPERM);
		if (!(grabPt->hModule = GetModuleHandle(NULL))) {
			winerr(EINTR);
			return mcr_err;
		}
		/* This can return null if invalid values. */
		/* Will also be null if admin permissions are not raised */
		if (!(grabPt->id = SetWindowsHookEx(grabPt->type,
											grabPt->proc, grabPt->hModule,
											0))) {
			winerr(EINTR);
			grabPt->hModule = NULL;
		}
	}
	return mcr_err;
}

static int grab_unhook(struct mcr_Grabber *grabPt)
{
	dassert(grabPt);
	mcr_err = 0;
	if (MCR_GRABBER_IS_ENABLED(*grabPt)) {
		if (!UnhookWindowsHookEx(grabPt->id))
			winerr(EINTR);
		grabPt->id = 0;
		grabPt->hModule = NULL;
	}
	return mcr_err;
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
		return (DWORD)err;
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

	mcr_err = 0;
	if (!MCR_GRABBER_IS_ENABLED(*me)) {
		locked = WaitForSingleObject(me->hMutex,
									 MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
		if (!locked)
			winerr(EINTR);
		grab_unhook(me);
		if (locked && !ReleaseMutex(me->hMutex))
			winerr(EINTR);
	}
	return (DWORD)mcr_err;
}

static DWORD WINAPI thread_disable(LPVOID lpParam)
{
	struct mcr_Grabber *grabPt = (struct mcr_Grabber *)lpParam;
	bool locked = WaitForSingleObject(grabPt->hMutex,
									  MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	mcr_err = 0;
	if (!locked)
		winerr(EINTR);
	disable_impl(grabPt);
	if (locked && !ReleaseMutex(grabPt->hMutex))
		winerr(EINTR);
	return (DWORD)mcr_err;
}

static int disable_impl(struct mcr_Grabber *grabPt)
{
	mcr_err = 0;
	grab_unhook(grabPt);

	if (grabPt->hThread) {
		if (!CloseHandle(grabPt->hThread))
			winerr(EINTR);
		grabPt->hThread = NULL;
		grabPt->dwThread = 0;
	}
	return mcr_err;
}
