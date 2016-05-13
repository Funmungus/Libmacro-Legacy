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

/*! \file
 * \brief Macro: When the macro is triggered,
 * its set of signals is sent one after the other.
 */

#ifndef MCR_MACRO_H
#define MCR_MACRO_H

#include "mcr/macro/trigger.h"

/*! \brief Pause, stop, or continue a running macro */
enum mcr_Interrupt {
	/*! \brief Able to trigger, or continue currently running */
	MCR_CONTINUE = 0,
	/*! \brief Briefly stop sending signals during critical sections. */
	MCR_PAUSE,
	/*!
	 * \brief One queued macro from one thread will cancel.
	 * After one is cancelled, MCR_CONTINUE is set.
	 */
	MCR_INTERRUPT,
	/*!
	 * \brief All threads and queued items will cancel.
	 * After all are cancelled, MCR_CONTINUE is set.
	 */
	MCR_INTERRUPT_ALL,
	/*! \brief Not able to trigger */
	MCR_DISABLE
};

struct mcr_Macro {
	bool block;
	bool sticky;
	unsigned int thread_max;

	/* Internal */
	cnd_t cnd;
	enum mcr_Interrupt interruptor;
	mtx_t lock;
	struct mcr_context *mcr_pt;
	struct mcr_Array signal_set;
	unsigned int thread_count;
	struct mcr_Trigger trigger;
	unsigned int queued;
};

MCR_API void mcr_Macro_init(void *mcrDataPt);
MCR_API int mcr_Macro_set_all(struct mcr_Macro *mcrPt,
	bool block, bool sticky, unsigned int threadMax, bool enable,
	struct mcr_context *ctx);
MCR_API void mcr_Macro_free(void *mcrDataPt);
MCR_API int mcr_Macro_copy(void *dstPt, void *srcPt);
MCR_API int mcr_Macro_set_trigger(struct mcr_Macro *mcrPt,
	struct mcr_Trigger *trigPt);
/*!
 * \brief Trigger with this macro.
 *
 * This will also install this macro into the trigger. That means
 * \ref mcr_Macro_trigger will be set with \ref mcr_Trigger.trigger.
 */
MCR_API void mcr_Macro_set_trigger_actor(struct mcr_Macro *mcrPt,
	struct mcr_Trigger *trigPt);
/*! \brief Stop macro execution according to \ref mcr_Interrupt.
 */
MCR_API int mcr_Macro_interrupt(struct mcr_Macro *mcrPt,
	enum mcr_Interrupt interruptType);
MCR_API struct mcr_Signal *mcr_Macro_signal(struct mcr_Macro *mcrPt,
	size_t index);
/*! \brief Copy all macro signals into a buffer. */
MCR_API void mcr_Macro_signals(struct mcr_Macro *mcrPt,
	struct mcr_Signal *signalbuffer, size_t bufferCount);
MCR_API int mcr_Macro_set_signal(struct mcr_Macro *mcrPt,
	struct mcr_Signal *copySig, size_t index);
/*!
 * \brief Set all signals for this macro. To avoid buffer overflow,
 * the macro will be completely disabled while modifying the set
 * of signals.
 */
MCR_API int mcr_Macro_set_signals(struct mcr_Macro *mcrPt,
	struct mcr_Signal *signalSet, size_t signalCount);
MCR_API int mcr_Macro_insert_signal(struct mcr_Macro *mcrPt,
	struct mcr_Signal *copySig, size_t index);
MCR_API void mcr_Macro_remove_signal(struct mcr_Macro *mcrPt, size_t index);
MCR_API int mcr_Macro_push_signal(struct mcr_Macro *mcrPt,
	struct mcr_Signal *newSig);
MCR_API void mcr_Macro_pop_signal(struct mcr_Macro *mcrPt);
MCR_API bool mcr_Macro_is_enabled(struct mcr_Macro *mcrPt);
MCR_API int mcr_Macro_set_enabled(struct mcr_Macro *mcrPt, bool enable);
/*!
 * \brief Send signal set directly from dispatch.
 *
 * This should be used if trigger is empty.
 * \param mcrDataPt \ref mcr_Macro *
 */
MCR_API bool mcr_Macro_receive(void *mcrDataPt,
	struct mcr_Signal *sigPt, unsigned int mods);
/*!
 * \brief Send signal set from \ref mcr_Trigger.trigger
 *
 * \param trigDataPt \ref mcr_Trigger *
 */
MCR_API bool mcr_Macro_trigger(void *trigDataPt,
	struct mcr_Signal *sigPt, unsigned int mods);

#define MCR_MACRO_IS_ENABLED(mcrPt) \
((mcrPt).interruptor != MCR_DISABLE)

#endif
