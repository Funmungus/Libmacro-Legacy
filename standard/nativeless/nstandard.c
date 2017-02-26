/* Libmacro - A multi-platform, extendable macro and hotkey C library
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

#include "mcr/signal/nativeless/nsignal.h"
#include "mcr/signal/signal.h"

void mcr_HidEcho_init(struct mcr_HidEcho *echoPt)
{
	UNUSED(echoPt);
}

void mcr_Key_init(struct mcr_Key *keyPt)
{
	UNUSED(keyPt);
}

void mcr_MoveCursor_init(struct mcr_MoveCursor *mcPt)
{
	UNUSED(mcPt);
}

void mcr_Scroll_init(struct mcr_Scroll *scrollPt)
{
	UNUSED(scrollPt);
}

void mcr_cursor_position(mcr_SpacePosition buffer)
{
	memcpy(buffer, mcr_cursor, sizeof(mcr_SpacePosition));
}

void mcr_standard_set_enabled(int enable)
{
	UNUSED(enable);
}

int mcr_standard_is_enabled()
{
	return 0;
}

int mcr_standard_native_initialize()
{
	return 0;
}

int mcr_standard_native_deinitialize(void)
{
	return 0;
}
