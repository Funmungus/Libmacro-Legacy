/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

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

#include "mcr/signal/none/p_signal.h"
#include "mcr/signal/signal.h"

void mcr_cursor_position(mcr_SpacePosition buffer)
{
	memcpy(buffer, mcr_cursor, sizeof(mcr_SpacePosition));
}

int mcr_standard_platform_initialize()
{
	return 0;
}

int mcr_standard_platform_deinitialize(void)
{
	return 0;
}
