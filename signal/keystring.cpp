
# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/keystring.h"

using namespace std ;

namespace funlibrary
{
	const std::string KeyString::name = "keystring" ;

	KeyString::KeyString ( string text, const unsigned long long delay,
												 const unsigned long long interval )
		: _delay ( delay ), _interval ( interval )
	{
		setText ( text ) ;
	}

	// static
	KeyString KeyString::fromKeys ( vector < unique_ptr < ISignal > > * keySet )
	{
		if ( keySet == NULL )
			return KeyString ( ) ;

		bool shift = false ;
		bool keyDown = false ;
		unsigned int shiftMod = Mods::getMod ( "shift" ) ;
		unsigned int modVal = 0 ;
		unsigned long long delay = 0, interval = 0 ;
		string val = "" ;
		char ascii ;
//		pair < int, bool > shifted ;

		// Handle  Keys (key code and shifting), and NoOp ( delay and interval ).
		for ( auto it = keySet->begin ( ) ; it != keySet->end ( ) ; it ++ )
		{
			// Should most often be Key signal or pause
			KeySignal * key = dynamic_cast < KeySignal * > ( it->get ( ) ) ;
			if ( key != NULL )
			{
				modVal = Mods::modFromKey ( key->getKey ( ) ) ;
				if ( ( modVal & shiftMod ) != 0 )
				{
					switch ( key->getKeyUp ( ) )
					{
					case DOWN :
						shift = true ;
						break ;
					case UP :
					case BOTH :
						shift = false ;
						break ;
					}
				}
				else
				{
					switch ( key->getKeyUp ( ) )
					{
					case DOWN :
						ascii = asciiFromKey ( key->getKey ( ), shift ) ;
						keyDown = true ;
						val += ascii ;
						break ;
					case BOTH :
						ascii = asciiFromKey ( key->getKey ( ), shift ) ;
						val += ascii ;
					case UP :
						keyDown = false ;
						break ;
					}
				}
				continue ;
			}
			NoOp * pause = dynamic_cast < NoOp * > ( it->get ( ) ) ;
			if ( pause != NULL )
			{
				if ( keyDown )
				{
					delay = ( delay == 0 ) ? pause->getMillis ( ) : ( delay + pause->getMillis ( ) ) / 2 ;
				}
				else
				{
					interval = ( interval == 0 ) ? pause->getMillis ( ) : ( interval + pause->getMillis ( ) ) / 2 ;
				}
				continue ;
			}
			Mods * mod = dynamic_cast < Mods * > ( it->get ( ) ) ;
			if ( mod != NULL )
			{
				if ( mod->hasMod ( shiftMod ) )
				{
					switch ( mod->getKeyUp ( ) )
					{
					case DOWN :
						shift = false ;
						break ;
					case UP :
					case BOTH :
						shift = true ;
						break ;
					}
				}
				continue ;
			}

		}
		return KeyString ( val, delay, interval ) ;
	}
	void KeyString::toKeys ( const KeyString * container,
											 vector < unique_ptr < ISignal > > * retVals )
	{
		if ( container == NULL || retVals == NULL ) return ;

		retVals->clear ( ) ;

		string text = container->getText ( ) ;
		if ( text.empty ( ) ) return ;

		Mods shiftKey = Mods ( Mods::getMod ( "shift" ), DOWN ) ;
		bool shifted = false, toShift = false ;
		unsigned long long delay = container->getDelay ( ) ;
		unsigned long long interval = container->getInterval ( ) ;
		int newKey ;

		for ( auto it = text.begin ( ) ; it != text.end ( ) ; it ++ )
		{
			toShift = needShift ( *it ) ;
			if ( shifted )
			{
				if ( ! toShift )
				{
					shiftKey.setKeyUp ( UP ) ;
					retVals->push_back ( unique_ptr < ISignal > ( new Mods ( shiftKey ) ) ) ;
					shifted = false ;
				}
			}
			else
			{
				if ( toShift )
				{
					shiftKey.setKeyUp ( DOWN ) ;
					retVals->push_back ( unique_ptr < ISignal > ( new Mods ( shiftKey ) ) ) ;
					shifted = true ;
				}
			}

			newKey = keyFromAscii ( *it ) ;
			if ( newKey == 0 ) continue ;

			if ( delay == 0 )
			{
				retVals->push_back ( unique_ptr < ISignal > ( new KeySignal ( newKey, 0, BOTH ) ) ) ;
			}
			else
			{
				retVals->push_back ( unique_ptr < ISignal > ( new KeySignal ( newKey, 0, DOWN ) ) ) ;
				retVals->push_back ( unique_ptr < ISignal > ( new NoOp ( delay ) ) ) ;
				retVals->push_back ( unique_ptr < ISignal > ( new KeySignal ( newKey, 0, UP ) ) ) ;
			}
			if ( interval != 0 )
				retVals->push_back ( unique_ptr < ISignal > ( new NoOp ( interval ) ) ) ;
		}
	}
	// static util
	int KeyString::asciiFromKey ( const int keyVal, const bool shifted )
	{
		auto it = _asciiMap.find ( pair < int, bool > ( keyVal, shifted ) ) ;
		if ( it != _asciiMap.end ( ) )
			return ( it->second ) ;
		return 0 ;
	}
	int KeyString::keyFromAscii ( const int asciiVal )
	{
		return _keyVals [ asciiVal ] ;
	}
	bool KeyString::needShift ( const int asciiVal )
	{
		return _needShift [ asciiVal ] ;
	}

	// KeyString specific
	string KeyString::getText ( ) const
	{
		return _text ;
	}
	void KeyString::setText ( string text )
	{
		// Setting signals to send is different from creating a raw set of signals from a text.
		// We use only key signals here, and reuse the delay and interval.
		// Good news is we only have to worry about key signals and not pauses.
		// Shifting is taken care of in send, so do not deal with that here either.
		_text = text ;
		_sigs.clear ( ) ;
		KeySignal key ( 0, 0, BOTH ) ;
		for ( auto it = _text.begin ( ) ; it != _text.begin ( ) ; it ++ )
		{
			char curChar = *it ;
			key.setKey ( keyFromAscii ( curChar ) ) ;

			if ( key.getKey ( ) != 0 )
				_sigs.push_back ( key );
		}
	}
	unsigned long long KeyString::getDelay ( ) const
	{
		return _delay.getMillis ( ) ;
	}
	void KeyString::setDelay ( unsigned long long millis )
	{
		_delay.setMillis ( millis ) ;
	}
	unsigned long long KeyString::getInterval ( ) const
	{
		return _interval.getMillis ( ) ;
	}
	void KeyString::setInterval ( unsigned long long millis )
	{
		_interval.setMillis ( millis ) ;
	}

	/* ISignal implementation */

	std::string KeyString::type ( ) const
	{
		return name ;
	}

	bool KeyString::send ( )
	{
		for ( unsigned int pt = 0 ; pt < _text.size ( ) ; )
		{
			if ( needShift ( _text [ pt ] ) )
				sendingShifted ( &pt ) ;
			else
				sendingUnshifted ( &pt ) ;
		}

		return true ;
	}

	// TODO Decide if sending key code 0 returned from keyFromAscii
	void KeyString::sendingShifted ( unsigned int * pos )
	{
		Mods ( Mods::getMod ( "shift" ), DOWN ).send ( ) ;
		if ( getDelay ( ) != 0 )
			_delay.send ( ) ;
		KeySignal keys ( 0, 0, BOTH ) ;
		while ( *pos < _text.size ( ) && needShift ( _text [ *pos ] ) )
		{
			keys.setKey ( keyFromAscii ( _text [ *pos ] ) ) ;
			if ( getDelay ( ) == 0 )
			{
				keys.setKeyUp ( BOTH ) ;
				keys.send ( ) ;
			}
			else
			{
				keys.setKeyUp ( DOWN ) ;
				keys.send ( ) ;
				_delay.send ( ) ;
				keys.setKeyUp ( UP ) ;
				keys.send ( ) ;
			}

			if ( getInterval ( ) != 0 )
				_interval.send ( ) ;

			++ *pos ;
		}
	}

	void KeyString::sendingUnshifted ( unsigned int * pos )
	{
		Mods ( Mods::getMod ( "shift" ), UP ).send ( ) ;
		if ( getDelay ( ) != 0 )
			_delay.send ( ) ;
		KeySignal keys ( 0, 0, BOTH ) ;
		while ( *pos < _text.size ( ) && ! needShift ( _text [ *pos ] ) )
		{
			keys.setKey ( keyFromAscii ( _text [ *pos ] ) ) ;
			if ( getDelay ( ) == 0 )
			{
				keys.setKeyUp ( BOTH ) ;
				keys.send ( ) ;
			}
			else
			{
				keys.setKeyUp ( DOWN ) ;
				keys.send ( ) ;
				_delay.send ( ) ;
				keys.setKeyUp ( UP ) ;
				keys.send ( ) ;
			}

			if ( getInterval ( ) != 0 )
				_interval.send ( ) ;

			++ *pos ;
		}
	}

	string KeyString::xml ( ) const
	{
		string modori = "" ;
		stringstream ss;

		ss << "<" << ISignal::name << " type=\"" << KeyString::name << "\">\n" ;

		ss << "<long type=\"delay\">" << (long) getDelay ( ) << "</long>\n" ;

		ss << "<long type=\"interval\">" << (long) getInterval ( ) << "</long>\n" ;

		ss << "<string type=\"text\">" << toXml ( getText ( ) ) << "</string>\n" ;

		ss << "</" << ISignal::name << ">" ;

		modori += ss.str ( ) ; ss.flush ( ) ;

		return modori ;
	}
	string KeyString::serialize ( ) const
	{
		static const size_t byteLen = sizeof ( uint64_t ) * 2 / sizeof ( unsigned char ) ;
		stringstream ss ;
		unsigned char bytes [ byteLen ] ;
		uint64_t * outPt = ( uint64_t * ) bytes  ;
		* outPt = ( uint64_t ) getDelay ( ) ;
		++ outPt ;
		* outPt = ( uint64_t ) getInterval ( ) ;
		for ( unsigned int i = 0 ; i < byteLen ; i++ )
		{
//			bytes [ i ] &= 0xFF ;
			ss << bytes [ i ] ;
		}

		ss << getText ( ) ;
		ss << SerialSeparator ;
		string modori = ss.str ( ) ; ss.flush ( ) ;
		return modori ;
	}
	string KeyString::deserialize ( string container, unsigned int pos )
	{
		// Expecting to find 4 bytes for key, 4 for scan, 1 for keyUp.
		static const size_t byteSize = sizeof ( uint64_t ) * 2 / sizeof ( unsigned char ) ;
		size_t len = container.length ( ) ;

		string dataNode = dataNodeFor ( container, KeyString::name, pos ) ;
		if ( dataNode.length ( ) < byteSize )
			return container ;

		const uint64_t * valPt = ( const uint64_t * ) dataNode.c_str ( ) ;
		setDelay ( *valPt ) ;
		++ valPt ;
		setInterval ( *valPt ) ;

		string textVal = nodeAt ( dataNode, byteSize ) ;

		pos += serialHeader ( name ).length ( ) ;
		// Should now be either in separator of header, or at next node.
		pos = nextNodePosition ( container, pos ) ;
		// Now at node after this signal.

		if ( pos >= len )
			pos = len - 1 ;

		return container.substr ( pos, len - pos ) ;
	}

	/* ICall */
	void KeyString::call ( char * name, long value )
	{
		if ( name == NULL ) return ;

		if ( containsAt ( name, 0, "delay", true ) )
		{
			_delay.setMillis ( value ) ;
		}
		else if ( containsAt ( name, 0, "interval", true ) )
		{
			_interval.setMillis ( value ) ;
		}
	}

	void KeyString::call ( char * name, char * value )
	{
		if ( name == NULL ) return ;

		if ( value == NULL )
		{
			setText ( "" ) ;
			return ;
		}

		if ( containsAt ( name, 0, "text", true ) )
		{
			setText ( string ( value ) ) ;
		}
	}
}

# ifdef LNX
# include "lnxkeystring.hxx"
# endif
# ifdef WIN
# include "winkeystring.hxx"
# endif

// OS dependency
namespace funlibrary
{
	// Map a key code and shift to ascii code
	map < pair < int, bool >, int > KeyString::initAsciiMap ( )
	{
		int len = sizeof ( _keyVals ) / sizeof ( _keyVals [ 0 ] ) ;
		map < pair < int, bool >, int > val ;

		for ( int i = 0 ; i < len ; i ++ )
		{
			pair < int, bool > keyShifted = pair < int, bool > ( _keyVals [ i ], _needShift [ i ] ) ;
			val.insert ( pair < pair < int, bool >, int > ( keyShifted, i ) ) ;
		}

		return val ;
	}

	const map < pair < int, bool >, int > KeyString::_asciiMap = initAsciiMap ( ) ;
}
