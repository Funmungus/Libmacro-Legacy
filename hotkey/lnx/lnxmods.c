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

# include "hotkey/mods.h"
# include "signal/lnx/def.h"

void mcr_Mods_load_key_contract ( )
{
	// Left defaults
	const unsigned int types [ ] = {
		MCR_ALT, MCR_ALTGR, MCR_COMPOSE,
		MCR_CTRL, MCR_FN, MCR_FRONT,
		MCR_SHIFT, MCR_META
	} ;
	const int modKeys [ ] = {
		KEY_LEFTALT, KEY_RIGHTALT, KEY_COMPOSE,
		KEY_LEFTCTRL, KEY_FN, KEY_FRONT,
		KEY_LEFTSHIFT, KEY_LEFTMETA
	} ;
	// Right extras
	const unsigned int extraTypes [ ] = {
		MCR_OPTION, MCR_CTRL, MCR_SHIFT, MCR_META
	} ;
	const int extraModKeys [ ] = {
		KEY_OPTION, KEY_RIGHTCTRL, KEY_RIGHTSHIFT, KEY_RIGHTMETA
	} ;
	int i ;
	int len = ( int ) sizeof ( types ) / sizeof ( int ) ;
	for ( i = 0 ; i < len ; i ++ )
	{
		mcr_Mod_set_key ( types [ i ], modKeys [ i ] ) ;
	}
	len = ( int ) sizeof ( extraTypes ) / sizeof ( int ) ;
	for ( i = 0 ; i < ( int ) ( sizeof ( extraTypes ) / sizeof ( int ) ) ; i ++ )
	{
		mcr_Mod_add_key ( extraTypes [ i ], extraModKeys [ i ] ) ;
	}
}
