/* signal/nativeless/nostandard.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "signal/nativeless/standard.h"

void mcr_Echo_init ( mcr_HIDEcho * echoPt )
{
	UNUSED ( echoPt ) ;
}
void mcr_Key_init ( mcr_Key * keyPt )
{
	UNUSED ( keyPt ) ;
}
void mcr_MoveCursor_init ( mcr_MoveCursor * mcPt )
{
	UNUSED ( mcPt ) ;
}
void mcr_Scroll_init ( mcr_Scroll * scrollPt )
{
	UNUSED ( scrollPt ) ;
}

void mcr_standard_native_initialize ( )
{
}

void mcr_standard_native_cleanup ( )
{
}
