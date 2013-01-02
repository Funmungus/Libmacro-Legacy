// signal.cpp : Defines the exported functions for the DLL application.
//

# ifdef WIN
# include "stdafx.h"
# endif

# include "hotkey/hotkey.h"

using namespace std ;

namespace funlibrary
{
	map < string, HotkeyFactory::HotkeyReturn >
		HotkeyFactory::_functionMap ;
	map < string, HotkeyFactory::CopyReturn >
		HotkeyFactory::_copyMap ;

	HotkeyFactory::HotkeyFactory ( )
	{
		string names [] = {
			Hotkey::name, StagedHotkey::name
		} ;
		HotkeyReturn hotReturns [] = {
			&HotkeyFactory::getSimple, &HotkeyFactory::getStaged
		} ;
		CopyReturn copyReturns [] = {
			&HotkeyFactory::copySimple, &HotkeyFactory::copyStaged
		} ;

		unsigned int len = sizeof ( names ) / sizeof ( string ) ;
		for ( unsigned int i = 0 ; i < len ; i++ )
		{
			add ( names [ i ], hotReturns [ i ], copyReturns [ i ] ) ;
		}
	}

	void HotkeyFactory::add ( string lowerName , HotkeyReturn func, CopyReturn cFunc )
	{
		_functionMap.insert ( pair < string, HotkeyReturn >
													( lowerName, func ) ) ;
		_copyMap.insert ( pair < string, CopyReturn >
													( lowerName, cFunc ) ) ;
		transform ( lowerName.begin ( ), lowerName.end ( ), lowerName.begin ( ), ::toupper ) ;
		_functionMap.insert ( pair < string, HotkeyReturn >
													( lowerName, func ) ) ;
		_copyMap.insert ( pair < string, CopyReturn >
													( lowerName, cFunc ) ) ;
	}

	IHotkey * HotkeyFactory::getSimple ( )
	{
		return new Hotkey ( ) ;
	}

	IHotkey * HotkeyFactory::copySimple ( const IHotkey * inVal )
	{
		const Hotkey * tmp = dynamic_cast < const Hotkey * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new Hotkey ( *tmp ) ;
	}

	IHotkey * HotkeyFactory::getStaged ( )
	{
		return new StagedHotkey ( ) ;
	}

	IHotkey * HotkeyFactory::copyStaged ( const IHotkey * inVal )
	{
		const StagedHotkey * tmp = dynamic_cast < const StagedHotkey * > ( inVal ) ;
		if ( tmp == NULL ) return NULL ;
		return new StagedHotkey ( *tmp ) ;
	}

	IHotkey * HotkeyFactory::get ( string hotkeyName )
	{
		auto mFunc = _functionMap.find ( hotkeyName ) ;
		if ( mFunc == _functionMap.end ( ) ) return NULL ;
		return mFunc->second ( ) ;
	}

	IHotkey * HotkeyFactory::copy ( const IHotkey * inVal )
	{
		if ( inVal == NULL ) return NULL ;
		auto mFunc = _copyMap.find ( inVal->type ( ) ) ;
		if ( mFunc == _copyMap.end ( ) ) return NULL ;
		return mFunc->second ( inVal ) ;
	}
}

funlibrary::HotkeyFactory funlibrary::HotkeyFactory::_instance ;
