
# include "signal/lnx/hidecho.h"
# include "signal/hidecho.h"

namespace funlibrary
{
	std::vector < KeySignal > NativeHIDEcho::defaultEvents ( )
	{
		std::vector < KeySignal > ret =
		{
			KeySignal ( BTN_LEFT, 0, DOWN ),
			KeySignal ( BTN_LEFT, 0, UP ),
			KeySignal ( BTN_MIDDLE, 0, DOWN ),
			KeySignal ( BTN_MIDDLE, 0, UP ),
			KeySignal ( BTN_RIGHT, 0, DOWN ),
			KeySignal ( BTN_RIGHT, 0, UP ),
		} ;
		return ret ;
	}
	std::vector < KeySignal > NativeHIDEcho::events = defaultEvents ( ) ;

	std::map < std::string, int> HIDEcho::defaultNameMap ( )
	{
		std::map < std::string, int> ret =
		{
			std::pair < std::string, int > ( "LeftDown", 0 ),
			std::pair < std::string, int > ( "LeftUp", 1 ),
			std::pair < std::string, int > ( "MiddleDown", 2 ),
			std::pair < std::string, int > ( "MiddleUp", 3 ),
			std::pair < std::string, int > ( "RightDown", 4 ),
			std::pair < std::string, int > ( "RightUp", 5 )
		} ;

		return ret ;
	}

	std::vector < std::string > HIDEcho::defaultNameList ( )
	{
		std::vector < std::string > ret =
		{
			"LeftDown",
			"LeftUp",
			"MiddleDown",
			"MiddleUp",
			"RightDown",
			"RightUp"
		} ;

		return ret ;
	}

	std::map < std::string, int > HIDEcho::_eventInts = defaultNameMap ( ) ;
	std::vector < std::string > HIDEcho::_eventNames = defaultNameList ( ) ;
}
