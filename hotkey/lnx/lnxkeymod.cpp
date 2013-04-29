
# include "hotkey/keymod.h"

namespace funlibrary
{
	KeyMod::map_t KeyMod::defaultKeyToMod ( )
	{
		KeyMod::map_t ret ( 0, 0 ) ;
		int keys [ ] =
		{
			KEY_LEFTALT, KEY_RIGHTALT,
			KEY_LEFTCTRL, KEY_RIGHTCTRL,
			KEY_LEFTSHIFT, KEY_RIGHTSHIFT,
			KEY_LEFTMETA, KEY_RIGHTMETA
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
