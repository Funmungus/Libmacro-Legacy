
# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/comsignal.h"

using namespace std ;

namespace funlibrary
{
	const std::string ComSignal::name = "comsignal" ;

	ComSignal::ComSignal ( string newMessageDescriptor )
		: _messageDescriptor ( newMessageDescriptor )
	{	}

	string ComSignal::getMessage ( ) const
	{
		return _messageDescriptor ;
	}
	void ComSignal::setMessage ( string newMessageDescriptor )
	{
		_messageDescriptor = newMessageDescriptor ;
	}

	std::string ComSignal::type ( ) const
	{
		return name ;
	}

	bool ComSignal::send ( )
	{

		return true ;
	}

	string ComSignal::xml ( ) const
	{
		string modori = "" ;

		stringstream ss ;//create a stringstream

		ss << "<" << ISignal::name << " type=\"" << ComSignal::name << "\">\n" ;

		ss << "<string type=\"messagedescriptor\">" ;

		ss << toXml ( _messageDescriptor ) ;

		ss << "</string>\n" ;

		ss << "</" << ISignal::name << ">" ;

		modori += ss.str ( ) ; ss.flush ( ) ;

		return modori ;
	}

	void ComSignal::call ( char * name, char * value )
	{
		if ( containsAt ( name, 0, "message", true ) ||
				 containsAt ( name, 0, "descript", true ) )
		{
			setMessage ( string ( value ) ) ;
		}
	}

	string ComSignal::serialize ( ) const
	{
		stringstream ss ;
		ss << serialHeader ( ComSignal::name )
			<< getMessage ( ) << SerialSeparator ;
		string modori = ss.str ( ) ; ss.flush ( ) ;
		return modori ;
	}

	string ComSignal::deserialize ( string container, unsigned int pos )
	{
		size_t len = container.length ( ) ;

		string dataNode = dataNodeFor ( container, ComSignal::name, pos ) ;
		dataNode = nodeAt ( dataNode, 0 ) ;
		setMessage ( dataNode ) ;

		pos += serialHeader ( ComSignal::name ).length ( ) ;
		// Should now be either in separator of header, or at next node.
		pos = nextNodePosition ( container, pos ) ;
		// Now at node after this signal.

		if ( pos >= len )
			pos = len - 1 ;

		return container.substr ( pos, len - pos ) ;
	}

}

