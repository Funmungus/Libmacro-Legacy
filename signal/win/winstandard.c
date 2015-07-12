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
mcr_Array mcr_echoEvents ;
mcr_Map mcr_flagToEcho ;
int mcr_flagMask = -1 ;

void mcr_standard_enable ( int enable )
{
	UNUSED ( enable ) ;
}

int mcr_Echo_set_mouseflag ( int echoCode, int mouseEventFlags )
{
	if ( echoCode == -1 )
		return -1 ;
	int ret = 1 ;
	if ( ( unsigned int ) echoCode < mcr_echoEvents.used )
		ret = mcr_Array_set ( & mcr_echoEvents,
				( unsigned int ) echoCode, & mouseEventFlags ) ;
	else
		ret = mcr_Array_insert_filled ( & mcr_echoEvents,
			( unsigned int ) echoCode, & mouseEventFlags, NULL ) ;
	if ( ret )
	{
		ret = mcr_Map_map ( & mcr_flagToEcho, & mouseEventFlags,
				& echoCode ) ;
	}
	return ret ;
}

void mcr_Echo_init ( void * echoPt )
{
	dassert ( echoPt ) ;
	( ( mcr_HIDEcho * ) echoPt )->event = 0 ;
}

void mcr_Key_init ( void * keyPt )
{
	dassert ( keyPt ) ;
	memset ( keyPt, 0, sizeof ( mcr_Key ) ) ;
}
void mcr_MoveCursor_init ( void * mcPt )
{
	dassert ( mcPt ) ;
	memset ( mcPt, 0, sizeof ( mcr_MoveCursor ) ) ;
	// Justify for default relative event.
	( ( mcr_MoveCursor * ) mcPt )->cursor_justify = 1 ;
}
void mcr_Scroll_init ( void * scrollPt )
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
		dmsg ;
	}
}

void mcr_standard_native_cleanup ( void )
{
	mcr_Array_free ( & mcr_echoEvents ) ;
	mcr_Map_free ( & mcr_flagToEcho ) ;
}

void mcr_standard_native_initialize ( )
{
	mcr_Array_init ( & mcr_echoEvents, sizeof ( int ) ) ;
	mcr_Map_init ( & mcr_flagToEcho, sizeof ( int ), sizeof ( int ) ) ;
	mcr_flagToEcho.compare = mcr_int_compare ;
}
