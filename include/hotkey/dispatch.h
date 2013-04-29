/*! \file hotkey/dispatch.h
 * \brief mcr_Dispatch, Blocking dispatcher definition for all signals.
 * Dispatches into mcr_Hot.
 * */

# ifndef MCR_DISPATCH_H
# define MCR_DISPATCH_H

# include "hotkey/hot.h"
//# include "hotkey/signalmod.h"
//# include "hotkey/stagedhotkey.h"

typedef struct mcr_Dispatch mcr_Dispatch ;
typedef void ( * mcr_dispatch_fnc ) ( mcr_Dispatch * ) ;
typedef void ( * mcr_modifier_provider ) ( mcr_Signal *, unsigned int * ) ;
typedef int ( * mcr_dispatch_specific_fnc ) ( mcr_Dispatch *,
		mcr_Signal *, unsigned int * ) ;
typedef void ( * mcr_add_specific_fnc ) ( mcr_Dispatch *, mcr_Hot *,
		mcr_Signal *, unsigned int ) ;
typedef void ( * mcr_remove_specific_fnc ) ( mcr_Dispatch *, mcr_Hot * ) ;

/*! \brief mcr_Dispatch, Blocking Dispatcher for any signal type.
 * Dispatches into mcr_Hot and specific hotkey types.
 *
 * generics - mcr_Array Set of mcr_Hot * for generic dispatch. <br>
 * enable_unspecific - int Enabler for unspecific intercept. <br>
 * enable_specific - int Enabler for specific intercept. <br>
 * dispatcher_pt - mcr_signal_fnc * Pointer to variable which enables
 * or disables signal dispatch to a function.
 * \ref mcr_ISignal#dispatch is set to \ref mcr_dispatch by default. <br>
 *
 * dispatch_specific - mcr_dispatch_specific_fnc Dispatching function for
 * specifics. Modifier assumed to already be found. <br>
 * add_specific - mcr_add_specific_fnc Add to specific hotkeys, dependant
 * on given signal and mods. <br>
 * remove_specific - mcr_remove_specific_fnc Remove given hotkey from
 * specific hotkeys. <br>
 * release_specific - mcr_dispatch_fnc Clear all specific hotkeys. <br>
 *
 * */
typedef struct mcr_Dispatch
{
	mcr_Array generics ;
	int enable_unspecific ;
	int enable_specific ;
	mcr_signal_fnc * dispatcher_pt ;
	//
	// mcr_Dispatch vtable
	//
	mcr_dispatch_specific_fnc dispatch_specific ;
	mcr_modifier_provider modifier ;
	mcr_add_specific_fnc add_specific ;
	mcr_remove_specific_fnc remove_specific ;
	mcr_dispatch_fnc release_specific ;
} mcr_Dispatch ;

/*!
 * \brief Enable with \ref mcr_Dispatch_enable but only if either enable
 * members are true. Otherwise this will disable.
 *
 * \param dispPt mcr_Dispatch *
 * */
MCR_API void mcr_Dispatch_enable_auto ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief Enable with \ref mcr_Dispatch_enable_to but only if either enable
 * members are true. Otherwise this will disable.
 *
 * \param dispPt mcr_Dispatch *
 * \param dispatchTo mcr_signal_fnc The dispatch function to use.
 * */
MCR_API void mcr_Dispatch_enable_auto_to ( mcr_Dispatch * dispPt,
		mcr_signal_fnc dispatchTo ) ;

/*!
 * \brief \ref mcr_Dispatch_enable_to with \ref mcr_dispatch.
 *
 * \param dispPt mcr_Dispatch *
 * \param enable int 0 to disable, otherwise enable.
 * */
MCR_API void mcr_Dispatch_enable ( mcr_Dispatch * dispPt, int enable ) ;
/*!
 * \brief If enabling set \ref mcr_Dispatch#dispatcher_pt, if disabling
 * nullify.
 *
 * Note that if neither of the enable members are true, then nothing will
 * happen during dispatch.
 *
 * \param dispPt mcr_Dispatch *
 * \param enable int 0 to disable, otherwise enable.
 * \param dispatchTo mcr_signal_fnc The dispatch function to use.
 * */
MCR_API void mcr_Dispatch_enable_to ( mcr_Dispatch * dispPt, int enable,
		mcr_signal_fnc dispatchTo ) ;
/*!
 * \brief \return int 0 if referenced dispatcher_pt is null. 1 if
 * referenced dispatcher_pt is \ref mcr_dispatch. -1 if it is set
 * to an unknown function.
 * */
MCR_API int mcr_Dispatch_is_enabled ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief \return int 0 if referenced dispatcher_pt is null. 1 if
 * it is the dispatching function given. -1 if it is set to another
 * function.
 * */
MCR_API int mcr_Dispatch_is_enabled_to ( mcr_Dispatch * dispPt,
		mcr_signal_fnc dispatchTo ) ;
/*!
 * \brief mcr_Dispatch_init \ref mcr_Dispatch ctor
 *
 * \param dispPt \ref mcr_Dispatch *
 * */
MCR_API void mcr_Dispatch_init ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief mcr_Dispatch_register Register a new dispatcher for a
 * signal's type id.
 *
 * \param dispPt mcr_Dispatch * Dispatcher to copy.
 * \param signalTypeId int Id of the associated signal.
 * */
MCR_API void mcr_Dispatch_register ( mcr_Dispatch * dispPt,
		int signalTypeId ) ;
/*!
 * \brief \return mcr_Dispatch * Reference to a dispatcher registered for
 * given signal's type id.
 *
 * \param signalTypeId int Id of signal.
 * */
MCR_API mcr_Dispatch * mcr_Dispatch_get ( int signalTypeId ) ;
/*!
 * \brief mcr_Dispatch_free Clean up \ref mcr_Dispatch resources.
 *
 * \param dispPt \ref mcr_Dispatch *
 * */
MCR_API void mcr_Dispatch_free ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief mcr_dispatch Blocking signal intercept.
 *
 * The dispatcher is found in registered dispatchers. The modifiers
 * for dispatching are \ref mcr_InternalMods
 *
 * \param signalData mcr_Signal * object to blocking dispatch.
 * \return int 0 to not block, otherwise block.
 * */
MCR_API int mcr_dispatch ( mcr_Signal * signalData ) ;
/*!
 * \brief Release and then mcr_Dispatch_enable_auto.
 * This includes specifics.
 *
 * \param dispPt mcr_Dispatch *
 * */
MCR_API void mcr_Dispatch_reset ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief Clear all hotkey callback objects. This includes specifics.
 *
 * \param dispPt mcr_Dispatch *
 * */
MCR_API void mcr_Dispatch_release ( mcr_Dispatch * dispPt ) ;
/*!
 * \brief Add callback for unspecific trigger.
 *
 * \param dispPt mcr_Dispatch *
 * \param newHotkey mcr_Hot * New hotkey reference to add into dispPt.
 * */
MCR_API void mcr_Dispatch_add ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey ) ;
/*!
 * \brief mcr_Dispatch_add_specific Register hotkey to intercept specific
 * signal and modifier.
 *
 * If no dispatcher is given, it will be found in registered dispatchers
 * from the signal id.
 *
 * \param dispPt mcr_Dispatch *
 * \param newHotkey mcr_Hot * New hotkey reference to add.
 * \param interceptPt mcr_Signal * Specified intercept.
 * \param interceptMods unsigned int Modifiers to intercept for.
 * */
MCR_API void mcr_Dispatch_add_specific ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey, mcr_Signal * interceptPt,
		unsigned int interceptMods ) ;
/*!
 * \brief Remove a hotkey callback for unspecific trigger.
 *
 * \param dispPt mcr_Dispatch *
 * \param delHotkey mcr_Hot * The hotkey callback to be removed.
 * */
MCR_API void mcr_Dispatch_remove ( mcr_Dispatch * dispPt,
		mcr_Hot * delHotkey ) ;
/*!
 * \brief mcr_Dispatch_dispatch_modified Blocking signal intercept,
 * with given modifier.
 *
 * \param dispPt mcr_Dispatch *
 * \param intercepted mcr_Signal * Signal to blocking dispatch.
 * \param mods unsigned int * Modifiers to dispatch with.
 * \return 0 to block intercepted signal. Otherwise block.
 * */
MCR_API int mcr_Dispatch_dispatch_modified ( mcr_Dispatch * dispPt,
		mcr_Signal * intercepted, unsigned int * mods ) ;

# define MCR_DISPATCH_SET( dispatchPt, dispatcherFncPt, dispFnc, \
		addFnc, remFnc, releaseFnc ) \
	( dispatchPt )->dispatcher_pt = ( dispatcherFncPt ) ; \
	( dispatchPt )->dispatch_specific = ( dispFnc ) ; \
	( dispatchPt )->add_specific = ( addFnc ) ; \
	( dispatchPt )->remove_specific = ( remFnc ) ; \
	( dispatchPt )->release_specific = ( releaseFnc ) ;

# define MCR_DISPATCH_MODIFIED( dispatchPt, interceptedPt, modsPt, \
		block ) \
	if ( ( dispatchPt )->enable_specific && \
			( dispatchPt )->dispatch_specific ) \
	{ \
		if ( ( dispatchPt )->dispatch_specific \
				( dispatchPt, interceptedPt, modsPt ) ) \
			block = 1 ; \
	} \
	if ( ( dispatchPt )->enable_unspecific ) \
	{ \
		MCR_HOT_TRIGGER_ARRAY ( ( ( mcr_Hot ** ) \
				( dispatchPt )->generics.array ), \
				( dispatchPt )->generics.used, interceptedPt, modsPt, \
				block ) \
	} \
	if ( ! block && ( dispatchPt )->modifier ) \
		( dispatchPt )->modifier ( interceptedPt, modsPt ) ;


MCR_API void mcr_DispatchGeneric_free ( ) ;
MCR_API int mcr_DispatchGeneric_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int * modsPt ) ;
MCR_API void mcr_DispatchGeneric_add_specific ( mcr_Dispatch * container,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchGeneric_remove_specific ( mcr_Dispatch * container,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchGeneric_release ( mcr_Dispatch * dispPt ) ;

MCR_API void mcr_dispatch_initialize ( ) ;
MCR_API void mcr_dispatch_cleanup ( void ) ;
//# include "hotkey/inline/dispatch.h"

# endif
