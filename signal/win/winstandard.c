/* signal/win/winstandard.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "signal/standard.h"
# include "signal/win/standard.h"

// index-> echo event integer
MCR_API mcr_Array mcr_EchoEvents ;

void mcr_Echo_init ( mcr_HIDEcho * echoPt )
{
	dassert ( echoPt ) ;
	echoPt->event = 0 ;
}
void mcr_Key_init ( mcr_Key * keyPt )
{
	dassert ( keyPt ) ;
	memset ( keyPt, 0, sizeof ( mcr_Key ) ) ;
}
void mcr_MoveCursor_init ( mcr_MoveCursor * mcPt )
{
	dassert ( mcPt ) ;
	memset ( mcPt, 0, sizeof ( mcr_MoveCursor ) ) ;
	// Justify for default relative event.
	mcPt->cursor_justify = 1 ;
}
void mcr_Scroll_init ( mcr_Scroll * scrollPt )
{
	dassert ( scrollPt ) ;
	memset ( scrollPt, 0, sizeof ( mcr_Scroll ) ) ;
}

# define CALC_ABS_X( x ) \
	( ( x * 65535 ) / GetSystemMetrics ( SM_CXSCREEN ) )

# define CALC_ABS_Y( y ) \
	( ( y * 65535 ) / GetSystemMetrics ( SM_CYSCREEN ) )

void mcr_cursor_position ( mcr_SpacePosition buffer )
{
	POINT p ;
	if ( GetCursorPos ( & p ) )
	{
		buffer [ MCR_X ] = p.x ;
		buffer [ MCR_Y ] = p.y ;
		buffer [ MCR_Z ] = 0 ;
	}
	else
	{
		dmsg ( "Unable to get cursor position, error #%d\n",
				( int ) GetLastError ( ) ) ;
	}
}

void mcr_standard_native_cleanup ( void )
{
	mcr_Array_free ( & mcr_EchoEvents ) ;
}

void mcr_standard_native_initialize ( )
{
	mcr_Array_init ( & mcr_EchoEvents, sizeof ( int ) ) ;

	int echoEvents [ ] =
	{
		MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP,
		MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP,
		MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP,
	} ;
	const char * names [ ] =
	{
		"LeftDown", "LeftUp", "MiddleDown", "MiddleUp",
		"RightDown", "RightUp"
	} ;
	const char * extraNames [ ] =
	{
		"left down", "left up", "middle down", "middle up",
		"right down", "right up"
	} ;
	const char * extraNames2 [ ] =
	{
		"left_down", "left_up", "middle_down", "middle_up",
		"right_down", "right_up"
	} ;
	size_t count = sizeof ( echoEvents ) / sizeof ( int ) ;
	if ( ! mcr_Array_resize ( & mcr_EchoEvents, count ) )
	{
		dmsg ( "mcr_standard_native_initialize unable to "
				"size native events.\n" ) ;
		return ;
	}
	for ( size_t i = 0 ; i < count ; i++ )
	{
		int success = mcr_Echo_set_name ( i, names [ i ] ) ;
		if ( success )
		{
			// TODO Native register.
			mcr_Array_set ( & mcr_EchoEvents, code, echoEvents + i ) ;
		}
		else
		{
			dmsg ( "Unable to register echo: %s.\n", names [ i ] ) ;
		}
		success = mcr_Echo_add_name ( i, extraNames [ i ] ) ;
		if ( ! mcr_Echo_add_name ( i, extraNames2 [ i ] ) )
			success = 0 ;
		if ( ! success )
		{
			dmsg ( "Unable to add extra name: %s.\n",
					extraNames [ i ] ) ;
		}
	}
}
