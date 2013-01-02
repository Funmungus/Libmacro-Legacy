# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/keysignal.h"

using namespace std;

namespace funlibrary
{
	const std::string KeySignal::name = "keysignal" ;
/* Static OS dependent moved to bottom
	KEY_COUNT =	length of _keyNames
	_keyInts = initKeys ( ) ;
*/

	KeySignal::KeySignal ( const int key, const int scan, const KeyUpType keyUp )
		: _key ( key ), _scan ( scan ), _keyUp ( keyUp )
	{
# ifdef LNX
		initVent ( ) ;
# endif
	}
	KeySignal::KeySignal ( const KeySignal & copytron )
		: _key ( copytron._key ), _scan ( copytron._scan ), _keyUp ( copytron._keyUp )
	{
# ifdef LNX
		initVent ( ) ;
# endif
	}

	KeySignal & KeySignal::operator = ( const KeySignal & copytron )
	{
		if ( &copytron != this )
		{
			setKey ( copytron._key ) ;
			setScan ( copytron._scan ) ;
			setKeyUp ( copytron._keyUp ) ;
		}

		return *this ;
	}

	int KeySignal::getKeyInt ( string keyString )
	{
		int modori = -1 ;

		map < string, int >::iterator it = _keyInts.find ( keyString ) ;
		if ( it == _keyInts.end ( ) )
			return modori ;

		modori = it->second ;

		return modori ;
	}

	string KeySignal::getKeyString ( const int keyInt )
	{
		string modori = "" ;

		if ( ( unsigned int ) keyInt < KEY_COUNT )
		{
			modori = _keyNames [ keyInt ] ;
		}

		return modori ;
	}

	int KeySignal::getKey ( ) const
	{
		return _key ;
	}

	void KeySignal::setKey ( const int key )
	{
		this->_key = key ;
# ifdef LNX
		_keyvent [ 1 ].code = key ;
# endif
	}

	KeyUpType KeySignal::getKeyUp ( ) const
	{
		return _keyUp ;
	}

	void KeySignal::setKeyUp ( const KeyUpType keyUp )
	{
		this->_keyUp = keyUp ;
	}

	int KeySignal::getScan ( ) const
	{
		return _scan ;
	}

	void KeySignal::setScan ( const int scan )
	{
		this->_scan = scan ;
# ifdef LNX
		_keyvent [ 0 ].value = scan ;
# endif
	}

	std::string KeySignal::type ( ) const
	{
		return name ;
	}

	string KeySignal::xml ( ) const
	{
		string modori = "" ;
		stringstream ss;

		ss << "<" << ISignal::name << " type=\"" << KeySignal::name << "\">\n" ;

		ss << "<integer type=\"key\"" ;
		
		string keyName = getKeyString ( getKey ( ) ) ;
		if ( ! keyName.empty ( ) )
			ss << " name=\"" << getKeyString ( getKey ( ) ) << "\"" ;
		ss << ">" << getKey ( ) << "</integer>\n" ;

		ss << "<integer type=\"scan\">" << getScan ( ) << "</integer>\n" ;

		ss << "<integer type=\"keyup\">" << ( int ) getKeyUp ( ) << "</integer>\n" ;

		ss << "</" << ISignal::name << ">" ;

		modori += ss.str ( ) ; ss.flush ( ) ;

		return modori ;
	}

	void KeySignal::call ( char * name, int value )
	{
		if ( containsAt ( name, 0, "keyup", true ) )
		{
			switch ( value )
			{
			case KeyUpType::DOWN :
				setKeyUp ( DOWN ) ;
				break ;
			case KeyUpType::UP :
				setKeyUp ( UP ) ;
				break ;
			case KeyUpType::BOTH :
				setKeyUp ( BOTH ) ;
				break ;
			}
		}
		else if ( containsAt ( name, 0, "key", true ) )
		{
			setKey ( value ) ;
		}
		else if ( containsAt ( name, 0, "scan", true ) )
		{
			setScan ( value ) ;
		}
	}

	string KeySignal::serialize ( ) const
	{
		static const size_t byteLen = 9 ;
		stringstream ss ;
		unsigned char bytes [ byteLen ] ;
		uint32_t * outPt = ( uint32_t * ) bytes  ;
		* outPt = ( uint32_t ) getKey ( ) ;
		// set bytes 0-3
		++ outPt ;
		* outPt = ( uint32_t ) getScan ( ) ;
		// set bytes 4-7
		bytes [ 8 ] = ( unsigned char ) getKeyUp ( ) ;
		//		bytes [ 13 ] = '\0' ;
		ss << serialHeader ( KeySignal::name ) ;
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

	string KeySignal::deserialize ( string container, unsigned int pos )
	{
		// Expecting to find 4 bytes for key, 4 for scan, 1 for keyUp.
		static const size_t byteSize = 9 ;
		size_t len = container.length ( ) ;

		string dataNode = dataNodeFor ( container, KeySignal::name, pos ) ;
		if ( dataNode.length ( ) < byteSize )
			return container ;

		const uint32_t * valPt = ( const uint32_t * ) dataNode.c_str ( ) ;
		setKey ( *valPt ) ;
		// set bytes 0-3
		++ valPt ;
		setScan ( *valPt ) ;
		// set bytes 4-7
		switch ( dataNode [ 8 ] )
		{
		case KeyUpType::DOWN :
			setKeyUp ( KeyUpType::DOWN ) ;
			break ;
		case KeyUpType::UP :
			setKeyUp ( KeyUpType::UP ) ;
			break ;
		case KeyUpType::BOTH :
			setKeyUp ( KeyUpType::BOTH ) ;
			break ;
		}

		pos += serialHeader ( name ).length ( ) ;
		// Should now be either in separator of header, or at next node.
		pos = nextNodePosition ( container, pos ) ;
		// Now at node after this signal.

		if ( pos >= len )
			pos = len - 1 ;

		return container.substr ( pos, len - pos ) ;
	}

}

# ifdef LNX
# include "lnxkeysignal.hxx"
# endif
# ifdef WIN
# include "winkeysignal.hxx"
# endif

// Some OS dependencies
namespace funlibrary
{
	const unsigned int KeySignal::KEY_COUNT =
		sizeof ( _keyNames ) / sizeof ( _keyNames [ 0 ] ) ;

	map < string, int > KeySignal::initKeys ( )
	{
		map < string, int > val ;

		string lower ;
		for ( unsigned int i = 0 ; i < KEY_COUNT ; i++ )
		{
			lower = _keyNames [ i ] ;
			val.insert ( pair <string, int> ( lower, i ) ) ;
			transform ( lower.begin ( ), lower.end ( ), lower.begin ( ), ::tolower ) ;
			val.insert ( pair <string, int> ( lower, i ) ) ;
		}

		return val ;
	}

	map < string, int > KeySignal::_keyInts = initKeys ( ) ;
}
