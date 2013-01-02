
# include "signal/mods.h"

namespace funlibrary
{
	bool Mods::send ( )
	{
		bool modori = true ;

		if ( get ( ) )
		{
			KeySignal modSig ;
			modSig.setScan ( 0 ) ;
			modSig.setKeyUp ( getKeyUp ( ) ) ;

			unsigned int modBit = getMod ( FUN_ALT ) ;
			if ( hasMod ( modBit ) ) {
				modSig.setKey ( KEY_LEFTALT ) ;
				modori = modSig.send ( ) ? modori : false ;
			}

			modBit = getMod ( FUN_CTRL ) ;
			if ( hasMod ( modBit ) ) {
				modSig.setKey ( KEY_LEFTCTRL ) ;
				modori = modSig.send ( ) ? modori : false  ;
			}

			modBit = getMod ( FUN_SHIFT ) ;
			if ( hasMod ( modBit ) ) {
				modSig.setKey ( KEY_LEFTSHIFT ) ;
				modori = modSig.send ( ) ? modori : false  ;
			}

			modBit = getMod ( FUN_META ) ;
			if ( hasMod ( modBit ) ) {
				modSig.setKey ( KEY_LEFTMETA ) ;
				modori = modSig.send ( ) ? modori : false  ;
			}
		}

		return modori ;
	}

	unsigned int Mods::modFromKey ( const int key )
	{
		switch ( key )
		{
			case KEY_LEFTALT :
			case KEY_RIGHTALT :
				return getMod ( FUN_ALT ) ;
			case KEY_LEFTCTRL :
			case KEY_RIGHTCTRL :
				return getMod ( FUN_CTRL ) ;
			case KEY_LEFTSHIFT :
			case KEY_RIGHTSHIFT :
				return getMod ( FUN_SHIFT ) ;
			case KEY_LEFTMETA :
			case KEY_RIGHTMETA :
				return getMod ( FUN_META ) ;
		}

		return 0 ;
	}
}

