 
# ifdef WIN
//# include "stdafx.h"
# endif

# include "util/util.h"

using namespace std ;

namespace funlibrary
{
	bool convertBool
		( const char * value )
	{
		if ( value == NULL )
			throw ( 0 ) ;

		if ( containsAt ( value, 0, "true", true ) )
		{
			return true ;
		}
		if ( containsAt ( value, 0, "false", true ) )
		{
			return false ;
		}

		int val = stoi ( value ) ;

		return ( val == 0 ) ? false : true ;
	}
}




