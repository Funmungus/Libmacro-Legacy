/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
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

/*! \file hotkey/dispatch.h
 * Blocking dispatcher to hotkeys
 * */

# ifndef MCR_DISPATCH_H
# define MCR_DISPATCH_H

# include "hotkey/hot.h"
# include "hotkey/mods.h"

//
// Defined by specific dispatcher
//
/*! \brief Add hotkey */
typedef void ( * mcr_add_specific_fnc ) ( mcr_Signal *,
		unsigned int, mcr_TriggerFor, mcr_Hot * ) ;
/*! \brief Dispatch signal and modifiers
 *
 * \return 0 to not block, otherwise do block */
typedef int ( * mcr_dispatch_specific_fnc ) ( mcr_Signal *,
		unsigned int ) ;
/*! \brief Remove hotkey */
typedef void ( * mcr_remove_specific_fnc ) ( mcr_Hot * ) ;
/*! \brief Modify modifiers from given signal
 *
 * \return New modifiers */
typedef unsigned int ( * mcr_modify_fnc ) ( mcr_Signal *,
		unsigned int ) ;

/*! \brief Blocking Dispatcher for any signal type,
 * Dispatches into \ref mcr_Hot and specific hotkey types.
 * */
typedef struct mcr_Dispatch
{
	//
	// Internal
	//
	//! \brief Set of pointers to \ref mcr_Hot for generic dispatch
	mcr_Array generics ;
	//
	// vtable
	//
	/*! \brief Add to specific hotkeys, dependant on given signal
	 * and mods */
	mcr_add_specific_fnc add ;
	//! \brief Clear all specific hotkeys
	void ( * clear ) ( ) ;
	/*! \brief Dispatching function for specifics, modifier is assumed
	 * to already be found. */
	mcr_dispatch_specific_fnc dispatch ;
	//! \brief Change modifiers by signal type.
	mcr_modify_fnc modifier ;
	//! \brief Remove given hotkey from specific hotkeys.
	mcr_remove_specific_fnc remove ;
} mcr_Dispatch ;

/*! \brief Add hotkey to the dispatch for given signal and modifiers.
 * */
MCR_API void mcr_Dispatch_add ( mcr_Signal * interceptPt,
		unsigned int mods, mcr_TriggerFor triggerFor, mcr_Hot * hotPt ) ;
/*! \brief Add hotkey to the dispatch for either the signal address,
 * or all signals if this is NULL, and a set of modifiers.
 * */
MCR_API void mcr_Dispatch_add_generic ( mcr_Signal * interceptPt,
		unsigned int mods, mcr_TriggerFor triggerFor, mcr_Hot * hotPt ) ;
/*! \brief \return 0 if dispatching is not enabled, otherwise it is */
MCR_API int mcr_Dispatch_enabled ( mcr_ISignal * typePt ) ;
/*! \brief Enable dispatching from given signal type. */
MCR_API void mcr_Dispatch_set_enabled ( mcr_ISignal * typePt,
		int enable ) ;
/*! \brief Remove a hotkey callback for both unspecific and specific
 * trigger.
 *
 * \param delHotkey The hotkey callback to be removed.
 * */
MCR_API void mcr_Dispatch_remove ( mcr_ISignal * typePt,
		mcr_Hot * delHotkey ) ;
/*! \brief \ref mcr_Dispatch_remove on all known dispatchers */
MCR_API void mcr_Dispatch_remove_all ( mcr_Hot * delHotkey ) ;
/*! \brief Clear all hotkey callback objects for specific or generic
 * dispatcher.
 * */
MCR_API void mcr_Dispatch_clear ( mcr_ISignal * typePt ) ;
/*! \brief Clear all hotkey callback objects for all
 * dispatchers.
 * */
MCR_API void mcr_Dispatch_clear_all ( ) ;
/*! \brief mcr_dispatch Blocking signal intercept.
 *
 * The dispatcher is found in registered dispatchers. The modifiers
 * for dispatching are \ref mcr_InternalMods
 *
 * \param signalData Object to blocking dispatch, and possibly block
 * \return 0 to not block, otherwise block
 * */
MCR_API int mcr_dispatch ( mcr_Signal * signalData ) ;

//
// Dispatcher development
//
/*!
 * \brief mcr_Dispatch_init \ref mcr_Dispatch ctor
 * */
MCR_API void mcr_Dispatch_init ( mcr_Dispatch * dispPt ) ;
MCR_API void mcr_Dispatch_init_with ( mcr_Dispatch * dispPt,
		mcr_add_specific_fnc add, void ( * clear ) ( ),
		mcr_dispatch_specific_fnc dispatch, mcr_modify_fnc modifier,
		mcr_remove_specific_fnc remove ) ;
/*!
 * \brief Register a new dispatcher for a
 * signal's type id.
 *
 * \param dispPt Dispatcher to copy
 * \param signalTypeId Id of the associated signal
 * */
MCR_API void mcr_Dispatch_register ( mcr_Dispatch * dispPt,
		size_t signalTypeId ) ;
/*! \brief Clean up \ref mcr_Dispatch resources.
 * */
MCR_API void mcr_Dispatch_free ( mcr_Dispatch * dispPt ) ;
# define mcr_Dispatch_free_foreach( dispPt, ... ) mcr_Dispatch_free ( dispPt )

//
// Utility
//
/*! \brief Get a dispatcher from signal id.
 *
 * \return Reference to a dispatcher registered for
 * given signal type's id
 * */
MCR_API mcr_Dispatch * mcr_Dispatch_get ( size_t signalTypeId ) ;
/*!
 * \brief Add callback for unspecific trigger.
 *
 * \param newHotkey New hotkey reference to add into dispatcher
 * */
MCR_API void mcr_Dispatch_add_unspecific ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey ) ;
/*! \brief Register a hotkey to intercept specific signal and modifier.
 *
 * If no dispatcher is given, it will be found in registered dispatchers
 * from the signal id.
 *
 * \param interceptPt Signal intercept to trigger for
 * \param interceptMods Modifiers to trigger for
 * */
MCR_API void mcr_Dispatch_add_specific (mcr_Dispatch * dispPt,
		mcr_Signal * interceptPt,
		unsigned int interceptMods, mcr_TriggerFor trigFor, mcr_Hot * newHotkey ) ;
/*! \brief Blocking signal intercept, with given modifier
 *
 * \param interceptedPt Pointer to signal to dispatch and possibly block
 * \param modsPt Pointer to modifiers to dispatch with
 * \return 0 to not block intercepted signal, otherwise block
 * */
MCR_API int mcr_Dispatch_dispatch_modified ( mcr_Dispatch * dispPt,
		mcr_Signal * interceptedPt, unsigned int * modsPt ) ;

/*! \brief Set the dispatch reference, and the specific add, dispatch,
 * remove, and clear functions */
# define MCR_DISPATCH_SET( dispatcher, \
		addFnc, clearFnc, dispFnc, modProvider, remFnc ) \
	( dispatcher ).add = ( addFnc ) ; \
	( dispatcher ).clear = ( clearFnc ) ; \
	( dispatcher ).dispatch = ( dispFnc ) ; \
	( dispatcher ).modifier = ( modProvider ) ; \
	( dispatcher ).remove = ( remFnc ) ;

# define MCR_DISPATCH( dispatcher, interceptedPt, mods, block ) \
	if ( ( dispatcher ).dispatch && \
			( dispatcher ).dispatch \
			( interceptedPt, mods ) ) \
		block = 1 ; \
	MCR_HOT_TRIGGER_REF_ARRAY ( ( ( mcr_Hot ** ) \
			( dispatcher ).generics.array ), \
			( dispatcher ).generics.used, interceptedPt, mods, \
			block ) ;

/*! \brief Dispatch if enabled, block if required, and modify mods
 * with \ref mcr_Dispatch#modifier if not blocking.
 * */
# define MCR_DISPATCH_MODIFY( dispatch, interceptedPt, mods, \
		block ) \
	MCR_DISPATCH ( dispatch, interceptedPt, mods, block ) ; \
	if ( ! block && ( dispatch ).modifier ) \
		( mods ) = ( dispatch ).modifier ( interceptedPt, mods ) ;

//
// Dispatching based on modifiers and signal reference
//
MCR_API void mcr_DispatchGeneric_init ( ) ;
MCR_API void mcr_DispatchGeneric_free ( ) ;
/*! \brief Specific hotkeys added based on modifiers and
 * signal address
 * */
MCR_API void mcr_DispatchGeneric_add (
		mcr_Signal * signalPt, unsigned int mods,
		mcr_TriggerFor triggerFor, mcr_Hot * newHotkey ) ;
/*! \brief Specific hotkeys triggered based on modifiers and
 * signal address
 * */
MCR_API int mcr_DispatchGeneric_dispatch (
		mcr_Signal * signalPt, unsigned int mods ) ;
//! \brief \ref mcr_Array_remove_all
MCR_API void mcr_DispatchGeneric_remove (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchGeneric_clear ( ) ;

MCR_API void mcr_dispatch_initialize ( ) ;
MCR_API void mcr_dispatch_cleanup ( void ) ;

# endif
