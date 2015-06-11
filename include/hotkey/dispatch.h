/* include/hotkey/dispatch.h - Blocking dispatcher to hotkeys.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file hotkey/dispatch.h
 * Blocking dispatcher to hotkeys.
 * */

# ifndef MCR_DISPATCH_H
# define MCR_DISPATCH_H

# include "hotkey/hot.h"
# include "hotkey/mods.h"

typedef void ( * mcr_add_specific_fnc ) ( mcr_Signal *,
		unsigned int, mcr_Hot * ) ;
typedef int ( * mcr_dispatch_specific_fnc ) ( mcr_Signal *,
		unsigned int ) ;
typedef void ( * mcr_remove_specific_fnc ) ( mcr_Hot * ) ;
typedef unsigned int ( * mcr_modify_fnc ) ( mcr_Signal *,
		unsigned int ) ;

/*! \brief Blocking Dispatcher for any signal type.
 * Dispatches into \ref mcr_Hot and specific hotkey types.
 * */
typedef struct mcr_Dispatch
{
	//
	// Internal
	//
	//! \brief Set of pointers to \ref mcr_Hot for generic dispatch.
	mcr_Array generics ;
	//
	// vtable
	//
	/*! \brief Add to specific hotkeys, dependant on given signal
	 * and mods. */
	mcr_add_specific_fnc add_specific ;
	//! \brief Clear all specific hotkeys.
	void ( * clear_specific ) ( ) ;
	/*! \brief Dispatching function for specifics. Modifier is assumed
	 * to already be found. */
	mcr_dispatch_specific_fnc dispatch_specific ;
	//! \brief Change modifiers by signal type.
	mcr_modify_fnc modifier ;
	//! \brief Remove given hotkey from specific hotkeys.
	mcr_remove_specific_fnc remove_specific ;
} mcr_Dispatch ;

MCR_API void mcr_Dispatch_add_generic ( mcr_Signal * interceptPt,
		unsigned int mods, mcr_Hot * hotPt ) ;
/*!
 * \brief Add hotkey to the dispatch for given signal.
 *
 * \param interceptPt If NULL, hotkey will add to generics.
 * */
MCR_API void mcr_Dispatch_add ( mcr_Signal * interceptPt,
		unsigned int mods, mcr_Hot * hotPt ) ;
MCR_API void mcr_Dispatch_enable ( mcr_ISignal * typePt, int enable ) ;
MCR_API int mcr_Dispatch_is_enabled ( mcr_ISignal * typePt ) ;
/*!
 * \brief Remove a hotkey callback for unspecific trigger.
 *
 * \param delHotkey The hotkey callback to be removed.
 * */
MCR_API void mcr_Dispatch_remove ( mcr_ISignal * typePt,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_Dispatch_remove_all ( mcr_Hot * delHotkey ) ;
/*!
 * \brief Clear all hotkey callback objects. This includes specifics.
 * */
MCR_API void mcr_Dispatch_clear ( mcr_ISignal * typePt ) ;
MCR_API void mcr_Dispatch_clear_all ( ) ;
/*!
 * \brief mcr_dispatch Blocking signal intercept.
 *
 * The dispatcher is found in registered dispatchers. The modifiers
 * for dispatching are \ref mcr_InternalMods
 *
 * \param signalData Object to blocking dispatch, and possibly block.
 * \return 0 to not block, otherwise block.
 * */
MCR_API int mcr_dispatch ( mcr_Signal * signalData ) ;

//
// Dispatcher development.
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
 * \brief mcr_Dispatch_register Register a new dispatcher for a
 * signal's type id.
 *
 * \param dispPt Dispatcher to copy.
 * \param signalTypeId Id of the associated signal.
 * */
MCR_API void mcr_Dispatch_register ( mcr_Dispatch * dispPt,
		size_t signalTypeId ) ;
/*!
 * \brief Clean up \ref mcr_Dispatch resources.
 * */
MCR_API void mcr_Dispatch_free ( mcr_Dispatch * dispPt ) ;


//
// Utility
//
/*!
 * \brief Get a dispatcher from signal id.
 *
 * \return Reference to a dispatcher registered for
 * given signal type's id.
 * */
MCR_API mcr_Dispatch * mcr_Dispatch_get ( size_t signalTypeId ) ;
/*!
 * \brief Add callback for unspecific trigger.
 *
 * \param newHotkey New hotkey reference to add into dispatcher.
 * */
MCR_API void mcr_Dispatch_add_unspecific ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey ) ;
/*!
 * \brief Register hotkey to intercept specific signal and modifier.
 *
 * If no dispatcher is given, it will be found in registered dispatchers
 * from the signal id.
 *
 * \param interceptPt Signal intercept to trigger for.
 * \param interceptMods Modifiers to trigger for.
 * */
MCR_API void mcr_Dispatch_add_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * interceptPt,
		unsigned int interceptMods, mcr_Hot * newHotkey ) ;
/*!
 * \brief Blocking signal intercept, with given modifier.
 *
 * \param interceptedPt Pointer to signal to dispatch and possibly block.
 * \param modsPt Pointer to modifiers to dispatch with. May change.
 * \return 0 to not block intercepted signal. Otherwise block.
 * */
MCR_API int mcr_Dispatch_dispatch_modified ( mcr_Dispatch * dispPt,
		mcr_Signal * interceptedPt, unsigned int * modsPt ) ;

/*! \brief Set the dispatch reference, and the specific add, dispatch,
 * remove, and clear functions */
# define MCR_DISPATCH_SET( dispatchPt, \
		addFnc, clearFnc, dispFnc, modProvider, remFnc ) \
	( dispatchPt )->add_specific = ( addFnc ) ; \
	( dispatchPt )->clear_specific = ( clearFnc ) ; \
	( dispatchPt )->dispatch_specific = ( dispFnc ) ; \
	( dispatchPt )->modifier = ( modProvider ) ; \
	( dispatchPt )->remove_specific = ( remFnc ) ;


# define MCR_DISPATCH( dispatchPt, interceptedPt, mods, block ) \
	if ( ( dispatchPt )->dispatch_specific && \
			( dispatchPt )->dispatch_specific \
			( interceptedPt, mods ) ) \
		block = 1 ; \
	MCR_HOT_TRIGGER_REF_ARRAY ( ( ( mcr_Hot ** ) \
			( dispatchPt )->generics.array ), \
			( dispatchPt )->generics.used, interceptedPt, mods, \
			block ) ;


/*! \brief Dispatch if enabled, block if required, and modify mods
 * with \ref mcr_Dispatch#modifier if not blocking.
 * */
# define MCR_DISPATCH_MODIFY( dispatchPt, interceptedPt, mods, \
		block ) \
	MCR_DISPATCH ( dispatchPt, interceptedPt, mods, block ) ; \
	if ( ! block && ( dispatchPt )->modifier ) \
		( mods ) = ( dispatchPt )->modifier ( interceptedPt, mods ) ;

//
// Dispatching based on modifiers and signal reference.
//
MCR_API void mcr_DispatchGeneric_init ( ) ;
MCR_API void mcr_DispatchGeneric_free ( ) ;
/*! \brief Specific hotkeys added based on modifiers and
 * signal address.
 * */
MCR_API void mcr_DispatchGeneric_add_specific (
		mcr_Signal * signalPt, unsigned int mods, mcr_Hot * newHotkey ) ;
/*! \brief Specific hotkeys triggered based on modifiers and
 * signal address.
 * */
MCR_API int mcr_DispatchGeneric_dispatch_specific (
		mcr_Signal * signalPt, unsigned int mods ) ;
//! \brief \ref mcr_Array_remove_all
MCR_API void mcr_DispatchGeneric_remove_specific (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchGeneric_clear ( ) ;

MCR_API void mcr_dispatch_initialize ( ) ;
MCR_API void mcr_dispatch_cleanup ( void ) ;

# endif
