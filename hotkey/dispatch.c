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

void mcr_Dispatch_add ( mcr_Hot * hotPt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	if ( interceptPt && interceptPt->type )
	{
		mcr_Dispatch_add_specific ( mcr_Dispatch_get (
				interceptPt->type->id ), hotPt, interceptPt,
				mods ) ;
	}
	else
	{
		mcr_Dispatch_add_specific ( & _dispatcherGeneric,
				hotPt, interceptPt, mods ) ;
	}
}

mcr_Dispatch * mcr_Dispatch_get ( int signalTypeId )
{
	if ( signalTypeId == -1 )
		return & _dispatcherGeneric ;
	return MCR_ARR_AT ( & _dispatchers, ( unsigned int ) signalTypeId ) ;
}

void mcr_Dispatch_add_unspecific ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey )
{
	if ( ! newHotkey || ! dispPt ) return ;
	mcr_Array_push_unique ( & dispPt->generics, & newHotkey ) ;
}

void mcr_Dispatch_add_specific ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey, mcr_Signal * interceptPt,
		unsigned int interceptMods )
{
	if ( dispPt && dispPt->add_specific )
	{
		dispPt->add_specific ( dispPt, newHotkey, interceptPt,
				interceptMods ) ;
	}
}

void mcr_Dispatch_enable_auto ( mcr_Dispatch * dispPt )
{
	mcr_Dispatch_enable_auto_to ( dispPt, mcr_dispatch ) ;
}

void mcr_Dispatch_enable_auto_to ( mcr_Dispatch * dispPt,
		mcr_signal_fnc dispatchTo )
{
	if ( ! dispPt ) return ;
	if ( dispPt->enable_specific || dispPt->enable_unspecific )
	{
		mcr_Dispatch_enable_to ( dispPt, 1, dispatchTo ) ;
	}
	else
	{
		mcr_Dispatch_enable_to ( dispPt, 0, dispatchTo ) ;
	}
}

void mcr_Dispatch_enable ( mcr_Dispatch * dispPt, int enable )
{
	mcr_Dispatch_enable_to ( dispPt, enable, mcr_dispatch ) ;
}

void mcr_Dispatch_enable_to ( mcr_Dispatch * dispPt, int enable,
		mcr_signal_fnc dispatchTo )
{
	if ( ! dispPt || ! dispPt->dispatcher_pt )
	{
		return ;
	}
	if ( enable )
	{
		( * dispPt->dispatcher_pt ) = dispatchTo ;
	}
	else
	{
		( * dispPt->dispatcher_pt ) = NULL ;
	}
}

int mcr_Dispatch_is_enabled ( mcr_Dispatch * dispPt )
{
	return mcr_Dispatch_is_enabled_to ( dispPt, mcr_dispatch ) ;
}

int mcr_Dispatch_is_enabled_to ( mcr_Dispatch * dispPt,
		mcr_signal_fnc dispatchTo )
{
	if ( ! dispPt || ! dispPt->dispatcher_pt ) return 0 ;
	// mcr_Dispatch is not allowing any dispatch through.
	if ( ! dispPt->enable_specific && ! dispPt->enable_unspecific )
		return 0 ;
	// If dispatcher is not dispatching,
	if ( ! * dispPt->dispatcher_pt )
	{
		return 0 ;
	}
	// Dispatcher dispatching to known dispatch function.
	if ( * dispPt->dispatcher_pt == dispatchTo )
	{
		return 1 ;
	}
	// Dispatcher dispatching, but we do not know where.
	return -1 ;
}

void mcr_Dispatch_remove ( mcr_Dispatch * dispPt, mcr_Hot * delHotkey )
{
	if ( ! delHotkey || ! dispPt ) return ;
	mcr_Array_remove_all ( & dispPt->generics, & delHotkey ) ;
	// Also remove specifics.
	if ( dispPt->remove_specific )
	{
		dispPt->remove_specific ( dispPt, delHotkey ) ;
	}
}

void mcr_Dispatch_reset ( mcr_Dispatch * dispPt )
{
	if ( ! dispPt ) return ;
	mcr_Dispatch_clear ( dispPt ) ;
	mcr_Dispatch_enable_auto ( dispPt ) ;
}

void mcr_Dispatch_clear ( mcr_Dispatch * dispPt )
{
	if ( ! dispPt ) return ;
	mcr_Dispatch_free ( dispPt ) ;
	if ( dispPt->clear_specific )
	{
		dispPt->clear_specific ( dispPt ) ;
	}
}

int mcr_dispatch ( mcr_Signal * signalData )
{
	mcr_Dispatch * found = MCR_ARR_AT ( & _dispatchers,
			signalData->type->id ) ;
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
	if ( ! dispPt ) return ;
	memset ( dispPt, 0, sizeof ( mcr_Dispatch ) ) ;
	dispPt->enable_specific = 1 ;
	dispPt->enable_unspecific = 1 ;
	mcr_Array_init ( & dispPt->generics, sizeof ( mcr_Hot * ) ) ;
}

void mcr_Dispatch_register ( mcr_Dispatch * dispPt, int signalTypeId )
{
	if ( ! dispPt ) return ;
	// id -1 is generic. Free before overwriting.
	if ( signalTypeId == -1 )
	{
		// Do not free your own dispatcher
		if ( dispPt == & _dispatcherGeneric )
			return ;
		mcr_DispatchGeneric_free ( ) ;
		memcpy ( & _dispatcherGeneric, dispPt,
				sizeof ( mcr_Dispatch ) ) ;
		return ;
	}

	// Avoid segmentation fault, extra space in dispatchers is 0'd out.
	if ( ( unsigned int ) signalTypeId >= _dispatchers.used )
		mcr_Array_insert_filled ( & _dispatchers, signalTypeId,
				dispPt, NULL ) ;
	// Overwrite positions that exist.
	else
		mcr_Array_set ( & _dispatchers, signalTypeId, dispPt ) ;
}

void mcr_Dispatch_free ( mcr_Dispatch * dispPt )
{
	if ( ! dispPt ) return ;
	mcr_Array_free ( & dispPt->generics ) ;
}

int mcr_Dispatch_dispatch_modified ( mcr_Dispatch * dispPt,
		mcr_Signal * interceptedPt, unsigned int * modsPt )
{
	int block = 0 ;
	if ( ! modsPt )
	{
		MCR_DISPATCH ( dispPt, interceptedPt, MCR_ANY_MOD, block ) ;
	}
	else
	{
		MCR_DISPATCH_MODIFY ( dispPt, interceptedPt, * modsPt, block ) ;
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
	mcr_Dispatch_init ( & _dispatcherGeneric ) ;
	MCR_DISPATCH_SET ( & _dispatcherGeneric, & mcr_allDispatch,
			mcr_DispatchGeneric_add_specific,
			mcr_DispatchGeneric_dispatch_specific,
			mcr_DispatchGeneric_remove_specific,
			mcr_DispatchGeneric_clear ) ;
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

void mcr_DispatchGeneric_add_specific ( mcr_Dispatch * dispPt,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods )
{
	if ( ! newHotkey ) return ;
	if ( ! signalPt && mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add_unspecific ( dispPt, newHotkey ) ;
	}
	else
	{
		add_mapped_map ( & _modAddress, & mods, & signalPt, & newHotkey,
				& _addressMapInitial, & _hotsInitial ) ;
	}
}

int mcr_DispatchGeneric_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int mods )
{
	UNUSED ( dispPt ) ;
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

void mcr_DispatchGeneric_remove_specific ( mcr_Dispatch * dispPt,
		mcr_Hot * delHotkey )
{
	UNUSED ( dispPt ) ;
	// Sanity.
	if ( ! delHotkey ) return ;
	if ( _modAddress.set.used )
	{
		map_map_remove_all ( & _modAddress, & delHotkey ) ;
	}
}

void mcr_DispatchGeneric_clear ( mcr_Dispatch * dispPt )
{
	UNUSED ( dispPt ) ;
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

static void disp_free_redirect ( mcr_Dispatch * dispPt,... )
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
