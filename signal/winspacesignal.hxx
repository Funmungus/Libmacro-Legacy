
namespace funlibrary
{ 
	/// <summary>
	/// mouse point x to system metric x
	/// </summary>
	int calculateAbsoluteCoordinateX ( int x ) ;
	/// <summary>
	/// mouse point y to system metric y
	/// </summary>
	int calculateAbsoluteCoordinateY ( int y ) ;

	const string SpaceSignal::_eventName [ ] = {
		// 0x00-0x0A
		"MOVE", "LDOWN", "LUP", "MDOWN", "MUP", "RDOWN", "RUP", "WHEEL", "XDOWN", "XUP"
	} ;

	// event type is the index % FUN_SPACE_COUNT / 2 as the index in _eventBtnMap
	// Let key up type be index % 2, 0 = up, 1 = down

	// event type to EV_KEY value
	const int SpaceSignal::_eventBtnMap [ ] = {
		MOUSEEVENTF_MOVE, MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP,
		MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_WHEEL, MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP
	} ;

	bool SpaceSignal::send ( )
	{
		int ev = _eventBtnMap [ getEvent ( ) ] ;
		if ( ! _cursorJustify )
			ev |= MOUSEEVENTF_ABSOLUTE ;
		mouse_event ( ev, ( DWORD ) _blip.x, ( DWORD ) _blip.y,	getEventData ( ), NULL ) ;

		return true ;
	}

	SpacePosition SpaceSignal::currentPosition ( )
	{
		SpacePosition modori ;
		POINT p ;
			
		if (GetCursorPos(&p))
		{
			modori.x = calculateAbsoluteCoordinateX ( p.x ) ;
			modori.y = calculateAbsoluteCoordinateY ( p.y ) ;
			modori.z = 0 ;
		}

		return modori ;
	}

	int calculateAbsoluteCoordinateX ( int x )
	{
		return ( x * 65536 ) / GetSystemMetrics ( SM_CXSCREEN ) ;
	}

	int calculateAbsoluteCoordinateY ( int y )
	{
		return ( y * 65536 ) / GetSystemMetrics ( SM_CYSCREEN ) ;
	}
}

