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

typedef struct mcr_Dispatch mcr_Dispatch ;
typedef void ( * mcr_dispatch_fnc ) ( mcr_Dispatch * ) ;
typedef void ( * mcr_add_specific_fnc ) ( mcr_Dispatch *, mcr_Hot *,
		mcr_Signal *, unsigned int ) ;
typedef int ( * mcr_dispatch_specific_fnc ) ( mcr_Dispatch *,
		mcr_Signal *, unsigned int ) ;
typedef void ( * mcr_remove_specific_fnc ) ( mcr_Dispatch *, mcr_Hot * ) ;
typedef unsigned int ( * mcr_modifier_provider ) ( mcr_Signal *,
		unsigned int ) ;

/*! \brief Blocking Dispatcher for any signal type.
 * Dispatches into \ref mcr_Hot and specific hotkey types.
 * */
typedef struct mcr_Dispatch
{
	//! \brief Set of pointers to \ref mcr_Hot for generic dispatch.
	mcr_Array generics ;
	//! \brief Enabler for unspecific intercept.
	int enable_unspecific ;
	//! \brief Enabler for specific intercept.
	int enable_specific ;
	/*! \brief Pointer to variable which enables or disables signal
	 * dispatch to a function. \ref mcr_ISignal#dispatch is set to
	 * \ref mcr_dispatch by default. */
	mcr_signal_fnc * dispatcher_pt ;
	//
	// Dispatch interface vtable
	//
	/*! \brief Add to specific hotkeys, dependant on given signal
	 * and mods. */
	mcr_add_specific_fnc add_specific ;
	/*! \brief Dispatching function for specifics. Modifier is assumed
	 * to already be found. */
	mcr_dispatch_specific_fnc dispatch_specific ;
	//! \brief Remove given hotkey from specific hotkeys.
	mcr_remove_specific_fnc remove_specific ;
	//! \brief Clear all specific hotkeys.
	mcr_dispatch_fnc clear_specific ;
	//! \brief Change modifiers by signal type.
	mcr_modifier_provider modifier ;
} mcr_Dispatch ;

/*!
 * \brief Add hotkey to the dispatch for given signal.
 *
 * \param interceptPt If NULL, hotkey will add to generics.
 * */
MCR_API void mcr_Dispatch_add ( mcr_Hot * hotPt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
/*!
 * \brief Get a dispatcher from signal id.
 *
 * \return Reference to a dispatcher registered for
 * given signal type's id.
 * */
MCR_API mcr_Dispatch * mcr_Dispatch_get ( int signalTypeId ) ;
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
		mcr_Hot * newHotkey, mcr_Signal * interceptPt,
		unsigned int interceptMods ) ;
/*!
 * \brief Set \ref mcr_Dispatch#dispatcher_pt to \ref mcr_dispatch but
 * only if either enable members are true. Otherwise this will disable.
 * */
MCR_API void mcr_Dispatch_enable_auto ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief Set \ref mcr_Dispatch#dispatcher_pt but only if either enable
 * members are true. Otherwise this will disable.
 *
 * \param dispatchTo The dispatch function to use.
 * */
MCR_API void mcr_Dispatch_enable_auto_to ( mcr_Dispatch * dispPt,
		mcr_signal_fnc dispatchTo ) ;

/*!
 * \brief \ref mcr_Dispatch_enable_to with \ref mcr_dispatch.
 *
 * \param enable 0 to disable, otherwise enable.
 * */
MCR_API void mcr_Dispatch_enable ( mcr_Dispatch * dispPt, int enable ) ;
/*!
 * \brief If enabling set \ref mcr_Dispatch#dispatcher_pt, if disabling
 * nullify.
 *
 * Note that if neither of the enable members are true, then nothing will
 * happen during dispatch.
 *
 * \param enable 0 to disable, otherwise enable.
 * \param dispatchTo The dispatch function to use.
 * */
MCR_API void mcr_Dispatch_enable_to ( mcr_Dispatch * dispPt, int enable,
		mcr_signal_fnc dispatchTo ) ;
/*!
 * \brief Get enabled state of dispatcher.
 *
 * \return 0 if referenced dispatcher_pt is null. 1 if
 * referenced dispatcher_pt is \ref mcr_dispatch. -1 if it is set
 * to an unknown function.
 * */
MCR_API int mcr_Dispatch_is_enabled ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief Get enabled state of dispatcher, using a specific dispatch
 * function.
 *
 * \return 0 if referenced dispatcher_pt is null. 1 if
 * it is the dispatching function given. -1 if it is set to another
 * function.
 * */
MCR_API int mcr_Dispatch_is_enabled_to ( mcr_Dispatch * dispPt,
		mcr_signal_fnc dispatchTo ) ;
/*!
 * \brief Remove a hotkey callback for unspecific trigger.
 *
 * \param delHotkey The hotkey callback to be removed.
 * */
MCR_API void mcr_Dispatch_remove ( mcr_Dispatch * dispPt,
		mcr_Hot * delHotkey ) ;
/*!
 * \brief \ref mcr_Dispatch_clear and then \ref mcr_Dispatch_enable_auto
 * This includes specifics.
 * */
MCR_API void mcr_Dispatch_reset ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief Clear all hotkey callback objects. This includes specifics.
 * */
MCR_API void mcr_Dispatch_clear ( mcr_Dispatch * dispPt ) ;
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
/*!
 * \brief mcr_Dispatch_register Register a new dispatcher for a
 * signal's type id.
 *
 * \param dispPt Dispatcher to copy.
 * \param signalTypeId Id of the associated signal.
 * */
MCR_API void mcr_Dispatch_register ( mcr_Dispatch * dispPt,
		int signalTypeId ) ;
/*!
 * \brief Clean up \ref mcr_Dispatch resources.
 * */
MCR_API void mcr_Dispatch_free ( mcr_Dispatch * dispPt ) ;


//
// Utility
//
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
# define MCR_DISPATCH_SET( dispatchPt, dispatcherFncPt, addFnc, dispFnc, \
		remFnc, clearFnc ) \
	( dispatchPt )->dispatcher_pt = ( dispatcherFncPt ) ; \
	( dispatchPt )->add_specific = ( addFnc ) ; \
	( dispatchPt )->dispatch_specific = ( dispFnc ) ; \
	( dispatchPt )->remove_specific = ( remFnc ) ; \
	( dispatchPt )->clear_specific = ( clearFnc ) ;


# define MCR_DISPATCH( dispatchPt, interceptedPt, mods, block ) \
	if ( ( dispatchPt )->enable_specific && \
			( dispatchPt )->dispatch_specific ) \
	{ \
		if ( ( dispatchPt )->dispatch_specific \
				( dispatchPt, interceptedPt, mods ) ) \
			block = 1 ; \
	} \
	if ( ( dispatchPt )->enable_unspecific ) \
	{ \
		MCR_HOT_TRIGGER_ARRAY ( ( ( mcr_Hot ** ) \
				( dispatchPt )->generics.array ), \
				( dispatchPt )->generics.used, interceptedPt, mods, \
				block ) \
	}


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
MCR_API void mcr_DispatchGeneric_add_specific ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods ) ;
/*! \brief Specific hotkeys triggered based on modifiers and
 * signal address.
 * */
MCR_API int mcr_DispatchGeneric_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int mods ) ;
//! \brief \ref mcr_Array_remove_all
MCR_API void mcr_DispatchGeneric_remove_specific ( mcr_Dispatch * dispPt,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchGeneric_clear ( mcr_Dispatch * dispPt ) ;

MCR_API void mcr_dispatch_initialize ( ) ;
MCR_API void mcr_dispatch_cleanup ( void ) ;

# endif
