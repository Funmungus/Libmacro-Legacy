/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */


# include "macro/macro.h"

namespace macro
{
	const std::string Macro::name = "macro" ;

	Macro::Macro ( std::string name, IHotkey * hotkey,
				std::vector < std::unique_ptr < ISignal > > * signalSet,
				const bool enable, const bool sticky )
		: _name ( name ), _hotkey ( nullptr ), _signalSet ( ), _enable ( false ),
		_sticky ( sticky )
	{
		setHotkey ( hotkey ) ;
		setSignals ( signalSet ) ;
		this->enable ( enable ) ;
	}
	Macro::Macro ( const Macro & copytron )
		: _name ( copytron._name ), _hotkey ( nullptr ), _signalSet ( ),
		_enable ( false ), _sticky ( copytron._sticky )
	{
		setHotkey ( copytron._hotkey.get ( ) ) ;
		setSignals ( & copytron._signalSet ) ;
		enable ( copytron._enable ) ;
	}

	Macro & Macro::operator= ( const Macro & copytron )
	{
		if ( & copytron != this )
		{
			_name = copytron._name ;
			_sticky = copytron._sticky ;
			setHotkey ( copytron._hotkey.get ( ) ) ;
			setSignals ( & copytron._signalSet ) ;
			enable ( copytron._enable ) ;
		}
		return * this ;
	}

	Macro::~Macro ( )
	{
		interrupt ( ) ;
	}

	std::string Macro::getName ( ) const
	{
		return _name ;
	}
	void Macro::setName ( const std::string & name )
	{
		_name = name ;
	}

	void Macro::getHotkey
		( std::unique_ptr < IHotkey > * outVal ) const
	{
		if ( outVal != NULL )
		{
			ISignal * tmp = SignalFactory::copy
					( dynamic_cast < ISignal * > ( _hotkey.get ( ) ) ) ;
			outVal->reset ( reinterpret_cast < IHotkey * > ( tmp ) ) ;
		}
	}
	void Macro::setHotkey ( const IHotkey * inVal )
	{
		ISignal * tmp = SignalFactory::copy
				( dynamic_cast < const ISignal * > ( inVal ) ) ;
		IHotkey * obj = reinterpret_cast < IHotkey * > ( tmp ) ;
		if ( obj != NULL )
		{
			obj->setCallbackObject ( this ) ;
			obj->enable ( _enable ) ;
			_hotkey.reset ( obj ) ;
		}
	}
	bool Macro::isEnabled ( ) const
	{
		if ( _hotkey.get ( ) == NULL )
		{
			return false ;
		}
		else
		{
			return _enable && _hotkey->isEnabled ( ) ;
		}
	}
	void Macro::enable ( const bool enable )
	{
		_enable = enable ;
		if ( _hotkey.get ( ) != NULL )
		{
			_hotkey->enable ( enable ) ;
		}
	}
	bool Macro::isSticky ( ) const
	{
		return _sticky ;
	}
	void Macro::setSticky ( const bool sticky )
	{
		_sticky = sticky ;
	}
	bool Macro::isRunning ( ) const
	{
		return _continuing ;
	}
	void Macro::interrupt ( )
	{
		_continuing = false ;
		_triggerLock.lock ( ) ;
		_continuing = false ;
		_triggerLock.unlock ( ) ;
	}

	void Macro::getSignals ( std::vector < std::unique_ptr < ISignal > > * outVal ) const
	{
		if ( outVal == NULL ) return ;

		outVal->clear ( ) ;
		for ( auto it = _signalSet.begin ( ) ; it != _signalSet.end ( ) ; it++ )
		{
			ISignal * obj = SignalFactory::copy ( it->get ( ) ) ;
			if ( obj != NULL )
			{
				outVal->push_back ( std::unique_ptr<ISignal> ( obj ) ) ;
			}
		}
	}
	void Macro::setSignals
		( const std::vector < std::unique_ptr < ISignal > > * inVal )
	{
		_signalSet.clear ( ) ;

		if ( inVal == NULL ) return ;

		for ( auto it = inVal->begin ( ) ; it != inVal->end ( ) ; it++ )
		{
			ISignal * obj = SignalFactory::copy ( it->get ( ) ) ;
			if ( obj != NULL )
			{
				_signalSet.push_back ( std::unique_ptr<ISignal> ( obj ) ) ;
			}
		}
	}
	void Macro::setSignals ( const ISignal * vals, const size_t len )
	{
		_signalSet.clear ( ) ;

		if ( vals == NULL ) return ;

		for ( size_t i = 0 ; i < len ; i++ )
		{
			ISignal * obj = SignalFactory::copy ( & vals [ i ] ) ;
			if ( obj != NULL )
			{
				_signalSet.push_back ( std::unique_ptr<ISignal> ( obj ) ) ;
			}
		}
	}
	void Macro::removeSignal ( const size_t pos )
	{
		if ( pos < _signalSet.size ( ) )
		{
			_signalSet.erase ( _signalSet.begin ( ) +pos ) ;
		}
	}
	void Macro::push_back ( const ISignal * val )
	{
		if ( val != NULL )
		{
			ISignal * obj = SignalFactory::copy ( val ) ;
			_signalSet.push_back ( std::unique_ptr<ISignal> ( obj ) ) ;
		}
	}
	std::unique_ptr < ISignal > Macro::pop_back ( )
	{
		std::unique_ptr < ISignal > pt ;
		pt.swap ( * ( _signalSet.rbegin ( ) ) ) ;
		_signalSet.pop_back ( ) ;
		return std::move ( pt ) ;
	}

	std::string Macro::type ( ) const
	{
		return name ;
	}
	std::string Macro::xml ( const std::string & extraAttributes ) const
	{
		std::string modori = "<" ;

		modori.append ( name ) ;
		if ( ! extraAttributes.empty ( ) )
			modori.append ( " " ).append ( extraAttributes ) ;

		modori.append ( ">\n" )
			.append ( "<name>" ).append ( _name ).append ( "</name>\n" )
			.append ( "<enable>" ).append ( std::to_string ( _enable ) ).append ( "</enable>\n" )
			.append ( "<sticky>" ).append ( std::to_string ( _sticky ) ).append ( "</sticky>\n" ) ;
		ISignal * tmp = dynamic_cast < ISignal * > ( _hotkey.get ( ) ) ;
		if ( tmp != NULL )
		{
			modori.append ( tmp->xml ( "name=\"hotkey\"" ) ).append ( "\n" ) ;
		}
		if ( ! _signalSet.empty ( ) )
		{
			modori.append ( "<set>" ) ;
			for ( auto it = _signalSet.begin ( ) ; it != _signalSet.end ( ) ; it ++ )
			{
				modori.append ( ( * it )->xml ( ) ).append ( "\n" ) ;
			}
			modori.append ( "</set>\n" ) ;
		}
		modori.append ( "</" ).append ( name ).append ( ">\n" ) ;

		return modori ;
	}
	void Macro::receive ( const std::string & name, const std::string & value )
	{
		bool bVal ;
		if ( containsAt ( name.c_str ( ), 0, "name", true ) )
		{
			_name = value ;
		}
		else if ( containsAt ( name.c_str ( ), 0, "enable", true ) )
		{
			if ( convertBool ( value, & bVal ) )
			{
				enable ( bVal ) ;
			}
		}
		else if ( containsAt ( name.c_str ( ), 0, "sticky", true ) )
		{
			if ( convertBool ( value, & bVal ) )
			{
				setSticky ( bVal ) ;
			}
		}
	}
}
