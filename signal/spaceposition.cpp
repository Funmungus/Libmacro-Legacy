
# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/spacesignal.h"

using namespace std ;

namespace funlibrary
{
	const std::string SpacePosition::name = "spaceposition" ;

	SpacePosition::SpacePosition ( const long long x, const long long y, const long long z )
		: x ( x ), y ( y ), z ( z )
	{
	}

	void SpacePosition::call ( char * name, long value )
	{
		if ( containsAt ( name, 0, "x", true ) )
		{
			x = value ;
		}
		else if ( containsAt ( name, 0, "y", true ) )
		{
			y = value ;
		}
		else if ( containsAt ( name, 0, "z", true ) )
		{
			z = value ;
		}
	}
}

