/* hotkey/mods.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "hotkey/mods.h"

MCR_API mcr_ISignal mcr_iMod ;

static mcr_Mod _defaultMod ;

void mcr_Mods_load_contract ( )
{
	const unsigned int mods [ ] =
	{
		MCR_ALT, MCR_ALTGR, MCR_AMIGA, MCR_CMD,
		MCR_CODE, MCR_COMPOSE, MCR_CTRL, MCR_FN, MCR_FRONT,
		MCR_GRAPH, MCR_HYPER, MCR_META, MCR_SHIFT, MCR_SUPER,
		MCR_SYMBOL, MCR_TOP, MCR_WIN
	} ;
	const char * modNames [ ] =
	{
		"Alt", "AltGr", "Amiga", "Cmd",
		"Code", "Compose", "Ctrl", "Fn", "Front",
		"Graph", "Hyper", "Meta", "Shift", "Super",
		"Symbol", "Top", "Win"
	} ;
	const char * addNames [ ] [ 2 ] =
	{
		{"Option", ""}, {"alt gr", "alt_gr"}, {"", ""}, {"Command", ""},
		{"", ""}, {"", ""}, {"Control", ""}, {"", ""}, {"", ""},
		{"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""},
		{"", ""}, {"", ""}, {"", ""}
	} ;
	size_t addLens [17] =
	{
		1, 2, 0, 1,
		0, 0, 1, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0
	} ;
	int len = sizeof ( mods ) / sizeof ( int ) ;
	for ( int i = 0 ; i < len ; i ++ )
	{
		mcr_Mod_set_names ( mods [ i ], modNames [ i ], addNames [ i ],
				addLens [ i ] ) ;
	}
	mcr_ISignal_set_name ( & mcr_iMod, "Mods" ) ;
	mcr_ISignal_add_name ( & mcr_iMod, "Mod" ) ;
	mcr_Mods_load_key_contract ( ) ;
}

static mcr_Map _modToName ;
static mcr_Map _nameToMod ;

static void names_init ( )
{
	mcr_Map_init ( & _modToName, sizeof ( unsigned int ),
			sizeof ( mcr_Array ) ) ;
	_modToName.compare = mcr_unsigned_compare ;
	mcr_StringMap_init ( & _nameToMod, sizeof ( unsigned int ) ) ;
}

static mcr_Map _echoToMod ;
static mcr_Map _modToEcho ;

static void echo_init ( )
{
	mcr_Map_init ( & _echoToMod, sizeof ( int ),
			sizeof ( mcr_Mod ) ) ;
	_echoToMod.compare = mcr_int_compare ;
	mcr_Map_init ( & _modToEcho, sizeof ( mcr_Mod ), sizeof ( int ) ) ;
	_modToEcho.compare = mcr_Mod_compare ;
}

static mcr_Map _keyToMod ;
static mcr_Map _modToKey ;

static void key_init ( )
{
	mcr_Map_init ( & _keyToMod, sizeof ( int ),
			sizeof ( unsigned int ) ) ;
	_keyToMod.compare = mcr_int_compare ;
	mcr_Map_init ( & _modToKey, sizeof ( unsigned int ),
			sizeof ( int ) ) ;
	_modToKey.compare = mcr_unsigned_compare ;
}

int mcr_Mod_send ( mcr_Signal * sigPt )
{
//	int success = 1 ;
	MCR_MOD_SEND ( ( mcr_Mod * ) sigPt->data.data, 0 ) ;
	return 1 ;
}

// Names
unsigned int mcr_Mod_get ( const char * name )
{
	if ( ! name )
		return MCR_ANY_MOD ;
	unsigned int * found = MCR_STRINGMAP_GET_VALUE ( & _nameToMod, name ) ;
	return found ? * found : MCR_ANY_MOD ;
}

const char * mcr_Mod_get_name ( const unsigned int modifier )
{
	const char ** found = MCR_MAP_GET_VALUE ( & _modToName,
			& modifier ) ;
	return found ? * found : NULL ;
}

size_t mcr_Mod_count ( )
{
	return _modToName.set.used ;
}

void mcr_Mod_get_all ( unsigned int * modBuffer,
		const size_t bufferLength )
{
	if ( ! modBuffer || bufferLength == ( size_t ) -1 )
		return ;
	for ( size_t i = 0 ; i < _modToName.set.used && i < bufferLength ;
			i ++ )
	{
		modBuffer [ i ] = * ( unsigned int * ) MCR_ARR_AT (
				& _modToName.set, i ) ;
	}
}

// Echo
mcr_Mod mcr_Mod_echo_get ( const int echo )
{
	mcr_Mod * found = MCR_MAP_GET_VALUE ( & _echoToMod, & echo ) ;
	return found ? * found : _defaultMod ;
}

int mcr_Mod_echo_get_echo ( const mcr_Mod modifier )
{
	int * found = MCR_MAP_GET_VALUE ( & _modToEcho, & modifier ) ;
	return found ? * found : ( int ) MCR_ANY_MOD ;
}

size_t mcr_Mod_echo_count ( )
{
	return _echoToMod.set.used ;
}

//unsigned int mcr_Mod_echo_modify ( unsigned int mods, const int echo )
//{
//	for ( size_t i = 0 ; i < _echoToMod.set.used ; i ++ )
//	{
//		int * echoPt = MCR_ARR_AT ( & _echoToMod.set, i ) ;
//		mcr_Mod * modPt = MCR_MAP_VALUE ( & _echoToMod, echoPt ) ;
//		if ( ( echo & * echoPt ) == * echoPt )
//		{
//			MCR_MOD_MODIFY_BITS ( mods, modPt->modifiers, modPt->up_type ) ;
//		}
//	}
//	return mods ;
//}

void mcr_Mod_echo_get_all ( int * echoBuffer,
		const size_t bufferLength )
{
	if ( ! echoBuffer || bufferLength == ( size_t ) -1 )
		return ;
	for ( size_t i = 0 ; i < _echoToMod.set.used && i < bufferLength ;
			i ++ )
	{
		echoBuffer [ i ] = * ( int * ) MCR_ARR_AT (
				& _echoToMod.set, i ) ;
	}
}

// Key
unsigned int mcr_Mod_key_get ( const int key )
{
	unsigned int * found = MCR_MAP_GET_VALUE ( & _keyToMod, & key ) ;
	return found ? * found : MCR_ANY_MOD ;
}

int mcr_Mod_key_get_key ( const unsigned int modifier )
{
	int * found = MCR_MAP_GET_VALUE ( & _modToKey, & modifier ) ;
	return found ? * found : MCR_ANY_KEY ;
}

size_t mcr_Mod_key_count ( )
{
	return _keyToMod.set.used ;
}

void mcr_Mod_key_get_all ( int * keyBuffer,
		const size_t bufferLength )
{
	if ( ! keyBuffer || bufferLength == ( size_t ) -1 )
		return ;
	for ( size_t i = 0 ; i < _keyToMod.set.used && i < bufferLength ;
			i ++ )
	{
		keyBuffer [ i ] = * ( int * ) MCR_ARR_AT (
				& _keyToMod.set, i ) ;
	}
}

int mcr_Mod_compare ( const void * lhs, const void * rhs )
{
	return memcmp ( lhs, rhs, sizeof ( mcr_Mod ) ) ;
}


//
// Modify modifiers
//
unsigned int mcr_Mod_combine ( const unsigned int * modsArray,
		const size_t length )
{
	if ( ! modsArray || length == ( size_t ) -1 )
		return MCR_ANY_MOD ;
	unsigned int ret = 0 ;
	for ( size_t i = 0 ; i < length ; i ++ )
	{
		ret |= modsArray [ i ] ;
	}
	return ret ;
}

void mcr_Mod_modify ( mcr_Mod * modPt,
		const unsigned int modifier, const mcr_KeyUpType modifierKeyUp )
{
	if ( ! modPt ) return ;
	// If same side of the MCR_DOWN, then adding.
	if ( ( modPt->up_type == MCR_DOWN ) ==
			( modifierKeyUp == MCR_DOWN ) )
	{
		MCR_MOD_ADD ( modPt->modifiers, modifier ) ;
	}
	else
	{
		MCR_MOD_REMOVE ( modPt->modifiers, modifier ) ;
	}
}

unsigned int mcr_Mod_modify_bits ( const unsigned int mods,
		const unsigned int modifier, const mcr_KeyUpType keyUp )
{
	if ( keyUp == MCR_DOWN )
		return mods | modifier ;
	return mods & ( ~ modifier ) ;
}

unsigned int mcr_Mod_add ( const unsigned int mods,
		const unsigned int newMod )
{
	return mods | newMod ;
}

int mcr_Mod_hasMod ( const unsigned int mods,
		const unsigned int modVal )
{
	return MCR_MOD_HAS ( mods, modVal ) ;
}

unsigned int mcr_Mod_remove ( const unsigned int mods,
		const unsigned int delMod )
{
	return mods & ( ~ delMod ) ;
}

//
// Modifier providers for standard types.
//
unsigned int mcr_HIDEcho_modify ( mcr_Signal * sigPt,
		unsigned int mods )
{
	if ( ! sigPt || ! sigPt->data.data ) return mods ;
	int echo = MCR_ECHO_GET ( ( mcr_HIDEcho * ) sigPt->data.data ) ;
	mcr_Mod * found = MCR_MAP_GET_VALUE ( & _echoToMod, & echo ) ;
	if ( found )
	{
		if ( found->up_type == MCR_DOWN )
			return mods | found->modifiers ;
		return mods & ( ~ found->modifiers ) ;
	}
	return mods ;
}

unsigned int mcr_Key_modify ( mcr_Signal * sigPt,
		unsigned int mods )
{
	if ( ! sigPt || ! sigPt->data.data ) return mods ;
	int key = MCR_KEY_GET ( ( mcr_Key * ) sigPt->data.data ) ;
	unsigned int * found = MCR_MAP_GET_VALUE ( & _keyToMod, & key ) ;
	if ( found )
	{
		if ( MCR_KEY_GET_UP_TYPE ( ( mcr_Key * ) sigPt->data.data )
				== MCR_DOWN )
			return mods | * found ;
		return mods & ( ~ ( * found ) ) ;
	}
	return mods ;
}


//
// Modifier development.
//
// Names
int mcr_Mod_set_name ( const unsigned int modifier,
		const char * name )
{
	// strcpy, map mod to new arr, strcpy, map new arr to mod.
	if ( ! name ) return 0 ;
	// mod => name
	mcr_Array * arrPt = mcr_Map_get_value ( & _modToName, & modifier ) ;
	if ( arrPt )
	{	/* Rename existing. */
		if ( ! mcr_String_from_string ( arrPt, name ) )
		{
			dmsg ;
			return 0 ;
		}
	}
	else
	{	/* Map new. */
		mcr_Array arr ;
		mcr_String_init ( & arr ) ;
		if ( ! mcr_String_from_string ( & arr, name ) ||
				! mcr_Map_map ( & _modToName, & modifier, & arr ) )
		{
			mcr_Array_free ( & arr ) ;
			return 0 ;
		}
	}
	// name => mod
	return mcr_Mod_add_name ( modifier, name ) ;
}

int mcr_Mod_add_name ( const unsigned int modifier,
		const char * name )
{
	if ( ! name )
		return 0 ;
	// name => mod
	if ( ! mcr_StringMap_map ( & _nameToMod, name, & modifier ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

int mcr_Mod_add_names ( const unsigned int modifier,
		const char ** addNames, size_t bufferLen )
{
	dassert ( addNames ) ;
	int ret = 1 ;
	for ( size_t i = 0 ; i < bufferLen ; i ++ )
	{
		if ( ! mcr_Mod_add_name ( modifier, addNames [ i ] ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	return ret ;
}

int mcr_Mod_set_names ( const unsigned int modifier,
		const char * name, const char ** addNames,
		size_t bufferLen )
{
	int ret = 1 ;
	if ( ! mcr_Mod_set_name ( modifier, name ) )
	{
		dmsg ;
		ret = 0 ;
	}
	if ( ! mcr_Mod_add_names ( modifier, addNames, bufferLen ) )
	{
		dmsg ;
		ret = 0 ;
	}
	return ret ;
}

int mcr_Mod_rename ( const unsigned int modifier,
		const char * newName )
{
	mcr_Array * found = mcr_Map_get_value ( & _modToName, & modifier ) ;
	if ( found )
	{
		mcr_StringMap_unmap ( & _nameToMod, found->array ) ;
	}
	return mcr_Mod_set_name ( modifier, newName ) ;
}

int mcr_Mod_rename_by_name ( const char * oldName,
		const char * newName )
{
	if ( ! oldName ) return 0 ;
	unsigned int * modPt = MCR_STRINGMAP_GET_VALUE ( & _nameToMod, oldName ) ;
	if ( modPt )
		return mcr_Mod_set_name ( * modPt, newName ) ;
	return 0 ;
}

void mcr_Mod_clear_names ( )
{
	MCR_MAP_FOR_EACH_VALUE ( & _modToName, mcr_Array_free_foreach, 0 ) ;
	MCR_MAP_FOR_EACH ( & _nameToMod, mcr_Array_free_foreach, 0 ) ;
	mcr_Map_free ( & _modToName ) ;
	mcr_Map_free ( & _nameToMod ) ;
}

// Echo
int mcr_Mod_set_echo ( const mcr_Mod modifiers,
		const int echo )
{
	int ret = mcr_Map_map ( & _echoToMod, & echo, & modifiers ) ;
	return ret && mcr_Map_map ( & _modToEcho, & modifiers, & echo ) ;
}

int mcr_Mod_add_echo ( const mcr_Mod modifiers,
		const int echo )
{
	return mcr_Map_map ( & _echoToMod, & echo, & modifiers ) ;
}

int mcr_Mod_add_echos ( const mcr_Mod modifiers,
		const int * addEchos, size_t bufferLen )
{
	dassert ( addEchos ) ;
	int ret = 1 ;
	for ( size_t i = 0 ; i < bufferLen ; i ++ )
	{
		if ( ! mcr_Mod_add_echo ( modifiers, addEchos [ i ] ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	return ret ;
}

int mcr_Mod_set_echos ( const mcr_Mod modifiers,
		const int echo, const int * addEchos, size_t bufferLen )
{
	int ret = 1 ;
	if ( ! mcr_Mod_set_echo ( modifiers, echo ) )
	{
		dmsg ;
		ret = 0 ;
	}
	if ( ! mcr_Mod_add_echos ( modifiers, addEchos, bufferLen ) )
	{
		dmsg ;
		ret = 0 ;
	}
	return ret ;
}

int mcr_Mod_reecho ( const mcr_Mod modifiers,
		const int echo )
{
	int * found = MCR_MAP_GET_VALUE ( & _modToEcho, & modifiers ) ;
	if ( found )
	{
		// Map modifiers from new echo.
		if ( ! mcr_Map_remap ( & _echoToMod, found, & echo ) )
		{
			dmsg ;
			return 0 ;
		}
		// Map modifiers to new echo.
		return mcr_Map_map ( & _modToEcho, & modifiers, & echo ) ;
	}
	return 0 ;
}

int mcr_Mod_reecho_by_echo ( const int oldEcho,
		const int newEcho )
{
	mcr_Mod * modPt = MCR_MAP_GET_VALUE ( & _echoToMod, & oldEcho ) ;
	if ( modPt )
	{
		return mcr_Mod_reecho ( * modPt, newEcho ) ;
	}
	return 0 ;
}

void mcr_Mod_clear_echo ( )
{
	mcr_Map_free ( & _modToEcho ) ;
	mcr_Map_free ( & _echoToMod ) ;
}

// Key
int mcr_Mod_set_key ( const unsigned int modifiers,
		const int key )
{
	int ret = mcr_Map_map ( & _keyToMod, & key, & modifiers ) ;
	return ret && mcr_Map_map ( & _modToKey, & modifiers, & key ) ;
}

int mcr_Mod_add_key ( const unsigned int modifiers,
		const int key )
{
	return mcr_Map_map ( & _keyToMod, & key, & modifiers ) ;
}

int mcr_Mod_add_keys ( const unsigned int modifiers,
		const int * addKeys, size_t bufferLen )
{
	dassert ( addKeys ) ;
	int ret = 1 ;
	for ( size_t i = 0 ; i < bufferLen ; i ++ )
	{
		if ( ! mcr_Mod_add_key ( modifiers, addKeys [ i ] ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	return ret ;
}

int mcr_Mod_set_keys ( const unsigned int modifiers,
		const int key, const int * addKeys, size_t bufferLen )
{
	int ret = 1 ;
	if ( ! mcr_Mod_set_key ( modifiers, key ) )
	{
		dmsg ;
		ret = 0 ;
	}
	if ( ! mcr_Mod_add_keys ( modifiers, addKeys, bufferLen ) )
	{
		dmsg ;
		ret = 0 ;
	}
	return ret ;
}

int mcr_Mod_rekey ( const unsigned int modifiers,
		const int key )
{
	int * found = MCR_MAP_GET_VALUE ( & _modToKey, & modifiers ) ;
	if ( found )
	{
		if ( ! mcr_Map_remap ( & _keyToMod, found, & key ) )
		{
			dmsg ;
			return 0 ;
		}
		return mcr_Map_map ( & _modToKey, & modifiers, & key ) ;
	}
	return 0 ;
}

int mcr_Mod_rekey_by_key ( const int oldKey,
		const int newKey )
{
	unsigned int * modPt = MCR_MAP_GET_VALUE ( & _keyToMod, & oldKey ) ;
	if ( modPt )
	{
		return mcr_Mod_rekey ( * modPt, newKey ) ;
	}
	return 0 ;
}

void mcr_Mod_clear_key ( )
{
	mcr_Map_free ( & _modToKey ) ;
	mcr_Map_free ( & _keyToMod ) ;
}

// All
void mcr_Mod_clear_all ( )
{
	mcr_Mod_clear_names ( ) ;
	mcr_Mod_clear_echo ( ) ;
	mcr_Mod_clear_key ( ) ;
}

void mcr_mods_initialize ( )
{
	mcr_ISignal_init ( & mcr_iMod, mcr_Mod_send,
			sizeof ( mcr_Mod ) ) ;
	if ( mcr_ISignal_register ( & mcr_iMod ) == ( size_t ) -1 )
	{
		dmsg ;
	}
	names_init ( ) ;
	echo_init ( ) ;
	key_init ( ) ;
	_defaultMod.modifiers = MCR_ANY_MOD ;
	_defaultMod.up_type = MCR_BOTH ;
}

void mcr_mods_cleanup ( )
{
	mcr_Mod_clear_all ( ) ;
}
