 
# ifdef WIN
//# include "stdafx.h"
# endif

# include "util/funstrings.h"

using namespace std ;

namespace funlibrary
{

	bool containsAt ( const char * container, unsigned int pos, const char * value,
		bool ignoreCase )
	{
		unsigned int i = 0 ;
		char cChar = 0 ;
		char vChar = 0 ;
		for ( ; container [ pos ] != '\0' && value [ i ] != '\0' ; pos++, i++ )
		{
			if ( ignoreCase )
			{
				cChar = tolower ( container [ pos ] ) ;
				vChar = tolower ( value [ i ] ) ;
				if ( cChar != vChar )
					return false ;
			}
			else
			{
				if ( container [ pos ] != value [ i ] )
					return false ;
			}
		}

		if ( container [ pos ] == '\0' && value [ i ] != '\0' )
			return false ;

		return true ;
	}
}

