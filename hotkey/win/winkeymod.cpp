
# include "hotkey/keymod.h"

namespace funlibrary
{
	KeyMod::map_t KeyMod::defaultKeyToMod ( )
	{
		KeyMod::map_t ret ;
		int keys [ ] =
		{
			VK_LMENU, VK_RMENU,
			VK_LCONTROL, VK_RCONTROL,
			VK_LSHIFT, VK_RSHIFT,
			VK_LWIN, VK_RWIN
		} ;
		unsigned int len = sizeof ( keys ) /sizeof ( int ), i = 0, modval = 1 ;
		while ( i < len )
		{
ret.replace ( keys [ i ], modval ) ;
ret.replace ( keys [ ++i ], modval ) ;
			++i ;
			modval <<= 1 ;
		}
		return ret ;
	}

	KeyMod::map_t KeyMod::KeyToMod = defaultKeyToMod ( ) ;
}
