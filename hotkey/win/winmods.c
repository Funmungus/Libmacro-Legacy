/* hotkey/win/winmods.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "hotkey/mods.h"
# include "signal/win/def.h"

void mcr_Mods_load_key_contract ( )
{
	// Left defaults
	const unsigned int types [ ] = {
		MCR_ALT, MCR_ALTGR,
		MCR_CTRL, MCR_SHIFT,
		MCR_WIN, MCR_META
	} ;
	const int modKeys [ ] = {
		VK_LMENU, VK_RMENU,
		VK_LCONTROL, VK_LSHIFT,
		VK_LWIN, VK_RWIN
	} ;
	// Right extras
	const unsigned int extraTypes [ ] = {
		MCR_CTRL, MCR_SHIFT
	} ;
	const int extraModKeys [ ] = {
		VK_RCONTROL, VK_RSHIFT
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
