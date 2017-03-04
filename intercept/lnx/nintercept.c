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
#include "mcr/standard/private.h"
#include "mcr/intercept/private.h"
#include MCR_INTERCEPT_NATIVE_INC
#include "mcr/modules.h"
#include <errno.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	struct mcr_context *ctx;
	struct mcr_Grabber grabber;
	bool is_complete;
} _grab_complete;

typedef struct {
	struct mcr_context *ctx;
	bool bVal;
} _context_bool;

static bool is_enabled_impl(struct mcr_context *ctx);
static int set_enabled_impl(struct mcr_context *ctx, bool enable);
static unsigned int get_mods_impl(struct mcr_context *ctx);
static int thread_enable(void *threadArgs);
static int thread_set_enable_impl(struct mcr_context *ctx, bool enable);
static int grab_impl(struct mcr_context *ctx, const char *grabPath);
static int intercept_start(void *threadArgs);
static int read_grabber_exclusive(struct mcr_context *ctx,
	struct mcr_Grabber *grabPt);
static unsigned int modify_eventbits(struct mcr_context *ctx,
	char *keybit_values);
static unsigned int max_mod_val();
static int clear_grabbers_impl(struct mcr_context *ctx);

#define MODBIT_SIZE ((sizeof(char)) * max_mod_val() / 8 + 1)

bool mcr_intercept_is_enabled(struct mcr_context *ctx)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = is_enabled_impl(ctx);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err;
}

int mcr_intercept_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	if (getuid() && geteuid()) {
		mset_error(EPERM);
		return EPERM;
	}
	int thrdErr = mtx_lock(&nPt->lock);
	int err = set_enabled_impl(ctx, enable);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err;
}

unsigned int mcr_intercept_modifiers(struct mcr_context *ctx)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	int thrdErr = mtx_lock(&nPt->lock);
	unsigned int ret = get_mods_impl(ctx);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return ret;
}

int mcr_intercept_add_grab(struct mcr_context *ctx, const char *grabPath)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = mcr_StringSet_add(&nPt->grab_paths, &grabPath, 1, true);
	dassert(grabPath);
	/* add grabber? */
	fixme;
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err;
}

void mcr_intercept_remove_grab(struct mcr_context *ctx, const char *grabPath)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	int thrdErr = mtx_lock(&nPt->lock);
	/* remove grabber? */
	fixme;
	dassert(grabPath);
	mcr_StringSet_remove(&nPt->grab_paths, grabPath);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
}

int mcr_intercept_set_grabs(struct mcr_context *ctx, const char **allGrabPaths,
	size_t pathCount)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = mcr_StringSet_replace(&nPt->grab_paths, allGrabPaths,
		pathCount);
	/* remove grabber? */
	fixme;
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err;
}

int mcr_intercept_native_initialize(struct mcr_context *ctx)
{
	int thrdErr = 0, err = 0;
	/* Free in deinitialize */
	struct mcr_mod_intercept_native *nPt =
		malloc(sizeof(struct mcr_mod_intercept_native));
	if (!nPt) {
		mset_error(ENOMEM);
		return ENOMEM;
	}
	memset(nPt, 0, sizeof(struct mcr_mod_intercept_native));
	ctx->intercept.native = nPt;
	/* Make const value of MODBIT_SIZE */
	fixme;
	if ((thrdErr = mtx_init(&nPt->lock, mtx_plain)) != thrd_success
		|| (thrdErr = cnd_init(&nPt->cnd)) != thrd_success) {
		err = mcr_thrd_errno(thrdErr);
		mset_error(err);
		return err;
	}
	mcr_Array_init(&nPt->grab_completes);
	mcr_Array_set_all(&nPt->grab_completes, mcr_ref_compare,
		sizeof(_grab_complete *));
	mcr_StringSet_init(&nPt->grab_paths);
	mcr_StringSet_set_all(&nPt->grab_paths, mcr_String_compare);
	return err;
}

int mcr_intercept_native_deinitialize(struct mcr_context *ctx)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = clear_grabbers_impl(ctx);
	if (err) {
		mset_error(err);
	}
	mcr_StringSet_deinit(&nPt->grab_paths);
	mcr_Array_deinit(&nPt->grab_completes);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	mtx_destroy(&nPt->lock);
	cnd_destroy(&nPt->cnd);
	free(nPt);
	ctx->intercept.native = NULL;
	return err;
}

static bool is_enabled_impl(struct mcr_context *ctx)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	_grab_complete **ptArr = MCR_ARR_FIRST(nPt->grab_completes);
	size_t i = nPt->grab_completes.used;
	while (i--) {
		if (ptArr[i]->grabber.fd != -1 && !ptArr[i]->is_complete)
			return true;
	}
	return false;
}

static int set_enabled_impl(struct mcr_context *ctx, bool enable)
{
	int err = 0, thrdErr = thrd_success;
	thrd_t trd;
	_context_bool *argEnable;
	/* Do not disable if already disabled. */
	if (enable || is_enabled_impl(ctx)) {
		/* Free in thread */
		argEnable = malloc(sizeof(_context_bool));
		if (!argEnable) {
			mset_error(ENOMEM);
			return ENOMEM;
		}
		argEnable->ctx = ctx;
		argEnable->bVal = enable;
		if ((thrdErr = thrd_create(&trd, thread_enable,
					argEnable)) == thrd_success) {
			if ((thrdErr = thrd_detach(trd)) != thrd_success) {
				err = mcr_thrd_errno(thrdErr);
				mset_error(err);
				return err;
			}
		} else {
			free(argEnable);
			err = mcr_thrd_errno(thrdErr);
			mset_error(err);
			return err;
		}
	}
	return 0;
}

static unsigned int get_mods_impl(struct mcr_context *ctx)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	_grab_complete **grabSet = MCR_ARR_FIRST(nPt->grab_completes);
	unsigned int ret;
	char *bitRetrieval = nPt->bit_retrieval;
	size_t modSize = MODBIT_SIZE, i = nPt->grab_completes.used;
	int err = ioctl(mcr_genDev.fd, EVIOCGKEY(modSize), bitRetrieval), fd;
	if (err < 0) {
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
		return MCR_MOD_ANY;
	}
	/* handle MCR_MOD_ANY as an error */
	fixme;
	ret = modify_eventbits(ctx, bitRetrieval);
	while (i--) {
		if ((fd = grabSet[i]->grabber.fd) != -1) {
			if ((err = ioctl(fd, EVIOCGKEY(modSize),
						bitRetrieval)) < 0) {
				err = errno;
				if (!err)
					err = EINTR;
				mset_error(err);
			} else {
				ret |= modify_eventbits(ctx, bitRetrieval);
			}
		}
	}
	return ret;
}

static int thread_enable(void *threadArgs)
{
	_context_bool *cbPt = threadArgs;
	struct mcr_mod_intercept_native *nPt = cbPt->ctx->intercept.native;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = thread_set_enable_impl(cbPt->ctx, cbPt->bVal);
	/* Free args */
	free(threadArgs);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err ? thrd_error : thrd_success;
}

static int thread_set_enable_impl(struct mcr_context *ctx, bool enable)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	mcr_String *pathArr = MCR_ARR_FIRST(nPt->grab_paths);
	size_t i = nPt->grab_paths.used;
	int err = 0;
	if (enable) {
		while (i--) {
			if ((err = grab_impl(ctx, pathArr[i].array)))
				return err;
		}
	} else {
		if ((err = clear_grabbers_impl(ctx)))
			return err;
	}
	return err;
}

/* /pre mutex locked */
static int grab_impl(struct mcr_context *ctx, const char *grabPath)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	int error = 0, thrdErr = thrd_success;
	thrd_t trd;
	/* Free in ? */
	_grab_complete *pt;
	if (!grabPath || grabPath[0] == '\0')
		return 0;
	pt = malloc(sizeof(_grab_complete));
	dassert(grabPath);
	if (!pt) {
		mset_error(ENOMEM);
		return ENOMEM;
	}
	memset(pt, 0, sizeof(_grab_complete));
	pt->ctx = ctx;
	mcr_Grabber_init(&pt->grabber);
	if ((error = mcr_Grabber_set_path(&pt->grabber, grabPath))) {
		mcr_Grabber_deinit(&pt->grabber);
		free(pt);
		return error;
	}
	if ((error = mcr_Array_add(&nPt->grab_completes, &pt, 1, true))) {
		/* Not able to add reference */
		mcr_Array_remove(&nPt->grab_completes, &pt);
	} else if ((thrdErr =
			thrd_create(&trd, intercept_start,
				pt)) != thrd_success) {
		/* Not able to start thread */
		mcr_Array_remove(&nPt->grab_completes, &pt);
		error = mcr_thrd_errno(thrdErr);
		mset_error(error);
	} else {
		/* Thread created, and grabber currently in use */
		if ((thrdErr = thrd_detach(trd)) != thrd_success) {
			/* Not able to detach, but thread is running */
			error = mcr_thrd_errno(thrdErr);
			mset_error(error);
		}
		/* GrabComplete set has changed */
		cnd_broadcast(&nPt->cnd);
		/* Do not free, grabber is in use */
		return error;
	}
	if (error) {
		mcr_Grabber_deinit(&pt->grabber);
		free(pt);
	}
	return error;
}

static inline int abspos(int evPos)
{
	switch (evPos) {
	case ABS_X:
		return MCR_X;
	case ABS_Y:
		return MCR_Y;
	case ABS_Z:
		return MCR_Z;
	}
	return MCR_DIMENSION_CNT;
}

static inline int relpos(int evPos)
{
	switch (evPos) {
	case REL_X:
	case REL_HWHEEL:
		return MCR_X;
	case REL_Y:
	case REL_WHEEL:
		return MCR_Y;
	case REL_Z:
	case REL_DIAL:
		return MCR_Z;
	}
	return MCR_DIMENSION_CNT;
}

static inline void abs_set_current(struct mcr_MoveCursor *abs,
	bool hasPosArray[])
{
	if (!(hasPosArray)[MCR_X]) {
		abs->pos[MCR_X] = mcr_cursor[MCR_X];
	}
	if (!(hasPosArray)[MCR_Y]) {
		abs->pos[MCR_Y] = mcr_cursor[MCR_Y];
	}
	if (!(hasPosArray)[MCR_Z]) {
		abs->pos[MCR_Z] = mcr_cursor[MCR_Z];
	}
}

/* Note: Excess time setting up and releasing is ok. Please try to limit
 * time-consumption during grabbing and callbacks. */
/* \pre Thread args is heap-allocated _grab_complete. It is assumed to
 * already have a path, which is already added to the _grabCompletes.
 */
static int intercept_start(void *threadArgs)
{
	/* Free in ? */
	_grab_complete *gcPt = threadArgs;
	struct mcr_mod_intercept_native *nPt = gcPt->ctx->intercept.native;
	struct mcr_Grabber *grabPt = &gcPt->grabber;
	int thrdErr = thrd_success, mtxErr = thrd_success, err = 0;
	struct timespec delay;
	dassert(threadArgs);
	delay.tv_sec = 0;
	/* nano -> micro -> milli -> 10 milli */
	delay.tv_nsec = 1000 * 1000 * 10;
	/* 1. Create grab from path.
	 * 2. Enable (Current step)
	 * 3. Use grabber
	 * 4. Remove grabber reference in main before thread ends.
	 */
	mtxErr = mtx_lock(&nPt->lock);
	err = mcr_Grabber_set_enabled(grabPt, true);
	/* Give a delay between grabs before unlocking. */
	thrd_sleep(&delay, NULL);
	if (mtxErr == thrd_success)
		mtx_unlock(&nPt->lock);
	if (err) {
		gcPt->is_complete = 1;
		cnd_broadcast(&nPt->cnd);
		return thrd_error;
	}
	/* Will return an error if not enabled. */
	thrdErr = read_grabber_exclusive(gcPt->ctx, grabPt);
	mtxErr = mtx_lock(&nPt->lock);
	err = mcr_Grabber_set_enabled(grabPt, false);
	gcPt->is_complete = 1;
	cnd_broadcast(&nPt->cnd);
	if (mtxErr == thrd_success)
		mtx_unlock(&nPt->lock);
	if (thrdErr) {
		err = mcr_thrd_errno(thrdErr);
		mset_error(err);
	}
	if (thrdErr)
		return thrdErr;
	return err ? thrd_error : thrd_success;
}

static int read_grabber_exclusive(struct mcr_context *ctx,
	struct mcr_Grabber *grabPt)
{
	/* For abs and rel, keep memory of having such event type
	 * at least once. */
#define DISP_WRITEMEM(signal) \
if (mcr_dispatch(ctx, &(signal)) && writegen) \
	writegen = false;
#define DISP_WRITEMEM_ABS(signal) \
if (mcr_dispatch(ctx, &(signal))) { \
	if (writeabs) \
		writeabs = false; \
} else if (!writeabs) { \
	++writeabs; \
}
	struct input_event events[MCR_GRAB_SET_LENGTH];
	struct input_event *curVent, *end;
	/* Always assume writing to the generic device.  Start writing
	   to abs and rel only if those events happen at least once. */
	ssize_t rdb;
	int i, pos;
	/* Write generic whenever not blocked, write abs only when available */
	bool writegen = true, writeabs = false;
	bool bAbs[MCR_DIMENSION_CNT] = { 0 };
	int *echoFound = NULL;
	struct mcr_Key key = { 0 };
	struct mcr_HidEcho echo = { 0 };
	struct mcr_MoveCursor abs = { 0 }, rel = {
	0};
	struct mcr_Scroll scr = { 0 };
	struct mcr_Signal keysig, echosig, abssig, relsig, scrsig;
	struct pollfd fd = { 0 };
	dassert(grabPt);
	mcr_Signal_init(&keysig);
	mcr_Signal_init(&echosig);
	mcr_Signal_init(&abssig);
	mcr_Signal_init(&relsig);
	mcr_Signal_init(&scrsig);
	mcr_Instance_set_all(&keysig, mcr_iKey(ctx), &key, NULL);
	mcr_Instance_set_all(&echosig, mcr_iHidEcho(ctx), &echo, NULL);
	mcr_Instance_set_all(&abssig, mcr_iMoveCursor(ctx), &abs, NULL);
	mcr_Instance_set_all(&relsig, mcr_iMoveCursor(ctx), &rel, NULL);
	mcr_Instance_set_all(&scrsig, mcr_iScroll(ctx), &scr, NULL);
	keysig.is_dispatch = true;
	echosig.is_dispatch = true;
	abssig.is_dispatch = true;
	relsig.is_dispatch = true;
	scrsig.is_dispatch = true;
	rel.is_justify = true;
	fd.events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND;
	fd.fd = grabPt->fd;
	/* Disable point 1 */
	while (grabPt->fd != -1) {
		/* Disable point 2. */
		i = poll(&fd, 1, MCR_POLL_TIMEOUT);
		/* Do not err on closed events */
		if (grabPt->fd == -1)
			return thrd_success;
		/* Error or no data in 10 seconds, try again. */
		if (!i || i < 0)
			continue;
		/* Disable point 3, but assume readable from poll above.
		 * If rdb < input_event size then we are returning an error. */
		rdb = read(fd.fd, events, sizeof(events));
		/* Do not err on closed events */
		if (grabPt->fd == -1)
			return thrd_success;
		if (rdb < (ssize_t) sizeof(struct input_event)) {
			i = errno;
			if (!i)
				i = EINTR;
			mset_error(i);
			return thrd_error;
		}
		curVent = events;
		end = (struct input_event *)(((char *)events) + rdb);
		while (curVent < end) {
			switch (curVent->type) {
				/* Handle MSC */
			case EV_MSC:
				if (curVent->code == MSC_SCAN) {
					/* Consume a key that already has scan code. */
					if (key.scan) {
						DISP_WRITEMEM(keysig);
						key.key = 0;
						key.scan = curVent->value;
						key.up_type = MCR_BOTH;
					} else {
						key.scan = curVent->value;
					}
				}
				break;
				/* Handle KEY */
			case EV_KEY:
				if (key.key) {
					echoFound =
						MCR_MAP_ELEMENT(mcr_keyToEcho
						[key.up_type], &key.key);
					if (echoFound) {
						echoFound =
							MCR_MAP_VALUEOF
							(mcr_keyToEcho
							[key.up_type],
							echoFound);
						echo.echo = *echoFound;
						DISP_WRITEMEM(echosig);
						/* No need to reset echo,
						 * which depends on EV_KEY */
					}
					DISP_WRITEMEM(keysig);
					key.key = curVent->code;
					key.scan = 0;
					key.up_type =
						curVent->value ? MCR_DOWN :
						MCR_UP;
				} else {
					key.key = curVent->code;
					key.up_type =
						curVent->value ? MCR_DOWN :
						MCR_UP;
				}
				break;
				/* Handle ABS */
			case EV_ABS:
				pos = abspos(curVent->code);
				if (bAbs[pos]) {
					abs_set_current(&abs, bAbs);
					DISP_WRITEMEM_ABS(abssig);
					MCR_DIMENSIONS_ZERO(bAbs);
				}
				abs.pos[pos] = curVent->value;
				++bAbs[pos];
				break;
			case EV_REL:
				pos = relpos(curVent->code);
				switch (curVent->code) {
				case REL_X:
				case REL_Y:
				case REL_Z:
					if (rel.pos[pos]) {
						DISP_WRITEMEM(relsig);
						MCR_DIMENSIONS_ZERO(rel.pos);
					}
					rel.pos[pos] = curVent->value;
					break;
				default:
					/* Currently assuming scroll if not relative movement. */
					if (scr.dm[pos]) {
						DISP_WRITEMEM(scrsig);
						MCR_DIMENSIONS_ZERO(scr.dm);
					}
					scr.dm[pos] = curVent->value;
					break;
				}
				break;
			}
			++curVent;
		}
		/* Call final event, it was not called yet. */
		if (key.key || key.scan) {
			if (key.key) {
				echoFound =
					MCR_MAP_ELEMENT(mcr_keyToEcho
					[key.up_type], &key.key);
				if (echoFound) {
					echoFound =
						MCR_MAP_VALUEOF(mcr_keyToEcho
						[key.up_type], echoFound);
					echo.echo = *echoFound;
					if (mcr_dispatch(ctx, &echosig)) {
						if (writegen)
							writegen = false;
					}
				}
			}
			DISP_WRITEMEM(keysig);
			key.key = 0;
			key.scan = 0;
			key.up_type = MCR_BOTH;
		}
		if (bAbs[MCR_X] || bAbs[MCR_Y] || bAbs[MCR_Z]) {
			abs_set_current(&abs, bAbs);
			DISP_WRITEMEM_ABS(abssig);
			MCR_DIMENSIONS_ZERO(bAbs);
		}
		for (i = MCR_DIMENSION_CNT; i--;) {
			if (rel.pos[i]) {
				DISP_WRITEMEM(relsig);
				MCR_DIMENSIONS_ZERO(rel.pos);
				break;
			}
		}
		for (i = MCR_DIMENSION_CNT; i--;) {
			if (scr.dm[i]) {
				DISP_WRITEMEM(scrsig);
				MCR_DIMENSIONS_ZERO(scr.dm);
				break;
			}
		}
		if (writegen) {
			if (write(mcr_genDev.fd, events, rdb) < 0) {
				i = errno;
				if (!i)
					i = EINTR;
				mset_error(i);
				return thrd_error;
			}
		} else {
			++writegen;
		}
		if (writeabs && write(mcr_absDev.fd, events, rdb) < 0) {
			i = errno;
			if (!i)
				i = EINTR;
			mset_error(i);
			return thrd_error;
		}
	}
	return thrd_success;
#undef DISP_WRITEMEM
#undef DISP_WRITEMEM_ABS
}

static unsigned int modify_eventbits(struct mcr_context *ctx,
	char *keybitValues)
{
	int curKey;
	unsigned int i, modValOut = 0;
	size_t modKeysCount = mcr_Key_mod_count(ctx);
	unsigned int *modArr = malloc(sizeof(unsigned int) * modKeysCount);
	if (!modArr) {
		mset_error(ENOMEM);
		return ENOMEM;
	}
	mcr_Key_mod_all(ctx, modArr, modKeysCount);
	for (i = modKeysCount; i--;) {
		curKey = mcr_Key_mod_key(ctx, modArr[i]);
		if ((keybitValues[MCR_EVENTINDEX(curKey)] &
				MCR_EVENTBIT(curKey)))
			modValOut |= modArr[i];
	}
	free(modArr);
	return modValOut;
}

static unsigned int max_mod_val()
{
	unsigned int val = KEY_LEFTALT;
	val = KEY_RIGHTALT > val ? KEY_RIGHTALT : val;
	val = KEY_OPTION > val ? KEY_OPTION : val;
	val = KEY_COMPOSE > val ? KEY_COMPOSE : val;
	val = KEY_LEFTCTRL > val ? KEY_LEFTCTRL : val;
	val = KEY_RIGHTCTRL > val ? KEY_RIGHTCTRL : val;
	val = KEY_FN > val ? KEY_FN : val;
	val = KEY_FRONT > val ? KEY_FRONT : val;
	val = KEY_LEFTSHIFT > val ? KEY_LEFTSHIFT : val;
	val = KEY_RIGHTSHIFT > val ? KEY_RIGHTSHIFT : val;
	val = KEY_LEFTMETA > val ? KEY_LEFTMETA : val;
	val = KEY_RIGHTMETA > val ? KEY_RIGHTMETA : val;
	return val;
}

static inline void grab_complete_disable(_grab_complete ** gcPtPt, int *error)
{
	int err = mcr_Grabber_set_enabled(&(*gcPtPt)->grabber, false);
	if (err) {
		*error = err;
	}
}

static inline void grab_complete_deinit(_grab_complete ** gcPtPt)
{
	mcr_Grabber_deinit(&(*gcPtPt)->grabber);
	free(*gcPtPt);
}

/*! \pre _enableLock locked */
/*! \post _enableLock locked */
static int clear_grabbers_impl(struct mcr_context *ctx)
{
	struct mcr_mod_intercept_native *nPt = ctx->intercept.native;
	/* Disable all, wait for all batch operation. */
	size_t prev = nPt->grab_completes.used, i;
	struct timespec timeout = { 10, 0 };
	_grab_complete **ptArr;
	int error = 0, thrdErr;
	/* TODO : thread destroy on timeout. */
	fixme;
#define localExp(itPt) \
grab_complete_disable(itPt, &error);
	MCR_ARR_FOR_EACH(nPt->grab_completes, localExp);
	while (nPt->grab_completes.used) {
		/* Disable any that are being created while clearing. */
		if (nPt->grab_completes.used >= prev) {
			MCR_ARR_FOR_EACH(nPt->grab_completes, localExp);
		}
		ptArr = MCR_ARR_FIRST(nPt->grab_completes);
		for (i = nPt->grab_completes.used; i--;) {
			if (ptArr[i]->is_complete) {
				grab_complete_deinit(ptArr + i);
				mcr_Array_remove_index(&nPt->grab_completes, i,
					1);
			}
		}
		prev = nPt->grab_completes.used;
		/* Impatient free-all, unconditional end for no wait signals */
		if (prev &&
			(thrdErr =
				cnd_timedwait(&nPt->cnd, &nPt->lock,
					&timeout)) == thrd_timedout) {
			error = mcr_thrd_errno(thrdErr);
			mset_error(error);
			MCR_ARR_FOR_EACH(nPt->grab_completes, localExp);
			MCR_ARR_FOR_EACH(nPt->grab_completes,
				grab_complete_deinit);
			nPt->grab_completes.used = 0;
		}
	}
#undef localExp
	return error;
}
