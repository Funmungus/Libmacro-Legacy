/* hotkey/dispatchstandard.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "hotkey/dispatchstandard.h"
# include "hotkey/dispatchutils.h"

// Blank mcr_Hot array for mcr_Map_get_ensured.
static mcr_Array _hotsInitial ;
// Blank map to _hotsInitial for mcr_Map_get_ensured.
static mcr_Map _intsMapInitial ;

//
// Initializers and freeing for standard dispatchers.
//
// Map modifier unsigned integer to mcr_Array of mcr_Hot *
static mcr_Map _alarmModMap ;
void mcr_DispatchAlarm_init ( mcr_Dispatch * dispPt )
{
	dassert ( dispPt ) ;
	mcr_Map_init ( & _alarmModMap, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
	_alarmModMap.compare = mcr_unsigned_compare ;

	mcr_Dispatch_init_with ( dispPt,
			mcr_DispatchAlarm_add_specific,
			mcr_DispatchAlarm_clear,
			mcr_DispatchAlarm_dispatch_specific,
			NULL, mcr_DispatchAlarm_remove_specific ) ;
}

void mcr_DispatchAlarm_free ( )
{
	map_arr_free_redirect ( & _alarmModMap ) ;
}

// map modifier to map event code to mcr_Hot *
static mcr_Map _modEcho ;
void mcr_DispatchHIDEcho_init ( mcr_Dispatch * dispPt )
{
	dassert ( dispPt ) ;
	mcr_Map_init ( & _modEcho, sizeof ( unsigned int ),
			sizeof ( mcr_Map ) ) ;
	_modEcho.compare = mcr_unsigned_compare ;

	mcr_Dispatch_init_with ( dispPt,
			mcr_DispatchHIDEcho_add_specific,
			mcr_DispatchHIDEcho_clear,
			mcr_DispatchHIDEcho_dispatch_specific,
			mcr_HIDEcho_modify,
			mcr_DispatchHIDEcho_remove_specific ) ;
}

void mcr_DispatchHIDEcho_free ( )
{
	map_map_free ( & _modEcho ) ;
}

// modifier to key to mcr_Hot *
static mcr_Map _modKeys [ 2 ] ;
// modifier to scan to mcr_Hot *
static mcr_Map _modScans [ 2 ] ;
# define KEY_COUNT 4
static mcr_Map * _keyMaps [ KEY_COUNT ] = { _modKeys, _modKeys + 1,
		_modScans, _modScans + 1 } ;
void mcr_DispatchKey_init ( mcr_Dispatch * dispPt )
{
	dassert ( dispPt ) ;
	for ( int i = 0 ; i < KEY_COUNT ; i++ )
	{
		mcr_Map_init ( _keyMaps [ i ], sizeof ( unsigned int ),
				sizeof ( mcr_Map ) ) ;
		_keyMaps [ i ]->compare = mcr_unsigned_compare ;
	}

	mcr_Dispatch_init_with ( dispPt,
			mcr_DispatchKey_add_specific,
			mcr_DispatchKey_clear,
			mcr_DispatchKey_dispatch_specific,
			mcr_Key_modify,
			mcr_DispatchKey_remove_specific ) ;
}

void mcr_DispatchKey_free ( )
{
	for ( int i = 0 ; i < KEY_COUNT ; i++ )
	{
		map_map_free ( _keyMaps [ i ] ) ;
	}
}

// Modifier to mcr_Hot *
static mcr_Map _cursorModMaps [ 2 ] ;
void mcr_DispatchMoveCursor_init ( mcr_Dispatch * dispPt )
{
	dassert ( dispPt ) ;
	mcr_Map_init ( _cursorModMaps, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
	mcr_Map_init ( _cursorModMaps + 1, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
	_cursorModMaps [ 0 ].compare = _cursorModMaps [ 1 ].compare =
			mcr_unsigned_compare ;

	mcr_Dispatch_init_with ( dispPt,
			mcr_DispatchMoveCursor_add_specific,
			mcr_DispatchMoveCursor_clear,
			mcr_DispatchMoveCursor_dispatch_specific, NULL,
			mcr_DispatchMoveCursor_remove_specific ) ;
}

void mcr_DispatchMoveCursor_free ( )
{
	map_arr_free_redirect ( _cursorModMaps ) ;
	map_arr_free_redirect ( _cursorModMaps + 1 ) ;
}

// mods to mcr_Hot *
static mcr_Map _noopModMap ;
void mcr_DispatchNoOp_init ( mcr_Dispatch * dispPt )
{
	dassert ( dispPt ) ;
	mcr_Map_init ( & _noopModMap, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
	_noopModMap.compare = mcr_unsigned_compare ;

	mcr_Dispatch_init_with ( dispPt,
			mcr_DispatchNoOp_add_specific, mcr_DispatchNoOp_clear,
			mcr_DispatchNoOp_dispatch_specific, NULL,
			mcr_DispatchNoOp_remove_specific ) ;
}

void mcr_DispatchNoOp_free ( )
{
	map_arr_free_redirect ( & _noopModMap ) ;
}

// mods to mcr_Hot *
static mcr_Map _scrollModMap ;
void mcr_DispatchScroll_init ( mcr_Dispatch * dispPt )
{
	dassert ( dispPt ) ;
	mcr_Map_init ( & _scrollModMap, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
	_scrollModMap.compare = mcr_unsigned_compare ;

	mcr_Dispatch_init_with ( dispPt,
			mcr_DispatchScroll_add_specific,
			mcr_DispatchScroll_clear,
			mcr_DispatchScroll_dispatch_specific, NULL,
			mcr_DispatchScroll_remove_specific ) ;
}

void mcr_DispatchScroll_free ( )
{
	map_arr_free_redirect ( & _scrollModMap ) ;
}

// mcr_Mods to mods to mcr_Hot *
static mcr_Map _modModMap ;
void mcr_DispatchMod_init ( mcr_Dispatch * dispPt )
{
	dassert ( dispPt ) ;
	mcr_Map_init ( & _modModMap, sizeof ( mcr_Mod ),
			sizeof ( mcr_Map ) ) ;
	_modModMap.compare = mcr_Mod_compare ;

	mcr_Dispatch_init_with ( dispPt,
			mcr_DispatchMod_add_specific,
			mcr_DispatchMod_clear,
			mcr_DispatchMod_dispatch_specific, NULL,
			mcr_DispatchMod_remove_specific ) ;
}

void mcr_DispatchMod_free ( )
{
	map_map_free ( & _modModMap ) ;
}

//
// Interface functions for specific dispatchers
//
// Alarm
void mcr_DispatchAlarm_add_specific ( mcr_Signal * signalPt,
		unsigned int mods, mcr_Hot * newHotkey )
{
	dassert ( signalPt ) ;
	dassert ( signalPt->type == & mcr_iAlarm ) ;
	dassert ( newHotkey ) ;
	if ( mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add_unspecific ( mcr_Dispatch_get (
				signalPt->type->iface.id ), newHotkey ) ;
		return ;
	}
	add_mapped_array_sorted ( & _alarmModMap, & mods, & newHotkey,
			& _hotsInitial, mcr_ref_compare ) ;
}

int mcr_DispatchAlarm_dispatch_specific ( mcr_Signal * signalPt,
		unsigned int mods )
{
	dassert ( signalPt ) ;
	int block = 0 ;
	// If mods are MCR_ANY_MOD, then this should be triggered by generics.
	if ( mods != MCR_ANY_MOD )
	{
		TRIGGER_MAPPED_ARRAY ( & _alarmModMap, & mods, signalPt, mods,
				block ) ;
	}
	return block ;
}

void mcr_DispatchAlarm_remove_specific ( mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	if ( _alarmModMap.set.used )
	{
		map_arr_remove_sorted ( & _alarmModMap, & delHotkey,
				mcr_ref_compare ) ;
	}
}

void mcr_DispatchAlarm_clear ( )
{
	mcr_DispatchAlarm_free ( ) ;
	mcr_Map_init ( & _alarmModMap, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
}

// Echo
void mcr_DispatchHIDEcho_add_specific ( mcr_Signal * signalPt,
		unsigned int mods, mcr_Hot * newHotkey )
{
	dassert ( newHotkey ) ;
	int echoKey = signalPt && signalPt->data.data ?
			MCR_ECHO_GET ( ( mcr_HIDEcho * ) ( signalPt->data.data ) ) :
			( int ) MCR_ANY_MOD ;
	if ( ( unsigned int ) echoKey == MCR_ANY_MOD && mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add_unspecific ( mcr_Dispatch_get (
				mcr_iHIDEcho.iface.id ), newHotkey ) ;
	}
	else
	{
		add_mapped_map_sorted ( & _modEcho, & mods, & echoKey,
				& newHotkey, & _intsMapInitial, & _hotsInitial,
				mcr_ref_compare ) ;
	}
}

int mcr_DispatchHIDEcho_dispatch_specific ( mcr_Signal * signalPt,
		unsigned int mods )
{
	dassert ( signalPt ) ;
	// most specific, both mod and echo
	int block = 0 ;
	int echoKey ;
	if ( signalPt->data.data )
		echoKey = MCR_ECHO_GET (
				( mcr_HIDEcho * ) signalPt->data.data ) ;
	else
		echoKey = MCR_ANY_MOD ;
	// Any mod delivered below.
	if ( mods != MCR_ANY_MOD )
	{
		// Trigger Map to map.
		TRIGGER_MAPPED_MAPS ( & _modEcho, & mods,
				& echoKey, & MCR_ANY_MOD, signalPt, mods, block ) ;
	}
	// Any mod + any event should be added to generics.
	if ( ( unsigned int ) echoKey != MCR_ANY_MOD )
	{
		TRIGGER_MAPPED_MAP ( & _modEcho, & MCR_ANY_MOD,
				& echoKey, signalPt, mods, block ) ;
	}
	return block ;
}

void mcr_DispatchHIDEcho_remove_specific ( mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	if ( _modEcho.set.used )
	{
		map_map_remove_sorted ( & _modEcho, & delHotkey,
				mcr_ref_compare ) ;
	}
}

void mcr_DispatchHIDEcho_clear ( )
{
	mcr_DispatchHIDEcho_free ( ) ;
	mcr_Map_init ( & _modEcho, sizeof ( unsigned int ),
			sizeof ( mcr_Map ) ) ;
}

// Key
void mcr_DispatchKey_add_specific ( mcr_Signal * signalPt,
		unsigned int mods, mcr_Hot * newHotkey )
{
	dassert ( signalPt ) ;
	dassert ( newHotkey ) ;
	int key, scan ;
	mcr_KeyUpType type ;
	// Set mapping keys. If not available, trigger for any.
	if ( signalPt->data.data )
	{
		mcr_Key * data = signalPt->data.data ;
		key = MCR_KEY_GET ( data ) ;
		scan = MCR_KEY_GET_SCAN ( data ) ;
		type = MCR_KEY_GET_UP_TYPE ( data ) ;
	}
	else
	{
		key = scan = MCR_ANY_KEY ;
		type = MCR_BOTH ;
	}
	// If everything is any trigger, add to the any trigger.
	if ( key == MCR_ANY_KEY && scan == MCR_ANY_KEY &&
		mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add_unspecific ( mcr_Dispatch_get (
				signalPt->type->iface.id ), newHotkey ) ;
	}
	// Trigger for some specific is assured.
	else
	{
		// Key may be any key, but adding to scan will not be any key.
		// Adding to key down trigger.
		if ( type != MCR_UP )
		{
			if ( key != MCR_ANY_KEY || scan == MCR_ANY_KEY )
			{
				add_mapped_map_sorted ( _modKeys, & mods, & key,
						& newHotkey, & _intsMapInitial, & _hotsInitial,
						mcr_ref_compare ) ;
			}
			if ( scan != MCR_ANY_KEY )
			{
				add_mapped_map_sorted ( _modScans, & mods, & scan,
						& newHotkey, & _intsMapInitial, & _hotsInitial,
						mcr_ref_compare ) ;
			}
		}
		// Adding to key up trigger.
		if ( type != MCR_DOWN )
		{
			if ( key != MCR_ANY_KEY || scan == MCR_ANY_KEY )
			{
				add_mapped_map_sorted ( _modKeys + 1, & mods, & key,
						& newHotkey, & _intsMapInitial, & _hotsInitial,
						mcr_ref_compare ) ;
			}
			if ( scan != MCR_ANY_KEY )
			{
				add_mapped_map_sorted ( _modScans + 1, & mods, & scan,
						& newHotkey, & _intsMapInitial, & _hotsInitial,
						mcr_ref_compare ) ;
			}
		}
	}
}

// Will not trigger MCR_ANY_MOD, but will trigger MCR_ANY_KEY.
# define KeyTriggerModifier(keyKey,scanKey,type,signalPt,mods,block) \
	if ( ( mods ) != MCR_ANY_MOD ) \
	{ \
		if ( ( type ) != MCR_UP ) \
		{ \
			TRIGGER_MAPPED_MAPS ( _modKeys, & mods, \
					& keyKey, & MCR_ANY_KEY, signalPt, mods, block ) ; \
			TRIGGER_MAPPED_MAPS ( _modScans, & mods, \
					& scanKey, & MCR_ANY_KEY, signalPt, mods, block ) ; \
		} \
		if ( ( type ) != MCR_DOWN ) \
		{ \
			TRIGGER_MAPPED_MAPS ( _modKeys+1, & mods, \
					& keyKey, & MCR_ANY_KEY, signalPt, mods, block ) ; \
			TRIGGER_MAPPED_MAPS ( _modScans+1, & mods, \
					& scanKey, & MCR_ANY_KEY, signalPt, mods, block ) ; \
		} \
	}

// Will trigger MCR_ANY_MOD, but will not trigger MCR_ANY_KEY.
# define KeyTriggerAnyMod(keyKey,scanKey,type,signalPt,mods,block) \
	if ( keyKey != MCR_ANY_KEY ) \
	{ \
		if ( type != MCR_UP ) \
		{ \
			TRIGGER_MAPPED_MAP ( _modKeys, & MCR_ANY_MOD, & keyKey, \
					signalPt, mods, block ) ; \
		} \
		if ( type != MCR_DOWN ) \
		{ \
			TRIGGER_MAPPED_MAP ( _modKeys + 1, & MCR_ANY_MOD, & keyKey, \
					signalPt, mods, block ) ; \
		} \
	} \
	if ( scanKey != MCR_ANY_KEY ) \
	{ \
		if ( type != MCR_UP ) \
		{ \
			TRIGGER_MAPPED_MAP ( _modScans, & MCR_ANY_MOD, & scanKey, \
					signalPt, mods, block ) ; \
		} \
		if ( type != MCR_DOWN ) \
		{ \
			TRIGGER_MAPPED_MAP ( _modScans + 1, & MCR_ANY_MOD, & scanKey, \
					signalPt, mods, block ) ; \
		} \
	}

int mcr_DispatchKey_dispatch_specific ( mcr_Signal * signalPt,
		unsigned int mods )
{
	dassert ( signalPt ) ;
	int block = 0 ;
	int key, scan ;
	mcr_KeyUpType type ;
	if ( signalPt->data.data )
	{
		mcr_Key * data = signalPt->data.data ;
		key = MCR_KEY_GET ( data ) ;
		scan = MCR_KEY_GET_SCAN ( data ) ;
		type = MCR_KEY_GET_UP_TYPE ( data ) ;
	}
	else
	{
		key = scan = MCR_ANY_KEY ;
		type = MCR_BOTH ;
	}
	// Will not trigger MCR_ANY_MOD, but will trigger MCR_ANY_KEY.
	KeyTriggerModifier ( key, scan, type, signalPt, mods, block ) ;
	// Will trigger MCR_ANY_MOD, but will not trigger MCR_ANY_KEY.
	KeyTriggerAnyMod ( key, scan, type, signalPt, mods, block ) ;
	return block ;
}

void mcr_DispatchKey_remove_specific ( mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	for ( int i = 0 ; i < KEY_COUNT ; i ++ )
	{
		if ( _keyMaps [ i ]->set.array )
		{
			map_map_remove_sorted ( _keyMaps [ i ], & delHotkey,
					mcr_ref_compare ) ;
		}
	}
}

void mcr_DispatchKey_clear ( )
{
	mcr_DispatchKey_free ( ) ;
	for ( int i = 0 ; i < KEY_COUNT ; i++ )
	{
		mcr_Map_init ( _keyMaps [ i ], sizeof ( unsigned int ),
				sizeof ( mcr_Map ) ) ;
	}
}

// MoveCursor
void mcr_DispatchMoveCursor_add_specific ( mcr_Signal * signalPt,
		unsigned int mods, mcr_Hot * newHotkey )
{
	dassert ( signalPt ) ;
	dassert ( newHotkey ) ;
	int justify = signalPt->data.data ?
			MCR_MOVECURSOR_IS_JUSTIFY (
			( mcr_MoveCursor * ) signalPt->data.data ) : -1 ;
	if ( mods == MCR_ANY_MOD && justify == -1 )
	{
		mcr_Dispatch_add_unspecific ( mcr_Dispatch_get (
				signalPt->type->iface.id ), newHotkey ) ;
		return ;
	}
	if ( justify == -1 )
	{
		add_mapped_array_sorted ( _cursorModMaps, & mods, & newHotkey,
				& _hotsInitial, mcr_ref_compare ) ;
		add_mapped_array_sorted ( _cursorModMaps + 1, & mods, & newHotkey,
				& _hotsInitial, mcr_ref_compare ) ;
	}
	else
	{
		justify &= 0x01 ;
		// Justify index is only 0 or 1.
		add_mapped_array_sorted ( _cursorModMaps + justify,
				& mods, & newHotkey, & _hotsInitial, mcr_ref_compare ) ;
	}
}

int mcr_DispatchMoveCursor_dispatch_specific ( mcr_Signal * signalPt,
		unsigned int mods )
{
	dassert ( signalPt ) ;
	int justify = signalPt->data.data ?
			MCR_MOVECURSOR_IS_JUSTIFY (
			( mcr_MoveCursor * ) signalPt->data.data ) : -1 ;
	int block = 0 ;
	// Any mod is dispatched in generics.
	if ( mods != MCR_ANY_MOD )
	{
		if ( justify == -1 )
		{
			TRIGGER_MAPPED_ARRAY ( _cursorModMaps, & mods, signalPt,
					mods, block ) ;
			TRIGGER_MAPPED_ARRAY ( _cursorModMaps, & MCR_ANY_MOD,
					signalPt, mods, block ) ;
			TRIGGER_MAPPED_ARRAY ( _cursorModMaps + 1, & mods, signalPt,
					mods, block ) ;
			TRIGGER_MAPPED_ARRAY ( _cursorModMaps + 1, & MCR_ANY_MOD,
					signalPt, mods, block ) ;
		}
		else
		{
			justify &= 0x01 ;
			TRIGGER_MAPPED_ARRAY ( _cursorModMaps + justify,
					& mods, signalPt, mods, block ) ;
			TRIGGER_MAPPED_ARRAY ( _cursorModMaps + justify,
					& MCR_ANY_MOD, signalPt, mods, block ) ;
		}
	}
	return block ;
}

void mcr_DispatchMoveCursor_remove_specific ( mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	map_arr_remove_sorted ( _cursorModMaps, & delHotkey,
			mcr_ref_compare ) ;
	map_arr_remove_sorted ( _cursorModMaps + 1, & delHotkey,
			mcr_ref_compare ) ;
}

void mcr_DispatchMoveCursor_clear ( )
{
	mcr_DispatchMoveCursor_free ( ) ;
	mcr_Map_init ( _cursorModMaps, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
	mcr_Map_init ( _cursorModMaps + 1, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
}

// NoOp
void mcr_DispatchNoOp_add_specific ( mcr_Signal * signalPt,
		unsigned int mods, mcr_Hot * newHotkey )
{
	dassert ( signalPt ) ;
	if ( mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add_unspecific ( mcr_Dispatch_get (
				signalPt->type->iface.id ), newHotkey ) ;
		return ;
	}
	add_mapped_array_sorted ( & _noopModMap, & mods, & newHotkey,
					& _hotsInitial, mcr_ref_compare ) ;
}

int mcr_DispatchNoOp_dispatch_specific ( mcr_Signal * signalPt,
		unsigned int mods )
{
	dassert ( signalPt ) ;
	int block = 0 ;
	// If mods are MCR_ANY_MOD, then this should be triggered by generics.
	if ( mods != MCR_ANY_MOD )
	{
		TRIGGER_MAPPED_ARRAY ( & _noopModMap, & mods, signalPt, mods,
				block ) ;
	}
	return block ;
}

void mcr_DispatchNoOp_remove_specific ( mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	if ( _noopModMap.set.used )
	{
		map_arr_remove_sorted ( & _noopModMap, & delHotkey,
				mcr_ref_compare ) ;
	}
}

void mcr_DispatchNoOp_clear ( )
{
	mcr_DispatchNoOp_free ( ) ;
	mcr_Map_init ( & _noopModMap, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
}

// Scroll
void mcr_DispatchScroll_add_specific ( mcr_Signal * signalPt,
		unsigned int mods, mcr_Hot * newHotkey )
{
	dassert ( signalPt ) ;
	dassert ( newHotkey ) ;
	if ( mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add_unspecific ( mcr_Dispatch_get (
				signalPt->type->iface.id ), newHotkey ) ;
		return ;
	}
	add_mapped_array_sorted ( & _scrollModMap, & mods, & newHotkey,
					& _hotsInitial, mcr_ref_compare ) ;
}

int mcr_DispatchScroll_dispatch_specific ( mcr_Signal * signalPt,
		unsigned int mods )
{
	dassert ( signalPt ) ;
	int block = 0 ;
	// If mods are MCR_ANY_MOD, then this should be triggered by generics.
	if ( mods != MCR_ANY_MOD )
	{
		TRIGGER_MAPPED_ARRAY ( & _scrollModMap, & mods, signalPt, mods,
				block ) ;
	}
	return block ;
}

void mcr_DispatchScroll_remove_specific ( mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	if ( _scrollModMap.set.used )
	{
		map_arr_remove_sorted ( & _scrollModMap, & delHotkey,
				mcr_ref_compare ) ;
	}
}

void mcr_DispatchScroll_clear ( )
{
	mcr_DispatchScroll_free ( ) ;
	mcr_Map_init ( & _scrollModMap, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
}

void mcr_DispatchMod_add_specific ( mcr_Signal * signalPt,
		unsigned int mods, mcr_Hot * newHotkey )
{
	dassert ( signalPt ) ;
	dassert ( newHotkey ) ;
	mcr_Mod anyMod, mod ;
	anyMod.modifiers = MCR_ANY_MOD ;
	anyMod.up_type = MCR_BOTH ;
	if ( signalPt->data.data )
		memcpy ( & mod, signalPt->data.data, sizeof ( mcr_Mod ) ) ;
	else
		memcpy ( & mod, & anyMod, sizeof ( mcr_Mod ) ) ;
	if ( ! memcmp ( & mod, & anyMod, sizeof ( mcr_Mod ) ) &&
			mods == MCR_ANY_MOD )
	{
		mcr_Dispatch_add_unspecific ( mcr_Dispatch_get (
				signalPt->type->iface.id ), newHotkey ) ;
	}
	add_mapped_map_sorted ( & _modModMap, & mod, & mods, & newHotkey,
			& _intsMapInitial, & _hotsInitial, mcr_ref_compare ) ;
}

int mcr_DispatchMod_dispatch_specific ( mcr_Signal * signalPt,
		unsigned int mods )
{
	dassert ( signalPt ) ;
	int block = 0 ;
	mcr_Mod anyMod, mod ;
	anyMod.modifiers = MCR_ANY_MOD ;
	anyMod.up_type = MCR_BOTH ;
	if ( signalPt->data.data )
		memcpy ( & mod, signalPt->data.data, sizeof ( mcr_Mod ) ) ;
	else
		memcpy ( & mod, & anyMod, sizeof ( mcr_Mod ) ) ;
	if ( memcmp ( & mod, & anyMod, sizeof ( mcr_Mod ) ) )
	{
		TRIGGER_MAPPED_MAPS ( & _modModMap, & mod, & mods,
				& MCR_ANY_MOD, signalPt, mods, block ) ;
	}
	if ( mods != MCR_ANY_MOD )
	{
		TRIGGER_MAPPED_MAP ( & _modModMap, & anyMod, & mods,
				signalPt, mods, block ) ;
	}
	return block ;
}

void mcr_DispatchMod_remove_specific ( mcr_Hot * delHotkey )
{
	dassert ( delHotkey ) ;
	if ( _modModMap.set.used )
	{
		map_map_remove_sorted ( & _modModMap, & delHotkey,
				mcr_ref_compare ) ;
	}
}

void mcr_DispatchMod_clear ( )
{
	mcr_DispatchMod_free ( ) ;
	mcr_Map_init ( & _modModMap, sizeof ( unsigned int ),
			sizeof ( mcr_Map ) ) ;
}


//
// Library initializer and cleanup.
//
void mcr_dispatchstandard_initialize ( )
{
	mcr_Array_init ( & _hotsInitial, sizeof ( mcr_Hot * ) ) ;
	mcr_Map_init ( & _intsMapInitial, sizeof ( int ),
			sizeof ( mcr_Array ) ) ;
	_intsMapInitial.compare = mcr_int_compare ;

	mcr_Dispatch disp ;
	typedef void ( * initfnc ) ( mcr_Dispatch * ) ;
	initfnc initFncs [ ] = {
		mcr_DispatchAlarm_init, mcr_DispatchHIDEcho_init,
		mcr_DispatchKey_init, mcr_DispatchMoveCursor_init,
		mcr_DispatchNoOp_init, mcr_DispatchScroll_init,
		mcr_DispatchMod_init
	} ;
	size_t ids [ ] = {
		mcr_iAlarm.iface.id, mcr_iHIDEcho.iface.id,
		mcr_iKey.iface.id, mcr_iMoveCursor.iface.id,
		mcr_iNoOp.iface.id, mcr_iScroll.iface.id,
		mcr_iMod.iface.id
	} ;
	int length = sizeof ( ids ) / sizeof ( size_t ) ;
	for ( int i = 0 ; i < length ; i++ )
	{
		initFncs [ i ] ( & disp ) ;
		mcr_Dispatch_register ( & disp, ids [ i ] ) ;
	}
}

void mcr_dispatchstandard_cleanup ( void )
{
	mcr_DispatchAlarm_free ( ) ;
	mcr_DispatchHIDEcho_free ( ) ;
	mcr_DispatchKey_free ( ) ;
	mcr_DispatchMoveCursor_free ( ) ;
	mcr_DispatchNoOp_free ( ) ;
	mcr_DispatchScroll_free ( ) ;
	mcr_DispatchMod_free ( ) ;
	// get_ensured maps should not need to be freed.
}
