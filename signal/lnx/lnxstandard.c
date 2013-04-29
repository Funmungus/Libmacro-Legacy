
# include "signal/standard.h"
# include "signal/lnx/standard.h"

MCR_API mcr_Array mcr_EchoEvents ;

void mcr_Echo_init ( mcr_HIDEcho * echoPt )
{
	echoPt->event = 0 ;
}
void mcr_Key_init ( mcr_Key * keyPt )
{
	memset ( keyPt, 0, sizeof ( mcr_Key ) ) ;
	keyPt->events [ 0 ].type = EV_MSC ;
	keyPt->events [ 0 ].code = MSC_SCAN ;
	keyPt->events [ 1 ].type = EV_KEY ;
	keyPt->events [ 2 ] = mcr_Syncer ;
	keyPt->key_up = MCR_BOTH ;
}
void mcr_MoveCursor_init ( mcr_MoveCursor * mcPt )
{
	memset ( mcPt, 0, sizeof ( mcr_MoveCursor ) ) ;
	mcPt->absvent [ 0 ].type = mcPt->absvent [ 1 ].type =
			mcPt->absvent [ 2 ].type = EV_ABS ;
	mcPt->relvent [ 0 ].type = mcPt->relvent [ 1 ].type =
			mcPt->relvent [ 2 ].type = EV_REL ;
	mcPt->absvent [ MCR_X ].code = ABS_X ;
	mcPt->absvent [ MCR_Y ].code = ABS_Y ;
	mcPt->absvent [ MCR_Z ].code = ABS_Z ;
	mcPt->relvent [ MCR_X ].code = REL_X ;
	mcPt->relvent [ MCR_Y ].code = REL_Y ;
	mcPt->relvent [ MCR_Z ].code = REL_Z ;
	mcPt->relvent [ MCR_DIMENSION_CNT ] =
			mcPt->absvent [ MCR_DIMENSION_CNT ] = mcr_Syncer ;
	mcPt->cursor_justify = 1 ;
}
void mcr_Scroll_init ( mcr_Scroll * scrollPt )
{
	memset ( scrollPt, 0, sizeof ( mcr_Scroll ) ) ;
	scrollPt->events [ 0 ].type = scrollPt->events [ 1 ].type =
			scrollPt->events [ 2 ].type = EV_REL ;
	scrollPt->events [ MCR_X ].code = REL_HWHEEL ;
	scrollPt->events [ MCR_Y ].code = REL_WHEEL ;
	scrollPt->events [ MCR_Z ].code = REL_DIAL ;
	scrollPt->events [ MCR_DIMENSION_CNT ] = mcr_Syncer ;
}


void mcr_standard_native_initialize ( )
{
	// Device initialization has a break, to wait for devices to be ready.
	mcr_Device_initialize ( ) ;
	mcr_Array_init ( & mcr_EchoEvents, sizeof ( mcr_Key ) ) ;
	int keys [ ] =
	{
		BTN_LEFT, BTN_LEFT, BTN_MIDDLE, BTN_MIDDLE, BTN_RIGHT, BTN_RIGHT
	} ;
	mcr_KeyUpType ups [ ] =
	{
		MCR_DOWN, MCR_UP, MCR_DOWN, MCR_UP, MCR_DOWN, MCR_UP
	} ;
	// TODO: Define names externally.
	const char * names [ ] =
	{
		"LeftDown", "LeftUp", "MiddleDown", "MiddleUp",
		"RightDown", "RightUp"
	} ;
	size_t count = sizeof ( keys ) / sizeof ( int ) ;
	mcr_Array_resize ( & mcr_EchoEvents, count ) ;
	mcr_Key key ;
	mcr_Key_init ( & key ) ;
	for ( size_t i = 0 ; i < count ; i++ )
	{
		MCR_KEY_SET ( & key, keys [ i ] ) ;
		MCR_KEY_SET_UP_TYPE ( & key, ups [ i ] ) ;
		int code = mcr_Echo_register ( names [ i ] ) ;
		if ( code == -1 )
		{
			DMSG ( "%s%s", "Unable to register echo: ", names [ i ] ) ;
		}
		else
		{
			// TODO Native register.
			mcr_Array_set ( & mcr_EchoEvents, code, & key ) ;
		}
	}
}

void mcr_standard_native_cleanup ( )
{
	mcr_Device_cleanup ( ) ;
	mcr_Array_free ( & mcr_EchoEvents ) ;
}

