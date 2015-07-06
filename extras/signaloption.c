/* extras/signaloption.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "extras/signaloption.h"

MCR_API const char * mcr_signalOptionName = "signal" ;
MCR_API const char * mcr_sendOptionName = "send" ;
static mcr_Map _sigOpts ;
static mcr_Array _sigMods ;

static void alarm_set_sec ( mcr_Alarm * almPt, int val ) ;
static void alarm_set_min ( mcr_Alarm * almPt, int val ) ;
static void alarm_set_hour ( mcr_Alarm * almPt, int val ) ;
static void alarm_set_mday ( mcr_Alarm * almPt, int val ) ;
static void alarm_set_mon ( mcr_Alarm * almPt, int val ) ;
static void alarm_set_year ( mcr_Alarm * almPt, int val ) ;
static void alarm_set_wday ( mcr_Alarm * almPt, int val ) ;
static void alarm_set_yday ( mcr_Alarm * almPt, int val ) ;
static void alarm_set_isdst ( mcr_Alarm * almPt, int val ) ;

int mcr_signal_option ( int argc, char ** argv, int index )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;

	// Need at least one additional argument for signal functions.
	MCR_OPTION_MIN_OPTS ( argc, argv, index, 1 ) ;

	mcr_option_fnc * fPt = mcr_StringMap_get_value
			( & _sigOpts, argv [ index ] ) ;
	if ( fPt )
		index = ( * fPt ) ( argc, argv, index ) ;
	return index ;
}

void mcr_signal_option_set ( char * key,
		mcr_option_fnc optFnc )
{
	dassert ( key ) ;
	if ( ! optFnc )
	{
		mcr_StringMap_unmap ( & _sigOpts, key ) ;
	}
	else if ( ! mcr_StringMap_map ( & _sigOpts, key, & optFnc ) )
		dmsg ;
}

int mcr_signal_send_option ( int argc, char ** argv, int index )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;

	MCR_OPTION_MIN_OPTS ( argc, argv, index, 1 ) ;
	mcr_Signal sendSig = { 0 } ;
	// Will be at one after "send"
	index = mcr_signal_modify ( argc, argv, index, & sendSig ) ;
	MCR_SEND ( & sendSig ) ;
	mcr_Signal_free ( & sendSig ) ;
	return index ;
}

int mcr_signal_add_name ( int argc, char ** argv, int index )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;

	// At least 2 args, signal to add name, and name to add.
	MCR_OPTION_MIN_OPTS ( argc, argv, index, 2 ) ;
	mcr_ISignal * iSigPt = mcr_ISignal_from_name ( argv [ index ++ ] ) ;
	mcr_ISignal_add_name ( iSigPt, argv [ index ++ ] ) ;
	return index ;
}

int mcr_signal_add_names ( int argc, char ** argv, int index )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;

	// At least 2 args, signal to add name, and name ( s ) to add.
	MCR_OPTION_MIN_OPTS ( argc, argv, index, 2 ) ;
	mcr_ISignal * iSigPt = mcr_ISignal_from_name ( argv [ index ++ ] ) ;
	int isSetNotation = argv [ index ] [ 0 ] == '{' ;
	if ( isSetNotation )
		++ index ;
	int first = index ;
	int end = first ;
	while ( index < argc )
	{
		if ( isSetNotation && argv [ index ] [ 0 ] == '}' )
		{
			end = index ;
			++ index ;
			break ;
		}

		if ( ++ index >= argc )
			end = index ;
	}
	mcr_ISignal_add_names ( iSigPt, ( const char ** ) argv + first,
			( size_t ) ( end - first ) ) ;
	return index ;
}

# define suredata( sigPt ) \
	if ( ! ( sigPt )->data.data ) \
		( sigPt )->data = mcr_ISignal_mkdata ( ( sigPt )->type ) ;

# define error_return( expression, errIndex ) \
	if ( expression ) \
	{ \
		dmsg ; \
		mcr_Script_parse_error ( ) ; \
		return errIndex ; \
	}

int mcr_signal_modify ( int argc, char ** argv, int index,
		mcr_Signal * sigPtOut )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;

	/* Should be at signal type name. */
	mcr_ISignal * isigPt = mcr_ISignal_from_name ( argv [ index ] ) ;
	if ( ! isigPt || _sigMods.used < isigPt->interface.id )
		return index ; 	/* First index is not processed. */
	if ( sigPtOut->type != isigPt )
	{
		mcr_Signal_free ( sigPtOut ) ;
		sigPtOut->type = isigPt ;
	}

	// One past signal type name. Might have no arguments to modify.
	++ index ;
	if ( index >= argc )
		return index ;
	suredata ( sigPtOut ) ;
	mcr_signal_modify_fnc * fPt = MCR_ARR_AT ( & _sigMods,
			isigPt->interface.id ) ;
	dassert ( fPt ) ;
	if ( * fPt )
		return ( * fPt ) ( argc, argv, index, sigPtOut ) ;
	return index ;
}

void mcr_signal_modify_set ( size_t sigId,
		mcr_signal_modify_fnc setModifier )
{
	dassert ( sigId != ( size_t ) -1 ) ;
	if ( sigId >= _sigMods.used )
	{
		mcr_Array_insert_filled ( & _sigMods, sigId, & setModifier,
				NULL ) ;
	}
	else
	{
		mcr_Array_set ( & _sigMods, sigId, & setModifier ) ;
	}
}

int mcr_alarm_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_iAlarm ) ;
	suredata ( sigPtOut ) ;

	typedef void ( * datafnc ) ( mcr_Alarm *, int ) ;
	const char * datanames [ ] =
	{
		"sec", "min", "hr", "hour", "mday", "monthday", "mon", "yr",
		"year", "wday", "weekday", "yday", "yearday", "dst", "isdst"
	} ;
	const int lens [ ] =
	{
		3, 3, 2, 4, 4, 8, 3, 2,
		4, 4, 7, 4, 7, 3, 5
	} ;
	const datafnc datafncs [ ] =
	{
		alarm_set_sec, alarm_set_min, alarm_set_hour, alarm_set_hour,
		alarm_set_mday, alarm_set_mday, alarm_set_mon, alarm_set_year,
		alarm_set_year, alarm_set_wday, alarm_set_wday, alarm_set_yday,
		alarm_set_yday, alarm_set_isdst, alarm_set_isdst
	} ;
	const int len = sizeof ( lens ) / sizeof ( int ) ;

	// If args exist, then it must have data.
	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	char * afterval = NULL, * name ;
	long val ;
	int i ;
	mcr_Alarm * data = sigPtOut->data.data ;
	while ( index + 1 < argc )
	{
		name = argv [ index ] ;
		if ( setNotation && * name == '}' )
			return index + 1 ;
		for ( i = 0 ; i < len ; i ++ )
		{
			if ( ! strncasecmp ( name, datanames [ i ],
					( unsigned ) lens [ i ] ) )
			{
				val = strtol ( argv [ ++ index ], & afterval, 0 ) ;
				// Should be at int value after data name.
				error_return ( afterval == argv [ index ], index ) ;
				datafncs [ i ] ( data, val ) ;
				// Now at next member name or end.
				break ;
			}
		}
		// No data name processed.
		error_return ( i == len, index ) ;
		++ index ;
	}
	if ( setNotation && * name == '}' )
		return index + 1 ;
	return index ;
}

int mcr_echo_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_iHIDEcho ) ;
	suredata ( sigPtOut ) ;

	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	if ( index >= argc )
		return index ;
	if ( argv [ index ] [ 0 ] == '}' )
		return index + 1 ;
	if ( index + 1 >= argc ) 	/*NoVal*/
		return index ;

	int echo ;
	char * afterval = NULL ;
	if ( isdigit ( argv [ index ] [ 0 ] ) )
		echo = strtol ( argv [ index ], & afterval, 0 ) ;
	else
		echo = mcr_Echo_code ( argv [ index ] ) ;
	// Already checked for digit on the strtol.
	MCR_ECHO_SET ( ( mcr_HIDEcho * ) sigPtOut->data.data, echo ) ;
	++ index ;
	if ( setNotation && argv [ index ] [ 0 ] == '}' )
		return index + 1 ;
	return index ;
}

int mcr_key_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_iKey ) ;
	suredata ( sigPtOut ) ;

	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	char * afterval = NULL, * name ;
	long val ;
	int i ;
	mcr_Key * data = sigPtOut->data.data ;
	while ( index + 1 < argc )
	{
		name = argv [ index ] ;
		if ( setNotation && * name == '}' )
			return index + 1 ;
		if ( ! strncasecmp ( name, "key", 3 ) ||
				! strncasecmp ( name, "scan", 4 ) )
		{
			++ index ;
			val = strtol ( argv [ index ], & afterval, 0 ) ;
			error_return ( afterval == argv [ index ], index ) ;
			if ( * name == 'k' ||
					* name == 'K' )
			{
				MCR_KEY_SET ( data, val ) ;
			}
			else
			{
				MCR_KEY_SET_SCAN ( data, val ) ;
			}
		}
		else if ( ! strncasecmp ( name, "up", 2 ) )
		{
			++ index ;
			if ( ! strncasecmp ( argv [ index ], "down", 4 ) )
			{
				MCR_KEY_SET_UP_TYPE ( data, MCR_DOWN ) ;
			}
			else if ( ! strncasecmp ( argv [ index ], "up", 2 ) )
			{
				MCR_KEY_SET_UP_TYPE ( data, MCR_UP ) ;
			}
			else if ( ! strncasecmp ( argv [ index ], "both", 4 ) )
			{
				MCR_KEY_SET_UP_TYPE ( data, MCR_BOTH ) ;
			}
			else
				error_return ( 1, index ) ;
		}
		else
			error_return ( 1, index ) ;
		++ index ;
	}
	return index ;
}

int mcr_movecursor_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_iMoveCursor ) ;
	suredata ( sigPtOut ) ;

	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	char * afterval = NULL, * name ;
	long val ;
	int i ;
	mcr_MoveCursor * data = sigPtOut->data.data ;
	while ( index + 1 < argc )
	{
		name = argv [ index ++ ] ;
		if ( setNotation && argv [ index ] [ 0 ] == '}' )
			return ++ index ;
		char oneChar = argv [ index ] [ 0 ] ;
		if ( ( oneChar >= 'x' && oneChar <= 'z' ) ||
				( oneChar >= 'X' && oneChar <= 'Z' ) )
		{
			++ index ;
			error_return ( index >= argc, index ) ;
			val = strtol ( argv [ index ], & afterval, 0 ) ;
			error_return ( afterval == argv [ index ], index ) ;
			oneChar = oneChar == 'x' || oneChar == 'X' ? MCR_X :
					oneChar == 'y' || oneChar == 'Y' ? MCR_Y :
					MCR_Z ;
			MCR_MOVECURSOR_ENABLE_JUSTIFY ( data, oneChar ) ;
		}
		else if ( ! strncasecmp ( argv [ index ],
				"justify", 7 ) )
		{
			++ index ;
			error_return ( index >= argc, index ) ;
			int justify ;
			if ( mcr_convert_bool ( argv [ index ],
					& justify ) )
			{
				MCR_MOVECURSOR_ENABLE_JUSTIFY ( data,
						justify ) ;
			}
			else
			{
				dmsg ;
				mcr_Script_parse_error ( ) ;
			}
		}
		// If no data name processed, then prev == index.
		error_return ( nameIndex == index, index ) ;
		++ index ;
	}
	return index ;
}

int mcr_noop_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{

	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_iNoOp ) ;
	suredata ( sigPtOut ) ;

	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	char * afterval = NULL, * name ;
	long val ;
	int i, nameIndex ;
	mcr_NoOp * data = sigPtOut->data.data ;
	while ( index + 1 < argc )
	{
		nameIndex = index ;
		if ( setNotation && argv [ index ] [ 0 ] == '}' )
			return ++ index ;
		name = argv [ index ++ ] ;
		if ( ! strncasecmp ( argv [ index ], "sec", 3 ) )
		{
			error_return ( index >= argc, index ) ;

		}
		else
			-- index ;
		// If no data name processed, then prev == index.
		error_return ( nameIndex == index, index ) ;
		++ index ;
	}
	return index ;
}

int mcr_scroll_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{

	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_i ) ;
	suredata ( sigPtOut ) ;

	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	char * afterval = NULL ;
	long val ;
	int i, nameIndex ;
	mcr_ * data = sigPtOut->data.data ;
	while ( index + 1 < argc )
	{
		nameIndex = index ;
		if ( setNotation && argv [ index ] [ 0 ] == '}' )
			return ++ index ;
		// If no data name processed, then prev == index.
		error_return ( nameIndex == index, index ) ;
		++ index ;
	}
	return index ;
}

int mcr_mods_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{

	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_i ) ;
	suredata ( sigPtOut ) ;

	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	char * afterval = NULL ;
	long val ;
	int i, nameIndex ;
	mcr_ * data = sigPtOut->data.data ;
	while ( index + 1 < argc )
	{
		nameIndex = index ;
		if ( setNotation && argv [ index ] [ 0 ] == '}' )
			return ++ index ;
		// If no data name processed, then prev == index.
		error_return ( nameIndex == index, index ) ;
		++ index ;
	}
	return index ;
}

int mcr_command_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{

	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_i ) ;
	suredata ( sigPtOut ) ;

	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	char * afterval = NULL ;
	long val ;
	int i, nameIndex ;
	mcr_ * data = sigPtOut->data.data ;
	while ( index < argc )
	{
		nameIndex = index ;
		if ( setNotation && argv [ index ] [ 0 ] == '}' )
			return ++ index ;
		// If no data name processed, then prev == index.
		error_return ( nameIndex == index, index ) ;
		++ index ;
	}
	return index ;
}

int mcr_stringkey_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut )
{

	dassert ( argv ) ;
	dassert ( index < argc ) ;
	dassert ( sigPtOut->type == & mcr_i ) ;
	suredata ( sigPtOut ) ;

	int setNotation = argv [ index ] [ 0 ] == '{' ;
	if ( setNotation )
		++ index ;
	char * afterval = NULL ;
	long val ;
	int i, nameIndex ;
	mcr_ * data = sigPtOut->data.data ;
	while ( index < argc )
	{
		nameIndex = index ;
		if ( setNotation && argv [ index ] [ 0 ] == '}' )
			return ++ index ;
		// If no data name processed, then prev == index.
		error_return ( nameIndex == index, index ) ;
		++ index ;
	}
	return index ;
}

static void alarm_set_sec ( mcr_Alarm * almPt, int val )
{
	almPt->tm_sec = val ;
	almPt->tm_sec %= 62 ;
}
static void alarm_set_min ( mcr_Alarm * almPt, int val )
{
	almPt->tm_min = val ;
	almPt->tm_min %= 60 ;
}
static void alarm_set_hour ( mcr_Alarm * almPt, int val )
{
	almPt->tm_hour = val ;
	almPt->tm_hour %= 24 ;
}
static void alarm_set_mday ( mcr_Alarm * almPt, int val )
{
	almPt->tm_mday = val ;
	almPt->tm_mday %= 32 ;
	if ( ! almPt->tm_mday )
		almPt->tm_mday = 1 ;
}
static void alarm_set_mon ( mcr_Alarm * almPt, int val )
{
	almPt->tm_mon = val ;
	almPt->tm_mon %= 12 ;
}
static void alarm_set_year ( mcr_Alarm * almPt, int val )
{
	almPt->tm_year = val ;
}
static void alarm_set_wday ( mcr_Alarm * almPt, int val )
{
	almPt->tm_wday = val ;
	almPt->tm_wday %= 7 ;
}
static void alarm_set_yday ( mcr_Alarm * almPt, int val )
{
	almPt->tm_yday = val ;
	almPt->tm_yday %= 366 ;
}
static void alarm_set_isdst ( mcr_Alarm * almPt, int val )
{
	almPt->tm_isdst = val ;
	almPt->tm_isdst = almPt->tm_isdst && 1 ;
}

void mcr_signaloption_initialize ( )
{
	mcr_StringMap_init ( & _sigOpts, sizeof ( void * ) ) ;
	mcr_Array_init ( & _sigMods, sizeof ( void * ) ) ;
}

void mcr_signaloption_cleanup ( )
{
	mcr_Map_free ( & _sigOpts ) ;
	mcr_Array_free ( & _sigMods ) ;
}
