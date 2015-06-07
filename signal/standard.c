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
	MCR_KEY_SET ( keyPt, key == -1 ? 0 : key ) ;
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
	MCR_KEY_SET_UP_TYPE ( keyPt, ( unsigned int ) keyUp > MCR_BOTH ?
			MCR_BOTH : keyUp ) ;
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
	if ( ! ( sPt )->data.data ) \
		return 0 ;

int mcr_Alarm_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	return MCR_ALARM_SEND ( ( mcr_Alarm * ) signalData->data.data ) ;
}

int mcr_tm_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return rhs ? -1 : ( lhs && 1 ) ;
	}
	time_t lT = mktime ( ( struct tm * ) lhs ) ;
	time_t rT = mktime ( ( struct tm * ) rhs ) ;
	return memcmp ( & lT, & rT, sizeof ( time_t ) ) ;
}

int mcr_Echo_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_ECHO_SEND ( ( mcr_HIDEcho * ) signalData->data.data, success ) ;
	return success ;
}

# define diff_ret( lVal, rVal ) \
	if ( ( lVal ) != ( rVal ) ) \
		return ( lVal ) < ( rVal ) ? -1 : 1 ;

int mcr_Echo_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
		return rhs ? -1 : ( lhs && 1 ) ;
	const mcr_HIDEcho * lPt = lhs, * rPt = rhs ;
	int l = MCR_ECHO_GET ( lPt ), r = MCR_ECHO_GET ( rPt ) ;
	diff_ret ( l, r ) ;
	return 0 ;
}

void mcr_Echo_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	MCR_ECHO_SET ( ( ( mcr_HIDEcho * ) dstPt ),
			MCR_ECHO_GET ( ( mcr_HIDEcho * ) srcPt ) ) ;
}

void mcr_Echo_init_with ( mcr_HIDEcho * echoPt, const int event )
{
	dassert ( echoPt ) ;
	// -1 means all echo
	mcr_Echo_init ( echoPt ) ;
	MCR_ECHO_SET ( echoPt, event ) ;
}

int mcr_Key_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_KEY_SEND ( ( mcr_Key * ) signalData->data.data, success ) ;
	return success ;
}

int mcr_Key_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
		return rhs ? -1 : ( lhs && 1 ) ;
	const mcr_Key * lPt = lhs, * rPt = rhs ;
	int l = MCR_KEY_GET ( lPt ), r = MCR_KEY_GET ( rPt ) ;
	diff_ret ( l, r ) ;
	l = MCR_KEY_GET_SCAN ( lPt ) ;
	r = MCR_KEY_GET_SCAN ( rPt ) ;
	diff_ret ( l, r ) ;
	l = MCR_KEY_GET_UP_TYPE ( lPt ) ;
	r = MCR_KEY_GET_UP_TYPE ( rPt ) ;
	diff_ret ( l, r ) ;
	return 0 ;
}

void mcr_Key_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	mcr_Key * dPt = dstPt, * sPt = srcPt ;
	MCR_KEY_SET ( dPt, MCR_KEY_GET ( sPt ) ) ;
	MCR_KEY_SET_SCAN ( dPt, MCR_KEY_GET_SCAN ( sPt ) ) ;
	MCR_KEY_SET_UP_TYPE ( dPt, MCR_KEY_GET_UP_TYPE ( sPt ) ) ;
}

void mcr_Key_init_with ( mcr_Key * keyPt, const int key, const int scan,
		const mcr_KeyUpType keyUp )
{
	dassert ( keyPt ) ;
	mcr_Key_init ( keyPt ) ;
	MCR_KEY_SET ( keyPt, key == -1 ? 0 : key ) ;
	MCR_KEY_SET_SCAN ( keyPt, scan ) ;
	MCR_KEY_SET_UP_TYPE ( keyPt, ( unsigned int ) keyUp > MCR_BOTH ?
			MCR_BOTH : keyUp ) ;
}

int mcr_MoveCursor_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_MOVECURSOR_SEND ( ( mcr_MoveCursor * )
			signalData->data.data, success ) ;
	return success ;
}

int mcr_MoveCursor_compare ( const void * lhs,
		const void * rhs )
{
	if ( ! lhs || ! rhs )
		return rhs ? -1 : ( lhs && 1 ) ;
	const mcr_MoveCursor * lPt = lhs, * rPt = rhs ;
	long long l = MCR_MOVECURSOR_IS_JUSTIFY ( lPt ),
			r = MCR_MOVECURSOR_IS_JUSTIFY ( rPt ) ;
	diff_ret ( l, r ) ;
	for ( int i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
	{
		l = MCR_MOVECURSOR_GET_POSITION ( lPt, i ) ;
		r = MCR_MOVECURSOR_GET_POSITION ( rPt, i ) ;
		diff_ret ( l, r ) ;
	}
	return 0 ;
}

void mcr_MoveCursor_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	mcr_MoveCursor * dPt = dstPt, * sPt = srcPt ;
	MCR_MOVECURSOR_ENABLE_JUSTIFY ( dPt,
			MCR_MOVECURSOR_IS_JUSTIFY ( sPt ) ) ;
	for ( int i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
	{
		MCR_MOVECURSOR_SET_POSITION ( dPt, i,
				MCR_MOVECURSOR_GET_POSITION ( sPt, i ) ) ;
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

int mcr_NoOp_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	return MCR_NOOP_SEND ( ( mcr_NoOp * ) signalData->data.data ) ;
}

int mcr_Scroll_send ( mcr_Signal * signalData )
{
	dassert ( signalData ) ;
	chkdata ( signalData ) ;
	int success = 1 ;
	MCR_SCROLL_SEND ( ( mcr_Scroll * ) signalData->data.data, success ) ;
	return success ;
}

int mcr_Scroll_compare ( const void * lhs,
		const void * rhs )
{
	if ( ! lhs || ! rhs )
		return rhs ? -1 : ( lhs && 1 ) ;
	const mcr_Scroll * lPt = lhs, * rPt = rhs ;
	long long l, r ;
	for ( int i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
	{
		l = MCR_SCROLL_GET_DIMENSION ( lPt, i ) ;
		r = MCR_SCROLL_GET_DIMENSION ( rPt, i ) ;
		diff_ret ( l, r ) ;
	}
	return 0 ;
}

void mcr_Scroll_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	mcr_Scroll * dPt = dstPt, * sPt = srcPt ;
	for ( int i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
	{
		MCR_SCROLL_SET_DIMENSION ( dPt, i,
				MCR_SCROLL_GET_DIMENSION ( sPt, i ) ) ;
	}
}

void mcr_Scroll_init_with ( mcr_Scroll * scrollPt,
		const mcr_Dimensions dimVals )
{
	dassert ( scrollPt ) ;
	mcr_Scroll_init ( scrollPt ) ;
	MCR_SCROLL_SET ( scrollPt, dimVals ) ;
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

void mcr_standard_initialize ( )
{
	mcr_ISignal_init ( & mcr_iAlarm, mcr_Alarm_send,
			sizeof ( mcr_Alarm ) ) ;
	mcr_iAlarm.interface.compare = mcr_tm_compare ;
	mcr_ISignal_init_with ( & mcr_iHIDEcho, mcr_Echo_compare,
			mcr_Echo_copy, sizeof ( mcr_HIDEcho ),
			mcr_Echo_init, NULL, NULL, mcr_Echo_send ) ;
	mcr_ISignal_init_with ( & mcr_iKey, mcr_Key_compare,
			mcr_Key_copy, sizeof ( mcr_Key ),
			mcr_Key_init, NULL, NULL, mcr_Key_send ) ;
	mcr_ISignal_init_with ( & mcr_iMoveCursor, mcr_MoveCursor_compare,
			mcr_MoveCursor_copy, sizeof ( mcr_MoveCursor ),
			mcr_MoveCursor_init, NULL, NULL, mcr_MoveCursor_send ) ;
	mcr_ISignal_init ( & mcr_iNoOp, mcr_NoOp_send,
			sizeof ( mcr_NoOp ) ) ;
	mcr_ISignal_init_with ( & mcr_iScroll, mcr_Scroll_compare,
			mcr_Scroll_copy, sizeof ( mcr_Scroll ),
			mcr_Scroll_init, NULL, NULL, mcr_Scroll_send ) ;
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
			dmsg ;
		}
	}
}

void mcr_standard_cleanup ( void )
{
}
