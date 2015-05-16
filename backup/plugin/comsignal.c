/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */


# include "signal/comsignal.h"

namespace macro
{
	const std::string ComSignal::name = "comsignal" ;
	bool ( * ComSignal::dispatcher ) ( ComSignal & ) ;

	std::string ComSignal::type ( ) const
	{
		return name ;
	}

	bool ComSignal::send ( )
	{
		return true ;
	}
}
