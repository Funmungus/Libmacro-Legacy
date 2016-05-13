/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

#include "mcr/macro/macro.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define thrd_conv_err(thrdErr) \
thrdErr = mcr_thrd_errno(thrdErr); \
mset_error(thrdErr);

static int thread_macro(void *);
static int thread_wait_reset(void *);
/* Will attempt to gently detach all threads, but force termination
 * if timed out. */
static void clear_threads(struct mcr_Macro *mcrPt, enum mcr_Interrupt clearType,
	bool stickyInterrupt);

static int resetthread(struct mcr_Macro *mcrPt)
{
	thrd_t trd;
	int thrdErr = thrd_create(&trd, thread_wait_reset, mcrPt);
	if (thrdErr == thrd_success) {
		if ((thrdErr = thrd_detach(trd)) != thrd_success) {
			thrd_conv_err(thrdErr);
			return thrdErr;
		}
		mcrPt->queued = 0;
		return 0;
	}
	thrd_conv_err(thrdErr);
	return thrdErr;
}

/* If threading fails during init, we exit */
void mcr_Macro_init(void *mcrDataPt)
{
	struct mcr_Macro *mcrPt = mcrDataPt;
	int thrdErr;
	if (!mcrPt)
		return;
	memset(mcrDataPt, 0, sizeof(struct mcr_Macro));
	mcrPt->thread_max = 1;
	mcrPt->interruptor = MCR_CONTINUE;
	/* Instead of mutex locking, disable macro when modifying */
	fixme;
	if ((thrdErr = cnd_init(&mcrPt->cnd)) != thrd_success ||
		(thrdErr = mtx_init(&mcrPt->lock, mtx_plain)) != thrd_success) {
		thrd_conv_err(thrdErr);
		exit(thrdErr);
	}
	mcr_Array_init(&mcrPt->signal_set);
	mcrPt->signal_set.element_size = sizeof(struct mcr_Signal);
	mcr_Trigger_init(&mcrPt->trigger);
	mcr_Trigger_set_all(&mcrPt->trigger, mcr_Macro_trigger, mcrPt);
}

int mcr_Macro_set_all(struct mcr_Macro *mcrPt, bool block,
	bool sticky, unsigned int threadMax, bool enable,
	struct mcr_context *ctx)
{
	dassert(mcrPt);
	mcrPt->block = block;
	mcrPt->sticky = sticky;
	mcrPt->thread_max = threadMax;
	mcrPt->mcr_pt = ctx;
	return mcr_Macro_set_enabled(mcrPt, enable);
}

void mcr_Macro_free(void *mcrDataPt)
{
	struct mcr_Macro *mcrPt = mcrDataPt;
	int thrdErr;
	if (!mcrPt)
		return;
	/* lock here? */
	fixme;
	if ((thrdErr = mtx_lock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(exit(thrdErr));
	}
	/* Absolutey disable. */
	clear_threads(mcrDataPt, MCR_DISABLE, true);
	MCR_ARR_FOR_EACH(mcrPt->signal_set, MCR_EXP(mcr_Signal_free_foreach),);
	mcr_Array_free(&mcrPt->signal_set);
	mcr_Trigger_free(&mcrPt->trigger);
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(exit(thrdErr));
	}
	cnd_destroy(&mcrPt->cnd);
	mtx_destroy(&mcrPt->lock);
}

int mcr_Macro_copy(void *dstPt, void *srcPt)
{
	dassert(dstPt);
	struct mcr_Macro *dPt = dstPt, *sPt = srcPt;
	int thrdErr, err = mcr_Macro_set_enabled(dPt, false);
	if (err)
		return err;
	if (!sPt)
		return 0;

	if ((thrdErr = mtx_lock(&sPt->lock) != thrd_success)) {
		thrd_conv_err(thrdErr);
		ddo(return thrdErr);
	}
	dPt->block = sPt->block;
	dPt->sticky = sPt->sticky;
	dPt->thread_max = sPt->thread_max;
	if ((err = mcr_Macro_set_signals(dPt,
				(struct mcr_Signal *)sPt->signal_set.array,
				sPt->signal_set.used)))
		return err;
	if ((err = mcr_Macro_set_trigger(dPt, &sPt->trigger)))
		return err;
	if ((err = mcr_Macro_set_enabled(dPt, MCR_MACRO_IS_ENABLED(*sPt))))
		return err;
	/* Leave queued at 0.  Copying data will not trigger */
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&sPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		return thrdErr;
	}
	return err;
}

int mcr_Macro_set_trigger(struct mcr_Macro *mcrPt, struct mcr_Trigger *trigPt)
{
	int interruptMem = mcrPt->interruptor;
	int err = mcr_Macro_interrupt(mcrPt, MCR_PAUSE);
	int thrdErr;
	dassert(mcrPt);
	if (err)
		return err;
	if ((thrdErr = mtx_lock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(return thrdErr);
	}
	if ((err = mcr_Trigger_copy(&mcrPt->trigger, trigPt)))
		return err;
	mcr_Trigger_set_all(&mcrPt->trigger, mcr_Macro_trigger, mcrPt);
	if ((err = mcr_Macro_interrupt(mcrPt, interruptMem)))
		return err;
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		return thrdErr;
	}
	return err;
}

void mcr_Macro_set_trigger_actor(struct mcr_Macro *mcrPt,
	struct mcr_Trigger *trigPt)
{
	dassert(mcrPt);
	dassert(trigPt);
	trigPt->trigger = mcr_Macro_trigger;
	trigPt->actor = mcrPt;
}

int mcr_Macro_interrupt(struct mcr_Macro *mcrPt,
	enum mcr_Interrupt interruptType)
{
	int thrdErr = mtx_lock(&mcrPt->lock), err = 0;
	enum mcr_Interrupt prev = mcrPt->interruptor;
	dassert(mcrPt);
	if (thrdErr != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(return thrdErr);
		err = thrdErr;
	}
	/* Enable or disable, just make it happen
	 * If interrupting, macro will be reenabled from the reset thread */
	switch (interruptType) {
	case MCR_PAUSE:
		/* Pausing while already paused? Somebody has concurrency issues. */
#ifdef DEBUG
		if (prev == MCR_PAUSE) {
			err = EBUSY;
			mset_error(EBUSY);
			fprintf(stderr, "Possible " MCR_STR(MCR_PAUSE)
				" concurrency issue\n");
		}
#endif
		/* just do it, no reset */
	case MCR_CONTINUE:
	case MCR_DISABLE:
	case MCR_INTERRUPT:
		if (prev != interruptType)
			mcrPt->interruptor = interruptType;
		break;
		/* Also need to reset after all have been interrupted. */
	case MCR_INTERRUPT_ALL:
		if (prev != MCR_INTERRUPT_ALL) {
			mcrPt->interruptor = interruptType;
			err = resetthread(mcrPt);
		}
		break;
	default:
		fprintf(stderr, "Invalid " MCR_STR(enum mcr_Interrupt) "\n");
		err = EINVAL;
		mset_error(EINVAL);
	}
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(return thrdErr);
	}
	return err;
}

struct mcr_Signal *mcr_Macro_signal(struct mcr_Macro *mcrPt, size_t index)
{
	dassert(mcrPt);
	int thrdErr = mtx_lock(&mcrPt->lock);
	struct mcr_Signal *ret = MCR_ARR_ELEMENT(mcrPt->signal_set, index);
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(return NULL);
	}
	return ret;
}

void mcr_Macro_signals(struct mcr_Macro *mcrPt,
	struct mcr_Signal *signalbuffer, size_t bufferCount)
{
	dassert(mcrPt);
	dassert(signalbuffer);
	int thrdErr = mtx_lock(&mcrPt->lock);
	struct mcr_Signal *arr = (struct mcr_Signal *)mcrPt->signal_set.array;
	size_t i = 0;
	while (i < mcrPt->signal_set.used && i < bufferCount) {
		signalbuffer[i] = arr[i];
		++i;
	}
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(exit(thrdErr));
	}
}

int mcr_Macro_set_signals(struct mcr_Macro *mcrPt,
	struct mcr_Signal *signalSet, size_t signalCount)
{
	dassert(mcrPt);
	size_t i;
	int ret, thrdErr;
	bool enableMem;
	struct mcr_Signal initial, *itPt;
	mcr_Signal_init(&initial);
	/* Another operation happening */
	if (mcrPt->interruptor == MCR_PAUSE)
		return EBUSY;
	thrdErr = mtx_lock(&mcrPt->lock);
	enableMem = mcrPt->interruptor != MCR_DISABLE;
	/* When setting signals absolute disable. */
	clear_threads(mcrPt, MCR_DISABLE, true);
	MCR_ARR_FOR_EACH(mcrPt->signal_set, MCR_EXP(mcr_Signal_free_foreach),);
	mcrPt->signal_set.used = 0;
	if (!(ret = mcr_Array_resize(&mcrPt->signal_set, signalCount)) &&
		!(ret = mcr_Array_fill(&mcrPt->signal_set, 0, signalCount,
				&initial))) {
		itPt = MCR_ARR_FIRST(mcrPt->signal_set);
		for (i = signalCount; i--;) {
			if ((ret = mcr_Signal_copy(itPt + i, signalSet + i))) {
				mset_error(ret);
			}
		}
	} else {
		mset_error(ret);
	}
	if (enableMem) {
		mcrPt->interruptor = MCR_INTERRUPT_ALL;
		ret = resetthread(mcrPt);
	}
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		mset_error(thrdErr);
		ret = thrdErr;
	}
	return ret;
}

int mcr_Macro_set_signal(struct mcr_Macro *mcrPt, struct mcr_Signal *copySig,
	size_t index)
{
	dassert(mcrPt);
	struct mcr_Signal *sigPt = mcr_Macro_signal(mcrPt, index);
	if (sigPt)
		return mcr_Signal_copy(sigPt, copySig);
	else
		return mcr_Macro_push_signal(mcrPt, copySig);
}

int mcr_Macro_insert_signal(struct mcr_Macro *mcrPt,
	struct mcr_Signal *copySig, size_t index)
{
	dassert(mcrPt);
	int ret;
	struct mcr_Signal addSig;
	mcr_Signal_init(&addSig);
	if (index >= mcrPt->signal_set.used) {
		return mcr_Macro_push_signal(mcrPt, copySig);
	} else {
		mcr_Signal_copy(&addSig, copySig);
		if ((ret = mcr_Array_insert(&mcrPt->signal_set, index, &addSig,
					1))) {
			mset_error(ret);
			mcr_Signal_free(&addSig);
			return ret;
		}
	}
	return 0;
}

void mcr_Macro_remove_signal(struct mcr_Macro *mcrPt, size_t index)
{
	dassert(mcrPt);
	struct mcr_Signal *sigPt = mcr_Macro_signal(mcrPt, index);
	if (sigPt) {
		mcr_Signal_free(sigPt);
		mcr_Array_remove_index(&mcrPt->signal_set, index, 1);
	}
}

int mcr_Macro_push_signal(struct mcr_Macro *mcrPt, struct mcr_Signal *newSig)
{
	dassert(mcrPt);

	int thrdErr = mtx_lock(&mcrPt->lock);
	int ret;
	struct mcr_Signal addSig;
	mcr_Signal_init(&addSig);
	if (!(ret = mcr_Signal_copy(&addSig, newSig))) {
		if ((ret = mcr_Array_push(&mcrPt->signal_set, &addSig, 0))) {
			mset_error(ret);
			mcr_Signal_free(&addSig);
		}
	} else {
		mset_error(ret);
	}
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ret = thrdErr;
	}
	return ret;
}

void mcr_Macro_pop_signal(struct mcr_Macro *mcrPt)
{
	dassert(mcrPt);
	int thrdErr = mtx_lock(&mcrPt->lock);
	struct mcr_Signal *sigPt;
	if (mcrPt->signal_set.used) {
		sigPt = MCR_ARR_LAST(mcrPt->signal_set);
		mcr_Signal_free(sigPt);
		--mcrPt->signal_set.used;
	}
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(exit(thrdErr));
	}
}

int mcr_Macro_set_enabled(struct mcr_Macro *mcrPt, bool enable)
{
	dassert(mcrPt);
	return mcr_Macro_interrupt(mcrPt, enable ? MCR_CONTINUE : MCR_DISABLE);
}

bool mcr_Macro_receive(void *mcrDataPt, struct mcr_Signal * sigPt,
	unsigned int mods)
{
/* Error unlock and exit on debug builds */
#define onErr \
ddo( \
	if (mtxErr == thrd_success) \
		mtxErr = mtx_unlock(&mcrPt->lock); \
	thrd_conv_err(mtxErr); \
	exit(mtxErr); \
);
	UNUSED(sigPt);
	UNUSED(mods);
	struct mcr_Macro *mcrPt = mcrDataPt;
	dassert(mcrPt);
	int mtxErr = mtx_lock(&mcrPt->lock), thrdErr;
	thrd_t trd;
	/* Interrupt or disable, then do not start thread. */
	switch (mcrPt->interruptor) {
	case MCR_CONTINUE:
	case MCR_PAUSE:
		if (!mcrPt->thread_max || mcrPt->thread_max > MCR_THREAD_MAX) {
			mset_error(EINVAL);
			mcrPt->thread_max = 1;
		}
		if (mcrPt->thread_count < mcrPt->thread_max &&
			mcrPt->thread_count < MCR_THREAD_MAX) {
			/* TODO: Possible unused threads if only one queued item. */
			if ((thrdErr = thrd_create(&trd, thread_macro,
						mcrPt)) == thrd_success) {
				++mcrPt->queued;
				++mcrPt->thread_count;
				if ((thrdErr = thrd_detach(trd)) !=
					thrd_success) {
					thrd_conv_err(thrdErr);
					onErr;
				}
			} else {
				thrd_conv_err(thrdErr);
				onErr;
			}
		}
		break;
	case MCR_INTERRUPT:
		mcrPt->interruptor = MCR_CONTINUE;
	case MCR_INTERRUPT_ALL:
	case MCR_DISABLE:
		break;
	default:
		mset_error(EINVAL);
		fprintf(stderr, "Invalid " MCR_STR(enum mcr_Interrupt) "\n");
		onErr;
		break;
	}
	if (mtxErr == thrd_success
		&& (mtxErr = mtx_unlock(&mcrPt->lock) != thrd_success)) {
		thrd_conv_err(mtxErr);
		ddo(exit(mtxErr));
	}
	return mcrPt->block;
#undef onErr
}

bool mcr_Macro_trigger(void *trigDataPt, struct mcr_Signal * sigPt,
	unsigned int mods)
{
/* Error unlock and exit on debug builds */
#define onErr \
ddo( \
	if (mtxErr == thrd_success) \
		mtxErr = mtx_unlock(&mcrPt->lock); \
	thrd_conv_err(mtxErr); \
	exit(mtxErr); \
);
	dassert(trigDataPt);
	UNUSED(sigPt);
	UNUSED(mods);
	struct mcr_Macro *mcrPt = ((struct mcr_Trigger *)trigDataPt)->actor;
	dassert(mcrPt);
	int mtxErr = mtx_lock(&mcrPt->lock), thrdErr;
	thrd_t trd;
	/* Interrupt or disable, then do not start thread. */
	switch (mcrPt->interruptor) {
	case MCR_CONTINUE:
	case MCR_PAUSE:
		if (!mcrPt->thread_max || mcrPt->thread_max > MCR_THREAD_MAX) {
			mset_error(EINVAL);
			mcrPt->thread_max = 1;
		}
		if (mcrPt->thread_count < mcrPt->thread_max &&
			mcrPt->thread_count < MCR_THREAD_MAX) {
			/* TODO: Possible unused threads if only one queued item. */
			if ((thrdErr = thrd_create(&trd, thread_macro,
						mcrPt)) == thrd_success) {
				++mcrPt->queued;
				++mcrPt->thread_count;
				if ((thrdErr = thrd_detach(trd)) !=
					thrd_success) {
					thrd_conv_err(thrdErr);
					onErr;
				}
			} else {
				thrd_conv_err(thrdErr);
				onErr;
			}
		}
		break;
	case MCR_INTERRUPT:
		mcrPt->interruptor = MCR_CONTINUE;
	case MCR_INTERRUPT_ALL:
	case MCR_DISABLE:
		break;
	default:
		mset_error(EINVAL);
		fprintf(stderr, "Invalid " MCR_STR(enum mcr_Interrupt) "\n");
		onErr;
		break;
	}
	if (mtxErr == thrd_success
		&& (mtxErr = mtx_unlock(&mcrPt->lock) != thrd_success)) {
		thrd_conv_err(mtxErr);
		ddo(exit(mtxErr));
	}
	return mcrPt->block;
#undef onErr
}

#define ceptloop(interruptor, continueOut) \
switch (interruptor) \
{ \
case MCR_CONTINUE: case MCR_PAUSE: case MCR_INTERRUPT: \
	continueOut = true; \
	break; \
default: \
	continueOut = false; \
}

/* \return \ref reterr */
static int pause_max_count(struct mcr_Macro *mcrPt)
{
	unsigned int pauseIter = MCR_MAX_PAUSE_COUNT;
	int err = thrd_success;
	struct timespec pauseDelay;
	pauseDelay.tv_sec = 1;
	pauseDelay.tv_nsec = 0;
	while (mcrPt->interruptor == MCR_PAUSE && pauseIter--) {
		if ((err = thrd_sleep(&pauseDelay, NULL)) != thrd_success) {
			mset_error(mcr_thrd_errno(err));
			return err;
		}
	}
	return err;
}

static int thread_macro(void *data)
{
#define onErr \
ddo( \
	mcrPt->interruptor = MCR_DISABLE; \
	errOut = thrd_error; \
);

	dassert(data);
	struct mcr_Macro *mcrPt = data;
	struct mcr_Signal *sigArr;
	struct mcr_context *ctx = mcrPt->mcr_pt;
	size_t index;
	bool continueFlag = false;
	int errOut = 0, mtxErr = thrd_success, thrdErr = thrd_success;
	/* max 5 seconds paused */

	mtxErr = mtx_lock(&mcrPt->lock);
	if (mcrPt->interruptor == MCR_PAUSE &&
		(thrdErr = pause_max_count(mcrPt))) {
		onErr;
	}
	ceptloop(mcrPt->interruptor, continueFlag);
	/* Loop only while triggers queued, or last thread is sticky. */
	while (continueFlag && (mcrPt->queued ||
			(mcrPt->sticky && mcrPt->thread_count == 1))) {
		if (mcrPt->queued)
			--mcrPt->queued;
		if (mtxErr == thrd_success &&
			(mtxErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
			onErr;
		}
		/* If sig set is changed, all threads should end. */
		sigArr = (struct mcr_Signal *)mcrPt->signal_set.array;
		index = 0;
		/* Assuming continueFlag is still valid */
		/* Unsafe loop in a non -critical seciton */
		fixme;
		while (continueFlag && index < mcrPt->signal_set.used) {
			if (mcrPt->interruptor == MCR_INTERRUPT &&
				mtx_trylock(&mcrPt->lock) == thrd_success) {
				/* Interrupt may change before locking. */
				if (mcrPt->interruptor == MCR_INTERRUPT) {
					mcrPt->interruptor = MCR_CONTINUE;
				}
				if ((mtxErr = mtx_unlock(&mcrPt->lock)) !=
					thrd_success) {
					onErr;
				}
			}
			if (sigArr[index].isig) {
				if ((errOut = mcr_send(ctx, sigArr + index))) {
					onErr;
				}
			}
			++index;
			if (mcrPt->interruptor == MCR_PAUSE &&
				(thrdErr = pause_max_count(mcrPt))) {
				onErr;
			}
			ceptloop(mcrPt->interruptor, continueFlag);
		}
		mtxErr = mtx_lock(&mcrPt->lock);
		if (mcrPt->interruptor == MCR_PAUSE &&
			(thrdErr = pause_max_count(mcrPt))) {
			onErr;
		}
		ceptloop(mcrPt->interruptor, continueFlag);
	}
	/* Is locked. */
	--mcrPt->thread_count;
	thrdErr = cnd_broadcast(&mcrPt->cnd);
	if (mtxErr == thrd_success)
		mtxErr = mtx_unlock(&mcrPt->lock);
	return mtxErr ? mtxErr : thrdErr ? thrdErr : errOut ? thrd_error : 0;
#undef ceptloop
#undef onErr
}

static int thread_wait_reset(void *data)
{
	dassert(data);
	struct mcr_Macro *mcrPt = data;

	int thrdErr = mtx_lock(&mcrPt->lock);
	/* Interrupt, but break if interrupt changes. */
	clear_threads(mcrPt, MCR_INTERRUPT_ALL, false);
	if (mcrPt->interruptor == MCR_INTERRUPT_ALL) {
		mcrPt->interruptor = MCR_CONTINUE;
	}
	if (thrdErr == thrd_success)
		mtx_unlock(&mcrPt->lock);
	return thrd_success;
}

static void clear_threads(struct mcr_Macro *mcrPt, enum mcr_Interrupt clearType,
	bool stickyInterrupt)
{
	/* Do not rely on a timeout to say no threads exist */
	fixme;
	/* If timed out then we probably have a long noop or alarm running.
	 * As long as pthread_exit is not called, then the C runtime
	 * will also exit other threads. In this case valgrind will report
	 * thread is still using resources. */
	struct timespec cndTimeout = {
		MCR_MACRO_JOIN_TIMEOUT, 0
	};
	int timedout = thrd_success;
	mcrPt->interruptor = clearType;
	while (mcrPt->thread_count && timedout != thrd_timeout) {
		/* Wait to be notified, or time out to destroy. */
		timedout = cnd_timedwait(&mcrPt->cnd, &mcrPt->lock,
			&cndTimeout);
		if (stickyInterrupt)
			mcrPt->interruptor = clearType;
		/* Changed interrupt. Exit only if not sticking our interrupt. */
		else if (mcrPt->interruptor != clearType)
			return;
	}
	/* If timed out, we lose control over threads. */
	if (timedout) {
		mcrPt->thread_count = 0;
	}
}
