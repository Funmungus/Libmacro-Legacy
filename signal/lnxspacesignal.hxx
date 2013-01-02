
# include "signal/spacesignal.h"

using namespace std ;

namespace funlibrary
{
	// event type to EV_KEY value
	const int SpaceSignal::_eventBtnMap [ ] = {
		KEY_MOVE, BTN_LEFT, BTN_LEFT, BTN_MIDDLE, BTN_MIDDLE, BTN_RIGHT, BTN_RIGHT, BTN_WHEEL
	} ;

	const string SpaceSignal::_eventName [ ] = {
		// 0x00-0x09
		"MOVE", "LDOWN", "LUP", "MDOWN", "MUP", "RDOWN", "RUP", "WHEEL"
	} ;

	// event type is the index % FUN_SPACE_COUNT
	// Let key up type be index % 2, 0 = up, 1 = down


	void SpaceSignal::initVent ( )
	{
		memset ( _spacevent, 0, sizeof ( _spacevent ) ) ;
		memset ( _relvent, 0, sizeof ( _relvent ) ) ;
		memset ( _absvent, 0, sizeof ( _absvent ) ) ;
		_spacevent [ 0 ].type = EV_KEY ;
		_relvent [ 0 ].type = _relvent [ 1 ].type = _relvent [ 2 ].type = EV_REL ;
		_relvent [ 0 ].code = REL_X ;
		_relvent [ 1 ].code = REL_Y ;
		_relvent [ 2 ].code = REL_Z ;
		_absvent [ 0 ].type = _absvent [ 1 ].type = _absvent [ 2 ].type = EV_ABS ;
		_absvent [ 0 ].code = ABS_X ;
		_absvent [ 1 ].code = ABS_Y ;
		_absvent [ 2 ].code = ABS_Z ;
		_relvent [ 0 ].value = _absvent [ 0 ].value = getX ( ) ;
		_relvent [ 1 ].value = _absvent [ 1 ].value = getY ( ) ;
		_relvent [ 2 ].value = _absvent [ 2 ].value = getZ ( ) ;
		_spacevent [ 1 ].type = _relvent [ 3 ].type = _absvent [ 3 ].type = EV_SYN ;
		_spacevent [ 1 ].code = _relvent [ 3 ].code = _absvent [ 3 ].code = SYN_REPORT ;
	}

	// Linux does not keep track of current position for mice, so we have to do this ourselves.
	// curPos is changed on send events, and is used as a backup in XQueryPointer does not work.
	SpacePosition curPos ( 0, 0, 0 ) ;
	mutex posLock ;

	bool SpaceSignal::send ( )
	{
		bool error = false ;
		static const unsigned int keySize = sizeof _spacevent ;
		static const unsigned int spaceSize = sizeof _absvent ;
		int code = _eventBtnMap [ getEvent ( ) ] ;

		if ( code == KEY_MOVE )
		{
			if ( isCursorJustify ( ) )
			{
				error = RelDev.send ( _relvent, spaceSize ) ? error : true ;
			}
			else
			{
				error = AbsDev.send ( _absvent, spaceSize ) ? error : true ;
			}
		}
		else
		{
			_spacevent [ 0 ].code = code ;
			if ( code == BTN_WHEEL )
				_spacevent [ 0 ].value = getEventData ( ) ;
			else
				_spacevent [ 0 ].value = getEvent ( ) % 2 ;	// keyUp determined by index, not value of index

			error = KeyDev.send ( _spacevent, keySize ) ? error : true ;
		}

		if ( error )
		{
			fprintf ( stderr, "Error sending spatial event." ) ;
			return false ;
		}

		return true ;
	}

	SpacePosition SpaceSignal::currentPosition ( )
	{
		static Display *dpy = XOpenDisplay ( NULL ) ;
		Window root, child ;
		int rootX, rootY, winX, winY ;
		unsigned int mask ;
		//bool error = false ;

		bool success = XQueryPointer ( dpy, DefaultRootWindow ( dpy ), &root, &child,
									   /*&curPos.x*/ &rootX, /*&curPos.y*/ &rootY, &winX, &winY, &mask ) ;
		posLock.lock ( ) ;
		curPos.x = rootX ;
		curPos.y = rootY ;
		posLock.unlock ( ) ;

		if ( ! success )
		{
			cerr << "Error retrieving current position from X libraries." << endl ;
		}

		return curPos ;
	}

}

