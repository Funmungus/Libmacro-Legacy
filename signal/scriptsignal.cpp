
# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/scriptsignal.h"

using namespace std ;

namespace funlibrary
{
	const std::string ScriptSignal::name = "scriptsignal" ;

	ScriptSignal::ScriptSignal ( string script )
		: _script ( script )
	{	}

	string ScriptSignal::getScript ( ) const
	{
		return _script ;
	}
	void ScriptSignal::setScript ( string script )
	{
		this->_script = script ;
	}

	std::string ScriptSignal::type ( ) const
	{
		return name ;
	}

	bool ScriptSignal::send ( )
	{
		if ( getScript ( ) == "" )
		{
			return false ;
		}

		const char * c_command = getScript ( ).c_str ( ) ;

		system ( c_command ) ;

		return true ;
	}

	string ScriptSignal::xml ( ) const
	{
		string modori = "" ;

		stringstream ss;//create a stringstream

		ss << "<" << ISignal::name << " type=\"" << ScriptSignal::name << "\">\n" ;

		ss << "<string type=\"script\">" << toXml ( _script ) << "</string>\n" ;

		ss << "</" << ISignal::name << ">" ;

		modori += ss.str ( ); ss.flush ( );

		return modori ;
	}

	void ScriptSignal::call ( char * name, char * value )
	{
		if ( containsAt ( name, 0, "script", true ) )
		{
			setScript ( string ( value ) ) ;
		}
	}

	string ScriptSignal::serialize ( ) const
	{
		stringstream ss ;
		ss << serialHeader ( ScriptSignal::name )
			<< getScript ( ) << SerialSeparator ;

		string modori = ss.str ( ) ; ss.flush ( ) ;
		return modori ;
	}

	string ScriptSignal::deserialize ( string container, unsigned int pos )
	{
		size_t len = container.length ( ) ;

		string dataNode = dataNodeFor ( container, ScriptSignal::name, pos ) ;
		dataNode = nodeAt ( dataNode, 0 ) ;
		setScript ( dataNode ) ;

		pos += serialHeader ( ScriptSignal::name ).length ( ) ;
		// Should now be either in separator of header, or at next node.
		pos = nextNodePosition ( container, pos ) ;
		// Now at node after this signal.

		if ( pos >= len )
			pos = len - 1 ;

		return container.substr ( pos, len - pos ) ;
	}
}

