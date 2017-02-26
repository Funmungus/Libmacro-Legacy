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
#include "mcr/intercept/private.h"
#include MCR_INTERCEPT_NATIVE_INC
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define winerr \
dmsg; \
ddo(fprintf(stderr, "Windows Error: %ld\n", GetLastError()));

static int enable_impl(struct mcr_Grabber *grabPt, bool enable);
static int grab_hook(struct mcr_Grabber *grabPt);
static int grab_unhook(struct mcr_Grabber *grabPt);
static DWORD WINAPI thread_receive_hooking(LPVOID lpParam);
static DWORD WINAPI thread_disable(LPVOID lpParam);
static int disable_impl(struct mcr_Grabber *grabPt);

void mcr_Grabber_init(void *grabDataPt)
{
	struct mcr_Grabber *grabPt = grabDataPt;
	if (!grabPt)
		return;
	memset(grabPt, 0, sizeof(struct mcr_Grabber));
	grabPt->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (!grabPt->hMutex) {
		winerr;
		ddo(exit(EXIT_FAILURE));
	}
}

void mcr_Grabber_deinit(void *grabDataPt)
{
	int err = 0;
	struct mcr_Grabber *grabPt = grabDataPt;
	if (!grabPt)
		return;
	if (WaitForSingleObject(grabPt->hMutex,
			MCR_INTERCEPT_WAIT_MILLIS) != WAIT_OBJECT_0 ||
		(err = disable_impl(grabPt))) {
		winerr;
		if (grabPt->id && !UnhookWindowsHookEx(grabPt->id)) {
			winerr;
		}
		if (grabPt->hThread && !CloseHandle(grabPt->hThread)) {
			winerr;
		}
		if (!CloseHandle(grabPt->hMutex)) {
			winerr;
		}
		ddo(exit(EXIT_FAILURE));
	}
	grabPt->hModule = NULL;
	grabPt->id = 0;
	grabPt->hThread = NULL;
	grabPt->dwThread = 0;
	if (!CloseHandle(grabPt->hMutex)) {
		winerr;
		ddo(exit(EXIT_FAILURE));
	}
	grabPt->hMutex = NULL;
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
	bool locked = WaitForSingleObject(grabPt->hMutex,
		MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	dassert(grabPt);
	if (!locked) {
		winerr;
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
	}
	err = enable_impl(grabPt, enable);
	if (locked && !ReleaseMutex(grabPt->hMutex)) {
		winerr;
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
	}
	return err;
}

bool mcr_Grabber_is_enabled(struct mcr_Grabber * grabPt)
{
	dassert(grabPt);
	return MCR_GRABBER_IS_ENABLED(*grabPt);
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
				err = errno;
				if (!err)
					err = EPERM;
				mset_error(err);
				CloseHandle(grabPt->hThread);
				return err;
			}
			if (!(grabPt->hThread = CreateThread(NULL, 0,
						(LPTHREAD_START_ROUTINE)
						thread_receive_hooking,
						(LPVOID) grabPt,
						CREATE_SUSPENDED,
						&grabPt->dwThread))) {
				winerr;
				err = errno;
				if (!err)
					err = EINTR;
				mset_error(err);
				return err;
			}
			if ((err = grab_hook(grabPt)) ||
				ResumeThread(grabPt->hThread) == (DWORD) - 1) {
				winerr;
				if (!err)
					err = EINTR;
				disable_impl(grabPt);
				return err;
			}
		}
	} else if (isEnabled) {
		/* Disabling from intercepting thread would block */
		HANDLE hThread = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE) thread_disable,
			(LPVOID) grabPt, 0, &grabPt->dwThread);
		if (!hThread) {
			err = EINTR;
			return err;
		}
		if (!CloseHandle(hThread))
			err = EINTR;
	}
	return err;
}

static int grab_hook(struct mcr_Grabber *grabPt)
{
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
			winerr;
			mset_error(EINTR);
			return EINTR;
		}
		/* This can return NULL if invalid values. */
		if (!(grabPt->id = SetWindowsHookEx(grabPt->type,
					grabPt->proc, grabPt->hModule,
					grabPt->dwThread))) {
			winerr;
			mset_error(EINTR);
			return EINTR;
		}
	}
	return 0;
}

static int grab_unhook(struct mcr_Grabber *grabPt)
{
	int err = 0;
	dassert(grabPt);
	if (MCR_GRABBER_IS_ENABLED(*grabPt)) {
		if (!UnhookWindowsHookEx(grabPt->id)) {
			winerr;
			err = errno;
			if (!err)
				err = EINTR;
			mset_error(err);
		}
		grabPt->id = 0;
		grabPt->hModule = NULL;
	}
	return err;
}

static DWORD WINAPI thread_receive_hooking(LPVOID lpParam)
{
	struct mcr_Grabber *me = (struct mcr_Grabber *)lpParam;
	int err = 0;
	MSG message;
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

	bool locked = WaitForSingleObject(me->hMutex,
		MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0;
	if (!locked) {
		winerr;
		err = EINTR;
		mset_error(EINTR);
	}
	if ((err = grab_unhook(me))) {
		mset_error(err);
	}
	if (locked && !ReleaseMutex(me->hMutex)) {
		winerr;
		err = EINTR;
		mset_error(EINTR);
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
		winerr;
		err = EINTR;
		mset_error(EINTR);
	}
	err = disable_impl(grabPt);
	if (locked && !ReleaseMutex(grabPt->hMutex)) {
		winerr;
		err = EINTR;
		mset_error(EINTR);
	}
	return err;
}

static int disable_impl(struct mcr_Grabber *grabPt)
{
	int err = grab_unhook(grabPt);

	if (grabPt->hThread) {
		if (WaitForSingleObject(grabPt->hThread,
				MCR_INTERCEPT_WAIT_MILLIS) == WAIT_OBJECT_0) {
			if (!CloseHandle(grabPt->hThread)) {
				winerr;
				err = EINTR;
				mset_error(EINTR);
			}
		} else {
			winerr;
			err = EINTR;
			mset_error(EINTR);
			if (!CloseHandle(grabPt->hThread)) {
				winerr;
				mset_error(EINTR);
			}
		}
		grabPt->hThread = NULL;
		grabPt->dwThread = 0;
	}
	return err;
}
