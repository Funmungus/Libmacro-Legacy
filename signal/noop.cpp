
# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/noop.h"

using namespace std ;

namespace funlibrary
{

	const std::string NoOp::name = "noop" ;

	NoOp::NoOp ( const unsigned long long millis )
		: _millis ( millis )
	{	}

	unsigned long long NoOp::getMillis ( ) const
	{
		return _millis ;
	}
	void NoOp::setMillis ( const unsigned long long millis )
	{
		_millis = millis ;
	}

	std::string NoOp::type ( ) const
	{
		return name ;
	}

	bool NoOp::send ( )
	{
		if ( _millis == 0 ) return true ;

		this_thread::sleep_for ( chrono::duration<int, milli> ( _millis ) );

		return true ;
	}

	string NoOp::xml ( ) const
	{
		string modori = "" ;

		stringstream ss ;//create a stringstream

		ss << "<" << ISignal::name << " type=\"" << NoOp::name << "\">\n" ;

		ss << "<long type=\"millis\">" ;

		ss << getMillis ( ) ;

		ss << "</long>\n" ;

		ss << "</" << ISignal::name << ">" ;

		modori += ss.str ( ) ; ss.flush ( ) ;

		return modori ;
	}

	void NoOp::call ( char * name, long value )
	{
		if ( containsAt ( name, 0, "millis", true ) )
		{
			if ( value < 0 )
			{
				setMillis ( ( unsigned long long ) ( ( long long ) value ) ) ;
			}
			else
			{
				setMillis ( value ) ;
			}
		}
	}

	string NoOp::serialize ( ) const
	{
		const size_t byteLen = 8 ;
		stringstream ss ;
		unsigned char bytes [ byteLen ] ;
		uint64_t * outPt = ( uint64_t * ) bytes ;
		* outPt = getMillis ( ) ;
		/*uint32_t * outPt = ( uint32_t * ) bytes ;
		* outPt = getMillis ( ) >> 32 ;
		++ outPt ;
		* outPt = getMillis ( ) & (uint32_t)-1 ;*/

		ss << serialHeader ( NoOp::name ) ;
		// Account for size of char not equal to 0x08.
		for ( unsigned int i = 0 ; i < byteLen ; i++ )
		{
			bytes [ i ] &= 0xFF ;
			ss << bytes [ i ] ;
		}

		ss << SerialSeparator ;
		string modori = ss.str ( ) ; ss.flush ( ) ;
		return modori ;
	}

	string NoOp::deserialize ( string container, unsigned int pos )
	{
		// Expecting 8 bytes for millis.
		const size_t byteSize = 8 ;
		size_t len = container.length ( ) ;

		string dataNode = dataNodeFor ( container, NoOp::name, pos ) ;
		if ( dataNode.length ( ) < byteSize )
			return container ;

		const uint64_t * valPt = ( const uint64_t * ) dataNode.c_str ( ) ;
		setMillis ( *valPt ) ;
		/*const uint32_t * valPt = ( const uint32_t * ) dataNode.c_str ( ) ;
		uint64_t val = ( (uint64_t) * valPt ) << 32 ;
		val |= * ( ++valPt )  ;*/
		// set bytes 0-3

		pos += serialHeader ( NoOp::name ).length ( ) ;
		// Should now be either in separator of header, or at next node.
		pos = nextNodePosition ( container, pos ) ;
		// Now at node after this signal.

		if ( pos >= len )
			pos = len - 1 ;

		return container.substr ( pos, len - pos ) ;
	}
}

