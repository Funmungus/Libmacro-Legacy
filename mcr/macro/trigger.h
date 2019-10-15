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
 *  \brief \ref mcr_Trigger - Receive dispatch and trigger actions.
 *
 *  Receive dispatch from \ref mcr_ITrigger.receive.
 */

#ifndef MCR_MACRO_TRIGGER_H_
#define MCR_MACRO_TRIGGER_H_

#include "mcr/macro/itrigger.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_Macro;
/*! Register to be triggered for \ref mcr_Signal or modifiers */
struct mcr_Trigger {
	/*! \ref mcr_Instance of \ref mcr_ITrigger */
	union {
		struct mcr_ITrigger *itrigger;
		struct mcr_Instance instance;
		struct mcr_Interface *interface;
	};
	/* Trigger specific */
	/*! Function to call when triggered.
	 *
	 *  This will be called from \ref mcr_ITrigger.receive
	 *  First argument is the \ref mcr_Trigger object.
	 */
	mcr_Dispatcher_receive_fnc trigger;
	/*! \ref opt Object which receives trigger action */
	void *actor;
};

/*! \pre \ref mcr_DispatchPair.receiver must be a \ref mcr_Trigger *
 *  \pre \ref mcr_DispatchPair.dispatch must be \ref mcr_Trigger_receive
 *  \brief \ref mcr_Dispatcher_receive_fnc for triggers
 *
 *  This is the default receiver function, when none is provided to
 *  add dispatcher receiver.
 *  If \ref mcr_Trigger.trigger is null this will do nothing.
 *  \param triggerPt \ref mcr_Trigger *
 *  \param dispatchSignal \ref opt Intercepted signal
 *  \param mods Intercepted modifiers
 *  \return False to not block intercepted signal, otherwise do block
 */
MCR_API bool mcr_Trigger_receive(void *triggerPt,
								 struct mcr_Signal *dispatchSignal, unsigned int mods);

/*! Data interface of trigger structures
 *
 *  \return Interface to manage trigger objects
 */
MCR_API const struct mcr_Interface *mcr_Trigger_interface();

/*! \ref mcr_Trigger ctor
 *
 *  \param triggerPt \ref opt \ref mcr_Trigger *
 *  \return 0
 */
MCR_API int mcr_Trigger_init(void *triggerPt);
/*! \ref mcr_Trigger_init and \ref mcr_Trigger_set_all
 *
 *  \param trigger \ref opt \ref mcr_Trigger.trigger
 *  \param actor \ref opt \ref mcr_Trigger.actor
 *  \return Constructed trigger
 */
MCR_API struct mcr_Trigger mcr_Trigger_new(mcr_Dispatcher_receive_fnc trigger,
		void *actor);
/*! \ref mcr_Trigger dtor
 *
 *  \return 0
 */
MCR_API int mcr_Trigger_deinit(void *trigPt);
/*! Set initial values
 *
 *  \param trigger \ref opt \ref mcr_Trigger.trigger
 *  \param actor \ref opt \ref mcr_Trigger.actor
 */
MCR_API void mcr_Trigger_set_all(struct mcr_Trigger *triggerPt,
								 mcr_Dispatcher_receive_fnc trigger, void *actor);
/*! Set trigger dispatch to a macro, and the correct macro receiving
 *  function.
 *
 *  This will also install this macro into the trigger. That means
 *  \ref mcr_Macro_trigger will be set with \ref mcr_Trigger.trigger.
 */
MCR_API void mcr_Trigger_set_macro(struct mcr_Trigger *triggerPt,
								   struct mcr_Macro *mcrPt);

/*! \ref mcr_Instance_copy, then copy trigger and actor
 *
 *  \param dstPt \ref mcr_Trigger * Destination to copy to
 *  \param srcPt \ref opt \ref mcr_Trigger * Source to copy from
 *  \return \ref reterr
 */
MCR_API int mcr_Trigger_copy(void *dstPt, const void *srcPt);
/*! Compare two triggers
 *
 *  \param lhsTriggerPt \ref opt mcr_Trigger *
 *  \param rhsTriggerPt \ref opt mcr_Trigger *
 *  \return \ref retcmp
 */
MCR_API int mcr_Trigger_compare(const void *lhsTriggerPt,
								const void *rhsTriggerPt);
/*! Compare two signal references
 *
 *  \param lhsPtPt \ref opt \ref mcr_Trigger **
 *  \param rhsPtPt \ref opt \ref mcr_Trigger **
 *  \return \ref retcmp
 */
MCR_API int mcr_Triggerref_compare(const void *lhsPtPt, const void *rhsPtPt);

/* Dispatch utilities */
/*! Add trigger as receiver and dispatch function into
 *  \ref mcr_Dispatcher_add
 *
 *  \param interceptPt \ref opt Signal to determine dispatch logic
 *  \return \ref reterr
 */
MCR_API int mcr_Trigger_add_dispatch(struct mcr_context *ctx,
									 struct mcr_Trigger *trigPt, struct mcr_Signal *interceptPt);
/*! Remove trigger from \ref mcr_Dispatcher_remove
 *
 *  \param isigPt \ref opt Signal interface to find dispatcher to remove from
 *  \return \ref reterr
 */
MCR_API int mcr_Trigger_remove_dispatch(struct mcr_context *ctx,
										struct mcr_Trigger *trigPt, struct mcr_ISignal *isigPt);

#ifdef __cplusplus
}
#endif

#endif
