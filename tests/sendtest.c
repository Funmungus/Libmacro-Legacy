
# include "macro.h"
# include "time.h"

# define SIZE 64
# define SAFESCAN(_buff_) scanf ( "%63s", _buff_ )

# define DOUBLEDIM (MCR_DIMENSION_CNT * 2)

int selectTest ( ) ;
void alarmTest ( ) ;
void echo ( ) ;
void keyPress ( ) ;
void justify ( ) ;
void absolute ( ) ;
void scroll ( ) ;

static mcr_NoOp delay = { { 1, 0 } } ;
static mcr_Signal delaySig = { & mcr_INoOp, & delay } ;
static char buffer [ SIZE ] ;

int main ( )
{
	mcr_macro_initialize ( ) ;
	printf ( "\nBegin...\n" ) ;
	printf ( "MCR_DIMENSION_CNT = %d, DOUBLEDIM = %d\n",
			MCR_DIMENSION_CNT, DOUBLEDIM ) ;
	mcr_send ( & delaySig ) ;
	for ( int i = selectTest ( ) ;
		i != -1 ; i = selectTest ( ) )
	{
		switch ( i )
		{
		case 1:
			alarmTest ( ) ;
			break ;
		case 2:
			echo ( ) ;
			break ;
		case 3:
			keyPress ( ) ;
			break ;
		case 4:
			justify ( ) ;
			break ;
		case 5:
			absolute ( ) ;
			break ;
		case 6:
			scroll ( ) ;
break ;
		}
	}
}

int selectTest ( )
{
	printf ( "\nPlease make an integer selection.\n" ) ;
	printf ( "%7s%30s\n %7s%30s\n %7s%30s\n %7s%30s\n %7s%30s\n "
			"%7s%30s\n %7s%30s\n %7s%30s\n ",
		"-1 ) ", "exit",
		"0 ) ", "nothing",
		"1 ) ", "alarm",
		"2 ) ", "echo",
		"3 ) ", "key press",
		"4 ) ", "justified cursor",
		"5 ) ", "absolute cursor",
		"6 ) ", "scroll" ) ;
	printf ( "Your selection : " ) ;

	int i = -1 ;
	SAFESCAN ( buffer ) ;
	i = atoi ( buffer ) ;
	return i ;
}

void alarmTest ( )
{
	printf ( "\nTest alarm.\nEnter seconds to justify: " ) ;
	SAFESCAN ( buffer ) ;
	int difference = atoi ( buffer ) ;
	if ( difference == 0 )
	{
		printf ( "Alarm seconds found 0. Nothing done.\n" ) ;
		return ;
	}

	time_t t = time ( NULL ) ;
	printf ( "Alarm begins at %s.\n", ctime ( & t ) ) ;

	t += difference ;
	struct tm * alPt = localtime ( & t ) ;
	mcr_Signal sig = { & mcr_IAlarm, 0 } ;
	sig.data = alPt ;
	mcr_send ( & sig ) ;

	t = time ( NULL ) ;
	printf ( "Alarm ends at %s.\n", ctime ( & t ) ) ;
}

void echo ( )
{
	printf ( "\nEcho test.\n" ) ;
	mcr_HIDEcho echo ;
	mcr_Echo_init ( & echo ) ;
	mcr_Signal sig = { & mcr_IHIDEcho, & echo } ;
	size_t count = mcr_Echo_count ( ) ;

	for ( size_t i = 0 ; i < count ; i++ )
	{
		// Set current.
		mcr_Echo_set ( & echo, i ) ;
		const char * name = mcr_Echo_name ( mcr_Echo_get ( & echo ) ) ;

		// Send current.
		printf ( "Send echo %d, named '%s.'\n",
				mcr_Echo_get ( & echo ), name ? name : "" ) ;
		mcr_send ( & delaySig ) ;
		mcr_send ( & sig ) ;
	}
}

void keyPress ( )
{
	printf ( "\nTesting key press and release.\nEnter integer key code: " ) ;
	SAFESCAN ( buffer ) ;
	int key = atoi ( buffer ) ;
	printf ( "Enter integer scan code: " ) ;
	SAFESCAN ( buffer ) ;
	int scan = atoi ( buffer ) ;

	mcr_Key mKey ;
	mcr_Signal sig = { & mcr_IKey, & mKey } ;
	mcr_Key_init_with ( & mKey, key, scan, MCR_DOWN ) ;

	printf ( "\nPressing key...\n" ) ;
	mcr_send ( & delaySig ) ;
	mcr_send ( & sig ) ;

	printf ( "\nKey is pressed. Releasing key...\n" ) ;
	mcr_Key_set_up_type ( & mKey, MCR_UP ) ;
	mcr_send ( & delaySig ) ;
	mcr_send ( & sig ) ;

	printf ( "\nNow testing both down and up together.\n" ) ;
	mcr_Key_set_up_type ( & mKey, MCR_BOTH ) ;
	mcr_send ( & delaySig ) ;
	mcr_send ( & sig ) ;
}

void justify ( )
{
//	char buffer [ 64 ] ;
	printf ( "\nTest justified cursor.\nEnter amount to justify: " ) ;
	SAFESCAN ( buffer ) ;
	int displacement = atoi ( buffer ) ;

	mcr_MoveCursor just ;
	mcr_Signal sig = { & mcr_IMoveCursor, & just } ;
	mcr_MoveCursor_init ( & just ) ;
	mcr_MoveCursor_enable_justify ( & just, 1 ) ;

	for ( int i = 0 ; i < DOUBLEDIM ; i++ )
	{
		int dimI = i / 2 ;
		// Odd numbers, starting with 1.
		if ( i % 2 )
		{
			mcr_MoveCursor_set_position ( & just, dimI, -displacement ) ;
		}
		// Even numbers, starting with 0.
		else
		{
			mcr_MoveCursor_set_position ( & just, dimI, displacement ) ;
		}

		// Send current.
		printf ( "Move cursor: x=%lld, y=%lld, z=%lld\n",
				mcr_MoveCursor_get_position ( & just, MCR_X ),
				mcr_MoveCursor_get_position ( & just, MCR_Y ),
				mcr_MoveCursor_get_position ( & just, MCR_Z ) ) ;
		mcr_send ( & delaySig ) ;
		mcr_send ( & sig ) ;

		// Reset.
		mcr_MoveCursor_set_position ( & just, dimI, 0 ) ;
	}
}

void absolute ( )
{
	printf ( "\nTest absolute cursor.\n" ) ;

	mcr_MoveCursor abs ;
	mcr_MoveCursor_init ( & abs ) ;
	mcr_Signal sig = { & mcr_IMoveCursor, & abs } ;
	mcr_MoveCursor_enable_justify ( & abs, 0 ) ;

	// Get and set x, y, z.
	printf ( "Enter x value: " ) ;
	SAFESCAN ( buffer ) ;
	long long p = atoll ( buffer ) ;
	mcr_MoveCursor_set_position ( & abs, MCR_X, p ) ;
	printf ( "Enter y value: " ) ;
	SAFESCAN ( buffer ) ;
	p = atoll ( buffer ) ;
	mcr_MoveCursor_set_position ( & abs, MCR_Y, p ) ;
	printf ( "Enter z value: " ) ;
	SAFESCAN ( buffer ) ;
	p = atoll ( buffer ) ;
	mcr_MoveCursor_set_position ( & abs, MCR_Z, p ) ;

	// Send current.
	printf ( "Move cursor: x=%lld, y=%lld, z=%lld\n",
			mcr_MoveCursor_get_position ( & abs, MCR_X ),
			mcr_MoveCursor_get_position ( & abs, MCR_Y ),
			mcr_MoveCursor_get_position ( & abs, MCR_Z ) ) ;
	mcr_send ( & delaySig ) ;
	mcr_send ( & sig ) ;
}

void scroll ( )
{
	printf ( "\nTest scrolling.\nEnter amount to scroll: " ) ;
	SAFESCAN ( buffer ) ;
	long long scrolling = atoll ( buffer ) ;

	mcr_Scroll scr ;
	mcr_Signal sig = { & mcr_IScroll, & scr } ;
	mcr_Dimensions dim = { 0 } ;
	mcr_Scroll_init_with ( & scr, dim ) ;

	for ( int i = 0 ; i < DOUBLEDIM ; i++ )
	{
		int dimI = i / 2 ;
		// Odd numbers, starting with 1.
		if ( i % 2 )
		{
			mcr_Scroll_set_dimension ( & scr, dimI, -scrolling ) ;
		}
		// Even numbers, starting with 0.
		else
		{
			mcr_Scroll_set_dimension ( & scr, dimI, scrolling ) ;
		}

		// Send current.
		printf ( "Scrolling: x=%lld, y=%lld, z=%lld\n",
				mcr_Scroll_get_dimension ( & scr, MCR_X ),
				mcr_Scroll_get_dimension ( & scr, MCR_Y ),
				mcr_Scroll_get_dimension ( & scr, MCR_Z ) ) ;
		mcr_send ( & delaySig ) ;
		mcr_send ( & sig ) ;

		// Reset to 0.
		mcr_Scroll_set_dimension ( & scr, dimI, 0 ) ;
	}
}
