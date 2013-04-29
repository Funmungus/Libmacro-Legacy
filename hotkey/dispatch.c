
# include "hotkey/dispatch.h"
# include "hotkey/dispatchutils.h"

static mcr_Array _dispatchers ;
static mcr_Dispatch _dispatcherGeneric ;

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
	mcr_Array_set ( & _dispatchers, signalTypeId, dispPt ) ;
}
mcr_Dispatch * mcr_Dispatch_get ( int signalTypeId )
{
	return MCR_ARR_AT ( & _dispatchers, ( unsigned int ) signalTypeId ) ;
}
void mcr_Dispatch_free ( mcr_Dispatch * dispPt )
{
	if ( ! dispPt ) return ;
	mcr_Array_free ( & dispPt->generics ) ;
}
int mcr_dispatch ( mcr_Signal * signalData )
{
	mcr_Dispatch * found = MCR_ARR_AT ( & _dispatchers,
			signalData->type->id ) ;
	int block = 0 ;
	mtx_lock ( & mcr_ModLock ) ;
	// If found only dispatch to enabled methods.
	if ( found )
	{
		MCR_DISPATCH_MODIFIED ( found, signalData, & mcr_InternalMods,
				block ) ;
	}
	MCR_DISPATCH_MODIFIED ( & _dispatcherGeneric, signalData,
			& mcr_InternalMods, block ) ;
	mtx_unlock ( & mcr_ModLock ) ;
	return block ;
}
void mcr_Dispatch_reset ( mcr_Dispatch * dispPt )
{
	if ( ! dispPt ) return ;
	mcr_Dispatch_release ( dispPt ) ;
	mcr_Dispatch_enable_auto ( dispPt ) ;
}
void mcr_Dispatch_release ( mcr_Dispatch * dispPt )
{
	if ( ! dispPt ) return ;
	mcr_Dispatch_free ( dispPt ) ;
	if ( dispPt->release_specific )
	{
		dispPt->release_specific ( dispPt ) ;
	}
}
void mcr_Dispatch_add ( mcr_Dispatch * dispPt, mcr_Hot * newHotkey )
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
int mcr_Dispatch_dispatch_modified ( mcr_Dispatch * dispPt,
		mcr_Signal * intercepted, unsigned int * mods )
{
	int block = 0 ;
	MCR_DISPATCH_MODIFIED ( dispPt, intercepted, mods, block ) ;
	return block ;
}

// Blank mcr_Hot array for mcr_Map_get_ensured
static mcr_Array _hotsInitial ;
static mcr_Map _addressMapInitial ;


// Map address to modifier to mcr_Hot *
static mcr_Map _modAddress ;
void mcr_DispatchGeneric_free ( )
{
	map_map_free ( & _modAddress ) ;
}
int mcr_DispatchGeneric_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int * modsPt )
{
	UNUSED ( dispPt ) ;
	int block = 0 ;
	// Any address is NULL.
	const void * nullPt = NULL ;
	// Any mod delivered below.
	if ( modsPt && * modsPt != MCR_ANY_MOD )
	{
		// Trigger Map to map.
		TRIGGER_MAPPED_MAPS ( & _modAddress, modsPt,
			& signalPt, & nullPt, signalPt, modsPt, block ) ;
	}
	if ( signalPt != NULL )
	{
		TRIGGER_MAPPED_MAP ( & _modAddress, & MCR_ANY_MOD,
								& signalPt, signalPt, modsPt, block ) ;
	}
	return block ;
}
void mcr_DispatchGeneric_add_specific ( mcr_Dispatch * container,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods )
{
	// Sanity.
	if ( ! newHotkey ) return ;
	if ( ! signalPt && mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add ( container, newHotkey ) ;
	}
	else
	{
		add_mapped_map ( & _modAddress, & mods, & signalPt, & newHotkey,
				& _addressMapInitial, & _hotsInitial ) ;
	}
}
void mcr_DispatchGeneric_remove_specific ( mcr_Dispatch * container,
		mcr_Hot * delHotkey )
{
	UNUSED ( container ) ;
	// Sanity.
	if ( ! delHotkey ) return ;
	if ( _modAddress.set.array )
	{
		map_map_remove_all ( & _modAddress, & delHotkey ) ;
	}
}
void mcr_DispatchGeneric_release ( mcr_Dispatch * dispPt )
{
	UNUSED ( dispPt ) ;
	mcr_DispatchGeneric_free ( ) ;
	mcr_Map_init ( & _modAddress, sizeof ( unsigned int ),
			sizeof ( mcr_Map ) ) ;
}

void mcr_dispatch_initialize ( )
{
	mcr_Array_init ( & _hotsInitial, sizeof ( mcr_Hot * ) ) ;
	mcr_Map_init ( & _addressMapInitial, sizeof ( void * ),
			sizeof ( mcr_Array ) ) ;
	_addressMapInitial.compare = mcr_ref_compare ;

	mcr_Array_init ( & _dispatchers, sizeof ( mcr_Dispatch ) ) ;

	mcr_Dispatch_init ( & _dispatcherGeneric ) ;
	MCR_DISPATCH_SET ( & _dispatcherGeneric,
		& mcr_AllDispatch,
		mcr_DispatchGeneric_dispatch_specific,
		mcr_DispatchGeneric_add_specific,
		mcr_DispatchGeneric_remove_specific,
		mcr_DispatchGeneric_release ) ;

	mcr_Map_init ( & _modAddress, sizeof ( unsigned int ),
			sizeof ( mcr_Map ) ) ;
	_modAddress.compare = mcr_unsigned_compare ;
}
static void disp_free_redirect ( void * arrIt,... )
{
	mcr_Dispatch_free ( arrIt ) ;
}
void mcr_dispatch_cleanup ( void )
{
	MCR_ARR_FOR_EACH ( & _dispatchers, disp_free_redirect, 0 ) ;
	mcr_Array_free ( & _dispatchers ) ;
	mcr_Dispatch_free ( & _dispatcherGeneric ) ;
	mcr_DispatchGeneric_free ( ) ;
}
