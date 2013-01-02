
# ifdef WIN
# include "stdafx.h"
# endif

# include "hotkey/keyintercept.h"

using namespace std ;

namespace funlibrary
{
	mutex KeyIntercept::_enableLock ;

	bool KeyIntercept::_enableUnspecific = false ;
	vector < ICall < bool, KeySignal *, Mods * > * > KeyIntercept::_unspecifieds ;

	bool KeyIntercept::_enableSpecific = false ;
	map < pair < int, int >, vector < ICall < bool, KeySignal *, Mods * > * > >
		KeyIntercept::_specifiedKeys [ 2 ] ;
	map < pair < int, int >, vector < ICall < bool, KeySignal *, Mods * > * > >
		KeyIntercept::_specifiedScans [ 2 ] ;

	// Event callbacks
	void KeyIntercept::enableUnspecified ( const bool enable )
	{
		_enableLock.lock ( ) ;
		// Need enable if enabling, and neither type enabled.
		bool toEnable = enable && ! isEnabled ( ) ;
		// Need disable if disabling, currently type enabled, and other is not enabled.
		bool toDisable = ! enable && _enableUnspecific && ! _enableSpecific ;
		_enableUnspecific = enable ;
		_enableLock.unlock ( ) ;
		if ( toEnable )
			KeyIntercept::enable ( true ) ;
		if ( toDisable )
			KeyIntercept::enable ( false ) ;
	}

	void KeyIntercept::add ( ICall < bool, KeySignal *, Mods * > * newCallback )
	{
		_unspecifieds.push_back ( newCallback ) ;
	}

	void KeyIntercept::remove ( ICall < bool, KeySignal *, Mods * > * delCallback )
	{
		for ( auto it = _unspecifieds.begin ( ) ;
			it != _unspecifieds.end ( ) ; it ++ )
		{
			while ( it != _unspecifieds.end ( ) && *it == delCallback )
			{
				it = _unspecifieds.erase ( it ) ;
			}
			if ( it == _unspecifieds.end ( ) )
				break ;
		}
	}

	void KeyIntercept::enableSpecified ( const bool enable )
	{
		_enableLock.lock ( ) ;
		// Need enable if enabling, and neither type enabled.
		bool toEnable = enable && ! isEnabled ( ) ;
		// Need disable if disabling, currently type enabled, and other is not enabled.
		bool toDisable = ! enable && ! _enableUnspecific && _enableSpecific ;
		_enableSpecific = enable ;
		_enableLock.unlock ( ) ;
		if ( toEnable )
			KeyIntercept::enable ( true ) ;
		if ( toDisable )
			KeyIntercept::enable ( false ) ;
	}

	// mod upType both means everything, upType up means anything but given.
	// 
	void KeyIntercept::addForKeycode ( ICall < bool, KeySignal *, Mods * > * newCallback,
									   KeySignal key, Mods mod )
	{
		pair < int, int > modified ( key.getKey ( ), ( int ) mod.get ( ) ) ;

		if ( mod.getKeyUp ( ) == KeyUpType::BOTH )
		{
			mod.setKeyUp ( KeyUpType::DOWN ) ;
			addForKeycode ( newCallback, key, mod ) ;
			mod.setKeyUp ( KeyUpType::UP ) ;
		}

		if ( mod.getKeyUp ( ) == KeyUpType::UP )
		{
			// Mods length 4, binary 1111, hex F
			modified.second = ( ~ modified.second ) & 0x0F ;
		}

		KeyUpType upType = key.getKeyUp ( ) == KeyUpType::BOTH ? KeyUpType::DOWN : key.getKeyUp ( ) ;

		auto it = _specifiedKeys [ upType ].find ( modified ) ;
		if ( it == _specifiedKeys [ upType ].end ( ) )
		{
			_specifiedKeys [ upType ] [ modified ].push_back ( newCallback ) ;
		}
		else
		{
			it->second.push_back ( newCallback ) ;
		}

		if ( key.getKeyUp ( ) == KeyUpType::BOTH )
		{
			key.setKeyUp ( KeyUpType::UP ) ;
			addForKeycode ( newCallback, key, mod ) ;
		}
	}

	void KeyIntercept::addForScancode ( ICall < bool, KeySignal *, Mods *> * newCallback,
										KeySignal scan, Mods mod )
	{
		pair < int, int > modified ( scan.getScan ( ), ( int ) mod.get ( ) ) ;

		if ( mod.getKeyUp ( ) == KeyUpType::BOTH )
		{
			mod.setKeyUp ( KeyUpType::DOWN ) ;
			addForScancode ( newCallback, scan, mod ) ;
			mod.setKeyUp ( KeyUpType::UP ) ;
		}

		if ( mod.getKeyUp ( ) == KeyUpType::UP )
		{
			// Mods length 4, binary 1111, hex F
			modified.second = ( ~ modified.second ) & 0x0F ;
		}

		KeyUpType upType = scan.getKeyUp ( ) == KeyUpType::BOTH ? KeyUpType::DOWN : scan.getKeyUp ( ) ;
		
		auto it = _specifiedScans [ upType ].find ( modified ) ;
		if ( it == _specifiedScans [ upType ].end ( ) )
		{
			_specifiedScans [ upType ] [ modified ].push_back ( newCallback ) ;
		}
		else
		{
			it->second.push_back ( newCallback ) ;
		}

		if ( scan.getKeyUp ( ) == KeyUpType::BOTH )
		{
			scan.setKeyUp ( KeyUpType::UP ) ;
			addForScancode ( newCallback, scan, mod ) ;
		}
	}

	void KeyIntercept::removeForKeycode ( ICall < bool, KeySignal *, Mods * > * delCallback,
										  KeySignal key, Mods mod )
	{
		pair < int, int > modified ( key.getKey ( ), ( int ) mod.get ( ) ) ;
		if ( mod.getKeyUp ( ) == KeyUpType::BOTH )
		{
			mod.setKeyUp ( KeyUpType::DOWN ) ;
			removeForKeycode ( delCallback, key, mod ) ;
			mod.setKeyUp ( KeyUpType::UP ) ;
		}

		if ( mod.getKeyUp ( ) == KeyUpType::UP )
		{
			// Mods length 4, binary 1111, hex F
			modified.second = ( ~ modified.second ) & 0x0F ;
		}

		KeyUpType upType = key.getKeyUp ( ) == KeyUpType::BOTH ? KeyUpType::DOWN : key.getKeyUp ( ) ;

		auto it = _specifiedKeys [ upType ].find ( modified ) ;
		if ( it != _specifiedKeys [ upType ].end ( ) )
		{
			for ( auto dIt = it->second.begin ( ) ;
				dIt != it->second.end ( ) ; dIt ++ )
			{
				while ( dIt != it->second.end ( ) && *dIt == delCallback )
				{
					dIt = it->second.erase ( dIt ) ;
				}
				if ( dIt == it->second.end ( ) )
					break ;
			}
		}

		if ( key.getKeyUp ( ) != KeyUpType::BOTH )
		{
			key.setKeyUp ( KeyUpType::UP ) ;
			removeForKeycode ( delCallback, key, mod ) ;
		}
 	}

	void KeyIntercept::removeForScancode ( ICall < bool, KeySignal *, Mods * > * delCallback,
										   KeySignal scan, Mods mod )
	{
		pair < int, int > modified ( scan.getScan ( ), ( int ) mod.get ( ) ) ;
		if ( mod.getKeyUp ( ) == KeyUpType::BOTH )
		{
			mod.setKeyUp ( KeyUpType::DOWN ) ;
			removeForScancode ( delCallback, scan, mod ) ;
			mod.setKeyUp ( KeyUpType::UP ) ;
		}

		if ( mod.getKeyUp ( ) == KeyUpType::UP )
		{
			modified.second = ( ~ modified.second ) & 0x0F ;
		}

		KeyUpType upType = scan.getKeyUp ( ) == KeyUpType::BOTH ? KeyUpType::DOWN : scan.getKeyUp ( ) ;

		auto it = _specifiedScans [ upType ].find ( modified ) ;
		if ( it != _specifiedScans [ upType ].end ( ) )
		{
			for ( auto dIt = it->second.begin ( ) ;
				dIt != it->second.end ( ) ; dIt ++ )
			{
				while ( dIt != it->second.end ( ) && *dIt == delCallback )
				{
					dIt = it->second.erase ( dIt ) ;
				}
				if ( dIt == it->second.end ( ) )
					break ;
			}
		}

		if ( scan.getKeyUp ( ) != KeyUpType::BOTH )
		{
			scan.setKeyUp ( KeyUpType::UP ) ;
			removeForScancode ( delCallback, scan, mod ) ;
		}
	}

	// need to call specific first
	// I do not want to deal with key up type of mods. assume down. user can handle up type
	bool KeyIntercept::trigger ( KeySignal * intercepted, Mods * mod )
	{
		if ( intercepted == NULL || mod == NULL )
		{
			fprintf ( stderr, "KeyIntercept trigger : KeySignal or mod is null.\n" ) ;
			return true ;
		}

		// UpType BOTH should always be virtual.  It is ok to add processing time to virtual.
		// Still do not want to deal with mods
		if ( intercepted->getKeyUp ( ) == KeyUpType::BOTH )
		{
			intercepted->setKeyUp ( KeyUpType::DOWN ) ;
			trigger ( intercepted, mod ) ;
			intercepted->setKeyUp ( KeyUpType::UP ) ;
		}
		bool block = false ;//, needGKey = false, needGScan = false, needGType = false, needGMod = false ;

		// Verified not to be upType both
		KeyUpType upType = intercepted->getKeyUp ( ) ;

		if ( _enableSpecific )
		{
			pair < int, int > key ( intercepted->getScan ( ), ( int ) mod->get ( ) ) ;

			auto kIt = _specifiedScans [ upType ].find ( key ) ;
			vector < ICall < bool, KeySignal *, Mods * > * >::iterator i ;
			if ( kIt != _specifiedScans [ upType ].end ( ) )
			{
				for ( i = kIt->second.begin ( ) ; i != kIt->second.end ( ) ; i++ )
				{
					block = (*i)->call ( intercepted, mod ) ;
					if ( block ) 
						return true ;
				}
			}

			key.first = intercepted->getKey ( ) ;
			kIt = _specifiedKeys [ upType ].find ( key ) ;
			if ( kIt != _specifiedKeys [ upType ].end ( ) )
			{
				for ( i = kIt->second.begin ( ) ; i != kIt->second.end ( ) ; i++ )
				{
					block = (*i)->call ( intercepted, mod ) ;
					if ( block )
						return true ;
				}
			}
		}

		if ( _enableUnspecific )
		{
			for ( auto it = _unspecifieds.begin ( ) ; it != _unspecifieds.end ( ) ; it ++ )
			{
				block = (*it)->call ( intercepted, mod ) ;
				if ( block )
					return true ;
			}
		}
		
		return false ;
	}

	unsigned int KeyIntercept::getMods ( )
	{
		return Dispatcher::getMods ( ) ;
	}

	void KeyIntercept::enable ( const bool enable )
	{
		_enableLock.lock ( ) ;
		//_enable = enable ;
		if ( enable )
		{
			Dispatcher::setKeyCallback ( trigger ) ;
		}
		else// if ( _callback != NULL )
		{
			Dispatcher::setKeyCallback ( NULL ) ;
		}

		if ( enable && ! _enableSpecific && ! _enableUnspecific )
		{
			_enableLock.unlock ( ) ;
			enableSpecified ( true ) ;
			enableUnspecified ( true ) ;
		}
		else
		{
			_enableLock.unlock ( ) ;
		}
	}

	bool KeyIntercept::isEnabled ( )
	{
		return Dispatcher::isEnabled ( ) && ( _enableUnspecific || _enableSpecific ) ;
	}

	void KeyIntercept::reset ( )
	{
		// Specific or unspecific are not touched.
		bool modori = isEnabled ( ) ;
		Dispatcher::reset ( ) ;
		enable ( modori ) ;
	}

	void KeyIntercept::release ( )
	{
		// Enables not touched.
		_unspecifieds.clear ( ) ;
		_specifiedKeys [ 0 ].clear ( ) ;
		_specifiedKeys [ 1 ].clear ( ) ;
		_specifiedScans [ 0 ].clear ( ) ;
		_specifiedScans [ 1 ].clear ( ) ;
	}
}

/* Dispatcher is now the same interface for both linux and windows
# ifdef WIN
# include "winkeyintercept.hxx"
# elif defined LNX
# include "lnxkeyintercept.hxx"
# endif
*/