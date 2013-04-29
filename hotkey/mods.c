
# include "hotkey/mods.h"

namespace macro
{
	const std::string Mods::ALT = "Alt" ;
	const std::string Mods::CTRL = "Control" ;
	const std::string Mods::SHIFT = "Shift" ;
	const std::string Mods::META = "Meta" ;
	const Mods::mod_t Mods::DEFAULT_MOD = Mods::mod_t ( 0, DOWN ) ;

	void Mods::set ( unsigned int & container, const unsigned int * newMods, const unsigned int length )
	{
		container = 0 ;

		for ( unsigned int i = 0 ; i < length ; i++ )
		{
			container |= newMods [ i ] ;
		}
	}

	unsigned int InternalMods ;
	std::mutex InternalModLock ;

	BiMap < int, Mods::mod_t > EchoToMod ( -1, Mods::DEFAULT_MOD ) ;
//	int ModifierMeasurementError = 10 ;
//	std::vector < std::pair < Scroll, Mods::mod_t > > ScrollToMod ; 	// x, y, z
//	std::vector < std::pair < MoveCursor, Mods::mod_t > > ScrollToMod ; 	// x, y, z
}
