/* hotkey/dispatch.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "hotkey/dispatch.h"
# include "hotkey/dispatchutils.h"

static mcr_Array _dispatchers ;
static mcr_Dispatch _dispatcherGeneric ;

void mcr_Dispatch_add_generic ( mcr_Signal * interceptPt,
		unsigned int mods, mcr_Hot * hotPt )
{
	if ( interceptPt || mods != MCR_ANY_MOD )
		mcr_Dispatch_add_specific ( & _dispatcherGeneric, interceptPt,
				mods, hotPt ) ;
	else
		mcr_Dispatch_add_unspecific ( & _dispatcherGeneric, hotPt ) ;
}

void mcr_Dispatch_add ( mcr_Signal * interceptPt, unsigned int mods,
		mcr_Hot * hotPt )
{
	mcr_Dispatch * dispPt = interceptPt && interceptPt->type ?
			mcr_Dispatch_get ( interceptPt->type->interface.id ) :
			& _dispatcherGeneric ;
	if ( ! dispPt )
		dispPt = & _dispatcherGeneric ;
	if ( ( interceptPt && interceptPt->data.data ) ||
			mods != MCR_ANY_MOD )
	{
		mcr_Dispatch_add_specific ( dispPt, interceptPt, mods, hotPt ) ;
	}
	else
		mcr_Dispatch_add_unspecific ( dispPt, hotPt ) ;
}

void mcr_Dispatch_enable ( mcr_ISignal * typePt, int enable )
{
	dassert ( typePt ) ;
	if ( enable )
	{
		typePt->dispatch = mcr_dispatch ;
		typePt->dispatch_object = mcr_Dispatch_get (
				typePt->interface.id ) ;
	}
	else
	{
		typePt->dispatch = NULL ;
		typePt->dispatch_object = NULL ;
	}
}

int mcr_Dispatch_is_enabled ( mcr_ISignal * typePt )
{
	dassert ( typePt ) ;
	return typePt->dispatch == mcr_dispatch ;
}

void mcr_Dispatch_remove ( mcr_ISignal * typePt, mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	if ( typePt )
	{
		mcr_Dispatch * dispPt = mcr_Dispatch_get (
				typePt->interface.id ) ;
		if ( dispPt )
		{
			mcr_Array_remove_sorted ( & dispPt->generics, & delHotkey,
					mcr_ref_compare ) ;
			if ( dispPt->remove_specific )
				dispPt->remove_specific ( delHotkey ) ;
		}
	}
	else
	{
		mcr_Array_remove_sorted ( & _dispatcherGeneric.generics,
				& delHotkey, mcr_ref_compare ) ;
		if ( _dispatcherGeneric.remove_specific )
			_dispatcherGeneric.remove_specific ( delHotkey ) ;
	}
}

void mcr_Dispatch_remove_all ( mcr_Hot * delHotkey )
{
	mcr_Dispatch * dispArr = ( mcr_Dispatch * ) _dispatchers.array ;
	for ( size_t i = 0 ; i < _dispatchers.used ; i ++ )
	{
		mcr_Array_remove_sorted ( & dispArr [ i ].generics, & delHotkey,
				mcr_ref_compare ) ;
		if ( dispArr [ i ].remove_specific )
			dispArr [ i ].remove_specific ( delHotkey ) ;
	}
	mcr_Array_remove_sorted ( & _dispatcherGeneric.generics, & delHotkey,
			mcr_ref_compare ) ;
	if ( _dispatcherGeneric.remove_specific )
		_dispatcherGeneric.remove_specific ( delHotkey ) ;
}

void mcr_Dispatch_clear ( mcr_ISignal * typePt )
{
	if ( typePt )
	{
		mcr_Dispatch * dispPt = mcr_Dispatch_get (
				typePt->interface.id ) ;
		if ( dispPt->clear_specific )
			dispPt->clear_specific ( ) ;
		mcr_Dispatch_free ( dispPt ) ;
	}
	else
	{
		if ( _dispatcherGeneric.clear_specific )
			_dispatcherGeneric.clear_specific ( ) ;
		mcr_Dispatch_free ( & _dispatcherGeneric ) ;
	}
}

void mcr_Dispatch_clear_all ( )
{
	mcr_Dispatch * dispArr = ( mcr_Dispatch * ) _dispatchers.array ;
	for ( size_t i = 0 ; i < _dispatchers.used ; i ++ )
	{
		if ( dispArr [ i ].clear_specific )
			dispArr [ i ].clear_specific ( ) ;
		mcr_Dispatch_free ( dispArr + i ) ;
	}
	if ( _dispatcherGeneric.clear_specific )
		_dispatcherGeneric.clear_specific ( ) ;
	mcr_Dispatch_free ( & _dispatcherGeneric ) ;
}

int mcr_dispatch ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	dassert ( signalData->type ) ;
	mcr_Dispatch * found = signalData->type->dispatch_object ;
	int block = 0 ;
	int locked = mtx_timedlock ( & mcr_modLock, & mcr_modLockTimeout ) ;
	// If found only dispatch to enabled methods.
	if ( found )
	{
		MCR_DISPATCH_MODIFY ( found, signalData, mcr_internalMods,
				block ) ;
	}
	MCR_DISPATCH_MODIFY ( & _dispatcherGeneric, signalData,
			mcr_internalMods, block ) ;
	if ( locked == thrd_success )
		mtx_unlock ( & mcr_modLock ) ;
	return block ;
}

void mcr_Dispatch_init ( mcr_Dispatch * dispPt )
{
	if ( ! dispPt )
	{
		dmsg ;
		return ;
	}
	memset ( dispPt, 0, sizeof ( mcr_Dispatch ) ) ;
	mcr_Array_init ( & dispPt->generics, sizeof ( mcr_Hot * ) ) ;
}

void mcr_Dispatch_init_with ( mcr_Dispatch * dispPt,
		mcr_add_specific_fnc add, void ( * clear ) ( ),
		mcr_dispatch_specific_fnc dispatch, mcr_modify_fnc modifier,
		mcr_remove_specific_fnc remove )
{
	if ( ! dispPt )
	{
		dmsg ;
		return ;
	}
	mcr_Dispatch_init ( dispPt ) ;
	MCR_DISPATCH_SET ( dispPt, add, clear, dispatch,
			modifier, remove ) ;
}

void mcr_Dispatch_register ( mcr_Dispatch * dispPt, size_t signalTypeId )
{
	dassert ( dispPt ) ;
	mcr_Dispatch * found = signalTypeId < _dispatchers.used ||
			signalTypeId == ( size_t ) -1 ?
			mcr_Dispatch_get ( signalTypeId ) : NULL ;
	if ( found )
	{
		// Do not free your own dispatcher
		if ( dispPt == found )
			return ;
		mcr_Dispatch_free ( found ) ;
		memcpy ( found, dispPt, sizeof ( mcr_Dispatch ) ) ;
	}
	else
	{
		mcr_Dispatch initial ;
		mcr_Dispatch_init ( & initial ) ;
		mcr_Array_insert_filled ( & _dispatchers, signalTypeId,
				dispPt, & initial ) ;
	}
}

void mcr_Dispatch_free ( mcr_Dispatch * dispPt )
{
	if ( ! dispPt )
	{
		dmsg ;
		return ;
	}
	mcr_Array_free ( & dispPt->generics ) ;
}

mcr_Dispatch * mcr_Dispatch_get ( size_t signalTypeId )
{
	mcr_Dispatch * ret = MCR_ARR_AT ( & _dispatchers, signalTypeId ) ;
	return ret ? ret : & _dispatcherGeneric ;
}

void mcr_Dispatch_add_unspecific ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey )
{
	dassert ( dispPt ) ;
	dassert ( newHotkey ) ;
	mcr_Array_add_sorted ( & dispPt->generics, & newHotkey,
			mcr_ref_compare ) ;
}

void mcr_Dispatch_add_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * interceptPt, unsigned int interceptMods,
		mcr_Hot * newHotkey )
{
	dassert ( dispPt ) ;
	if ( dispPt->add_specific )
	{
		dispPt->add_specific ( interceptPt,
				interceptMods, newHotkey ) ;
	}
}

int mcr_Dispatch_dispatch_modified ( mcr_Dispatch * dispPt,
		mcr_Signal * interceptedPt, unsigned int * modsPt )
{
	int block = 0 ;
	if ( modsPt )
	{
		MCR_DISPATCH_MODIFY ( dispPt, interceptedPt, * modsPt, block ) ;
	}
	else
	{
		MCR_DISPATCH ( dispPt, interceptedPt, MCR_ANY_MOD, block ) ;
	}
	return block ;
}

//
// Generic dispatcher
//
// Blank mcr_Hot pointer array for mcr_Map_get_ensured
static mcr_Array _hotsInitial ;
// Blank map from pointer to pointers of mcr_Hot.
static mcr_Map _addressMapInitial ;

// Map address to modifier to mcr_Hot *
static mcr_Map _modAddress ;
void mcr_DispatchGeneric_init ( )
{
	// mcr_Dispatch object.
	mcr_Dispatch_init_with ( & _dispatcherGeneric,
			mcr_DispatchGeneric_add_specific,
			mcr_DispatchGeneric_clear,
			mcr_DispatchGeneric_dispatch_specific,
			NULL, mcr_DispatchGeneric_remove_specific ) ;
	// Map modifier to address to hotkeys.
	mcr_Map_init ( & _modAddress, sizeof ( unsigned int ),
			sizeof ( mcr_Map ) ) ;
	_modAddress.compare = mcr_unsigned_compare ;
}

void mcr_DispatchGeneric_free ( )
{
	map_map_free ( & _modAddress ) ;
	mcr_Dispatch_free ( & _dispatcherGeneric ) ;
}

void mcr_DispatchGeneric_add_specific ( mcr_Signal * signalPt,
		unsigned int mods, mcr_Hot * newHotkey )
{
	dassert ( newHotkey ) ;
	if ( ! signalPt && mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add_unspecific ( & _dispatcherGeneric, newHotkey ) ;
	}
	else
	{
		add_mapped_map_sorted ( & _modAddress, & mods, & signalPt,
				& newHotkey, & _addressMapInitial, & _hotsInitial,
				mcr_ref_compare ) ;
	}
}

int mcr_DispatchGeneric_dispatch_specific ( mcr_Signal * signalPt,
		unsigned int mods )
{
	int block = 0 ;
	// Any address is NULL.
	const void * nullPt = NULL ;
	// Any mod delivered below.
	if ( mods != MCR_ANY_MOD )
	{
		// Trigger Map to map.
		TRIGGER_MAPPED_MAPS ( & _modAddress, & mods,
				& signalPt, & nullPt, signalPt, mods, block ) ;
	}
	if ( signalPt != NULL )
	{
		TRIGGER_MAPPED_MAP ( & _modAddress, & MCR_ANY_MOD,
				& signalPt, signalPt, mods, block ) ;
	}
	return block ;
}

void mcr_DispatchGeneric_remove_specific ( mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	if ( _modAddress.set.used )
	{
		map_map_remove_sorted ( & _modAddress, & delHotkey,
				mcr_ref_compare ) ;
	}
}

void mcr_DispatchGeneric_clear ( )
{
	mcr_DispatchGeneric_free ( ) ;
	mcr_Map_init ( & _modAddress, sizeof ( unsigned int ),
			sizeof ( mcr_Map ) ) ;
}

void mcr_dispatch_initialize ( )
{
	// Insurance objects.
	mcr_Array_init ( & _hotsInitial, sizeof ( mcr_Hot * ) ) ;
	mcr_Map_init ( & _addressMapInitial, sizeof ( void * ),
			sizeof ( mcr_Array ) ) ;
	_addressMapInitial.compare = mcr_ref_compare ;
	// Generic
	mcr_DispatchGeneric_init ( ) ;
	// Specifics
	mcr_Array_init ( & _dispatchers, sizeof ( mcr_Dispatch ) ) ;
}

static void disp_free_redirect ( mcr_Dispatch * dispPt, ... )
{
	mcr_Dispatch_free ( dispPt ) ;
}

void mcr_dispatch_cleanup ( void )
{
	// Specifics
	MCR_ARR_FOR_EACH ( & _dispatchers, disp_free_redirect, 0 ) ;
	mcr_Array_free ( & _dispatchers ) ;
	// Generic
	mcr_DispatchGeneric_free ( ) ;
	// Insurance objects should not need freeing.
}
