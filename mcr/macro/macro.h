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

/*! \file
 *  \brief \ref mcr_Macro - When the macro is triggered,
 *  its set of signals is sent one after the other.
 *
 *  \ref mcr_macro - Macro module
 *  All signal functions will initialize and deinitialize as needed
 */

#ifndef MCR_MACRO_MACRO_H_
#define MCR_MACRO_MACRO_H_

#include "mcr/macro/trigger.h"
#include "mcr/util/c11threads.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Pause, stop, or continue a running macro */
enum mcr_Interrupt {
	/*! Able to trigger, or continue currently running */
	MCR_CONTINUE = 0,
	/*! Briefly stop sending signals during critical sections. */
	MCR_PAUSE,
	/*! One queued macro from one thread will cancel.
	 *  After one is cancelled, MCR_CONTINUE is set. */
	MCR_INTERRUPT,
	/*! All threads and queued items will cancel.
	 *  After all are cancelled, MCR_CONTINUE is set. */
	MCR_INTERRUPT_ALL,
	/*! Not able to trigger */
	MCR_DISABLE
};

/*! A macro, when triggered, will send all its signals */
struct mcr_Macro {
	/*! This will be the blocking
	 *  return value of intercepting a signal. */
	bool blocking;
	/*! If false the signals will be sent only once.  Otherwise
	 *  the signals sent will repeat sending until interrupted */
	bool sticky;
	/*! Maximum number of threads this macro can start when it is
	 *  triggered.
	 *
	 *  Any value greater than \ref MCR_THREAD_MAX will be ignored. */
	unsigned int thread_max;

	/* Internal */
	/*! See \ref mcr_Macro_interrupt */
	enum mcr_Interrupt interruptor;
	/*! Used for \ref mcr_send */
	struct mcr_context *ctx;
	/*! Set of signals sent when triggered */
	struct mcr_Array signal_set;
	/*! Current number of threads created from macro being triggered */
	int thread_count;
	/*! Wait for thread_count changes. */
	cnd_t thread_count_cnd;
	/*! Current number of times the macro has been triggered. */
	unsigned queued;
};

/*! Data interface of macro structures
 *
 *  \return Interface to manage macro objects
 */
MCR_API const struct mcr_Interface *mcr_Macro_interface();

/*! \ref mcr_Macro ctor
 *
 *  \param mcrPt \ref opt \ref mcr_Macro *
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_init(void *mcrPt);
/*! \ref mcr_Macro dtor
 *
 *  \param mcrPt \ref opt \ref mcr_Macro *
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_deinit(void *mcrPt);
/*! Set initial values
 *
 *  \param blocking \ref mcr_Macro.blocking
 *  \param sticky \ref mcr_Macro.sticky
 *  \param threadMax \ref mcr_Macro.thread_max
 *  \param enable \ref mcr_Macro_set_enabled
 *  \param ctx \ref mcr_Macro.ctx
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_set_all(struct mcr_Macro *mcrPt,
							  bool blocking, bool sticky, unsigned int threadMax, bool enable,
							  struct mcr_context *ctx);
/*! Copy a macro
 *
 *  \param dstPt \ref mcr_Macro * Destination macro
 *  \param srcPt \ref opt \ref mcr_Macro * Source macro
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_copy(void *dstPt, const void *srcPt);
/*! Stop or continue macro execution according to \ref mcr_Interrupt.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_interrupt(struct mcr_Macro *mcrPt,
								enum mcr_Interrupt interruptType);
/*! Get the number of signals in the set to send
 *
 *  \param mcrPt \ref opt
 *  \return \ref retind
 */
MCR_API size_t mcr_Macro_count(const struct mcr_Macro *mcrPt);
/*! Get the array of signals
 *
 *  If the signal set changes, this reference will be invalid.
 *  \param mcrPt \ref opt
 *  \return Signal reference, or null if set is empty
 */
MCR_API struct mcr_Signal *mcr_Macro_signals(struct mcr_Macro *mcrPt);
/*! Set all signals
 *
 *  To avoid buffer overflow, the macro will be completely disabled while
 *  modifying the set of signals.
 *  \param signalSet \ref opt Array of signals to copy
 *  \param signalCount Number of signals to copy
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_set_signals(struct mcr_Macro *mcrPt,
								  const struct mcr_Signal *signalSet, size_t signalCount);
/*! Get a signal reference
 *
 *  If the signal set changes, this reference will be invalid.
 *  \param mcrPt \ref opt
 *  \param index Index of signal
 *  \return Signal reference, or null if not found
 */
MCR_API struct mcr_Signal *mcr_Macro_signal(struct mcr_Macro *mcrPt,
		size_t index);
/*! Copy signal into signal set
 *
 *  Signal will replace at given index
 *  \param copySig \ref opt Signal to copy
 *  \param index Index to replace at
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_set_signal(struct mcr_Macro *mcrPt,
								 const struct mcr_Signal *copySig, size_t index);
/*! Insert and add a signal copy
 *
 *  \param copySig \ref opt Signal to copy
 *  \param index Index to insert into
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_insert_signal(struct mcr_Macro *mcrPt,
									const struct mcr_Signal *copySig, size_t index);
/*! Remove the signal at given index
 *
 *  \param index Index to remove from
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_remove_signal(struct mcr_Macro *mcrPt, size_t index);
/*! Add signal to the end of signal set
 *
 *  \param newSig \ref opt Signal to copy
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_push_signal(struct mcr_Macro *mcrPt,
								  const struct mcr_Signal *newSig);
/*! Remove signal from the end of signal set
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_pop_signal(struct mcr_Macro *mcrPt);
/*! Get enabled state
 *
 *  \param mcrPt \ref opt
 *  \return False if \ref MCR_DISABLE, otherwise true
 */
MCR_API bool mcr_Macro_is_enabled(const struct mcr_Macro *mcrPt);
/*! Set enabled state
 *
 *  \param enable \ref mcr_Macro.interruptor If false set to \ref MCR_DISABLE,
 *  otherwise set to \ref MCR_CONTINUE.
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_set_enabled(struct mcr_Macro *mcrPt, bool enable);
/*! Disable and wait in current thread to confirm disable completed.
 *
 *  Use before modifying signal set, C++ deconstructors, and program exit.
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_disable_confirmed(struct mcr_Macro *mcrPt);
/*! Send all signals in current thread.
 *
 *  Any interrupt besides MCR_CONTINUE will complete this function and return.
 *  Sticky is ignored, only run once.
 */
MCR_API int mcr_Macro_send(struct mcr_Macro *mcrPt);

/* Triggering functions */
/*! Add correct receivers and dispatch functions into
 *  \ref mcr_Dispatcher_add
 *
 *  See \ref mcr_Macro_dispatcher for receiver and dispatch rules.
 *  \param trigPt \ref opt Trigger with logic to trigger this macro
 *  \param interceptPt \ref opt Signal to determine dispatch logic
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_add_dispatch(struct mcr_Macro *mcrPt,
								   struct mcr_Trigger *trigPt, struct mcr_Signal *interceptPt);
/*! Remove correct receivers from \ref mcr_Dispatcher_remove
 *
 *  See \ref mcr_Macro_dispatcher for receiver.
 *  \param trigPt \ref opt Trigger with logic to trigger this macro
 *  \param isigPt \ref opt Signal interface to find dispatcher to remove from
 *  \return \ref reterr
 */
MCR_API int mcr_Macro_remove_dispatch(struct mcr_Macro *mcrPt,
									  struct mcr_Trigger *trigPt, struct mcr_ISignal *isigPt);
/*! Prepare a \ref mcr_DispatchPair to trigger this macro.
 *
 *  \param trigPt \ref opt If null or uninitialized we will dispatch directly
 *  into the macro.  Otherwise the macro is triggered
 *  \return Initialized and prepared dispatcher, ready to add to dispatching
 */
MCR_API struct mcr_DispatchPair mcr_Macro_dispatcher(struct mcr_Macro *mcrPt,
		struct mcr_Trigger *trigPt);
/*! \pre \ref mcr_DispatchPair.receiver must be a \ref mcr_Macro *
 *  \pre \ref mcr_DispatchPair.dispatch must be this function
 *  \brief \ref mcr_DispatchPair.dispatch function to send signal set directly
 *  from dispatch.
 *
 *  \param mcrPt \ref mcr_Macro *
 *  \param sigPt \ref opt Intercepted signal, ignored
 *  \param mods Intercepted modifiers, ignored
 *  \return False to not block intercepted signal, otherwise do block
 */
MCR_API bool mcr_Macro_receive(void *mcrPt,
							   struct mcr_Signal *sigPt, unsigned int mods);
/*! \pre \ref mcr_DispatchPair.receiver must be a \ref mcr_Trigger *
 *  \pre \ref mcr_DispatchPair.dispatch must be \ref mcr_Trigger_receive
 *  \pre \ref mcr_Trigger.actor must be a \ref mcr_Macro *
 *  \pre \ref mcr_Trigger.trigger must be this function
 *  \brief \ref mcr_Trigger.trigger function to send signal set from a trigger
 *
 *  \param trigPt \ref mcr_Trigger *
 *  \param sigPt \ref opt Intercepted signal, ignored
 *  \param mods Intercepted modifiers, ignored
 *  \return False to not block intercepted signal, otherwise do block
 */
MCR_API bool mcr_Macro_trigger(void *trigPt,
							   struct mcr_Signal *sigPt, unsigned int mods);

/*! \ref mcr_Macro_is_enabled */
#define MCR_MACRO_IS_ENABLED(mcrPt) \
((mcrPt).interruptor != MCR_DISABLE)

/*! Macro module
 *
 *  In cases of extreme complexity, please break glass.
 */
struct mcr_macro {
	/*! Lock critical sections inside running macro thread. */
	mtx_t critical_section;
	/*! \ref mcr_ITrigger registry */
	struct mcr_IRegistry itriggers;
};

#ifdef __cplusplus
}
#endif

#endif
