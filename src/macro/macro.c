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

#include "mcr/macro/macro.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcr/libmacro.h"

#define bind_max_thread(mcrPt) { \
	if (!mcrPt->thread_max \
		|| mcrPt->thread_max > MCR_THREAD_MAX) { \
		mset_error(EINVAL); \
		mcrPt->thread_max = 1; \
	} \
}
#define dequeue(mcrPt, interruptorVal) { \
	mcrPt->interruptor = interruptorVal; \
	mcrPt->queued = 0; \
}
#define isrunning(mcrPt) (mcrPt->thread_count)
#define isenabled(mcrPt) mcr_Macro_is_enabled(mcrPt)
#define nonzero(val) if(val < 1) { val = 1; }
#define nonneg(val) if(val < 0) { val = 0; }
#define reenable(mcrPt) mcrPt->interruptor = MCR_CONTINUE
#define ss signal_set
#define thrd_conv_err(thrdErr) { \
	thrdErr = mcr_thrd_errno(thrdErr); \
	mset_error(thrdErr); \
}

#define ifndisable_errout(mcrPt) \
bool enableMem = isenabled(mcrPt); \
if (mcr_Macro_disable_confirmed(mcrPt)) \
	return mcr_err;

#define disreenable(mcrPt) if (enableMem) { reenable(mcrPt); }

static const struct mcr_Interface _MCR_MACRO_IFACE = {
	.id = (size_t)-1,
	.copy = mcr_Macro_copy,
	.data_size = sizeof(struct mcr_Macro),
	.init = mcr_Macro_init,
	.deinit = mcr_Macro_deinit
};

static int thread_macro(void *);
static int thread_wait_reset(void *);
/* Will attempt to gently detach all threads, but force termination
 * if timed out. */
static int clear_threads(struct mcr_Macro *mcrPt, enum mcr_Interrupt clearType,
						 bool stickyInterrupt);

/* Specifically \ref MCR_INTERRUPT_ALL, wait for all threads to finish, and
 * reset to \ref MCR_CONTINUE. */
static int resetthread(struct mcr_Macro *mcrPt)
{
	dequeue(mcrPt, MCR_INTERRUPT_ALL);
	thrd_yield();
	/* Oh? It seems we are already reset. Boop. */
	if (!isrunning(mcrPt)) {
		reenable(mcrPt);
		return 0;
	}
	if (mcr_thrd(thread_wait_reset, mcrPt)) {
		dmsg;
		return mcr_err;
	}
	return 0;
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
	reenable(localPt);
	if ((thrdErr = cnd_init(&localPt->thread_count_cnd)) != thrd_success) {
		thrd_conv_err(thrdErr);
		return thrdErr;
	}
	mcr_Array_init(&localPt->ss);
	localPt->ss.element_size = sizeof(struct mcr_Signal);
	return 0;
}

int mcr_Macro_deinit(void *mcrPt)
{
	struct mcr_Macro *localPt = mcrPt;
	if (!localPt)
		return 0;
	/* Absolutey disable. Will cause problems on program exit. */
	if (mcr_Macro_disable_confirmed(localPt))
		dexit;
	MCR_ARR_FOR_EACH(localPt->ss, mcr_Signal_deinit);
	mcr_Array_deinit(&localPt->ss);
	cnd_destroy(&localPt->thread_count_cnd);
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
	bind_max_thread(mcrPt);
	mcrPt->ctx = ctx;
	return mcr_Macro_set_enabled(mcrPt, enable);
}

int mcr_Macro_copy(void *dstPt, const void *srcPt)
{
	dassert(dstPt);
	struct mcr_Macro *dPt = dstPt;
	const struct mcr_Macro *sPt = srcPt;
	if (mcr_Macro_disable_confirmed(dPt))
		return mcr_err;
	if (!sPt)
		return 0;

	dPt->blocking = sPt->blocking;
	dPt->sticky = sPt->sticky;
	dPt->thread_max = sPt->thread_max;
	bind_max_thread(dPt);
	if (mcr_Macro_set_signals(dPt, (void *)sPt->ss.array, sPt->ss.used))
		return mcr_err;
	return mcr_Macro_set_enabled(dPt, isenabled(sPt));
}

int mcr_Macro_interrupt(struct mcr_Macro *mcrPt,
						enum mcr_Interrupt interruptType)
{
	enum mcr_Interrupt prev = mcrPt->interruptor;
	dassert(mcrPt);
	/// \todo Out of range enum type
	if (interruptType < 0 || interruptType > MCR_DISABLE) {
		fprintf(stderr, "Invalid " MCR_STR(enum mcr_Interrupt) "\n");
		mset_error_return(EINVAL);
	}
	/* Enable or disable, just make it happen
	 * If interrupting, macro will be reenabled from the reset thread */
	if (prev != interruptType)
		mcrPt->interruptor = interruptType;

	/* Wait for all to finish, then reenable. */
	if (interruptType == MCR_INTERRUPT_ALL && prev != MCR_INTERRUPT_ALL)
		return resetthread(mcrPt);

	/* Pausing while already paused? Somebody has concurrency issues. */
#ifdef MCR_DEBUG
	if (interruptType == MCR_PAUSE && prev == MCR_PAUSE) {
		fprintf(stderr, "Possible " MCR_STR(MCR_PAUSE)
				" concurrency issue\n");
		mset_error_return(EBUSY);
	}
#endif
	return 0;
}

size_t mcr_Macro_count(const struct mcr_Macro * mcrPt)
{
	return mcrPt ? mcrPt->ss.used : 0;
}

struct mcr_Signal *mcr_Macro_signals(struct mcr_Macro *mcrPt)
{
	if (!mcrPt || !mcrPt->ss.used)
		return NULL;
	return (void *)mcrPt->ss.array;
}

int mcr_Macro_set_signals(struct mcr_Macro *mcrPt,
						  const struct mcr_Signal *signalSet, size_t signalCount)
{
	dassert(mcrPt);
	size_t i;
	struct mcr_Signal initial, *itPt;
	ifndisable_errout(mcrPt);
	mcr_Signal_init(&initial);
	MCR_ARR_FOR_EACH(mcrPt->ss, mcr_Signal_deinit);
	mcrPt->ss.used = 0;
	if (!mcr_Array_resize(&mcrPt->ss, signalCount) &&
		!mcr_Array_fill(&mcrPt->ss, 0,
						&initial, signalCount)) {
		itPt = MCR_ARR_FIRST(mcrPt->ss);
		for (i = signalCount; i--;) {
			if (mcr_Signal_copy(itPt + i, signalSet + i))
				dmsg;
		}
	}
	disreenable(mcrPt);
	return mcr_err;
}

struct mcr_Signal *mcr_Macro_signal(struct mcr_Macro *mcrPt, size_t index)
{
	if (!mcrPt)
		return NULL;
	return MCR_ARR_ELEMENT(mcrPt->ss, index);
}

int mcr_Macro_set_signal(struct mcr_Macro *mcrPt,
						 const struct mcr_Signal *copySig,
						 size_t index)
{
	dassert(mcrPt);
	struct mcr_Signal *sigPt = mcr_Macro_signal(mcrPt, index);
	ifndisable_errout(mcrPt);
	if (sigPt) {
		mcr_Signal_copy(sigPt, copySig);
	} else {
		mcr_Macro_push_signal(mcrPt, copySig);
	}
	disreenable(mcrPt);
	return mcr_err;
}

int mcr_Macro_insert_signal(struct mcr_Macro *mcrPt,
							const struct mcr_Signal *copySig, size_t index)
{
	dassert(mcrPt);
	struct mcr_Signal addSig;
	ifndisable_errout(mcrPt);
	mcr_Signal_init(&addSig);
	if (index >= mcrPt->ss.used) {
		mcr_Macro_push_signal(mcrPt, copySig);
	} else {
		if (mcr_Signal_copy(&addSig, copySig)
			|| mcr_Array_insert(&mcrPt->ss, index, &addSig, 1)) {
			mcr_Signal_deinit(&addSig);
			return mcr_err;
		}
	}
	disreenable(mcrPt);
	return mcr_err;
}

int mcr_Macro_remove_signal(struct mcr_Macro *mcrPt, size_t index)
{
	struct mcr_Signal *sigPt = mcr_Macro_signal(mcrPt, index);
	ifndisable_errout(mcrPt);
	if (sigPt) {
		mcr_Signal_deinit(sigPt);
		mcr_Array_remove_index(&mcrPt->ss, index, 1);
	}
	disreenable(mcrPt);
	return mcr_err;
}

int mcr_Macro_push_signal(struct mcr_Macro *mcrPt,
						  const struct mcr_Signal *newSig)
{
	struct mcr_Signal addSig;
	ifndisable_errout(mcrPt);
	mcr_Signal_init(&addSig);
	if (!mcr_Signal_copy(&addSig, newSig)) {
		if (mcr_Array_push(&mcrPt->ss, &addSig)) {
			dmsg;
			mcr_Signal_deinit(&addSig);
		}
	}
	disreenable(mcrPt);
	return mcr_err;
}

int mcr_Macro_pop_signal(struct mcr_Macro *mcrPt)
{
	ifndisable_errout(mcrPt);
	if (mcrPt->ss.used) {
		mcr_Signal_deinit(MCR_ARR_LAST(mcrPt->ss));
		--mcrPt->ss.used;
	}
	disreenable(mcrPt);
	return mcr_err;
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

int mcr_Macro_disable_confirmed(struct mcr_Macro *mcrPt)
{
//	if (!isenabled(mcrPt) && !isrunning(mcrPt))
//		return 0;
	return clear_threads(mcrPt, MCR_DISABLE, true);
}

int mcr_Macro_send(struct mcr_Macro *mcrPt)
{
	struct mcr_Signal *sigPt = (void *)mcrPt->ss.array, *end;
	struct mcr_context *ctx = mcrPt->ctx;
	end = sigPt + mcrPt->ss.used;
	while (mcrPt->interruptor == MCR_CONTINUE && sigPt < end) {
		if (sigPt->isignal && mcr_send(ctx, sigPt))
			break;
		++sigPt;
	}
	return mcr_err;
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
	struct mcr_Macro *localPt = mcrPt;
	dassert(localPt);
	/* Fairly safe optimization to avoid dereferencing.  Concurrency may cause
	 * an extra thread, so check thread_count again in thread function. */
	int count = localPt->thread_count;
	UNUSED(sigPt);
	UNUSED(mods);

	/* If interrupted or disabled, then do not start thread. */
	switch (localPt->interruptor) {
	case MCR_CONTINUE:
	case MCR_PAUSE:
		/* There was a concurrency blip, but we can correct it. */
		if (count < 0) {
			localPt->thread_count = count = 0;
			cnd_broadcast(&localPt->thread_count_cnd);
		}
		/* Double-check thread_max valid after normal thread_count check.
		 * Any value greater than MCR_THREAD_MAX will be ignored. */
		if (count < MCR_THREAD_MAX && (unsigned)count < localPt->thread_max) {
			/// \todo Possible unused threads if only one queued item.
			if (++localPt->queued == 0) {
				dmsg;
			} else if (mcr_thrd(thread_macro, localPt)) {
				dexit;
			}
		}
		break;
	case MCR_INTERRUPT:
		localPt->interruptor = MCR_CONTINUE;
	case MCR_INTERRUPT_ALL:
	case MCR_DISABLE:
		break;
	}
	return localPt->blocking;
}

bool mcr_Macro_trigger(void *trigPt, struct mcr_Signal * sigPt,
					   unsigned int mods)
{
	dassert(trigPt);
	return mcr_Macro_receive(((struct mcr_Trigger *)trigPt)->actor, sigPt, mods);
}

static void pause_macro(struct mcr_Macro *mcrPt)
{
	unsigned int pauseIter = MCR_MAX_PAUSE_COUNT;
	struct timespec pauseDelay = { .tv_sec = 1 };
	while (mcrPt->interruptor == MCR_PAUSE && pauseIter--) {
		thrd_sleep(&pauseDelay, NULL);
	}
	/* Max pause reached */
	if (!pauseIter && mcrPt->interruptor == MCR_PAUSE)
		mcrPt->interruptor = MCR_CONTINUE;
}

static int thread_macro(void *data)
{
#define canPause(mcrPt) if (mcrPt->interruptor == MCR_PAUSE) pause_macro(mcrPt);
#define isContinue(interruptor) (interruptor < MCR_INTERRUPT_ALL)
#define onErr ddo(mcrPt->interruptor = MCR_DISABLE;)
#define onExit(mcrPt) { \
	--mcrPt->thread_count; \
	nonneg(mcrPt->thread_count); \
	cnd_broadcast(&mcrPt->thread_count_cnd); \
}

	struct mcr_Macro *mcrPt = data;
	struct mcr_Signal *sigArr, *sigPt, *end;
	struct mcr_context *ctx = mcrPt->ctx;
	dassert(data);
	/* Abort! */
	if (!isContinue(mcrPt->interruptor))
		return thrd_success;

	++mcrPt->thread_count;
	/* Immediate disable check to avoid destroyed macro reference. */
	if (!isContinue(mcrPt->interruptor)) {
		onExit(mcrPt);
		return thrd_success;
	}
	cnd_broadcast(&mcrPt->thread_count_cnd);
	/* thread_count is in the clear. */
	/* Signal set changes disable the macro, so the set should be valid for
	 * the entire loop. */
	sigArr = (void *)mcrPt->ss.array;
	end = sigArr + mcrPt->ss.used;
	/* No signal set is grounds for immediate dismissal. */
	if (sigArr == end) {
		mcrPt->queued = 0;
		onExit(mcrPt);
		return thrd_success;
	}
	canPause(mcrPt);
	/* Loop only while triggers queued, or last thread is sticky. */
	while ((isContinue(mcrPt->interruptor) && mcrPt->queued) ||
		   (mcrPt->sticky && mcrPt->thread_count == 1)) {
		/* Reduce from queue or we are definitely sticky. */
		if (mcrPt->queued && --mcrPt->queued == 0) {
			/* Queued overdraw.  All that read below zero enter critical
			 * section to reset queue (and exit before running signal set). */
			if (mtx_lock(&ctx->macro.critical_section) == thrd_success) {
				mcrPt->queued = 0; // ensure
				mtx_unlock(&ctx->macro.critical_section);
			}
			break;
		}
		for (sigPt = sigArr; isContinue(mcrPt->interruptor) && sigPt < end;
			 sigPt++) {
			if (mcrPt->interruptor == MCR_INTERRUPT) {
				/* Critical section to ensure only one thread is interrupted. */
				if (mtx_lock(&ctx->macro.critical_section) == thrd_success) {
					if (mcrPt->interruptor == MCR_INTERRUPT) {
						mcrPt->interruptor = MCR_CONTINUE;
						mtx_unlock(&ctx->macro.critical_section);
						break;
					}
					mtx_unlock(&ctx->macro.critical_section);
				}
			}
			if (sigPt->isignal && mcr_send(ctx, sigPt))
				onErr;
			/* Hmm.  Maybe not pause during this loop. */
		}
		canPause(mcrPt);
	}
	onExit(mcrPt);
	return mcr_err ? thrd_error : 0;
#undef canPause
#undef isContinue
#undef onErr
#undef onExit
}

static int thread_wait_reset(void *data)
{
	dassert(data);
	struct mcr_Macro *mcrPt = data;
	/* Interrupt, but break if interrupt changes. */
	clear_threads(mcrPt, MCR_INTERRUPT_ALL, false);
	if (mcrPt->interruptor == MCR_INTERRUPT_ALL)
		mcrPt->interruptor = MCR_CONTINUE;
	return thrd_success;
}

/* clearType should not be MCR_CONTINUE or MCR_PAUSE.
 */
static int clear_threads(struct mcr_Macro *mcrPt, enum mcr_Interrupt clearType,
						 bool stickyInterrupt)
{
	/// \todo Do not rely on a timeout to say no threads exist
	/* If timed out then we probably have a long-running signal.
	 * As long as pthread_exit is not called, then the C runtime
	 * will also exit other threads. In this case valgrind will report
	 * thread is still using resources. */
	struct timespec cndTimeout = {
		MCR_MACRO_JOIN_TIMEOUT, 0
	};
	int thrdErr;
	mtx_t lock;
	dequeue(mcrPt, clearType);
	thrd_yield();
	/* No threads are running, and we just set the interruptor. */
	if (!isrunning(mcrPt))
		return 0;
	thrdErr = mtx_init(&lock, mtx_plain);
	if (thrdErr == thrd_success && (thrdErr = mtx_lock(&lock)) != thrd_success)
		mtx_destroy(&lock);
	if (thrdErr != thrd_success) {
		thrd_conv_err(thrdErr);
		return thrdErr;
	}
	/* When timed out there may be a long-running signal(e.g. NoOp) */
	while (mcrPt->thread_count && thrdErr != thrd_timedout) {
		dequeue(mcrPt, clearType);
		/* Wait to be notified, or time out to destroy. */
		thrdErr = cnd_timedwait(&mcrPt->thread_count_cnd, &lock, &cndTimeout);
		/* Changed interrupt. Exit only if not sticking our interrupt. */
		if (!stickyInterrupt && mcrPt->interruptor != clearType) {
			thrdErr = thrd_success;
			break;
		}
	}
	/* If timed out, we lose control over threads. */
	if (thrdErr) {
		thrd_conv_err(thrdErr);
		mcrPt->thread_count = 0;
		cnd_broadcast(&mcrPt->thread_count_cnd);
	}
	mtx_destroy(&lock);
	return thrdErr;
}

int mcr_macro_initialize(struct mcr_context *ctx)
{
	int thrdErr = mtx_init(&ctx->macro.critical_section, mtx_plain);
	if (thrdErr != thrd_success) {
		thrd_conv_err(thrdErr);
		return mcr_err;
	}
	return mcr_reg_init(mcr_ITrigger_reg(ctx));
}

int mcr_macro_deinitialize(struct mcr_context *ctx)
{
	mtx_destroy(&ctx->macro.critical_section);
	return mcr_reg_deinit(mcr_ITrigger_reg(ctx));
}

void mcr_macro_trim(struct mcr_context *ctx)
{
	mcr_reg_trim(mcr_ITrigger_reg(ctx));
}
