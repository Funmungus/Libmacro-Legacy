 
# ifdef WIN
//# include "stdafx.h"
# endif

# include "util/serial.h"

using namespace std ;

namespace funlibrary
{
	// Note SerialSeparator is not constant.  This value can be configured.
	char SerialSeparator = ~0 ;
	//const char ISignal::SerialSeparator = -1 ; // ( const char ) FUN_SERIAL_SEPARATOR ;

	string serialHeader ( string name )
	{
		return name + SerialSeparator ;
	}
	//bool ISignal::verifySerialHeader ( string container, unsigned int pos, string name )
	//{
	//	// First find what is possibly this name.
	//	size_t len = container.length ( ) ;
	//	unsigned int curPos = pos ;
	//	while ( curPos < len && container [ curPos ] != name [ 0 ] )
	//	{
	//		++ curPos ;
	//	}

	//	// Verify this is the correct name.
	//	bool modori = containsAt ( container.c_str ( ), curPos,
	//		name.c_str ( ), true ) ;
	//	return modori ;
	//}

	//bool verifySerial ( std::string container, std::string name,
	//	unsigned int * firstData, unsigned int * pos )
	//{
	//	// First verify correct signal.
	//	* firstData = container.find_first_of ( name [ 0 ], *pos ) ;
	//	bool isValid = containsAt ( container.c_str ( ), * firstData,
	//		name.c_str ( ), true ) ;
	//	if ( !isValid )
	//		return isValid ;

	//	// Correct name.  Now we can move position ahead until the end.
	//	// Move past name, find first Serial, then find first
	//	// position after serial.
	//	* pos += name.length ( ) ;
	//	* pos = container.find_first_of ( SerialSeparator, * pos ) ;
	//	* pos = container.find_first_not_of ( SerialSeparator, * pos ) ;

	//	// firstPos is beginning of data after first separator substring.
	//	* firstData = * pos ;
	//	* pos = container.find_first_of ( SerialSeparator, * pos ) ;

	//	// count is verified, but pos is at a separator.  Move it to
	//	// the first position that is not a separator.
	//	//* pos = container.find_first_of ( SerialSeparator, * pos ) ;

	//	return true ;
	//}

	std::string dataNodeFor ( std::string container, std::string name,
		unsigned int pos )
	{
		size_t len = container.length ( ) ;
		unsigned int firstData = dataPositionFor ( container, name, pos ) ;
		bool isValid = firstData < len ;
		if ( !isValid )
			return "" ;

		//string dataNode = nodeAt ( container, firstData ) ;
		string dataNode = container.substr ( firstData, len - firstData ) ;
		return dataNode ;
	}

	unsigned int dataPositionFor ( std::string container, std::string name,
		unsigned int pos )
	{
		// First verify correct signal.
		pos = container.find_first_of ( name [ 0 ], pos ) ;
		bool isValid = containsAt ( container.c_str ( ), pos,
			name.c_str ( ), true ) ;
		if ( !isValid )
			return -1 ;

		// Correct name.  Now we can move position ahead until the end.
		// Move past name, find first Serial, then find first
		// position after serial.
		pos += serialHeader ( name ).length ( ) ;
		//pos = container.find_first_of ( SerialSeparator, pos ) ;
		//pos = container.find_first_not_of ( SerialSeparator, pos ) ;

		// pos is at the first character after the separator. FOUND!

		return pos ;
	}

	std::string nodeAt ( std::string container,
		unsigned int pos )
	{
		size_t len = container.length ( ) ;
		if ( pos >= len )
			return "" ;

		// Find first position after data, which is the next serial separator, or end of text.
		unsigned int end = container.find_first_of ( SerialSeparator, pos ) ;
		if ( end >= len ) // cannot substring end >= len
		{
			end = len - 1;
		}

		return container.substr ( pos, end - pos ) ;
	}

	unsigned int nextNodePosition ( std::string container,
		unsigned int pos )
	{
		pos = container.find_last_not_of ( SerialSeparator, pos ) ;
		pos = container.find_first_of ( SerialSeparator, pos ) ;
		pos = container.find_first_not_of ( SerialSeparator, pos ) ;
		return pos ;
	}

}

