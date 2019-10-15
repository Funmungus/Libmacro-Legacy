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

#include "mcr/macro/macro.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define thrd_conv_err(thrdErr) \
thrdErr = mcr_thrd_errno(thrdErr); \
mset_error(thrdErr);

static const struct mcr_Interface _MCR_MACRO_IFACE = {
	.id = ~0,
	.copy = mcr_Macro_copy,
	.data_size = sizeof(struct mcr_Macro),
	.init = mcr_Macro_init,
	.deinit = mcr_Macro_deinit
};

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

const struct mcr_Interface *mcr_Macro_interface()
{
	return &_MCR_MACRO_IFACE;
}

int mcr_Macro_init(void *mcrPt)
{
	struct mcr_Macro *localPt = mcrPt;
	int thrdErr;
	if (!localPt)
		return 0;
	memset(localPt, 0, sizeof(struct mcr_Macro));
	localPt->thread_max = 1;
	localPt->interruptor = MCR_CONTINUE;
	/// \todo Instead of mutex locking, disable macro when modifying
	if ((thrdErr = cnd_init(&localPt->cnd)) != thrd_success ||
		(thrdErr =
			 mtx_init(&localPt->lock, mtx_plain)) != thrd_success) {
		thrd_conv_err(thrdErr);
		return thrdErr;
	}
	mcr_Array_init(&localPt->signal_set);
	localPt->signal_set.element_size = sizeof(struct mcr_Signal);
	return 0;
}

int mcr_Macro_deinit(void *mcrPt)
{
	struct mcr_Macro *localPt = mcrPt;
	int thrdErr;
	if (!localPt)
		return 0;
	/// \todo lock here?
	if ((thrdErr = mtx_lock(&localPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		return thrdErr;
	}
	/* Absolutey disable. */
	clear_threads(localPt, MCR_DISABLE, true);
	MCR_ARR_FOR_EACH(localPt->signal_set, mcr_Signal_deinit);
	mcr_Array_deinit(&localPt->signal_set);
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&localPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(exit(thrdErr));
	}
	cnd_destroy(&localPt->cnd);
	mtx_destroy(&localPt->lock);
	return 0;
}

int mcr_Macro_set_all(struct mcr_Macro *mcrPt, bool blocking,
					  bool sticky, unsigned int threadMax, bool enable,
					  struct mcr_context *ctx)
{
	dassert(mcrPt);
	mcrPt->blocking = blocking;
	mcrPt->sticky = sticky;
	mcrPt->thread_max = threadMax;
	mcrPt->ctx = ctx;
	return mcr_Macro_set_enabled(mcrPt, enable);
}

int mcr_Macro_copy(void *dstPt, const void *srcPt)
{
	dassert(dstPt);
	struct mcr_Macro *dPt = dstPt;
	const struct mcr_Macro *sPt = srcPt;
	int thrdErr, err = mcr_Macro_set_enabled(dPt, false);
	if (err)
		return err;
	if (!sPt)
		return 0;

	if ((thrdErr = mtx_lock((mtx_t *)&sPt->lock) != thrd_success)) {
		thrd_conv_err(thrdErr);
		ddo(return thrdErr);
	}
	dPt->blocking = sPt->blocking;
	dPt->sticky = sPt->sticky;
	dPt->thread_max = sPt->thread_max;
	if ((err = mcr_Macro_set_signals(dPt,
									 (struct mcr_Signal *)sPt->signal_set.array,
									 sPt->signal_set.used)))
		return err;
	if ((err = mcr_Macro_set_enabled(dPt, MCR_MACRO_IS_ENABLED(*sPt))))
		return err;
	/* Leave queued at 0.  Copying data will not trigger */
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock((mtx_t *)&sPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		return thrdErr;
	}
	return err;
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
#ifdef DEBUG
		/* Pausing while already paused? Somebody has concurrency issues. */
		if (prev == MCR_PAUSE) {
			err = EBUSY;
			mset_error(EBUSY);
			fprintf(stderr, "Possible " MCR_STR(MCR_PAUSE)
					" concurrency issue\n");
		}
#endif
		/* Implicit fallthrough warning if not duplicated here and below */
		if (prev != interruptType)
			mcrPt->interruptor = interruptType;
		break;
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

size_t mcr_Macro_count(const struct mcr_Macro * mcrPt)
{
	return mcrPt ? mcrPt->signal_set.used : 0;
}

struct mcr_Signal *mcr_Macro_signals(struct mcr_Macro *mcrPt)
{
	if (!mcrPt || !mcrPt->signal_set.used)
		return NULL;
	return (struct mcr_Signal *)mcrPt->signal_set.array;
}

int mcr_Macro_set_signals(struct mcr_Macro *mcrPt,
						  const struct mcr_Signal *signalSet, size_t signalCount)
{
	dassert(mcrPt);
	size_t i;
	int err, thrdErr;
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
	MCR_ARR_FOR_EACH(mcrPt->signal_set, mcr_Signal_deinit);
	mcrPt->signal_set.used = 0;
	if (!(err = mcr_Array_resize(&mcrPt->signal_set, signalCount)) &&
		!(err = mcr_Array_fill(&mcrPt->signal_set, 0,
							   &initial, signalCount))) {
		itPt = MCR_ARR_FIRST(mcrPt->signal_set);
		for (i = signalCount; i--;) {
			if ((err = mcr_Signal_copy(itPt + i, signalSet + i))) {
				mset_error(err);
			}
		}
	} else {
		mset_error(err);
	}
	if (enableMem) {
		mcrPt->interruptor = MCR_INTERRUPT_ALL;
		err = resetthread(mcrPt);
	}
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		mset_error(thrdErr);
		err = thrdErr;
	}
	return err;
}

struct mcr_Signal *mcr_Macro_signal(struct mcr_Macro *mcrPt, size_t index)
{
	int thrdErr;
	struct mcr_Signal *ret;
	if (!mcrPt)
		return NULL;
	thrdErr = mtx_lock(&mcrPt->lock);
	ret = MCR_ARR_ELEMENT(mcrPt->signal_set, index);
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(return NULL);
	}
	return ret;
}

int mcr_Macro_set_signal(struct mcr_Macro *mcrPt,
						 const struct mcr_Signal *copySig,
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
							const struct mcr_Signal *copySig, size_t index)
{
	dassert(mcrPt);
	int err;
	struct mcr_Signal addSig;
	mcr_Signal_init(&addSig);
	if (index >= mcrPt->signal_set.used) {
		return mcr_Macro_push_signal(mcrPt, copySig);
	} else {
		mcr_Signal_copy(&addSig, copySig);
		if ((err = mcr_Array_insert(&mcrPt->signal_set, index, &addSig,
									1))) {
			mset_error(err);
			mcr_Signal_deinit(&addSig);
			return err;
		}
	}
	return 0;
}

int mcr_Macro_remove_signal(struct mcr_Macro *mcrPt, size_t index)
{
	struct mcr_Signal *sigPt = mcr_Macro_signal(mcrPt, index);
	int err = 0;
	if (sigPt) {
		err = mcr_Signal_deinit(sigPt);
		mcr_Array_remove_index(&mcrPt->signal_set, index, 1);
	}
	return err;
}

int mcr_Macro_push_signal(struct mcr_Macro *mcrPt,
						  const struct mcr_Signal *newSig)
{
	int thrdErr = mtx_lock(&mcrPt->lock);
	int err;
	struct mcr_Signal addSig;
	mcr_Signal_init(&addSig);
	if (!(err = mcr_Signal_copy(&addSig, newSig))) {
		if ((err = mcr_Array_push(&mcrPt->signal_set, &addSig))) {
			mset_error(err);
			mcr_Signal_deinit(&addSig);
		}
	} else {
		mset_error(err);
	}
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		err = thrdErr;
	}
	return err;
}

int mcr_Macro_pop_signal(struct mcr_Macro *mcrPt)
{
	int thrdErr = mtx_lock(&mcrPt->lock);
	struct mcr_Signal *sigPt;
	int err = 0;
	if (mcrPt->signal_set.used) {
		sigPt = MCR_ARR_LAST(mcrPt->signal_set);
		err = mcr_Signal_deinit(sigPt);
		--mcrPt->signal_set.used;
	}
	if (thrdErr == thrd_success &&
		(thrdErr = mtx_unlock(&mcrPt->lock)) != thrd_success) {
		thrd_conv_err(thrdErr);
		ddo(exit(thrdErr));
	}
	return err;
}

bool mcr_Macro_is_enabled(const struct mcr_Macro * mcrPt)
{
	return mcrPt ? MCR_MACRO_IS_ENABLED(*mcrPt) : false;
}

int mcr_Macro_set_enabled(struct mcr_Macro *mcrPt, bool enable)
{
	dassert(mcrPt);
	return mcr_Macro_interrupt(mcrPt, enable ? MCR_CONTINUE : MCR_DISABLE);
}

int mcr_Macro_add_dispatch(struct mcr_Macro *mcrPt,
						   struct mcr_Trigger *trigPt, struct mcr_Signal *interceptPt)
{
	struct mcr_DispatchPair disp = mcr_Macro_dispatcher(mcrPt, trigPt);
	dassert(mcrPt);
	return mcr_Dispatcher_add(mcrPt->ctx, interceptPt, disp.receiver,
							  disp.dispatch);
}

int mcr_Macro_remove_dispatch(struct mcr_Macro *mcrPt,
							  struct mcr_Trigger *trigPt, struct mcr_ISignal *isigPt)
{
	dassert(mcrPt);
	mcr_err = 0;
	mcr_Dispatcher_remove(mcrPt->ctx, isigPt, trigPt);
	mcr_Dispatcher_remove(mcrPt->ctx, isigPt, mcrPt);
	return mcr_err;
}

struct mcr_DispatchPair mcr_Macro_dispatcher(struct mcr_Macro *mcrPt,
		struct mcr_Trigger *trigPt)
{
	struct mcr_DispatchPair ret;
	dassert(mcrPt);
	if (trigPt && trigPt->itrigger) {
		mcr_Trigger_set_macro(trigPt, mcrPt);
		ret.receiver = trigPt;
		ret.dispatch = mcr_Trigger_receive;
	} else {
		ret.receiver = mcrPt;
		ret.dispatch = mcr_Macro_receive;
	}
	return ret;
}

bool mcr_Macro_receive(void *mcrPt, struct mcr_Signal * sigPt,
					   unsigned int mods)
{
	/* Error unlock and exit on debug builds */
#define onErr \
ddo( \
	if (mtxErr == thrd_success) \
		mtxErr = mtx_unlock(&localPt->lock); \
	thrd_conv_err(mtxErr); \
	exit(mtxErr); \
);
	UNUSED(sigPt);
	UNUSED(mods);
	struct mcr_Macro *localPt = mcrPt;
	dassert(localPt);
	int mtxErr = mtx_lock(&localPt->lock), thrdErr;
	thrd_t trd;
	/* Interrupt or disable, then do not start thread. */
	switch (localPt->interruptor) {
	case MCR_CONTINUE:
	case MCR_PAUSE:
		if (!localPt->thread_max
			|| localPt->thread_max > MCR_THREAD_MAX) {
			mset_error(EINVAL);
			localPt->thread_max = 1;
		}
		if (localPt->thread_count < localPt->thread_max &&
			localPt->thread_count < MCR_THREAD_MAX) {
			/// \todo Possible unused threads if only one queued item.
			if ((thrdErr = thrd_create(&trd, thread_macro,
									   localPt)) == thrd_success) {
				++localPt->queued;
				++localPt->thread_count;
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
		localPt->interruptor = MCR_CONTINUE;
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
		&& (mtxErr = mtx_unlock(&localPt->lock) != thrd_success)) {
		thrd_conv_err(mtxErr);
		ddo(exit(mtxErr));
	}
	return localPt->blocking;
#undef onErr
}

bool mcr_Macro_trigger(void *trigPt, struct mcr_Signal * sigPt,
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
	dassert(trigPt);
	UNUSED(sigPt);
	UNUSED(mods);
	struct mcr_Macro *mcrPt = ((struct mcr_Trigger *)trigPt)->actor;
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
			/// \todo Possible unused threads if only one queued item.
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
	return mcrPt->blocking;
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
		thrd_sleep(&pauseDelay, NULL);
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

	struct mcr_Macro *mcrPt = data;
	struct mcr_Signal *sigArr;
	struct mcr_context *ctx = mcrPt->ctx;
	size_t index;
	bool continueFlag = false;
	int errOut = 0, mtxErr = thrd_success, thrdErr = thrd_success;
	dassert(data);
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
		/// \bug Unsafe loop in a non-critical seciton
		while (continueFlag && index < mcrPt->signal_set.used) {
			if (mcrPt->interruptor == MCR_INTERRUPT &&
				mtx_trylock(&mcrPt->lock) == thrd_success) {
				/* Interrupt may change before locking. */
				if (mcrPt->interruptor == MCR_INTERRUPT) {
					mcrPt->interruptor = MCR_CONTINUE;
					index = mcrPt->signal_set.used;
				}
				if ((mtxErr = mtx_unlock(&mcrPt->lock)) !=
					thrd_success) {
					onErr;
				}
			}
			if (sigArr[index].isignal) {
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
	/// \todo Do not rely on a timeout to say no threads exist
	/* If timed out then we probably have a long noop or alarm running.
	 * As long as pthread_exit is not called, then the C runtime
	 * will also exit other threads. In this case valgrind will report
	 * thread is still using resources. */
	struct timespec cndTimeout = {
		MCR_MACRO_JOIN_TIMEOUT, 0
	};
	int timedout = thrd_success;
	mcrPt->interruptor = clearType;
	while (mcrPt->thread_count && timedout != thrd_timedout) {
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

int mcr_macro_initialize(struct mcr_context *ctx)
{
	return mcr_reg_init(mcr_ITrigger_reg(ctx));
}

int mcr_macro_deinitialize(struct mcr_context *ctx)
{
	return mcr_reg_deinit(mcr_ITrigger_reg(ctx));
}

void mcr_macro_trim(struct mcr_context *ctx)
{
	mcr_reg_trim(mcr_ITrigger_reg(ctx));
}
