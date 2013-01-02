
# ifdef WIN
# include "stdafx.h"
# endif

# include "macro/macro.h"

using namespace std ;

namespace funlibrary
{
	const std::string IMacro::name = "macro" ;
	const std::string Macro::name = "macro" ;

	IMacro::IMacro ( std::string name )
		: _name ( name )
	{	}

	std::string IMacro::getName ( ) const
	{
		return _name ;
	}

	void IMacro::setName ( const string name )
	{
		_name = name ;
	}

	void IMacro::call ( char * name, char * value )
	{
		if ( name == NULL || value == NULL )
		{
			if ( std::string ( name ) == "name" )
			{
				_name = value ;
			}
		}
	}


	Macro::Macro ( std::string name, IHotkey * hotkey,
					std::vector < std::unique_ptr < ISignal > > * signalSet )
		: IMacro ( name )
	{

	}

	void Macro::getSignals ( std::vector < std::unique_ptr < ISignal > > * outVal ) const
	{

	}
	void Macro::setSignals ( const std::vector < std::unique_ptr < ISignal > > * inVal )
	{

	}
	void Macro::setSignal ( const ISignal * val, const size_t pos )
	{

	}
	void Macro::removeSignal ( const size_t pos )
	{

	}
	void Macro::push_back ( const ISignal * val )
	{

	}
	std::unique_ptr < ISignal > Macro::pop_back ( )
	{

	}

	void Macro::getHotkey
		( std::unique_ptr < IHotkey > * outVal ) const
	{

	}
	void Macro::setHotkey ( const IHotkey * inVal )
	{

	}
	bool Macro::isEnabled ( ) const
	{

	}
	void Macro::enable ( const bool enable )
	{

	}

	void Macro::call ( )
	{

	}
	void Macro::call ( rapidxml::xml_node < char > * obj )
	{

	}

	std::string Macro::type ( ) const
	{

	}
	bool Macro::send ( )
	{

	}
	std::string Macro::xml ( ) const
	{

	}
	std::string Macro::serialize ( ) const
	{

	}
	std::string Macro::deserialize
		( std::string container, unsigned int pos = 0 )
	{

	}

	void Macro::setHotkey ( const Hotkey * inVal )
	{

	}
	void Macro::setHotkey ( const StagedHotkey * inVal )
	{

	}
}
