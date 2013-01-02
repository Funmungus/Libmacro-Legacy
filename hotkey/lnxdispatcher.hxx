
# include "hotkey/dispatcher.h"
# include "signal/signal.h"

using namespace std ;

namespace funlibrary
{
	vector < Dispatcher::GrabThread > Dispatcher::_threads ;
	map < string, Grabber * > Dispatcher::_threadGrabs ;
	mutex Dispatcher::_threadInitLock ;	// merge _enableThread?
//    mutex Dispatcher::_resetLock ;
	thread Dispatcher::_enableThread ;
	mutex Dispatcher::_enableLock ;
	NoOp Dispatcher::_delay ( 500 ) ;

    unsigned int eventState_index ( const int keyValue )
    {
        return keyValue / 8 ;
    }

    uint8_t eventState_bit ( const int keyValue )
    {
        return 1 << keyValue % 8 ;
    }

	void Dispatcher::setGrabFromFile ( )
	{
		vector < string > container ;

		char buff [512] ;

		FILE * fp = fopen ( "grab_devices", "r" ) ;

		if ( fp == NULL )
			fp = fopen ( "grab_devices.txt", "r" ) ;

		if ( fp == NULL )
		{
			fprintf ( stderr, "Could not open 'grab_devices' nor 'grab_devices.txt'.\n" ) ;
			return ;
		}

		const static string dirBase = "/dev/input/" ;
		const static string eventBase = dirBase + "event" ;

		//Read file loop.
		while ( fgets ( buff, 512, fp ) )
		{
			char * str ;
			str = buff ;
			char val [ 512 ] ;
			int evNum ;
			string postFix ;

			while ( * str != '\0' )
			{
				// Ignore whitespace at the beginning
				while ( isspace ( * str ) ) str++;
				// Ignore blank lines, comments
				if ( * str  == '\0' ||  * str == '#' ) continue;

				if ( sscanf ( str, "event%d", & evNum ) == 1 ||
					sscanf ( str, "%d", & evNum ) == 1 )
				{
					postFix = to_string ( evNum ) ;
					container.push_back ( eventBase + postFix ) ;
				}
				else if ( sscanf ( str, "/%[^, \r\n=]", val ) == 1 )
				{
					postFix = string ( val ) ;
					container.push_back ( "/" + postFix ) ;
				}
				else if ( sscanf ( str, "%[^, \r\n=]", val ) == 1 )
				{
					postFix = string ( val ) ;
					container.push_back ( dirBase + postFix ) ;
				}
				// Skip over the value that was just read or invalidated.
				while ( ! isspace ( *str ) ) str ++ ;
			}
		}

		Dispatcher::setGrabEvents ( container ) ;
	}

	void Dispatcher::setGrabEvents ( vector < string > paths )
	{
		_enableLock.lock ( ) ;
		_enableThread = thread ( threadSetGrabEvents, paths ) ;
		_enableThread.detach ( ) ;
	}

	// Assumes _enableLock already locked, and unlocks when finished.
	void Dispatcher::threadSetGrabEvents ( vector < string > paths )
	{
		_enableLock.try_lock ( ) ;

        bool modori = isEnabled ( ) ;

		enable_impl ( false ) ;

		_threads.clear ( ) ;

		// Implementation of unique is unclear.  May not be completely unique.
		auto it = unique ( paths.begin ( ), paths.end ( ) ) ;
		paths.resize ( distance ( paths.begin ( ), it ) ) ;

		_threads.resize ( paths.size ( ) ) ;
		// We now have all path strings
		for ( unsigned int i = 0 ; i < paths.size ( ) ; i ++ )
		{
			_threads [ i ].grabPath = paths [ i ] ;
		}

		enable_impl ( modori ) ;
		_enableLock.unlock ( ) ;
	}

	Dispatcher::Dispatcher ( )
	{
		if ( _threads.empty ( ) )
			setGrabFromFile ( ) ;
		//enable ( beginEnabled ) ;
	}

	Dispatcher::~Dispatcher ( )
	{
		_enableLock.lock ( ) ;
		enable_impl ( false ) ;
		// enable_impl uses isEnabled loop, and ensures _threadGrabs will be empty.
		// Do not unlock, do not want re-enable.
		_keyback = NULL ;
		_spaceback = NULL ;
//		_threads.clear ( ) ;
	}

	void Dispatcher::setKeyCallback ( const KeyCallback newCallback )
	{
		bool prev = isEnabled ( ) ;
		_keyback = newCallback ;
		// When previously enabled, possibly disable.
		if ( prev )
		{
			if ( _keyback == NULL && _spaceback == NULL )
			{
				enable ( false ) ;
			}
		}
		// When disabled, possibly enable.
		else if ( _keyback != NULL || _spaceback != NULL )
		{
			enable ( true ) ;
		}
	}

	void Dispatcher::setSpaceCallback ( const SpaceCallback newCallback )
	{
		bool prev = isEnabled ( ) ;
		_spaceback = newCallback ;
		// When previously enabled, possibly disable.
		if ( prev )
		{
			if ( _keyback == NULL && _spaceback == NULL )
				enable ( false ) ;
		}
		// When disabled, possibly enable.
		else if ( _keyback != NULL || _spaceback != NULL )
		{
			enable ( true ) ;
		}
	}

	static unsigned int getMaxModVal ( )
	{
		unsigned int val = KEY_LEFTALT ;
		val = KEY_RIGHTALT > val ? KEY_RIGHTALT : val ;
		val = KEY_LEFTCTRL > val ? KEY_LEFTCTRL : val ;
		val = KEY_RIGHTCTRL > val ? KEY_RIGHTCTRL : val ;
		val = KEY_LEFTSHIFT > val ? KEY_LEFTSHIFT : val ;
		val = KEY_RIGHTSHIFT > val ? KEY_RIGHTSHIFT : val ;
		val = KEY_LEFTMETA > val ? KEY_LEFTMETA : val ;
		val = KEY_RIGHTMETA > val ? KEY_RIGHTMETA : val ;
		return val ;
	}

	uint8_t bit_retrieval [ KEY_CNT / 8 + 1 ] ;
	const unsigned int MODBIT_SIZE = ( sizeof ( uint8_t ) ) * getMaxModVal ( ) / 8 ;

	const unsigned int ModKeysLen =
			2 * Mods::MODSET_LENGTH ;

	void modify ( Mods * cur, uint8_t * keybit_values )
	{
		static const unsigned int modVals [ ] = {
			Mods::getMod ( FUN_ALT ), Mods::getMod ( FUN_ALT ),
			Mods::getMod ( FUN_CTRL ), Mods::getMod ( FUN_CTRL ),
			Mods::getMod ( FUN_SHIFT ), Mods::getMod ( FUN_SHIFT ),
			Mods::getMod ( FUN_META ), Mods::getMod ( FUN_META )
		} ;
		static const unsigned int indices [ ] = {
			eventState_index ( KEY_LEFTALT ), eventState_index ( KEY_RIGHTALT ),
			eventState_index ( KEY_LEFTCTRL ), eventState_index ( KEY_RIGHTCTRL ),
			eventState_index ( KEY_LEFTSHIFT ), eventState_index ( KEY_RIGHTSHIFT ),
			eventState_index ( KEY_LEFTMETA ), eventState_index ( KEY_RIGHTMETA )
		} ;
		static const uint8_t bits [ ] = {
			eventState_bit ( KEY_LEFTALT ), eventState_bit ( KEY_RIGHTALT ),
			eventState_bit ( KEY_LEFTCTRL ), eventState_bit ( KEY_RIGHTCTRL ),
			eventState_bit ( KEY_LEFTSHIFT ), eventState_bit ( KEY_RIGHTSHIFT ),
			eventState_bit ( KEY_LEFTMETA ), eventState_bit ( KEY_RIGHTMETA ),
		} ;

		unsigned int value ;
		for ( unsigned int i = 0 ; i < ModKeysLen ; i++ )
		{
			if ( cur->hasMod ( modVals [ i ] ) )
				continue ;

			// 1 held down 0 up.  value is the value at index BIT-AND bit index of the keycode.
			value = keybit_values [ indices [ i ] ] & bits [ i ] ;
			if ( value != 0 )
				cur->add ( modVals [ i ] ) ;
		}
	}

	unsigned int Dispatcher::getMods ( )
	{
		Mods cur ( 0, KeyUpType::DOWN ) ;

		int ret = ioctl ( KeyDev.getEventFd ( ), EVIOCGKEY ( MODBIT_SIZE ), bit_retrieval ) ;
        if ( ret < 0 )
			fprintf ( stderr, "Dispatcher getMods fd %d : %s.\n", KeyDev.getEventFd ( ), strerror ( errno ) ) ;
        else
            modify ( &cur, bit_retrieval ) ;

		return cur.get ( ) ;
	}

    void Dispatcher::enable ( bool enable )
	{
		_enableLock.lock ( ) ;
		_enableThread = thread ( threadEnable, enable ) ;
		_enableThread.detach ( ) ;
	}

	void Dispatcher::threadEnable ( bool enable )
	{
		_enableLock.try_lock ( ) ;
		enable_impl ( enable ) ;
		_enableLock.unlock ( ) ;
	}

	// Implementation makes sure _enableLock is locked, but does not unlock.
	void Dispatcher::enable_impl ( bool enable )
	{
//		_enableLock.try_lock ( ) ;
		if ( enable && _threads.empty ( ) )
		{
			fprintf ( stderr, "Cannot enable KeyIntercept, no grabbing path available.\n" ) ;
			return ;
		}
		if ( enable )
		{
			InternalMods.set ( getMods ( ) ) ;
			// Lock here, since we need access to _threadGrabs to determine if thread currently running.
			_threadInitLock.lock ( ) ;
			for ( auto pathIt = _threads.begin ( ) ;
				pathIt != _threads.end ( ) ;
				pathIt ++ )
			{
				// For each thread, see if grabber has reference.
				// If grabber is not referenced, need to start or restart it.
				if ( _threadGrabs.find ( pathIt->grabPath ) == _threadGrabs.end ( ) )
				{
//					 TODO decide if _enableLock needed here, or maybe _threadInitLock?
					pathIt->trd = thread ( interceptingFunction, pathIt->grabPath ) ;
					pathIt->trd.detach ( ) ;
				}
			}
			_threadInitLock.unlock ( ) ;
		}
		else
		{
			_threadInitLock.lock ( ) ;
			for ( auto grabIt = _threadGrabs.begin ( ) ;
				grabIt != _threadGrabs.end ( ) ;
				grabIt ++ )
			{
				// If null, error and remove. Decrement because it will be incremented again.
				if ( grabIt->second != NULL )
					grabIt->second->enable ( false ) ;
				// TODO join/detach?
			}
			_threadGrabs.clear ( ) ;

			while ( isEnabled ( ) )
			{
				_delay.send ( ) ;
			}
			_threadInitLock.unlock ( ) ;
		}
	}

	void setFromCurrent ( SpaceSignal * siggy, bool x, bool y, bool z ) ;
	void defaultRel ( SpaceSignal * siggy, bool x, bool y, bool z ) ;

	// Note: Excess time setting up and releasing is ok.  Please try to limit
	// time-consumption during grabbing and callbacks.
	void Dispatcher::interceptingFunction ( string grabPath )
    {
		// 1. Create grab from path.
		// 2. Enable.
		// 3. Send back address of grabber.
		// 4. Use grabber.
		// 5. Remove grabber reference in main.

		_threadInitLock.lock ( ) ;
		Grabber grabbi ( grabPath, true ) ;

		// grab path guaranteed unique in setGrabPaths

		if ( ! grabbi.enable_accessor )
		{
			_threadGrabs.erase ( grabPath ) ;
			_threadInitLock.unlock ( ) ;
			return ;
		}
		_threadGrabs [ grabPath ] = & grabbi ;
		_threadInitLock.unlock ( ) ;

		int rdb, rdn, i ;
		bool bScan = false, bKey = false, bX = false, bY = false, bZ = false, bAbs = false, bRel = false ;

		KeySignal keyback ;
		SpaceSignal spaceback ;
        const static int moveType = SpaceSignal::getEventInt ( "move" ) ;

		// Point 1
		while ( grabbi.enable_accessor )
		{
			// Point 2, break blocking read by disabling grabber
			rdb = grabbi.read ( ) ;
			// Point 3,
			if ( ! grabbi.enable_accessor )
			{
				_threadInitLock.lock ( ) ;
				_threadGrabs.erase ( grabPath ) ;
				_threadInitLock.unlock ( ) ;
				break ;
			}

			// Disable broken reads.
			if ( rdb < Grabber::oneEventSize )
			{
				perror ( "Grabbing thread read : " ) ;
				_threadInitLock.lock ( ) ;
				_threadGrabs.erase ( grabPath ) ;
				_threadInitLock.unlock ( ) ;
				//grabbi->enable ( false ) ;
				break;
			}

			rdn = rdb / Grabber::oneEventSize ;

			// Intercept each input event read.
			for ( i = 0 ; i < rdn ; i++ )
			{
				switch ( grabbi.grabSet [ i ].type )
				{
				case EV_MSC :
					if ( grabbi.grabSet [ i ].code == MSC_SCAN )
					{
						// scan will be consumed.
						if ( bScan )
						{
							if ( ! bKey )
                            {
								keyback.setKeyUp ( KeyUpType::DOWN ) ;
                                keyback.setKey ( 0 ) ;
							}
							else
								bKey = false ;

							// The other being NULL should not happen, so heavy lift here.
							if ( ! call ( & keyback, & InternalMods ) )
                            {
								keyback.send ( ) ;
                            }

							// bScan is about to be set to true. Do not bother changing it now.
						}

						keyback.setScan ( grabbi.grabSet [ i ].value ) ;
						bScan = true ;
					}
					break ;
				case EV_KEY :
                    // Key will be consumed.
                    if ( bKey )
					{
						if ( ! bScan )
						{
							keyback.setScan ( 0 ) ;
						}
						else
							bScan = false ;

						// The other being NULL should not happen, so heavy lift here.
						if ( ! call ( & keyback, & InternalMods ) )
                        {
                            keyback.send ( ) ;
                        }

						// bKey is about to be set to true. Do not bother changing it now.
					}

					keyback.setKey ( grabbi.grabSet [ i ].code ) ;
					keyback.setKeyUp ( grabbi.grabSet [ i ].value != 0 ?
									KeyUpType::DOWN : KeyUpType::UP ) ;
					bKey = true ;
					break ;
				// Spatial can be send right away.
				case EV_ABS :
					if ( bRel )
					{
						spaceback.setEvent ( moveType ) ;
						spaceback.setEnableCursorJustify ( bRel ) ;
                        defaultRel ( & spaceback, ! bX, ! bY, ! bZ ) ;

						if ( ! call ( & spaceback, & InternalMods ) )
                        {
							spaceback.send ( ) ;
                        }
						bRel = bX = bY = bZ = false ;
					}
					bAbs = true ;

					goto spatial_common ;
					break ;
                case EV_REL :
					if ( bAbs )
                    {
						spaceback.setEvent ( moveType ) ;
						spaceback.setEnableCursorJustify ( bRel ) ;
                        setFromCurrent ( & spaceback, ! bX, ! bY, ! bZ ) ;

						if ( ! call ( & spaceback, & InternalMods ) )
                        {
                            spaceback.send ( ) ;
                        }
						bAbs = bX = bY = bZ = false ;
					}
					bRel = true ;

                spatial_common :
					switch ( grabbi.grabSet [ i ].code )
					{
					case REL_X :
						if ( bX )
						{
                            spaceback.setEvent ( moveType ) ;
                            spaceback.setEnableCursorJustify ( bRel ) ;

							if ( bAbs )
							{
								setFromCurrent ( & spaceback, ! bX, ! bY, ! bZ ) ;
								bAbs = false ;
							}
							else
                            {
								defaultRel ( & spaceback, ! bX, ! bY, ! bZ ) ;
								bRel = false ;
							}

							if ( ! call ( & spaceback, & InternalMods ) )
                            {
                                spaceback.send ( ) ;
                            }
							bY = bZ = false ;
						}

						spaceback.setX ( grabbi.grabSet [ i ].value ) ;
						bX = true ;
						break ;
					case REL_Y :
						if ( bY )
						{
                            spaceback.setEvent ( moveType ) ;
                            spaceback.setEnableCursorJustify ( bRel ) ;

							if ( bAbs )
							{
								setFromCurrent ( & spaceback, ! bX, ! bY, ! bZ ) ;
								bAbs = false ;
							}
							else
							{
								defaultRel ( & spaceback, ! bX, ! bY, ! bZ ) ;
								bRel = false ;
                            }
							if ( ! call ( & spaceback, & InternalMods ) )
                            {
                                spaceback.send ( ) ;
                            }
							bY = bZ = false ;
						}

						spaceback.setY ( grabbi.grabSet [ i ].value ) ;
						bY = true ;
						break ;
					case REL_Z :
						if ( bZ )
                        {
                            spaceback.setEvent ( moveType ) ;
                            spaceback.setEnableCursorJustify ( bRel ) ;

							if ( bAbs )
							{
								setFromCurrent ( & spaceback, ! bX, ! bY, ! bZ ) ;
								bAbs = false ;
							}
							else
							{
								defaultRel ( & spaceback, ! bX, ! bY, ! bZ ) ;
								bRel = false ;
                            }
							if ( ! call ( & spaceback, & InternalMods ) )
                            {
                                spaceback.send ( ) ;
                            }
							bX = bY = false ;
						}

						spaceback.setZ ( grabbi.grabSet [ i ].value ) ;
						bZ = true ;
						break ;
					}
					break ;
                }
			}
/*
 * BUG: after reset, a callback which disables sends the last key one extra time
 *
 */
			//Call final event, as it was not called from previous switch statement.
			if ( bScan || bKey )
			{
				if ( ! bScan )
				{
					keyback.setScan ( 0 ) ;
				}
                else
                    bScan = false ;

				if ( ! bKey )
				{
					keyback.setKey ( 0 ) ;
					keyback.setKeyUp ( KeyUpType::DOWN ) ;
				}
                else
                    bKey = false ;

                // The other being NULL should not happen, so heavy lift here.
				if ( ! call ( & keyback, & InternalMods ) )
                    keyback.send ( ) ;
            }

			if ( bAbs || bRel )
            {
                spaceback.setEvent ( moveType ) ;
                spaceback.setEnableCursorJustify ( bRel ) ;

				if ( bAbs )
				{
					setFromCurrent ( & spaceback, ! bX, ! bY, ! bZ ) ;
                    bAbs = false ;
				}
				else
				{
					defaultRel ( & spaceback, ! bX, ! bY, ! bZ ) ;
					bRel = false ;
                }
				if ( ! call ( & spaceback, & InternalMods ) )
                    spaceback.send ( ) ;

				bX = bY = bZ = false ;
			}
		}

		_threadInitLock.lock ( ) ;
		_threadGrabs.erase ( grabPath ) ;
		_threadInitLock.unlock ( ) ;
//		grabbi.enable ( false ) ;
//		* alive = false ;
	}

	void setFromCurrent ( SpaceSignal * siggy, bool x, bool y, bool z )
	{
		if ( ! ( x || y || z ) )
			return ;

		SpacePosition pos = SpaceSignal::currentPosition ( ) ;
		if ( x )
			siggy->setX ( pos.x ) ;
		if ( y )
			siggy->setY ( pos.y ) ;
		if ( z )
			siggy->setZ ( pos.z ) ;
	}

	void defaultRel ( SpaceSignal * siggy, bool x, bool y, bool z )
	{
		if ( x )
			siggy->setX ( 0 ) ;
		if ( y )
			siggy->setY ( 0 ) ;
		if ( z )
			siggy->setZ ( 0 ) ;
	}

	bool Dispatcher::isEnabled ( )
	{
		return ! _threadGrabs.empty ( ) ;
//		for ( auto i = _grabberSet.begin ( ) ;
//			  i != _grabberSet.end ( ) ; i ++ )
//		{
//			if ( i->grabber.enable_accessor &&
//				i->trd.joinable ( ) &&
//				i->alive )
//			{
//				return true ;
//			}
//		}
//		return false ;
	}

	//TODO decide if reset should always end on true, or if reverting to original.
	void Dispatcher::reset ( )
	{
//		enable ( false ) ;

		_enableLock.lock ( ) ;
        thread resetThread ( threadReset ) ;
        resetThread.detach ( ) ;

        //setGrabFromFile ( ) ;
//		enable ( true ) ;
	}

    void Dispatcher::threadReset ( )
    {
        // Only let one reset thread take action.
		_enableLock.try_lock ( ) ;
		bool modori = isEnabled ( ) ;
		enable_impl ( false ) ;
		if ( modori )
			enable_impl ( true ) ;
		_enableLock.unlock ( ) ;
//        if ( _resetLock.try_lock ( ) )
//        {
//            bool modori = isEnabled ( ) ;
//            enable ( false ) ;
////            static NoOp delay ( 500 ) ;
//            while ( isEnabled ( ) )
//            {
//				_delay.send ( ) ;
//            }
//            setGrabFromFile ( ) ;
//            if ( modori )
//                enable ( modori ) ;
//            _resetLock.unlock ( ) ;
//        }
    }
}
