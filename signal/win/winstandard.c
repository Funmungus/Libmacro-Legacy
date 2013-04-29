
# include "signal/standard.h"
# include "signal/win/standard.h"

// index-> echo event integer
MCR_API mcr_Array mcr_EchoEvents ;

void mcr_Echo_init ( mcr_HIDEcho * echoPt )
{
	echoPt->event = 0 ;
}
void mcr_Key_init ( mcr_Key * keyPt )
{
	memset ( keyPt, 0, sizeof ( mcr_Key ) ) ;
}
void mcr_MoveCursor_init ( mcr_MoveCursor * mcPt )
{
	memset ( mcPt, 0, sizeof ( mcr_MoveCursor ) ) ;
	// Justify for default relative event.
	mcPt->cursor_justify = 1 ;
}
void mcr_Scroll_init ( mcr_Scroll * scrollPt )
{
	memset ( scrollPt, 0, sizeof ( mcr_Scroll ) ) ;
}


void mcr_standard_native_cleanup ( void )
{
	mcr_Array_free ( & mcr_EchoEvents ) ;
}

void mcr_standard_native_initialize ( )
{
	mcr_Array_init ( & mcr_EchoEvents, sizeof ( int ) ) ;

	int echoEvents [ ] =
	{
		MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP,
		MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP,
		MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP,
	} ;
	const char * names [ ] =
	{
		"LeftDown", "LeftUp", "MiddleDown", "MiddleUp",
		"RightDown", "RightUp"
	} ;
	size_t count = sizeof ( echoEvents ) / sizeof ( int ) ;
	if ( ! mcr_Array_resize ( & mcr_EchoEvents, count ) )
	{
		DMSG ( "%s\n", "mcr_standard_native_initialize unable to "
				"size native events." ) ;
		return ;
	}
	for ( size_t i = 0 ; i < count ; i++ )
	{
		int code = mcr_Echo_register ( names [ i ] ) ;
		if ( code == -1 )
		{
			DMSG ( "%s%s\n", "Unable to register echo: ", names [ i ] ) ;
		}
		else
		{
			// TODO Native register.
			mcr_Array_set ( & mcr_EchoEvents, code, echoEvents + i ) ;
		}
	}
}
