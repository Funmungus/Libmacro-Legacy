
namespace funlibrary
{
	bool Mods::send ( )
	{
		bool modori = true ;
	
		if ( get ( ) != 0 )
		{
			KeySignal modSig ;
			modSig.setScan ( 0 ) ;
			modSig.setKeyUp ( getKeyUp ( ) ) ;

			unsigned int modBit = getMod ( FUN_ALT ) ;
			if ( hasMod ( modBit ) ) {
				modSig.setKey ( VK_LMENU ) ;
				modori = modSig.send ( ) ? modori : false  ;
			}

			modBit = getMod ( FUN_CTRL ) ;
			if ( hasMod ( modBit ) ) {
				modSig.setKey ( VK_LCONTROL ) ;
				modori = modSig.send ( ) ? modori : false  ;
			}

			modBit = getMod ( FUN_SHIFT ) ;
			if ( hasMod ( modBit ) ) {
				modSig.setKey ( VK_LSHIFT ) ;
				modori = modSig.send ( ) ? modori : false  ;
			}

			modBit = getMod ( FUN_META ) ;
			if ( hasMod ( modBit ) ) {
				modSig.setKey ( VK_LWIN ) ;
				modori = modSig.send ( ) ? modori : false ;
			}
		}
		
		return modori ;
	}

	unsigned int Mods::modFromKey ( const int key )
	{
		switch ( key )
		{
			case VK_LMENU :
			case VK_RMENU :
				return getMod ( FUN_ALT ) ;
			case VK_LCONTROL :
			case VK_RCONTROL :
				return getMod ( FUN_CTRL ) ;
			case VK_LSHIFT :
			case VK_RSHIFT :
				return getMod ( FUN_SHIFT ) ;
			case VK_LWIN :
			case VK_RWIN :
				return getMod ( FUN_META ) ;
		}

		return 0 ;
	}
}

