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

/*! \file
 * \brief \ref mcr_Dispatcher - Blocking dispatcher, intercept signals before
 * sending.
 *
 * Dispatchers are the consumers of modifiers, so we define
 * them here.
 */

#ifndef MCR_DISPATCHER_H
#define MCR_DISPATCHER_H

#include "mcr/signal/dispatch_pair.h"
#include "mcr/signal/isignal.h"

/*! \brief Reference to Libmacro internal modifiers. */
MCR_API unsigned int *mcr_modifiers(struct mcr_context *ctx);
/*! \brief Add modifiers to current
 *
 * \param addMods Modifiers to add
 */
MCR_API void mcr_add_modifiers(struct mcr_context *ctx, unsigned int addMods);
/*! \brief Remove modifiers from current
 *
 * \param remMods Modifiers to remove
 */
MCR_API void mcr_remove_modifiers(struct mcr_context *ctx,
				  unsigned int remMods);

/* Dispatcher instances are intended to inherit mcr_Dispatcher as the first member,
 * so we will ease casting with voids. */
/*! \brief Function on dispatcher with no arguments.
 *
 * \param dispPt \ref mcr_Dispatcher *
 * \return \ref reterr
 */
typedef int (*mcr_Dispatcher_fnc) (void *dispPt);
/*! \brief Add dispatching receiver for a signal.
 *
 * \param dispPt \ref mcr_Dispatcher * Object which registers dispatch
 * logic, and dispatches signals with modifiers
 * \param sigPt \ref opt Signal-specific logic for what to intercept
 * \param receiver \ref opt If null the receiverFnc will be sent with
 * a null pointer.  This must be set for null receiver function, or
 * mcr_Trigger_receive
 * \param receiverFnc \ref opt If null \ref mcr_Trigger_receive will be
 * used.  If receiver is not set, then this cannot be used with
 * \ref mcr_Trigger_receive.
 * \return \ref reterr
 */
typedef int (*mcr_Dispatcher_add_fnc) (void *dispPt,
				       struct mcr_Signal * sigPt, void *receiver,
				       mcr_Dispatcher_receive_fnc receiverFnc);
/*! \brief Remove receiver
 *
 * \param dispPt \ref mcr_Dispatcher *
 * \param remReceiver \ref opt Receiver to remove
 * \return \ref reterr
 */
typedef int (*mcr_Dispatcher_remove_fnc) (void *dispPt, void *remReceiver);
/*! \brief Dispatch signal and modifiers
 *
 * \param dispPt \ref mcr_Dispatcher *
 * \param sigPt \ref opt Intercepted signal
 * \param mods Intercepted modifiers
 * \return true to block sending signal
 */
typedef bool(*mcr_Dispatcher_dispatch_fnc) (void *dispPt,
		struct mcr_Signal * sigPt, unsigned int mods);
/*! \brief Modify modifiers from given signal
 *
 * \param dispPt \ref mcr_Dispatcher *
 * \param sigPt \ref opt Intercepted signal
 * \param modsPt The modifiers passed in will be modified directly.
 */
typedef void (*mcr_Dispatcher_modify_fnc) (void *dispPt,
		struct mcr_Signal * sigPt, unsigned int *modsPt);

/*! \brief Blocking Dispatcher, dispatches into \ref mcr_DispatchPair.
 *
 * This is just the function table.  Other members are intended to exist
 * elsewhere.
 */
struct mcr_Dispatcher {
	/*! \brief Add to receivers depending on a signal data. */
	mcr_Dispatcher_add_fnc add;
	/*! \brief Clear all receivers. */
	mcr_Dispatcher_fnc clear;
	/*! \brief Dispatch and return blocking status. */
	mcr_Dispatcher_dispatch_fnc dispatch;
	/*! \brief Change modifiers from the given signal. */
	mcr_Dispatcher_modify_fnc modifier;
	/*! \brief Remove given receiver. */
	mcr_Dispatcher_remove_fnc remove;
	/*! \brief Minimize allocated data */
	mcr_Dispatcher_fnc trim;
};

/*! \pre \ref mcr_Dispatcher.dispatch must be set
 * \brief Blocking signal intercept
 *
 * Dispatch with signal and known modifiers into
 * \ref mcr_Dispatcher.dispatch
 * \param sigPt \ref opt Object to dispatch, and possibly block sending
 * \return true to block
 */
MCR_API bool mcr_dispatch(struct mcr_context *ctx, struct mcr_Signal *sigPt);
/*! \brief Get the number of registered dispatchers, including null values
 *
 * \return \ref retind
 */
MCR_API size_t mcr_Dispatcher_count(struct mcr_context *ctx);
/*! \brief Dispatching enabled for a signal type
 *
 * \param isigPt \ref opt Signal interface
 * \return True if signal interface has a dispatcher
 */
MCR_API bool mcr_Dispatcher_is_enabled(struct mcr_context *ctx,
				       struct mcr_ISignal *isigPt);
/*! \brief Enable dispatching from the given signal type.
 *
 * \param typePt \ref opt Signal interface to enable dispatch
 * \param enable True to set the correct dispatcher for the signal interface.
 * Otherwise set the signal interface dispatcher to null.
 */
MCR_API void mcr_Dispatcher_set_enabled(struct mcr_context *ctx,
					struct mcr_ISignal *typePt, bool enable);
/*! \brief Set dispatching enabled for all registered dispatchers
 *
 * \param enable True to set the correct dispatcher for all signal interfaces.
 * Otherwise set the signal interface dispatchers are set to null.
 */
MCR_API void mcr_Dispatcher_set_enabled_all(struct mcr_context *ctx,
		bool enable);
/*! \brief Add a receiver to the dispatch for given signal.
 *
 * \param interceptPt \ref opt Used for dispatch logic and to find the dispatcher
 * to add to
 * \param receiver \ref opt If null the receiverFnc will be sent with
 * a null pointer.  This must be set for null receiver function, or
 * mcr_Trigger_receive
 * \param receiveFnc \ref opt If null \ref mcr_Trigger_receive will be
 * used.  If receiver is not set, then this cannot be used with
 * \ref mcr_Trigger_receive.
 * \return \ref reterr
 */
MCR_API int mcr_Dispatcher_add(struct mcr_context *ctx,
			       struct mcr_Signal *interceptPt, void *receiver,
			       mcr_Dispatcher_receive_fnc receiveFnc);
/*! \brief Add a receiver to the generic dispatcher
 *
 * The generic dispatcher is dispatched for all signals.  If a signal is
 * provided the address will be used for dispatch logic.
 * \param interceptPt \ref opt Used for dispatch logic
 * \param receiver \ref opt If null the receiverFnc will be sent with
 * a null pointer.  This must be set for null receiver function, or
 * mcr_Trigger_receive
 * \param receiveFnc \ref opt If null \ref mcr_Trigger_receive will be
 * used.  If receiver is not set, then this cannot be used with
 * \ref mcr_Trigger_receive.
 * \return \ref reterr
 */
MCR_API int mcr_Dispatcher_add_generic(struct mcr_context *ctx,
				       struct mcr_Signal *interceptPt,
				       void *receiver, mcr_Dispatcher_receive_fnc receiveFnc);
/*! \brief Remove all receivers for a signal type
 *
 * \param isigPt \ref opt Signal type to remove receivers for
 * \return \ref reterr
 */
MCR_API int mcr_Dispatcher_clear(struct mcr_context *ctx,
				 struct mcr_ISignal *isigPt);
/*! \brief Remove all receivers for all registered dispatchers
 *
 * \return \ref reterr
 */
MCR_API int mcr_Dispatcher_clear_all(struct mcr_context *ctx);
/*! \brief Use a signal to change modifiers
 *
 * \param interceptPt \ref opt Signal to use to change modifiers
 * \param modsPt Modifiers to change
 */
MCR_API void mcr_Dispatcher_modify(struct mcr_context *ctx,
				   struct mcr_Signal *interceptPt, unsigned int *modsPt);
/*! \brief Remove a receiver callback for a signal type.
 *
 * \param typePt \ref opt Signal type to remove from
 * \param remReceiver \ref opt The receiver callback to be removed
 * \return \ref reterr
 */
MCR_API int mcr_Dispatcher_remove(struct mcr_context *ctx,
				  struct mcr_ISignal *typePt, void *remReceiver);
/*! \brief Remove a receiver callback for all signal types.
 *
 * \param remReceiver \ref opt The receiver object to be removed
 * \return \ref reterr
 */
MCR_API int mcr_Dispatcher_remove_all(struct mcr_context *ctx,
				      void *remReceiver);
/*! \brief Minimize allocation for a signal type.
 *
 * \param isigPt \ref opt Signal type to minimize
 * \return \ref reterr
 */
MCR_API int mcr_Dispatcher_trim(struct mcr_context *ctx,
				struct mcr_ISignal *isigPt);
/*! \brief Minimize allocation for all dispatchers.
 *
 * \return \ref reterr
 */
MCR_API int mcr_Dispatcher_trim_all(struct mcr_context *ctx);

/*
 * Dispatcher development
 */
/*! \brief \ref mcr_Dispatcher ctor */
MCR_API int mcr_Dispatcher_init(void *dispPt);
/*! \brief \ref mcr_Dispatcher_init and \ref mcr_Dispatcher_set_all
 *
 * \param add \ref opt \ref mcr_Dispatcher.add
 * \param clear \ref opt \ref mcr_Dispatcher.clear
 * \param dispatch \ref opt \ref mcr_Dispatcher.dispatch
 * \param modifier \ref opt \ref mcr_Dispatcher.modifier
 * \param remove \ref opt \ref mcr_Dispatcher.remove
 * \param trim \ref opt \ref mcr_Dispatcher.trim
 * \return New dispatcher
 */
MCR_API struct mcr_Dispatcher mcr_Dispatcher_new(mcr_Dispatcher_add_fnc add,
		mcr_Dispatcher_fnc clear, mcr_Dispatcher_dispatch_fnc dispatch,
		mcr_Dispatcher_modify_fnc modifier, mcr_Dispatcher_remove_fnc remove,
		mcr_Dispatcher_fnc trim);
/*! \brief Set initial values
 *
 * \param add \ref opt \ref mcr_Dispatcher.add
 * \param clear \ref opt \ref mcr_Dispatcher.clear
 * \param dispatch \ref opt \ref mcr_Dispatcher.dispatch
 * \param modifier \ref opt \ref mcr_Dispatcher.modifier
 * \param remove \ref opt \ref mcr_Dispatcher.remove
 * \param trim \ref opt \ref mcr_Dispatcher.trim
 */
MCR_API void mcr_Dispatcher_set_all(struct mcr_Dispatcher *dispPt,
				    mcr_Dispatcher_add_fnc add, mcr_Dispatcher_fnc clear,
				    mcr_Dispatcher_dispatch_fnc dispatch,
				    mcr_Dispatcher_modify_fnc modifier, mcr_Dispatcher_remove_fnc remove,
				    mcr_Dispatcher_fnc trim);
/*! \brief Register a new dispatcher for a
 * signal's type id.
 *
 * \param dispPt \ref mcr_Dispatcher * Dispatcher to copy
 * \param signalTypeId Id of the associated signal
 * \return reterr
 */
MCR_API int mcr_Dispatcher_register(struct mcr_context *ctx,
				    void *dispPt, size_t signalTypeId);
/*! \brief Get a dispatcher from a signal id.
 *
 * \param signalTypeId Id of the signal type
 * \return Reference to a dispatcher registered for
 * given signal type's id
 */
MCR_API struct mcr_Dispatcher *mcr_Dispatcher_from_id(struct mcr_context
		*ctx, size_t signalTypeId);

#endif
