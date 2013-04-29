
# include "signal/standard.h"

MCR_API mcr_ISignal mcr_IAlarm ;
MCR_API mcr_ISignal mcr_IHIDEcho ;
MCR_API mcr_ISignal mcr_IKey ;
MCR_API mcr_ISignal mcr_IMoveCursor ;
MCR_API mcr_ISignal mcr_INoOp ;
MCR_API mcr_ISignal mcr_IScroll ;

// echo name-> echo index
static mcr_Map _echoMap ;
// echo index-> echo name
static mcr_Array _echoNames ;

int mcr_Echo_code ( const char * eventName )
{
	if ( ! eventName )
		return -1 ;
	int * codePt = MCR_MAP_GET ( & _echoMap, & eventName ) ;
	if ( codePt )
	{
		codePt = MCR_MAP_VALUE ( & _echoMap, codePt ) ;
		return * codePt ;
	}
	return -1 ;
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
int mcr_Echo_register ( const char * eventName )
{
	if ( ! eventName || * eventName == '\0' )
		return -1 ;
	int * codePt = MCR_MAP_GET ( & _echoMap, & eventName ) ;
	if ( codePt )
	{
		return * codePt ;
	}
	int ret = mcr_Array_push ( & _echoNames, & eventName ) ;
	if ( ! ret )
	{
		DMSG ( "%s\n", "mcr_Echo_register unable to pushed" ) ;
		return -1 ;
	}
	ret = _echoNames.used - 1 ; // Last index, last pushed item.
	mcr_Map_map ( & _echoMap, & eventName, & ret ) ;
	return ret ;
}

// Initializers
void mcr_Echo_init_with ( mcr_HIDEcho * echoPt, const int event )
{
	// -1 means all echo
	mcr_Echo_init ( echoPt ) ;
	MCR_ECHO_SET ( echoPt, event ) ;
}
void mcr_Key_init_with ( mcr_Key * keyPt, const int key, const int scan,
		const mcr_KeyUpType keyUp )
{
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
	mcr_MoveCursor_init ( mcPt ) ;
	MCR_MOVECURSOR_SET ( mcPt, blip ) ;
	MCR_MOVECURSOR_ENABLE_JUSTIFY ( mcPt, cursorJustify ) ;
}
void mcr_Scroll_init_with ( mcr_Scroll * scrollPt,
		const mcr_Dimensions dimVals )
{
	mcr_Scroll_init ( scrollPt ) ;
	MCR_SCROLL_SET ( scrollPt, dimVals ) ;
}

//
// native Signal data manipulation
//
// Echo
int mcr_Echo_get ( const mcr_HIDEcho * echoPt )
{
	return MCR_ECHO_GET ( echoPt ) ;
}
void mcr_Echo_set ( mcr_HIDEcho * echoPt, int echo )
{
	MCR_ECHO_SET ( echoPt, echo ) ;
}
// Key
int mcr_Key_get ( const mcr_Key * keyPt )
{
	return MCR_KEY_GET ( keyPt ) ;
}
void mcr_Key_set ( mcr_Key * keyPt, int key )
{
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
	return MCR_KEY_GET_SCAN ( keyPt ) ;
}
void mcr_Key_set_scan ( mcr_Key * keyPt, int scan )
{
	MCR_KEY_SET_SCAN ( keyPt, scan ) ;
}
mcr_KeyUpType mcr_Key_get_up_type ( const mcr_Key * keyPt )
{
	return MCR_KEY_GET_UP_TYPE ( keyPt ) ;
}
void mcr_Key_set_up_type ( mcr_Key * keyPt, const mcr_KeyUpType keyUp )
{
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
	MCR_MOVECURSOR_GET ( mcPt, buffer ) ;
}
void mcr_MoveCursor_set ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition pos )
{
	MCR_MOVECURSOR_SET ( mcPt, pos ) ;
}
long long mcr_MoveCursor_get_position ( const mcr_MoveCursor * mcPt,
		int coordinate )
{
	return MCR_MOVECURSOR_GET_POSITION ( mcPt, coordinate ) ;
}
void mcr_MoveCursor_set_position ( mcr_MoveCursor * mcPt, int coordinate,
		long long value )
{
	MCR_MOVECURSOR_SET_POSITION ( mcPt, coordinate, value ) ;
}
int mcr_MoveCursor_is_justify ( const mcr_MoveCursor * mcPt )
{
	return MCR_MOVECURSOR_IS_JUSTIFY ( mcPt ) ;
}
void mcr_MoveCursor_enable_justify ( mcr_MoveCursor * mcPt,
		int cursorJustify )
{
	MCR_MOVECURSOR_ENABLE_JUSTIFY ( mcPt, cursorJustify ) ;
}
// Scroll
void mcr_Scroll_get ( const mcr_Scroll * scrollPt, mcr_Dimensions buffer )
{
	MCR_SCROLL_GET ( scrollPt, buffer ) ;
}
void mcr_Scroll_set ( mcr_Scroll * scrollPt, const mcr_Dimensions pos )
{
	MCR_SCROLL_SET ( scrollPt, pos ) ;
}
long long mcr_Scroll_get_dimension ( mcr_Scroll * scrollPt, int coordinate )
{
	return MCR_SCROLL_GET_DIMENSION ( scrollPt, coordinate ) ;
}
void mcr_Scroll_set_dimension ( mcr_Scroll * scrollPt, int coordinate,
		long long value )
{
	MCR_SCROLL_SET_DIMENSION ( scrollPt, coordinate, value ) ;
}

//
// Send functions that are placed into ISignals.
//
int mcr_Alarm_send ( mcr_Signal * signalData )
{
	mcr_Alarm * data = signalData->data ;
	int success = 1 ;
	MCR_ALARM_SEND ( data, success ) ;
	return success ;
}
int mcr_Echo_send ( mcr_Signal * signalData )
{
	mcr_HIDEcho * data = signalData->data ;
	int success = 1 ;
	MCR_ECHO_SEND ( data, success ) ;
	return success ;
}
int mcr_Key_send ( mcr_Signal * signalData )
{
	mcr_Key * data = signalData->data ;
	int success = 1 ;
	MCR_KEY_SEND ( data, success ) ;
	return 1 ;
}
int mcr_MoveCursor_send ( mcr_Signal * signalData )
{
	mcr_MoveCursor * data = signalData->data ;
	int success = 1 ;
	MCR_MOVECURSOR_SEND ( data, success ) ;
	return success ;
}
int mcr_NoOp_send ( mcr_Signal * signalData )
{
	mcr_NoOp * data = signalData->data ;
	int success = 1 ;
	MCR_NOOP_SEND ( data, success ) ;
	return success ;
}
int mcr_Scroll_send ( mcr_Signal * signalData )
{
	mcr_Scroll * data = signalData->data ;
	int success = 1 ;
	MCR_SCROLL_SEND ( data, success ) ;
	return success ;
}

// Position utils.
int mcr_resembles ( const mcr_MoveCursor * lhs, const mcr_MoveCursor * rhs,
		const unsigned int measurementError )
{
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

void mcr_standard_initialize ( )
{
	mcr_Array_init ( & _echoNames, sizeof ( const char * ) ) ;
	mcr_Map_init ( & _echoMap, sizeof ( const char * ), sizeof ( int ) ) ;
	_echoMap.compare = mcr_name_compare ;
	mcr_ISignal_init ( & mcr_IAlarm, "Alarm", mcr_Alarm_send,
			sizeof ( mcr_Alarm ) ) ;
	mcr_ISignal_init ( & mcr_IHIDEcho, "Echo", mcr_Echo_send,
			sizeof ( mcr_HIDEcho ) ) ;
	mcr_ISignal_init ( & mcr_IKey, "Key", mcr_Key_send,
			sizeof ( mcr_Key ) ) ;
	mcr_ISignal_init ( & mcr_IMoveCursor, "MoveCursor",
			mcr_MoveCursor_send, sizeof ( mcr_MoveCursor ) ) ;
	mcr_ISignal_init ( & mcr_INoOp, "NoOp", mcr_NoOp_send,
			sizeof ( mcr_NoOp ) ) ;
	mcr_ISignal_init ( & mcr_IScroll, "Scroll", mcr_Scroll_send,
			sizeof ( mcr_Scroll ) ) ;
	mcr_ISignal * sigs [ ] =
	{
		& mcr_IAlarm, & mcr_IHIDEcho, & mcr_IKey,
		& mcr_IMoveCursor, & mcr_INoOp, & mcr_IScroll
	} ;
	unsigned int count = sizeof ( sigs ) / sizeof ( mcr_ISignal * ) ;
	for ( unsigned int i = 0 ; i < count ; i++ )
	{
		if ( mcr_ISignal_register ( sigs [ i ] ) == ( size_t ) -1 )
		{
			DMSG ( "%s%s", "Unable to register signal type ",
					sigs [ i ] [ 0 ].name ) ;
		}
	}
}

void mcr_standard_cleanup ( )
{
	mcr_Map_free ( & _echoMap ) ;
	mcr_Array_free ( & _echoNames ) ;
}
