/* hotkey/lnx/lnxmods.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

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
