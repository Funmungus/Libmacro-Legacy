
# ifdef WIN
# include "stdafx.h"
# endif

using namespace std ;

# include "signal/signal.h"
# include "hotkey/dispatcher.h"

namespace funlibrary
{
	KeyCallback Dispatcher::_keyback = NULL ;
	SpaceCallback Dispatcher::_spaceback = NULL ;

    bool Dispatcher::call ( KeySignal * key, Mods * mod )
    {
		if ( _keyback != NULL )
            return _keyback ( key, mod ) ;
		return false ;
	}

    bool Dispatcher::call ( SpaceSignal * space, Mods * mod )
	{
		if ( _spaceback != NULL )
            return _spaceback ( space, mod ) ;
		return false ;
	}
}
# ifdef LNX
# include "lnxdispatcher.hxx"
# endif
# ifdef WIN
# include "windispatcher.hxx"
# endif
funlibrary::Dispatcher funlibrary::Dispatcher::_instance ;	// Only needed for destruction.