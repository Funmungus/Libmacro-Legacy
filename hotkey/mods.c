
# include "hotkey/mods.h"

MCR_API mcr_ISignal mcr_IMod ;

static mcr_Mod defaultMod ;

static mcr_Map _modToName ;
static mcr_Map _nameToMod ;

static void names_init ( )
{
	mcr_Map_init ( & _modToName, sizeof ( unsigned int ),
			sizeof ( const char * ) ) ;
	_modToName.compare = mcr_unsigned_compare ;
	mcr_Map_init ( & _nameToMod, sizeof ( const char * ),
			sizeof ( unsigned int ) ) ;
	_nameToMod.compare = mcr_name_compare ;
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
	MCR_MOD_SEND ( ( mcr_Mod * ) sigPt->data, 0 ) ;
	return 1 ;
}

// Names
unsigned int mcr_Mod_get ( const char * name )
{
	if ( ! name )
		return MCR_ANY_MOD ;
	unsigned int * found = MCR_MAP_GET_VALUE ( & _nameToMod, & name ) ;
	return found ? * found : MCR_ANY_MOD ;
}

const char * mcr_Mod_get_name ( const unsigned int modifier )
{
	const char ** found = MCR_MAP_GET_VALUE ( & _modToName, & modifier ) ;
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
	return found ? * found : defaultMod ;
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
	if ( ! sigPt || ! sigPt->data ) return mods ;
	int echo = MCR_ECHO_GET ( ( mcr_HIDEcho * ) sigPt->data ) ;
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
	if ( ! sigPt || ! sigPt->data ) return mods ;
	int key = MCR_KEY_GET ( ( mcr_Key * ) sigPt->data ) ;
	unsigned int * found = MCR_MAP_GET_VALUE ( & _keyToMod, & key ) ;
	if ( found )
	{
		if ( MCR_KEY_GET_UP_TYPE ( ( mcr_Key * ) sigPt->data )
				== MCR_DOWN )
			return mods | * found ;
		return mods & ( ~ ( * found ) ) ;
	}
	return mods ;
}

int mcr_Mod_compare ( const void * lhs, const void * rhs )
{
	return memcmp ( lhs, rhs, sizeof ( mcr_Mod ) ) ;
}

//
// Modifier development.
//
// Names
int mcr_Mod_set_name ( const unsigned int modifier,
		const char * name )
{
	if ( ! name ) return 0 ;
	int ret = mcr_Map_map ( & _modToName, & modifier, & name ) ;
	return ret && mcr_Map_map ( & _nameToMod, & name, & modifier ) ;
}

int mcr_Mod_add_name ( const unsigned int modifier,
		const char * name )
{
	return mcr_Map_map ( & _nameToMod, & name, & modifier ) ;
}

int mcr_Mod_rename ( const unsigned int modifier,
		const char * newName )
{
	const char ** oldNamePt = MCR_MAP_GET_VALUE ( & _modToName,
			& modifier ) ;
	if ( oldNamePt )
	{
		if ( ! mcr_Map_remap ( & _nameToMod, oldNamePt, & newName ) )
			return 0 ;
		return mcr_Map_map ( & _modToName, & modifier, & newName ) ;
	}
	return mcr_Mod_set_name ( modifier, newName ) ;
}

int mcr_Mod_rename_by_name ( const char * oldName,
		const char * newName )
{
	if ( ! oldName ) return 0 ;
	unsigned int * modPt = MCR_MAP_GET_VALUE ( & _nameToMod, & oldName ) ;
	if ( modPt )
	{
		return mcr_Mod_rename ( * modPt, newName ) ;
	}
	return 0 ;
}

void mcr_Mod_clear_names ( )
{
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

int mcr_Mod_reecho ( const mcr_Mod modifiers,
		const int echo )
{
	int * found = MCR_MAP_GET_VALUE ( & _modToEcho, & modifiers ) ;
	if ( found )
	{
		// Map modifiers from new echo.
		if ( ! mcr_Map_remap ( & _echoToMod, found, & echo ) )
			return 0 ;
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

int mcr_Mod_rekey ( const unsigned int modifiers,
		const int key )
{
	int * found = MCR_MAP_GET_VALUE ( & _modToKey, & modifiers ) ;
	if ( found )
	{
		if ( ! mcr_Map_remap ( & _keyToMod, found, & key ) )
			return 0 ;
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
	mcr_ISignal_init ( & mcr_IMod, "Mod", mcr_Mod_send,
			sizeof ( mcr_Mod ) ) ;
	mcr_IMod.dispatch = NULL ;
	if ( mcr_ISignal_register ( & mcr_IMod ) == ( size_t ) -1 )
	{
		DMSG ( "%s", "Mods unable to register signal type." ) ;
	}
	if ( ! mcr_ISignal_add_name ( & mcr_IMod, "Mods" ) )
	{
		DMSG ( "%s", "Mods unable to add signal name." ) ;
	}
	mcr_ISignal_add_name ( & mcr_IMod, "Mods" ) ;
	names_init ( ) ;
	echo_init ( ) ;
	key_init ( ) ;
	mcr_Mod_set_name ( -1, "Any" ) ;
	mcr_Mod_set_name ( 0, "None" ) ;
	mcr_Mod_add_name ( -1, "AnyMod" ) ;
	mcr_Mod_add_name ( -1, "any mod" ) ;
	mcr_Mod_add_name ( -1, "any_mod" ) ;
	defaultMod.modifiers = MCR_ANY_MOD ;
	defaultMod.up_type = MCR_BOTH ;
}

void mcr_mods_cleanup ( )
{
	mcr_Mod_clear_all ( ) ;
}
