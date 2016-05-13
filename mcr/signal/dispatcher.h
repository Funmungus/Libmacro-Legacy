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
 * \brief Blocking dispatcher, intercept signals before sending.
 *
 * Dispatchers are the consumers of modifiers, so we define
 * them here.
 */

#ifndef MCR_DISPATCHER_H
#define MCR_DISPATCHER_H

#include "mcr/signal/dispatch_map.h"

/*! \brief All modifiers known by Libmacro to be set. */
MCR_API unsigned int mcr_modifiers(struct mcr_context *ctx);
MCR_API void mcr_set_modifiers(struct mcr_context *ctx, unsigned int setMods);
MCR_API void mcr_add_modifiers(struct mcr_context *ctx, unsigned int addMods);
MCR_API void mcr_remove_modifiers(struct mcr_context *ctx,
	unsigned int remMods);
MCR_API struct timespec mcr_modifiers_timeout(struct mcr_context *ctx);
MCR_API void mcr_modifiers_set_timeout(struct mcr_context *ctx,
	struct timespec *setTime);

/* Dispatcher instances are intended to inherit mcr_Dispatcher as the first member,
 * so we will ease casting with voids. */
/*!
 * \brief Function on dispatcher with no arguments.
 *
 * Take some action on existing object and data.
 */
typedef mcr_data_fnc mcr_Dispatcher_fnc;
/*!
 * \brief Add dispatching receiver for the data of a signal.
 *
 * \param dispatcher Object which registers dispatch logic, and dispatches
 * signals with modifiers
 * \param sigPt Signal-specific logic for what to intercept.
 * \param receiver (opt) If NULL the receiverFnc will be sent with
 * a NULL pointer
 * \param receiverFnc (opt) If NULL \ref mcr_Trigger_receive will be
 * used.  If receiver is not set, then this cannot be used with
 * \ref mcr_Trigger_receive.
 * \return \ref reterr
 */
typedef int (*mcr_Dispatcher_add_fnc) (void *dispatcher,
	struct mcr_Signal * sigPt, void *receiver,
	mcr_Dispatcher_receive_fnc receiverFnc);
/*! \brief Remove receiver */
typedef void (*mcr_Dispatcher_remove_fnc) (void *dispPt, void *remReceiver);
/*!
 * \brief Dispatch signal and modifiers
 *
 * \param sigPt
 * \return true for blocking
 */
typedef bool(*mcr_Dispatcher_dispatch_fnc) (void *dispatcher,
	struct mcr_Signal * sigPt, unsigned int mods);
/*!
 * \brief Modify modifiers from given signal
 *
 * \param sigPt
 * \param modsPt The modifiers passed in will be modified directly.
 */
typedef void (*mcr_Dispatcher_modify_fnc) (void *dispatcher,
	struct mcr_Signal * sigPt, unsigned int *modsPt);

/*!
 * \brief Blocking Dispatcher for any signal type,
 * dispatches into \ref mcr_DispatchPair.
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
	/*! \brief Free any resources for this dispatccher. */
	mcr_Dispatcher_fnc free;
	/*! \brief Change modifiers from the given signal. */
	mcr_Dispatcher_modify_fnc modifier;
	/*! \brief Remove given receiver. */
	mcr_Dispatcher_remove_fnc remove;
	/*! \brief \ref mcr_Array_trim for all receiver data. */
	mcr_Dispatcher_fnc trim;
};

/*!
 * \brief Both \ref mcr_Dispatcher and corresponding \ref mcr_DispatchMap
 *
 * Paired together for convenience, since this is the most common form of
 * dispatcher.
 */
struct mcr_MapDispatcher {
	struct mcr_Dispatcher dispatcher;
	struct mcr_DispatchMap map;
};

/*!
 * \brief Blocking signal intercept.
 *
 * Dispatch with signal and known modifiers into
 * \ref mcr_Dispatcher.dispatch
 *
 * \param signalData Object to dispatch, and possibly block sending
 * \return true to block
 */
MCR_API bool mcr_dispatch(struct mcr_context *ctx,
	struct mcr_Signal *signalData);
MCR_API size_t mcr_Dispatcher_count(struct mcr_context *ctx);
/*!
 * \brief 0 if dispatching is not enabled, otherwise it is enabled
 *
 * \param isigPt
 * \return bool
 */
MCR_API bool mcr_Dispatcher_is_enabled(struct mcr_context *ctx,
	struct mcr_ISignal *isigPt);
/*! \brief Enable dispatching from the given signal type. */
MCR_API void mcr_Dispatcher_set_enabled(struct mcr_context *ctx,
	struct mcr_ISignal *typePt, bool enable);
MCR_API void mcr_Dispatcher_set_enabled_all(struct mcr_context *ctx,
	bool enable);
/*! \brief Add trigger to the dispatch for given signal and modifiers */
MCR_API int mcr_Dispatcher_add(struct mcr_context *ctx,
	struct mcr_Signal *interceptPt, void *receiver,
	mcr_Dispatcher_receive_fnc receiveFnc);
/*!
 * \brief Add a trigger to the dispatch for a set of modifiers.  If a
 * signal is provided, the address is also used for dispatch mapping.
 */
MCR_API int mcr_Dispatcher_add_generic(struct mcr_context *ctx,
	struct mcr_Signal *interceptPt,
	void *receiver, mcr_Dispatcher_receive_fnc receiveFnc);
MCR_API void mcr_Dispatcher_clear(struct mcr_context *ctx,
	struct mcr_ISignal *isigPt);
MCR_API void mcr_Dispatcher_clear_all(struct mcr_context *ctx);
MCR_API void mcr_Dispatcher_free(struct mcr_context *ctx,
	struct mcr_ISignal *isigPt);
MCR_API void mcr_Dispatcher_free_all(struct mcr_context *ctx);
MCR_API void mcr_Dispatcher_modify(struct mcr_context *ctx,
	struct mcr_Signal *interceptPt, unsigned int *modsPt);
/*!
 * \brief Remove a trigger callback for both unspecific and specific
 * trigger.
 *
 * \param remTrigger The trigger callback to be removed
 */
MCR_API void mcr_Dispatcher_remove(struct mcr_context *ctx,
	struct mcr_ISignal *typePt, void *remReceiver);
/*! \ref mcr_Dispatch_remove on all known dispatchers */
MCR_API void mcr_Dispatcher_remove_all(struct mcr_context *ctx,
	void *remReceiver);
MCR_API void mcr_Dispatcher_trim(struct mcr_context *ctx,
	struct mcr_ISignal *isigPt);
MCR_API void mcr_Dispatcher_trim_all(struct mcr_context *ctx);

/*
 * Dispatcher development
 */
/*!
 * \ref mcr_Dispatcher ctor
 */
MCR_API void mcr_Dispatcher_init(void *dispPt);
MCR_API void mcr_Dispatcher_set_all(struct mcr_Dispatcher *dispPt,
	mcr_Dispatcher_add_fnc add, mcr_Dispatcher_fnc clear,
	mcr_Dispatcher_dispatch_fnc dispatch, mcr_Dispatcher_fnc free,
	mcr_Dispatcher_modify_fnc modifier, mcr_Dispatcher_remove_fnc remove,
	mcr_Dispatcher_fnc trim);
/*!
 * \brief Register a new dispatcher for a
 * signal's type id.
 *
 * \param dispPt Dispatcher to copy
 * \param signalTypeId Id of the associated signal
 */
MCR_API int mcr_Dispatcher_register(struct mcr_context *ctx,
	void *dispPt, size_t signalTypeId);
/*!
 * \brief Get a dispatcher from a signal id.
 *
 * \return Reference to a dispatcher registered for
 * given signal type's id
 */
MCR_API struct mcr_Dispatcher *mcr_Dispatcher_from_id(struct mcr_context
	*ctx, size_t signalTypeId);

#endif
