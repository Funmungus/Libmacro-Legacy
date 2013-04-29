
# include "signal/nativeless/hidecho.h"
# include "signal/hidecho.h"

namespace funlibrary
{
	MapSet HIDEcho::defaultEventMap ( )
	{
MapSet ret ;
		ret.replace ( "LeftDown", 0 ) ;
		ret.replace ( "LeftUp", 1 ) ;
		ret.replace ( "MiddleDown", 2 ) ;
		ret.replace ( "MiddleUp", 3 ) ;
		ret.replace ( "RightDown", 4 ) ;
		ret.replace ( "RightUp", 5 ) ;
		return ret ;
	}

	MapSet HIDEcho::EventMap = defaultEventMap ( ) ;
}
