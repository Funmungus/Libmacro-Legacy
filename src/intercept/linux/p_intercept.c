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

#include "mcr/intercept/linux/p_intercept.h"
#include "mcr/intercept/intercept.h"

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcr/libmacro.h"

typedef struct {
	struct mcr_context *ctx;
	struct mcr_Grabber grabber;
} _grab_context;

/* Remember (*gcPt) is a _grab_context reference, and not a grabber */
static int grab_context_malloc(_grab_context ** gcPt, struct mcr_context *ctx)
{
	*gcPt = malloc(sizeof(_grab_context));
	if (!*gcPt)
		mset_error_return(ENOMEM);
	if (mcr_Grabber_init(&(*gcPt)->grabber)) {
		free(*gcPt);
		*gcPt = NULL;
		return mcr_err;
	}
	mcr_Grabber_set_blocking(&(*gcPt)->grabber, ctx->intercept.blockable);
	(*gcPt)->ctx = ctx;
	return 0;
}

static int grab_context_free(_grab_context ** gcPt)
{
	int err = mcr_Grabber_deinit(&(*gcPt)->grabber);
	free(*gcPt);
	*gcPt = NULL;
	return err;
}

static inline void grab_context_disable(_grab_context ** gcPtPt)
{
	mcr_Grabber_set_enabled(&(*gcPtPt)->grabber, false);
}

typedef struct {
	struct mcr_context *ctx;
	bool bVal;
} _context_bool;

static bool is_enabled_impl(struct mcr_context *ctx);
static int set_enabled_impl(struct mcr_context *ctx, bool enable);
static unsigned int get_mods_impl(struct mcr_context *ctx);
static int thread_enable(void *threadArgs);
static int grab_impl(struct mcr_context *ctx, const char *grabPath);
static int intercept_start(void *threadArgs);
static int read_grabber_loop(struct mcr_context *ctx,
							 struct mcr_Grabber *grabPt);
static unsigned int modify_eventbits(struct mcr_context *ctx,
									 unsigned int *modBuffer, size_t modBufferSize, char *keybit_values);
static unsigned int max_modifier_val(void);
static int clear_grabbers(struct mcr_context *ctx);

static inline size_t modbit_size()
{
	return sizeof(char) * max_modifier_val() / 8 + 1;
}

bool mcr_intercept_is_enabled(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = is_enabled_impl(ctx);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err;
}

int mcr_intercept_set_enabled(struct mcr_context *ctx, bool enable)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = set_enabled_impl(ctx, enable);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err;
}

unsigned int mcr_intercept_modifiers(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	int thrdErr = mtx_lock(&nPt->lock);
	unsigned int ret = get_mods_impl(ctx);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return ret;
}

int mcr_intercept_add_grab(struct mcr_context *ctx, const char *grabPath)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = mcr_StringSet_add(&nPt->grab_paths, &grabPath, 1, true);
	dassert(grabPath);
	/// \todo add grabber?
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err;
}

void mcr_intercept_remove_grab(struct mcr_context *ctx, const char *grabPath)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	int thrdErr = mtx_lock(&nPt->lock);
	/// \todo remove grabber?
	dassert(grabPath);
	mcr_StringSet_remove(&nPt->grab_paths, grabPath);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
}

int mcr_intercept_set_grabs(struct mcr_context *ctx, const char **allGrabPaths,
							size_t pathCount)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	int thrdErr = mtx_lock(&nPt->lock);
	int err = mcr_StringSet_replace(&nPt->grab_paths, allGrabPaths,
									pathCount);
	/// \todo remove grabber?
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	return err;
}

int mcr_intercept_platform_initialize(struct mcr_context *ctx)
{
	int thrdErr = 0, err = 0;
	/* Free in deinitialize */
	struct mcr_intercept_platform *nPt =
		malloc(sizeof(struct mcr_intercept_platform));
	if (!nPt)
		mset_error_return(ENOMEM);
	memset(nPt, 0, sizeof(struct mcr_intercept_platform));
	if ((thrdErr = mtx_init(&nPt->lock, mtx_plain)) != thrd_success) {
		err = mcr_thrd_errno(thrdErr);
		free(nPt);
		mset_error_return(err);
	}
	ctx->intercept.platform = nPt;
	mcr_Array_init(&nPt->grab_contexts);
	mcr_Array_set_all(&nPt->grab_contexts, mcr_ref_compare,
					  sizeof(_grab_context *));
	mcr_StringSet_init(&nPt->grab_paths);
	mcr_StringSet_set_all(&nPt->grab_paths, mcr_String_compare);
	return err;
}

int mcr_intercept_platform_deinitialize(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	/* Clear grabbers manages lock internally */
	int thrdErr;
	if (clear_grabbers(ctx))
		dmsg;
	thrdErr = mtx_lock(&nPt->lock);
	mcr_StringSet_deinit(&nPt->grab_paths);
	mcr_Array_deinit(&nPt->grab_contexts);
	if (thrdErr == thrd_success)
		mtx_unlock(&nPt->lock);
	mtx_destroy(&nPt->lock);
	free(nPt);
	ctx->intercept.platform = NULL;
	return mcr_err;
}

static bool is_enabled_impl(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	_grab_context **ptArr = MCR_ARR_FIRST(nPt->grab_contexts);
	size_t i = nPt->grab_contexts.used;
	while (i--) {
		if (ptArr[i]->grabber.fd != -1)
			return true;
	}
	return false;
}

static int set_enabled_impl(struct mcr_context *ctx, bool enable)
{
	_context_bool *argEnable;
	/* Do not disable if already disabled. */
	if (enable || is_enabled_impl(ctx)) {
		/* Free in thread */
		argEnable = malloc(sizeof(_context_bool));
		if (!argEnable)
			mset_error_return(ENOMEM);
		argEnable->ctx = ctx;
		argEnable->bVal = enable;
		if (mcr_thrd(thread_enable, argEnable)) {
			dmsg;
			free(argEnable);
			return mcr_err;
		}
	}
	return 0;
}

static unsigned int get_mods_impl(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	_grab_context **grabSet = MCR_ARR_FIRST(nPt->grab_contexts);
	unsigned int ret;
	/* One modifier for each key that has a modifier mapped */
	size_t modKeysCount = mcr_Key_modifier_count(ctx);
	unsigned int *modArr = malloc(sizeof(unsigned int) * modKeysCount);
	char *bitRetrieval = nPt->bit_retrieval;
	size_t modSize = modbit_size(), i = nPt->grab_contexts.used;
	int fd;
	if (!modArr)
		mset_error_return(ENOMEM);
	/* Fill modifiers mapped from a key */
	mcr_Key_modifier_all(ctx, modArr, modKeysCount);
	if (ioctl(mcr_genDev.fd, EVIOCGKEY(modSize), bitRetrieval) < 0) {
		mcr_errno(EINTR);
		free(modArr);
		return MCR_MF_NONE;
	}
	/// \bug handle MCR_MOD_ANY as an error
	ret = modify_eventbits(ctx, modArr, modKeysCount, bitRetrieval);
	while (i--) {
		if ((fd = grabSet[i]->grabber.fd) != -1) {
			if (ioctl(fd, EVIOCGKEY(modSize),
					  bitRetrieval) < 0) {
				mcr_errno(EINTR);
			} else {
				ret |= modify_eventbits(ctx, modArr,
										modKeysCount, bitRetrieval);
			}
		}
	}
	free(modArr);
	return ret;
}

static int thread_enable(void *threadArgs)
{
	_context_bool cb = *(_context_bool *) threadArgs;
	struct mcr_intercept_platform *nPt = cb.ctx->intercept.platform;
	int err = 0, thrdErr;
	mcr_String *pathArr = MCR_ARR_FIRST(nPt->grab_paths);
	size_t i = nPt->grab_paths.used;
	/* Free args when no longer in use */
	free(threadArgs);
	/* Always clear. Do not grab what is already grabbed */
	/* Clear grabbers manages mutex internally */
	if (nPt->grab_contexts.used)
		err = clear_grabbers(cb.ctx);
	if (!err && cb.bVal) {
		thrdErr = mtx_lock(&nPt->lock);
		while (i--) {
			if (grab_impl(cb.ctx, pathArr[i].array))
				break;
		}
		if (thrdErr == thrd_success)
			mtx_unlock(&nPt->lock);
	}
	return mcr_err ? thrd_error : thrd_success;
}

/* \pre mutex locked
 * \post mutex locked
 */
static int grab_impl(struct mcr_context *ctx, const char *grabPath)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	/* Freed when thread is done with it */
	_grab_context *pt;
	if (!grabPath || grabPath[0] == '\0')
		return 0;
	dassert(grabPath);
	if (grab_context_malloc(&pt, ctx))
		return mcr_err;
	if (mcr_Grabber_set_path(&pt->grabber, grabPath)) {
		grab_context_free(&pt);
		return mcr_err;
	}
	if (mcr_Array_add(&nPt->grab_contexts, &pt, 1, true)) {
		/* Not able to add reference */
		mcr_Array_remove(&nPt->grab_contexts, &pt);
		grab_context_free(&pt);
		return mcr_err;
	} else if (mcr_thrd(intercept_start,
						pt)) {
		/* Not able to start thread */
		mcr_Array_remove(&nPt->grab_contexts, &pt);
		grab_context_free(&pt);
		return mcr_err;
	}
	return 0;
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
	if (!(hasPosArray)[MCR_X])
		abs->pos[MCR_X] = mcr_cursor[MCR_X];
	if (!(hasPosArray)[MCR_Y])
		abs->pos[MCR_Y] = mcr_cursor[MCR_Y];
	if (!(hasPosArray)[MCR_Z])
		abs->pos[MCR_Z] = mcr_cursor[MCR_Z];
}

/* Note: Excess time setting up and releasing is ok. Please try to limit
 * time-consumption during grabbing and callbacks. */
/* \pre Thread args is heap-allocated _grab_context. It is assumed to
 * already have a path.
 */
static int intercept_start(void *threadArgs)
{
	/* Free in thread when done with it */
	_grab_context *gcPt = threadArgs;
	struct mcr_intercept_platform *nPt = gcPt->ctx->intercept.platform;
	struct mcr_Grabber *grabPt = &gcPt->grabber;
	int thrdErr = thrd_success, mtxErr = thrd_success;
	struct timespec delay;
	dassert(threadArgs);
	delay.tv_sec = 0;
	/* 10 milli */
	delay.tv_nsec = 1000 * 1000 * 10;
	/*! \todo Enable grabber before thread.
	 * 1. Create grab from path.
	 * 2. Enable (Current step)
	 * 3. Use grabber
	 * 4. Always free grabber before end of thread
	 */
	mtxErr = mtx_lock(&nPt->lock);
	if (mcr_Grabber_set_enabled(grabPt, true)) {
		mcr_Array_remove(&nPt->grab_contexts, &gcPt);
		grab_context_free(&gcPt);
		if (mtxErr == thrd_success)
			mtx_unlock(&nPt->lock);
		return thrd_error;
	}
	/* Give a delay between grabs before unlocking. */
	thrd_sleep(&delay, NULL);
	if (mtxErr == thrd_success)
		mtx_unlock(&nPt->lock);
	/* Will return an error if not enabled. */
	thrdErr = read_grabber_loop(gcPt->ctx, grabPt);
	mtxErr = mtx_lock(&nPt->lock);
	mcr_Grabber_set_enabled(grabPt, false);
	mcr_Array_remove(&nPt->grab_contexts, &gcPt);
	grab_context_free(&gcPt);
	if (mtxErr == thrd_success)
		mtx_unlock(&nPt->lock);
	if (thrdErr) {
		if (!mcr_err) {
			mset_error(mcr_thrd_errno(thrdErr));
		}
		return thrdErr;
	}
	return mcr_err ? thrd_error : thrd_success;
}

static int read_grabber_loop(struct mcr_context *ctx,
							 struct mcr_Grabber *grabPt)
{
	/* For abs and rel, keep memory of having such event type
	 * at least once. */
#define DISP_WRITEMEM(signal) \
if (mcr_dispatch(ctx, &(signal)) && blocking && writegen) { \
	writegen = false; \
}
#define DISP_WRITEMEM_ABS(signal) \
if (mcr_dispatch(ctx, &(signal))) { \
	if (blocking && writeabs) \
		writeabs = false; \
} else if (blocking && !writeabs) { \
	writeabs = true; \
}
	struct input_event events[MCR_GRAB_SET_LENGTH];
	struct input_event *curVent, *end;
	/* Always assume writing to the generic device.  Start writing
	   to abs and rel only if those events happen at least once. */
	int rdb;
	int i, pos;
	/* Write generic whenever not blocked, write abs only when available */
	bool writegen = true, writeabs = false, blocking = grabPt->blocking;
	bool bAbs[MCR_DIMENSION_CNT] = { 0 };
	int *echoFound = NULL;
	struct mcr_Key key = { 0 };
	struct mcr_HidEcho echo = { 0 };
	struct mcr_MoveCursor abs = { 0 }, rel = {
		0
	};
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
		if (rdb < (int) sizeof(struct input_event)) {
			mcr_errno(0);
			if (!mcr_err || mcr_err == EINTR)
				return thrd_success;
			return thrd_error;
		}
		curVent = events;
		end = (struct input_event *)(((char *)events) + rdb);
		while (curVent < end) {
			switch (curVent->type) {
			/* Handle KEY */
			case EV_KEY:
				if (key.key) {
					echoFound =
						MCR_MAP_ELEMENT(mcr_keyToEcho
										[key.apply], &key.key);
					if (echoFound) {
						echoFound =
							MCR_MAP_VALUEOF
							(mcr_keyToEcho
							 [key.apply],
							 echoFound);
						echo.echo = *echoFound;
						DISP_WRITEMEM(echosig);
						/* No need to reset echo,
						 * which depends on EV_KEY */
					}
					DISP_WRITEMEM(keysig);
					key.key = curVent->code;
					key.apply =
						curVent->value ? MCR_SET :
						MCR_UNSET;
				} else {
					key.key = curVent->code;
					key.apply =
						curVent->value ? MCR_SET :
						MCR_UNSET;
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
				bAbs[pos] = true;
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
		if (key.key) {
			echoFound =
				MCR_MAP_ELEMENT(mcr_keyToEcho
								[key.apply], &key.key);
			if (echoFound) {
				echoFound =
					MCR_MAP_VALUEOF(mcr_keyToEcho
									[key.apply], echoFound);
				echo.echo = *echoFound;
				if (mcr_dispatch(ctx, &echosig) && blocking && writegen)
					writegen = false;
			}
			DISP_WRITEMEM(keysig);
			key.key = 0;
			key.apply = MCR_BOTH;
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
		/* Non-grab does not write to device */
		if (blocking && writegen) {
			if (write(mcr_genDev.fd, events, rdb) < 0) {
				/// \todo Is non-error ok with thrd error or success?
				mcr_errno(0);
				/* EINTR is disabled during read. */
				if (!mcr_err || mcr_err == EINTR)
					return thrd_success;
				return thrd_error;
			}
		} else {
			writegen = true;
		}
		/* Non-grab does not write to device */
		if (blocking && writeabs) {
			if (write(mcr_absDev.fd, events, rdb) < 0) {
				mcr_errno(0);
				/* EINTR is disabled during read. */
				if (!mcr_err || mcr_err == EINTR)
					return thrd_success;
				return thrd_error;
			}
		}
	}
	return thrd_success;
#undef DISP_WRITEMEM
#undef DISP_WRITEMEM_ABS
}

static unsigned int modify_eventbits(struct mcr_context *ctx,
									 unsigned int *modBuffer, size_t modBufferSize, char *keybitValues)
{
	int curKey;
	unsigned int modValOut = 0;
	size_t i;
	if (!modBuffer || !modBufferSize)
		return 0;
	/* For each modifier, find the modifier of that key, and check
	 * if key is set in the key bit value set. */
	for (i = modBufferSize; i--;) {
		curKey = mcr_Key_modifier_key(ctx, modBuffer[i]);
		if (curKey != MCR_KEY_ANY
			&& (keybitValues[MCR_EVENTINDEX(curKey)] &
				MCR_EVENTBIT(curKey))) {
			modValOut |= modBuffer[i];
		}
	}
	return modValOut;
}

static unsigned int max_modifier_val()
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

/*! \pre Mutex not locked */
/*! \post Mutex not locked */
static int clear_grabbers(struct mcr_context *ctx)
{
	struct mcr_intercept_platform *nPt = ctx->intercept.platform;
	/* Disable all, wait for all batch operation. */
	int timeout = 50;	// Total 10 sec
	struct mcr_NoOp delay = { 0 };
	int thrdErr;
	delay.msec = 200;
	/// \todo thread destroy on timeout.
	while (nPt->grab_contexts.used && timeout--) {
		/* Disable all */
		thrdErr = mtx_lock(&nPt->lock);
		MCR_ARR_FOR_EACH(nPt->grab_contexts, grab_context_disable);
		if (thrdErr == thrd_success)
			mtx_unlock(&nPt->lock);
		/* While unlocked and paused, threads will remove themselves */
		mcr_NoOp_send_data(&delay);
	}
	/* Impatient free-all, unconditional end for timed out waiting */
	if (nPt->grab_contexts.used) {
		thrdErr = mtx_lock(&nPt->lock);
		MCR_ARR_FOR_EACH(nPt->grab_contexts, grab_context_disable);
		MCR_ARR_FOR_EACH(nPt->grab_contexts, grab_context_free);
		nPt->grab_contexts.used = 0;
		if (thrdErr == thrd_success)
			mtx_unlock(&nPt->lock);
	}
	return 0;
}
