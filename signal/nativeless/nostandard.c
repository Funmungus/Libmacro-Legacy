/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

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

void mcr_cursor_position ( mcr_SpacePosition buffer )
{
	memcpy ( buffer, mcr_cursor, sizeof ( mcr_SpacePosition ) ) ;
}

void mcr_standard_enable ( int enable )
{
	UNUSED ( enable ) ;
}

void mcr_standard_native_initialize ( )
{
}

void mcr_standard_native_cleanup ( void )
{
}
