/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */


// Not yet available in linux
# include "signal/screencrop.h"

namespace macro
{
	const std::string ScreenCrop::name = "screencrop" ;
	bool ( * ScreenCrop::dispatcher ) ( ScreenCrop & ) ;

	std::string ScreenCrop::type ( ) const
	{
		return ScreenCrop::name ;
	}
}
