# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/mods.h"

using namespace std ;

namespace funlibrary
{
	const string Mods::name = "mods" ;

	const string Mods::MODSET [ ] = { FUN_ALT, FUN_CTRL, FUN_SHIFT, FUN_META } ;
	const unsigned int Mods::MODSET_LENGTH =
			sizeof ( MODSET ) / sizeof ( MODSET [ 0 ] ) ;

	// static map name to value
	map < string, unsigned int > Mods::initVals ( )
	{
		map < string, unsigned int > val ;

		unsigned int modval = 1 ;
		unsigned int len = sizeof ( MODSET ) / sizeof ( MODSET [ 0 ] ) ;

		for ( unsigned int i = 0 ; i < len ; i ++ )
		{
			string lower = MODSET [ i ] ;
			val.insert ( pair < string, unsigned int > ( lower, modval ) ) ;
			transform ( lower.begin ( ), lower.end ( ), lower.begin ( ), ::tolower ) ;
			val.insert ( pair < string, unsigned int > ( lower, modval ) ) ;
			modval <<= 1 ;	// modval * 2
			// unique mod bits
		}

		return val ;
	}

	// static map value to name
	map < unsigned int, string > Mods::initNames ( )
	{
		map < unsigned int, string > val ;

		unsigned int modval = 1 ;
		unsigned int len = sizeof ( MODSET ) / sizeof ( MODSET [ 0 ] ) ;

		for ( unsigned int i = 0 ; i < len ; i ++ )
		{
			val.insert ( pair < unsigned int, string > ( modval, MODSET [ i ] ) ) ;
			modval <<= 1 ;	// modval * 2
			// unique mod bits
		}

		return val ;
	}

	map < string, unsigned int > Mods::_modVals = initVals ( ) ;
	map < unsigned int, string > Mods::_modNames = initNames ( ) ;

	string Mods::getModName ( const unsigned int modVal )
	{
		string modori = "" ;

		map < unsigned int, string >::iterator it = _modNames.find ( modVal ) ;
		if ( it == _modNames.end ( ) )
			return modori ;

		modori = it->second ;

		return modori ;
	}

	unsigned int Mods::getMod ( string modName )
	{
		unsigned int modori = (unsigned int) -1 ;

		map < string, unsigned int >::iterator it = _modVals.find ( modName ) ;
		if ( it == _modVals.end ( ) )
			return modori ;

		modori = it->second ;

		return modori ;
	}

	Mods::Mods ( const unsigned int values, const KeyUpType keyUp )
		: _values ( values ), _keyUp ( keyUp )
	{	}

	unsigned int Mods::get ( ) const
	{
		return _values ;
	}

	void Mods::set ( const unsigned int newMod )
	{
		this->_values = newMod;
	}

	void Mods::set ( const unsigned int * newMods, const unsigned int length )
	{
		unsigned int temp = 0 ;

		for ( unsigned int i = 0 ; i < length ; i++ )
		{

			temp = temp | newMods [ i ] ;

		}

		set ( temp );

	}

	void Mods::set ( const long newMod )
	{
		unsigned int modori = newMod ;
		if ( (int) modori < 0 )
			modori *= -1 ;

		set ( modori ) ;
	}

	void Mods::add ( const unsigned int newMod )
	{
		if ( newMod == 0 )
			return ;

		unsigned int temp = get ( ) | newMod ;
		set ( temp ) ;
	}

	bool Mods::hasMod ( const unsigned int modVal ) const
	{
		unsigned int temp ;

		temp = get ( ) & modVal ;

		bool hasMod = temp == modVal ;

		return hasMod ;
	}

	void Mods::remove ( const unsigned int delMod )
	{
		unsigned int temp = ~ delMod ;
		temp &= get ( ) ;

		set ( temp ) ;
	}

	KeyUpType Mods::getKeyUp ( ) const
	{
		return _keyUp ;
	}

	void Mods::setKeyUp ( KeyUpType keyUp )
	{
		_keyUp = keyUp ;
	}

	std::string Mods::type ( ) const
	{
		return name ;
	}

	string Mods::xml ( ) const
	{
		string modori = "" ;

		stringstream ss;//create a stringstream

		ss << "<" << ISignal::name << " type=\"" << Mods::name << "\">\n" ;

		ss << "<unsigned type=\"sparkles\">" << get ( ) << "</unsigned>\n" ;

		ss << "<integer type=\"keyup\">" << ( int ) getKeyUp ( ) << "</integer>\n" ;

		ss << "</" << ISignal::name << ">" ;

		modori += ss.str ( ); ss.flush ( );

		return modori ;
	}

	void Mods::call ( char * name, int value )
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
		else if ( containsAt ( name, 0, "value", true ) ||
							containsAt ( name, 0, "sparkle", true ) )
		{
			set ( ( unsigned int ) value ) ;
		}
	}

	void Mods::call ( char * name, unsigned int value )
	{
		if ( containsAt ( name, 0, "value", true ) ||
									containsAt ( name, 0, "sparkle", true ) )
		{
			set ( value ) ;
		}
	}

	string Mods::serialize ( ) const
	{
		const size_t byteLen = 5 ;

		stringstream ss ;
		char bytes [ byteLen ] ;
		uint32_t * outPt = ( uint32_t * ) bytes ;
		* outPt = get ( ) ;
		bytes [ 4 ] = ( char ) getKeyUp ( ) ;

		ss << serialHeader ( Mods::name ) ;
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

	string Mods::deserialize ( string container, unsigned int pos )
	{
		// Expecting 4 bytes for mods, 1 for keyUp.
		const size_t byteSize = 5 ;
		size_t len = container.length ( ) ;

		string dataNode = dataNodeFor ( container, Mods::name, pos ) ;
		if ( dataNode.length ( ) < byteSize )
			return container ;

		const uint32_t * valPt = ( const uint32_t * ) dataNode.c_str ( ) ;
		set ( *valPt ) ;
		// set bytes 0-3
		++ valPt ;
		switch ( ( char ) *valPt )
		{
		case KeyUpType::DOWN:
			setKeyUp ( KeyUpType::DOWN ) ;
			break ;
		case KeyUpType::UP:
			setKeyUp ( KeyUpType::UP ) ;
			break ;
		case KeyUpType::BOTH:
			setKeyUp ( KeyUpType::BOTH ) ;
			break ;
		}
		
		pos += serialHeader ( Mods::name ).length ( ) ;
		// Should now be either in separator of header, or at next node.
		pos = nextNodePosition ( container, pos ) ;
		// Now at node after this signal.

		if ( pos >= len )
			pos = len - 1 ;

		return container.substr ( pos, len - pos ) ;
	}

//	void modify ( const Mods & values, KeyUpType keyUp )
//	{
//		if ( keyUp == KeyUpType::UP )
//		{
//			InternalMods.remove ( values.get ( ) ) ;
//		}
//		else
//		{
//			InternalMods.add ( values.get ( ) ) ;
//		}

//		if ( keyUp == KeyUpType::BOTH )
//			InternalMods.remove ( values.get ( ) ) ;
//	}
}

# ifdef WIN
# include "winmods.hxx"
# endif
# ifdef LNX
# include "lnxmods.hxx"
# endif

funlibrary::Mods funlibrary::InternalMods ( 0, KeyUpType::DOWN ) ;

