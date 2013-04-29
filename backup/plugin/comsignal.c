
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
