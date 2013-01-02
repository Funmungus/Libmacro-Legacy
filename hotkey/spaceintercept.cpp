
# ifdef WIN
# include "stdafx.h"
# endif

# include "hotkey/spaceintercept.h"

using namespace std ;

namespace funlibrary
{
	mutex SpaceIntercept::_enableLock ;

	bool SpaceIntercept::_enableUnspecific ;
	vector < ICall < bool, SpaceSignal *, Mods * > * > SpaceIntercept::_unspecifieds ;

	bool SpaceIntercept::_enableSpecific ;

	map < pair < int, int >, vector < ICall < bool, SpaceSignal *, Mods * > * > > SpaceIntercept::_specifiedEvents ;
	vector < ICall < bool, SpaceSignal *, Mods * > * > SpaceIntercept::_specifiedJustify [ 2 ] ;

	// Trigger and dispatch.
	// Unspecifics.
	void SpaceIntercept::enableUnspecified ( const bool enable )
	{
		_enableLock.lock ( ) ;
		// Need enable if enabling, and neither type enabled.
		bool toEnable = enable && ! isEnabled ( ) ;
		// Need disable if disabling, currently type enabled, and other is not enabled.
		bool toDisable = ! enable && _enableUnspecific && ! _enableSpecific ;
		_enableUnspecific = enable ;
		_enableLock.unlock ( ) ;
		if ( toEnable )
			SpaceIntercept::enable ( true ) ;
		if ( toDisable )
			SpaceIntercept::enable ( false ) ;
	}

	void SpaceIntercept::add ( ICall < bool, SpaceSignal *, Mods * > * newCallback )
	{
		_unspecifieds.push_back ( newCallback ) ;
	}

	void SpaceIntercept::remove ( ICall < bool, SpaceSignal *, Mods * > * delCallback )
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

	// Specifics.
	void SpaceIntercept::enableSpecified ( bool enable )
	{
		_enableLock.lock ( ) ;
		// Need enable if enabling, and neither type enabled.
		bool toEnable = enable && ! isEnabled ( ) ;
		// Need disable if disabling, currently type enabled, and other is not enabled.
		bool toDisable = ! enable && ! _enableUnspecific && _enableSpecific ;
		_enableSpecific = enable ;
		_enableLock.unlock ( ) ;
		if ( toEnable )
			SpaceIntercept::enable ( true ) ;
		if ( toDisable )
			SpaceIntercept::enable ( false ) ;
	}

	void SpaceIntercept::addForEvent ( ICall < bool, SpaceSignal *, Mods * > * newCallback, SpaceSignal spatial, Mods mod )
	{
		pair < int, int > modified ( spatial.getEvent ( ), ( int ) mod.get ( ) ) ;

		if ( mod.getKeyUp ( ) == KeyUpType::BOTH )
		{
			mod.setKeyUp ( KeyUpType::DOWN ) ;
			addForEvent ( newCallback, spatial, mod ) ;
			mod.setKeyUp ( KeyUpType::UP ) ;
		}

		if ( mod.getKeyUp ( ) == KeyUpType::UP )
		{
			// Mods length 4, binary 1111, hex F
			modified.second = ( ~ modified.second ) & 0x0F ;
		}

		auto it = _specifiedEvents.find ( modified ) ;
		if ( it == _specifiedEvents.end ( ) )
		{
			_specifiedEvents [ modified ].push_back ( newCallback ) ;
		}
		else
		{
			it->second.push_back ( newCallback ) ;
		}
	}

	void SpaceIntercept::addForJustify ( ICall < bool, SpaceSignal *, Mods * > * newCallback, const bool cursorJustify )
	{
		int justIndex = cursorJustify ? 1 : 0 ;

		_specifiedJustify [ justIndex ].push_back ( newCallback ) ;
	}

	void SpaceIntercept::removeForEvent ( ICall < bool, SpaceSignal *, Mods * > * delCallback, SpaceSignal spatial, Mods mod )
	{
		pair < int, int > modified ( spatial.getEvent ( ), ( int ) mod.get ( ) ) ;
		if ( mod.getKeyUp ( ) == KeyUpType::BOTH )
		{
			mod.setKeyUp ( KeyUpType::DOWN ) ;
			removeForEvent ( delCallback, spatial, mod ) ;
			mod.setKeyUp ( KeyUpType::UP ) ;
		}

		if ( mod.getKeyUp ( ) == KeyUpType::UP )
		{
			// Mods length 4, binary 1111, hex F
			modified.second = ( ~ modified.second ) & 0x0F ;
		}

		auto it = _specifiedEvents.find ( modified ) ;
		if ( it != _specifiedEvents.end ( ) )
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
	}

	void SpaceIntercept::removeForJustify ( ICall < bool, SpaceSignal *, Mods * > * delCallback, bool cursorJustify )
	{
		int justIndex = cursorJustify ? 1 : 0 ;

		for ( auto it = _specifiedJustify [ justIndex ].begin ( ) ;
			it != _specifiedJustify [ justIndex ].end ( ) ; it ++ )
		{
			while ( it != _specifiedJustify [ justIndex ].end ( ) && *it == delCallback )
			{
				it = _specifiedJustify [ justIndex ].erase ( it ) ;
			}
			if ( it == _specifiedJustify [ justIndex ].end ( ) )
				break ;
		}
	}


	// And finally, the dispatching function.
	bool SpaceIntercept::trigger ( SpaceSignal * intercepted, Mods * mod )
	{
		if ( intercepted == NULL || mod == NULL )
		{
			fprintf ( stderr, "SpaceIntercept trigger : SpaceSignal or mod is null.\n" ) ;
			return true ;
		}

		bool block = false ;

		if ( _enableSpecific )
		{
			pair < int, int > key ( intercepted->getEvent ( ), ( int ) mod->get ( ) ) ;

			auto kIt = _specifiedEvents.find ( key ) ;
			vector < ICall < bool, SpaceSignal *, Mods * > * >::iterator i ;

			if ( kIt != _specifiedEvents.end ( ) )
			{
				for ( i = kIt->second.begin ( ) ; i != kIt->second.end ( ) ; i++ )
				{
					block = (*i)->call ( intercepted, mod ) ;
					if ( block )
						return true ;
				}
			}

			int justIndex = intercepted->isCursorJustify ( ) ? 1 : 0 ;
			for ( i = _specifiedJustify [ justIndex ].begin ( ) ;
				  i != _specifiedJustify [ justIndex ].end ( ) ; i++ )
			{
				block = (*i)->call ( intercepted, mod ) ;
				if ( block )
					return true ;
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

	void SpaceIntercept::enable ( bool enable )
	{
		_enableLock.lock ( ) ;
		if ( enable )
		{
			Dispatcher::setSpaceCallback ( trigger ) ;
		}
		else
		{
			Dispatcher::setSpaceCallback ( NULL ) ;
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

	bool SpaceIntercept::isEnabled ( )
	{
		return Dispatcher::isEnabled ( ) && ( _enableUnspecific || _enableSpecific ) ;
	}

	void SpaceIntercept::reset ( )
	{
		bool modori = isEnabled ( ) ;
		Dispatcher::reset ( ) ;
		enable ( modori ) ;
	}

	void SpaceIntercept::release ( )
	{
		_unspecifieds.clear ( ) ;
		_specifiedEvents.clear ( ) ;
		_specifiedJustify [ 0 ].clear ( ) ;
		_specifiedJustify [ 1 ].clear ( ) ;
	}

}

/* Dispatcher is now the same for linux and windows
# ifdef WIN
# include "winspaceintercept.hxx"
# elif defined LNX
# include "lnxspaceintercept.hxx"
# endif
*/