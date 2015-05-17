/* signal/standard.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "signal/standard.h"

MCR_API mcr_ISignal mcr_iAlarm ;
MCR_API mcr_ISignal mcr_iHIDEcho ;
MCR_API mcr_ISignal mcr_iKey ;
MCR_API mcr_ISignal mcr_iMoveCursor ;
MCR_API mcr_ISignal mcr_iNoOp ;
MCR_API mcr_ISignal mcr_iScroll ;

//
// Code memory
//
// Echo
// echo name-> echo index
static mcr_Map _echoMap ;
// echo index-> echo name
static mcr_Array _echoNames ;
int mcr_Echo_code ( const char * eventName )
{
	if ( ! eventName )
		return -1 ;
	int * codePt = MCR_MAP_GET_VALUE ( & _echoMap, & eventName ) ;
	return codePt ? * codePt : -1 ;
}

const char * mcr_Echo_name ( int eventCode )
{
	const char ** namePt = MCR_ARR_AT ( & _echoNames,
			( size_t ) eventCode ) ;
	return namePt ? * namePt : NULL ;
}

size_t mcr_Echo_count ( )
{
	return _echoNames.used ;
}

// Key
static mcr_Array _keyNames ;
static mcr_Map _keyMap ;
int mcr_Key_code ( const char * keyName )
{
	if ( ! keyName )
		return 0 ;
	int * keyPt = MCR_MAP_GET_VALUE ( & _keyMap, & keyName ) ;
	return keyPt ? * keyPt : 0 ;
}

const char * mcr_Key_name ( int keyCode )
{
	const char ** namePt = MCR_ARR_AT ( & _keyNames,
			( size_t ) keyCode ) ;
	return namePt ? * namePt : NULL ;
}

size_t mcr_Key_count ( )
{
	return _keyNames.used ;
}

// Initializers
void mcr_Echo_init_with ( mcr_HIDEcho * echoPt, const int event )
{
	dassert ( echoPt ) ;
	// -1 means all echo
	mcr_Echo_init ( echoPt ) ;
	MCR_ECHO_SET ( echoPt, event ) ;
}

void mcr_Key_init_with ( mcr_Key * keyPt, const int key, const int scan,
		const mcr_KeyUpType keyUp )
{
	dassert ( keyPt ) ;
	mcr_Key_init ( keyPt ) ;
	if ( key == -1 )
	{
		MCR_KEY_SET ( keyPt, 0 ) ;
	}
	else
	{
		MCR_KEY_SET ( keyPt, key ) ;
	}
	MCR_KEY_SET_SCAN ( keyPt, scan ) ;
	if ( ( unsigned int ) keyUp > MCR_BOTH )
	{
		MCR_KEY_SET_UP_TYPE ( keyPt, MCR_BOTH ) ;
	}
	else
	{
		MCR_KEY_SET_UP_TYPE ( keyPt, keyUp ) ;
	}
}

void mcr_MoveCursor_init_with ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition blip, const int cursorJustify )
{
	dassert ( mcPt ) ;
	mcr_MoveCursor_init ( mcPt ) ;
	MCR_MOVECURSOR_SET ( mcPt, blip ) ;
	MCR_MOVECURSOR_ENABLE_JUSTIFY ( mcPt, cursorJustify ) ;
}

void mcr_Scroll_init_with ( mcr_Scroll * scrollPt,
		const mcr_Dimensions dimVals )
{
	dassert ( scrollPt ) ;
	mcr_Scroll_init ( scrollPt ) ;
	MCR_SCROLL_SET ( scrollPt, dimVals ) ;
}

//
// native Signal data manipulation
//
// Echo
int mcr_Echo_get ( const mcr_HIDEcho * echoPt )
{
	dassert ( echoPt ) ;
	return MCR_ECHO_GET ( echoPt ) ;
}

void mcr_Echo_set ( mcr_HIDEcho * echoPt, int echo )
{
	dassert ( echoPt ) ;
	MCR_ECHO_SET ( echoPt, echo ) ;
}

// Key
int mcr_Key_get ( const mcr_Key * keyPt )
{
	dassert ( keyPt ) ;
	return MCR_KEY_GET ( keyPt ) ;
}

void mcr_Key_set ( mcr_Key * keyPt, int key )
{
	dassert ( keyPt ) ;
	if ( key == -1 )
	{
		MCR_KEY_SET ( keyPt, 0 ) ;
	}
	else
	{
		MCR_KEY_SET ( keyPt, key ) ;
	}
}

int mcr_Key_get_scan ( const mcr_Key * keyPt )
{
	dassert ( keyPt ) ;
	return MCR_KEY_GET_SCAN ( keyPt ) ;
}

void mcr_Key_set_scan ( mcr_Key * keyPt, int scan )
{
	dassert ( keyPt ) ;
	MCR_KEY_SET_SCAN ( keyPt, scan ) ;
}

mcr_KeyUpType mcr_Key_get_up_type ( const mcr_Key * keyPt )
{
	dassert ( keyPt ) ;
	return MCR_KEY_GET_UP_TYPE ( keyPt ) ;
}

void mcr_Key_set_up_type ( mcr_Key * keyPt, const mcr_KeyUpType keyUp )
{
	dassert ( keyPt ) ;
	if ( ( unsigned int ) keyUp > MCR_BOTH )
	{
		MCR_KEY_SET_UP_TYPE ( keyPt, MCR_BOTH ) ;
	}
	else
	{
		MCR_KEY_SET_UP_TYPE ( keyPt, keyUp ) ;
	}
}

// MoveCursor
void mcr_MoveCursor_get ( const mcr_MoveCursor * mcPt,
		mcr_SpacePosition buffer )
{
	dassert ( mcPt ) ;
	MCR_MOVECURSOR_GET ( mcPt, buffer ) ;
}

void mcr_MoveCursor_set ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition pos )
{
	dassert ( mcPt ) ;
	MCR_MOVECURSOR_SET ( mcPt, pos ) ;
}

long long mcr_MoveCursor_get_position ( const mcr_MoveCursor * mcPt,
		int coordinate )
{
	dassert ( mcPt ) ;
	return MCR_MOVECURSOR_GET_POSITION ( mcPt, coordinate ) ;
}

void mcr_MoveCursor_set_position ( mcr_MoveCursor * mcPt, int coordinate,
		long long value )
{
	dassert ( mcPt ) ;
	MCR_MOVECURSOR_SET_POSITION ( mcPt, coordinate, value ) ;
}

int mcr_MoveCursor_is_justify ( const mcr_MoveCursor * mcPt )
{
	dassert ( mcPt ) ;
	return MCR_MOVECURSOR_IS_JUSTIFY ( mcPt ) ;
}

void mcr_MoveCursor_enable_justify ( mcr_MoveCursor * mcPt,
		int cursorJustify )
{
	dassert ( mcPt ) ;
	MCR_MOVECURSOR_ENABLE_JUSTIFY ( mcPt, cursorJustify ) ;
}

// Scroll
void mcr_Scroll_get ( const mcr_Scroll * scrollPt, mcr_Dimensions buffer )
{
	dassert ( scrollPt ) ;
	MCR_SCROLL_GET ( scrollPt, buffer ) ;
}

void mcr_Scroll_set ( mcr_Scroll * scrollPt, const mcr_Dimensions pos )
{
	dassert ( scrollPt ) ;
	MCR_SCROLL_SET ( scrollPt, pos ) ;
}

long long mcr_Scroll_get_dimension ( mcr_Scroll * scrollPt, int coordinate )
{
	dassert ( scrollPt ) ;
	return MCR_SCROLL_GET_DIMENSION ( scrollPt, coordinate ) ;
}

void mcr_Scroll_set_dimension ( mcr_Scroll * scrollPt, int coordinate,
		long long value )
{
	dassert ( scrollPt ) ;
	MCR_SCROLL_SET_DIMENSION ( scrollPt, coordinate, value ) ;
}

//
// Send functions that are placed into ISignals.
//
# define chkdata(sPt) \
	if ( ! ( sPt )->data ) \
		return 0 ;

int mcr_Alarm_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_ALARM_SEND ( ( mcr_Alarm * ) signalData->data, success ) ;
	return success ;
}

int mcr_Echo_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_ECHO_SEND ( ( mcr_HIDEcho * ) signalData->data, success ) ;
	return success ;
}

int mcr_Key_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_KEY_SEND ( ( mcr_Key * ) signalData->data, success ) ;
	return success ;
}

int mcr_MoveCursor_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_MOVECURSOR_SEND ( ( mcr_MoveCursor * )
			signalData->data, success ) ;
	return success ;
}

int mcr_NoOp_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_NOOP_SEND ( ( mcr_NoOp * ) signalData->data, success ) ;
	return success ;
}

int mcr_Scroll_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_SCROLL_SEND ( ( mcr_Scroll * ) signalData->data, success ) ;
	return success ;
}

// Position utils.
int mcr_resembles ( const mcr_MoveCursor * lhs, const mcr_MoveCursor * rhs,
		const unsigned int measurementError )
{
	dassert ( lhs ) ;
	dassert ( rhs ) ;
	if ( MCR_MOVECURSOR_IS_JUSTIFY ( lhs ) !=
			MCR_MOVECURSOR_IS_JUSTIFY ( rhs ) )
		return 0 ;
	mcr_SpacePosition me, them ;
	MCR_MOVECURSOR_GET ( lhs, me ) ;
	MCR_MOVECURSOR_GET ( rhs, them ) ;

	if ( MCR_MOVECURSOR_IS_JUSTIFY ( lhs ) )
	{
		return mcr_resembles_justified ( me, them ) ;
	}
	else
	{
		return mcr_resembles_absolute ( me, them, measurementError ) ;
	}
	return 1 ;
}

int mcr_resembles_justified ( const mcr_Dimensions first,
							const mcr_Dimensions second )
{
	dassert ( first ) ;
	dassert ( second ) ;
	for ( unsigned int i = 0 ; i < MCR_DIMENSION_CNT ; i++ )
	{
		if ( first [ i ] == 0 || second [ i ] == 0 )
			continue ;
		if ( ( first [ i ] > 0 ) != ( second [ i ] > 0 ) )
			return 0 ;
	}
	return 1 ;
}

int mcr_resembles_absolute ( const mcr_Dimensions first,
		const mcr_Dimensions second, const unsigned int measurementError )
{
	dassert ( first ) ;
	dassert ( second ) ;
	long long err ;
	for ( unsigned int i = 0 ; i < MCR_DIMENSION_CNT ; i++ )
	{
		err = first [ i ] - second [ i ] ;
		if ( err < 0 ) err *= -1 ;
		if ( err > measurementError )
			return 0 ;
	}
	return 1 ;

}

//
// Code development
//
// Echo
int mcr_Echo_set_name ( int eventCode, const char * eventName )
{
	int ret = 0 ;
	if ( eventCode != -1 )
	{
		if ( ( unsigned int ) eventCode > _echoNames.used )
		{
			ret = mcr_Array_insert_filled ( & _echoNames, eventCode,
					& eventName, NULL ) ;
		}
		else
		{
			ret = mcr_Array_set ( & _echoNames, eventCode, & eventName ) ;
		}
	}
	if ( ret )
	{
		ret = mcr_Echo_add_name ( eventCode, eventName ) ;
	}
	return ret ;
}

int mcr_Echo_add_name ( int eventCode, const char * addName )
{
	return mcr_Map_map ( & _echoMap, & addName, & eventCode ) ;
}

int mcr_Echo_rename ( int eventCode,
		const char * newName )
{
	const char ** found = mcr_Array_at ( & _echoNames, eventCode ) ;
	if ( found )
	{
		if ( mcr_Map_remap ( & _echoMap, found, & newName ) )
		{
			 * found = newName ;
			return 1 ;
		}
	}
	return 0 ;
}

int mcr_Echo_rename_from_name ( const char * oldName,
		const char * newName )
{
	dassert ( oldName ) ;
	int * found = MCR_MAP_GET_VALUE ( & _echoMap, & oldName ) ;
	if ( found )
		return mcr_Echo_rename ( * found, newName ) ;
	return 0 ;
}

void mcr_Echo_clear_all ( )
{
	mcr_Map_free ( & _echoMap ) ;
	mcr_Array_free ( & _echoNames ) ;
}

// Key
int mcr_Key_set_name ( int keyCode, const char * newName )
{
	int ret = 0 ;
	if ( keyCode != -1 )
	{
		if ( ( unsigned int ) keyCode > _keyNames.used )
		{
			ret = mcr_Array_insert_filled ( & _keyNames, keyCode,
					& newName, NULL ) ;
		}
		else
		{
			ret = mcr_Array_set ( & _keyNames, keyCode, & newName ) ;
		}
	}
	if ( ret )
	{
		ret = mcr_Key_add_name ( keyCode, newName ) ;
	}
	return ret ;
}

int mcr_Key_add_name ( int keyCode, const char * addName )
{
	return mcr_Map_map ( & _keyMap, & addName, & keyCode ) ;
}

int mcr_Key_rename ( int keyCode, const char * newName )
{
	const char ** found = mcr_Array_at ( & _keyNames, keyCode ) ;
	if ( found )
	{
		if ( mcr_Map_remap ( & _keyMap, found, & newName ) )
		{
			 * found = newName ;
			return 1 ;
		}
	}
	return 0 ;
}

int mcr_Key_rename_from_name ( const char * oldName,
		const char * newName )
{
	dassert ( oldName ) ;
	int * found = MCR_MAP_GET_VALUE ( & _keyMap, & oldName ) ;
	if ( found )
		return mcr_Key_rename ( * found, newName ) ;
	return 0 ;
}

void mcr_Key_clear_all ( )
{
	mcr_Map_free ( & _keyMap ) ;
	mcr_Array_free ( & _keyNames ) ;
}

void mcr_standard_initialize ( )
{
	mcr_Array_init ( & _echoNames, sizeof ( const char * ) ) ;
	mcr_Map_init ( & _echoMap, sizeof ( const char * ), sizeof ( int ) ) ;
	_echoMap.compare = mcr_name_compare ;
	mcr_Array_init ( & _keyNames, sizeof ( const char * ) ) ;
	mcr_Map_init ( & _keyMap, sizeof ( const char * ), sizeof ( int ) ) ;
	_keyMap.compare = mcr_name_compare ;

	mcr_ISignal_init ( & mcr_iAlarm, "Alarm", mcr_Alarm_send,
			sizeof ( mcr_Alarm ) ) ;
	mcr_ISignal_init ( & mcr_iHIDEcho, "HIDEcho", mcr_Echo_send,
			sizeof ( mcr_HIDEcho ) ) ;
	mcr_ISignal_init ( & mcr_iKey, "Key", mcr_Key_send,
			sizeof ( mcr_Key ) ) ;
	mcr_ISignal_init ( & mcr_iMoveCursor, "MoveCursor",
			mcr_MoveCursor_send, sizeof ( mcr_MoveCursor ) ) ;
	mcr_ISignal_init ( & mcr_iNoOp, "NoOp", mcr_NoOp_send,
			sizeof ( mcr_NoOp ) ) ;
	mcr_ISignal_init ( & mcr_iScroll, "Scroll", mcr_Scroll_send,
			sizeof ( mcr_Scroll ) ) ;
	mcr_ISignal * sigs [ ] =
	{
		& mcr_iAlarm, & mcr_iHIDEcho, & mcr_iKey,
		& mcr_iMoveCursor, & mcr_iNoOp, & mcr_iScroll
	} ;
	unsigned int count = sizeof ( sigs ) / sizeof ( mcr_ISignal * ) ;
	for ( unsigned int i = 0 ; i < count ; i++ )
	{
		sigs [ i ]->dispatch = NULL ;
		if ( mcr_ISignal_register ( sigs [ i ] ) == ( size_t ) -1 )
		{
			dmsg ( "Unable to register signal type %s.\n",
					sigs [ i ] [ 0 ].name ) ;
		}
	}
	mcr_ISignal_add_name ( & mcr_iHIDEcho, "hid echo" ) ;
	mcr_ISignal_add_name ( & mcr_iHIDEcho, "hid_echo" ) ;
	mcr_ISignal_add_name ( & mcr_iHIDEcho, "echo" ) ;
	mcr_ISignal_add_name ( & mcr_iMoveCursor, "move cursor" ) ;
	mcr_ISignal_add_name ( & mcr_iMoveCursor, "move_cursor" ) ;
	mcr_ISignal_add_name ( & mcr_iNoOp, "no op" ) ;
	mcr_ISignal_add_name ( & mcr_iNoOp, "no_op" ) ;
}

void mcr_standard_cleanup ( )
{
	mcr_Echo_clear_all ( ) ;
	mcr_Key_clear_all ( ) ;
}
