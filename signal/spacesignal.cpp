# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/spacesignal.h"

using namespace std ;

namespace funlibrary
{
	const std::string SpaceSignal::name = "spacesignal" ;

	// no event, no eventData, not cursorJustify, x=0, y=0, z=0, no mods
	SpaceSignal::SpaceSignal ( const int event, const SpacePosition & blip,
														 const bool cursorJustify, const int eventData)
		: _blip ( blip ), _cursorJustify ( cursorJustify ), _eventData ( eventData )
	{
		setEvent ( event ) ;
# ifdef LNX
		initVent ( ) ;
# endif
	}

	SpaceSignal::SpaceSignal ( const SpaceSignal & copytron )
		: _blip ( copytron._blip ), _cursorJustify ( copytron._cursorJustify ),
			_eventData ( copytron._eventData )
	{
		setEvent ( copytron._event ) ;
# ifdef LNX
		initVent ( ) ;
# endif
	}

	SpaceSignal & SpaceSignal::operator = ( const SpaceSignal & copytron )
	{
		if ( &copytron != this )
		{
			setEvent ( copytron._event ) ;
			setPosition ( copytron._blip ) ;
			setEnableCursorJustify ( copytron._cursorJustify ) ;
			setEventData ( copytron._eventData ) ;
		}

		return *this ;
	}

	string SpaceSignal::getEventString ( const int eventInt )
	{
		if ( (unsigned int) eventInt > FUN_SPACE_COUNT || eventInt < 0 )
			return "" ;

		return _eventName [ eventInt ] ;
	}

	int SpaceSignal::getEventInt ( string eventString )
	{
		map < string, int >::iterator it = _eventInts.find ( eventString ) ;
		if ( it == _eventInts.end ( ) )
			return -1 ;

		return it->second ;
	}

	int SpaceSignal::getEvent ( ) const
	{
		return _event ;
	}
	void SpaceSignal::setEvent ( const int event )
	{
		this->_event = event % FUN_SPACE_COUNT ;
	}

	int SpaceSignal::getEventData ( ) const
	{
		return _eventData ;
	}
	void SpaceSignal::setEventData ( const int eventData )
	{
		this->_eventData = eventData ;
	}

	bool SpaceSignal::isCursorJustify ( ) const
	{
		return _cursorJustify ;
	}
	void SpaceSignal::setEnableCursorJustify ( const bool cursorJustify )
	{
		this->_cursorJustify = cursorJustify ;
	}

	long long SpaceSignal::getX ( ) const
	{
		return _blip.x ;
	}
	void SpaceSignal::setX ( const long long x )
	{
		_blip.x = x ;
# ifdef LNX
		_relvent [ 0 ].value = _absvent [ 0 ].value = x ;
# endif
	}

	long long SpaceSignal::getY ( ) const
	{
		return _blip.y ;
	}
	void SpaceSignal::setY ( const long long y )
	{
		_blip.y = y ;
# ifdef LNX
		_relvent [ 1 ].value = _absvent [ 1 ].value = y ;
# endif
	}

	long long SpaceSignal::getZ ( ) const
	{
		return _blip.z ;
	}
	void SpaceSignal::setZ ( const long long z )
	{
		_blip.z = z ;
# ifdef LNX
		_relvent [ 2 ].value = _absvent [ 2 ].value = z ;
# endif
	}

	SpacePosition SpaceSignal::getPosition ( ) const
	{
		return _blip ;
	}
	void SpaceSignal::setPosition ( const SpacePosition & blip )
	{
		this->_blip.x = blip.x ;
		this->_blip.y = blip.y ;
		this->_blip.z = blip.z ;
# ifdef LNX
		_relvent [ 0 ].value = _absvent [ 0 ].value = blip.x ;
		_relvent [ 1 ].value = _absvent [ 1 ].value = blip.y ;
		_relvent [ 2 ].value = _absvent [ 2 ].value = blip.z ;
# endif
	}

	std::string SpaceSignal::type ( ) const
	{
		return name ;
	}

	string SpaceSignal::xml ( ) const
	{
		string modori = "";

		stringstream ss;

		ss << "<" << ISignal::name  << " type=\"" << SpaceSignal::name << "\">\n" ;

		ss << "<integer type=\"event\"" ;
		
		string name = getEventString ( getEvent ( ) ) ;
		if ( ! name.empty ( ) )
			ss << " name=\"" << getEventString ( getEvent ( ) ) << "\"" ;
		ss << ">" << getEvent ( ) << "</integer>\n" ;

		ss << "<integer type=\"eventdata\">" << getEventData ( ) << "</intenger>\n" ;

		ss << "<boolean type=\"cursorjustify\">" << isCursorJustify ( ) << "</boolean>\n" ;

		ss << "<long type=\"x\">" << getX ( ) << "</integer>\n" ;

		ss << "<long type=\"y\">" << getY ( ) << "</integer>\n" ;

		ss << "<long type=\"z\">" << getZ ( ) << "</integer>\n" ;

		ss << "</" << ISignal::name << ">" ;

		modori += ss.str ( ); ss.flush ( );

		return modori ;
	}

	void SpaceSignal::call ( char * name, bool value )
	{
		if ( containsAt ( name, 0, "cursorjustify", true ) ||
				 containsAt ( name, 0, "justify", true ) )
		{
			setEnableCursorJustify ( value ) ;
		}
	}

	void SpaceSignal::call ( char * name, int value )
	{
		if ( containsAt ( name, 0, "eventdata", true ) )
		{
			setEventData ( value ) ;
		}
		else if ( containsAt ( name, 0, "event", true ) )
		{
			setEvent ( value ) ;
		}
	}

	void SpaceSignal::call ( char * name, long value )
	{
		if ( containsAt ( name, 0, "x", true ) )
		{
			setX ( value ) ;
		}
		else if ( containsAt ( name, 0, "y", true ) )
		{
			setY ( value ) ;
		}
		else if ( containsAt ( name, 0, "z", true ) )
		{
			setZ ( value ) ;
		}
	}

	void SpaceSignal::call ( rapidxml::xml_node < char >* obj )
	{
		const unsigned int typeLen = strlen ( "type" ) ;
		bool isSig = verifyNodeName ( obj, SpacePosition::name.c_str ( ) ) ;
		if ( ! isSig )
		{
			isSig = verifyNodeAttribute ( obj, "type", typeLen, SpacePosition::name.c_str ( ) ) ;
			if ( ! isSig )
				return ;
		}

		receiveXml ( & _blip, obj ) ;

	}

	string SpaceSignal::serialize ( ) const
	{
		const size_t byteLen = 33 ;
		stringstream ss ;
		unsigned char bytes [ byteLen ] ;
		uint32_t * valPt = ( uint32_t * ) bytes ;
		* valPt = getEvent ( ) ;
		// set bytes 0-3
		++ valPt ;
		* valPt = getEventData ( ) ;
		// set bytes 4-7
		++ valPt ;
		// Done with event, and event data. move on to spatial positions, which are int64
		uint64_t * val64Pt = ( uint64_t * ) valPt ;
		* val64Pt = getX ( ) ;
		// set bytes 8-15
		++ val64Pt ;
		* val64Pt = getY ( ) ;
		// set bytes 16-23
		++ val64Pt ;
		* val64Pt = getZ ( ) ;
		// set bytes 24-31
		bytes [ 32 ] = isCursorJustify ( ) ;

		ss << serialHeader ( SpaceSignal::name ) ;
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

	string SpaceSignal::deserialize ( string container, unsigned int pos )
	{
		// Expecting to find 4 bytes for event, 4 for event data, 8 for x, 8 for y,
		// 8 for z, 1 for cursor justify.
		const size_t byteSize = 33 ;
		size_t len = container.length ( ) ;

		string dataNode = dataNodeFor ( container, SpaceSignal::name, pos ) ;
		if ( dataNode.length ( ) < byteSize )
			return container ;

		const uint32_t * valPt = ( const uint32_t * ) dataNode.c_str ( ) ;
		setEvent ( *valPt ) ;
		// set bytes 0-3
		++ valPt ;
		setEventData ( *valPt ) ;
		// set bytes 4-7
		++ valPt ;
		// Done with event, and event data. Move on to spatial positions, which are int64
		uint64_t * val64Pt = ( uint64_t * ) valPt ;
		setX ( * val64Pt ) ;
		// set bytes 8-15
		++ val64Pt ;
		setY ( * val64Pt ) ;
		// set bytes 16-23
		++ val64Pt ;
		setZ ( * val64Pt ) ;
		// set bytes 24-31
		++ val64Pt ;
		unsigned char * mPt = ( unsigned char * ) val64Pt ;
		setEnableCursorJustify ( *mPt != 0 ) ;

		pos += serialHeader ( SpaceSignal::name ).length ( ) ;
		// Should now be either in separator of header, or at next node.
		pos = nextNodePosition ( container, pos ) ;
		// Now at node after this signal.

		if ( pos >= len )
			pos = len - 1 ;

		return container.substr ( pos, len - pos ) ;
	}

}

# ifdef WIN
# include "winspacesignal.hxx"
# endif
# ifdef LNX
# include "lnxspacesignal.hxx"
# endif

// OS dependencies
namespace funlibrary
{
	const unsigned int SpaceSignal::FUN_SPACE_COUNT =
			sizeof ( _eventBtnMap ) / sizeof ( _eventBtnMap [ 0 ] ) ;

	map < string, int > SpaceSignal::initVals ( )
	{
		map < string, int > val ;
		string lower ;
		for ( unsigned int i = 0 ; i < FUN_SPACE_COUNT ; i++ )
		{
			lower = _eventName [ i ] ;
			val.insert ( pair <string, int> ( lower, i ) ) ;
			transform ( lower.begin ( ), lower.end ( ), lower.begin ( ), ::tolower ) ;
			val.insert ( pair <string, int> ( lower, i ) ) ;
		}

		return val ;
	}

	map < int, int > SpaceSignal::initBtnEvent ( )
	{
		map < int, int > val ;
		for ( unsigned int i = 0 ; i < FUN_SPACE_COUNT ; i++ )
		{
			val.insert ( pair <int, int> ( _btnEvents [ i ], i ) ) ;
		}

		return val ;
	}

	map<string, int> SpaceSignal::_eventInts = initVals ( ) ;
	map<int, int> SpaceSignal::_btnEvents = initBtnEvent ( ) ;
}
