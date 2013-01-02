// signal.cpp : Defines the exported functions for the DLL application.
//

# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/signal.h"

using namespace std ;

namespace funlibrary
{
	map < string, SignalFactory::SignalReturn >
		SignalFactory::_functionMap ;
	map < string, SignalFactory::CopyReturn >
		SignalFactory::_copyMap ;

	SignalFactory::SignalFactory ( )
	{
		string names [] = {
			KeySignal::name, SpaceSignal::name, Mods::name,
			ScriptSignal::name, ComSignal::name, NoOp::name,
			KeyString::name
		} ;
		SignalReturn sigReturns [] = {
			&SignalFactory::getKey, &SignalFactory::getSpace, &SignalFactory::getMods,
			&SignalFactory::getScript, &SignalFactory::getCom, &SignalFactory::getNoOp,
			&SignalFactory::getKeyString
		} ;
		CopyReturn copyReturns [] = {
			&SignalFactory::copyKey, &SignalFactory::copySpace, &SignalFactory::copyMods,
			&SignalFactory::copyScript, &SignalFactory::copyCom, &SignalFactory::copyNoOp,
			&SignalFactory::copyKeyString
		} ;

		unsigned int len = sizeof ( names ) / sizeof ( string ) ;
		for ( unsigned int i = 0 ; i < len ; i++ )
		{
			add ( names [ i ], sigReturns [ i ], copyReturns [ i ] ) ;
		}
	}

	void SignalFactory::add ( string lowerName , SignalReturn func, CopyReturn cFunc )
	{
		_functionMap.insert ( pair < string, SignalReturn >
													( lowerName, func ) ) ;
		_copyMap.insert ( pair < string, CopyReturn >
													( lowerName, cFunc ) ) ;
		transform ( lowerName.begin ( ), lowerName.end ( ), lowerName.begin ( ), ::toupper ) ;
		_functionMap.insert ( pair < string, SignalReturn >
													( lowerName, func ) ) ;
		_copyMap.insert ( pair < string, CopyReturn >
													( lowerName, cFunc ) ) ;
	}

	ISignal * SignalFactory::getMods ( )
	{
		return new Mods ( ) ;
	}
	ISignal * SignalFactory::getKey ( )
	{
		return new KeySignal ( ) ;
	}
	ISignal * SignalFactory::getSpace ( )
	{
		return new SpaceSignal ( ) ;
	}
	ISignal * SignalFactory::getScript ( )
	{
		return new ScriptSignal ( ) ;
	}
	ISignal * SignalFactory::getCom ( )
	{
		return new ComSignal ( ) ;
	}
	ISignal * SignalFactory::getNoOp ( )
	{
		return new NoOp ( ) ;
	}
	ISignal * SignalFactory::getKeyString ( )
	{
		return new KeyString ( ) ;
	}

	ISignal * SignalFactory::copyKey ( const ISignal * inVal )
	{
		const KeySignal * tmp = dynamic_cast < const KeySignal * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new KeySignal ( *tmp ) ;
	}
	ISignal * SignalFactory::copySpace ( const ISignal * inVal )
	{
		const SpaceSignal * tmp = dynamic_cast < const SpaceSignal * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new SpaceSignal ( *tmp ) ;
	}
	ISignal * SignalFactory::copyMods ( const ISignal * inVal )
	{
		const Mods * tmp = dynamic_cast < const Mods * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new Mods ( *tmp ) ;
	}
	ISignal * SignalFactory::copyScript ( const ISignal * inVal )
	{
		const ScriptSignal * tmp = dynamic_cast < const ScriptSignal * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new ScriptSignal ( *tmp ) ;
	}
	ISignal * SignalFactory::copyCom ( const ISignal * inVal )
	{
		const ComSignal * tmp = dynamic_cast < const ComSignal * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new ComSignal ( *tmp ) ;
	}
	ISignal * SignalFactory::copyNoOp ( const ISignal * inVal )
	{
		const NoOp * tmp = dynamic_cast < const NoOp * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new NoOp ( *tmp ) ;
	}
	ISignal * SignalFactory::copyKeyString ( const ISignal * inVal )
	{
		const KeyString * tmp = dynamic_cast < const KeyString * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new KeyString ( *tmp ) ;
	}

	ISignal * SignalFactory::get ( string signalName )
	{
		auto mFunc = _functionMap.find ( signalName ) ;
		if ( mFunc == _functionMap.end ( ) ) return NULL ;
		return mFunc->second ( ) ;
	}

	ISignal * SignalFactory::copy ( const ISignal * inVal )
	{
		if ( inVal == NULL ) return NULL ;
		auto mFunc = _copyMap.find ( inVal->type ( ) ) ;
		if ( mFunc == _copyMap.end ( ) ) return NULL ;
		return mFunc->second ( inVal ) ;
	}
}

funlibrary::SignalFactory funlibrary::SignalFactory::_instance ;
