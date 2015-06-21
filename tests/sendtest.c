/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */


# include "macro.h"
# include "time.h"
# include "signal/lnx/standard.h"

# define SIZE 64
# define SAFESCAN(_buff_) fscanf ( mcr_stdin, "%63s", _buff_ )

# define DOUBLEDIM (MCR_DIMENSION_CNT * 2)

int selectTest ( ) ;
void alarmTest ( ) ;
void echo ( ) ;
void keyPress ( ) ;
void justify ( ) ;
void absolute ( ) ;
void scroll ( ) ;

static mcr_NoOp delay = { 1, 0 } ;
static mcr_Signal delaySig = { & mcr_iNoOp, { & delay, 0 } } ;
static char buffer [ SIZE ] ;

int main ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( mcr_signal_cleanup ) ;
	mcr_signal_initialize ( ) ;
	mcr_standard_enable ( 1 ) ;
	fprintf ( mcr_stdout, "\nBegin...\n" ) ;
	fprintf ( mcr_stdout, "MCR_DIMENSION_CNT = %d, DOUBLEDIM = %d\n",
			MCR_DIMENSION_CNT, DOUBLEDIM ) ;
	// Some platforms need to rest before being able to send some signals.
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
	mcr_standard_enable ( 0 ) ;
}

int selectTest ( )
{
	fprintf ( mcr_stdout, "\nPlease make an integer selection.\n" ) ;
	fprintf ( mcr_stdout, "%7s%30s\n %7s%30s\n %7s%30s\n %7s%30s\n %7s%30s\n "
			"%7s%30s\n %7s%30s\n %7s%30s\n ",
		"-1 ) ", "exit",
		"0 ) ", "nothing",
		"1 ) ", "alarm",
		"2 ) ", "echo",
		"3 ) ", "key press",
		"4 ) ", "justified cursor",
		"5 ) ", "absolute cursor",
		"6 ) ", "scroll" ) ;
	fprintf ( mcr_stdout, "Your selection : " ) ;

	int i = -1 ;
	SAFESCAN ( buffer ) ;
	i = atoi ( buffer ) ;
	return i ;
}

void alarmTest ( )
{
	fprintf ( mcr_stdout, "\nTest alarm.\nEnter seconds to justify: " ) ;
	SAFESCAN ( buffer ) ;
	int difference = atoi ( buffer ) ;
	if ( difference == 0 )
	{
		fprintf ( mcr_stdout, "Alarm seconds found 0. Nothing done.\n" ) ;
		return ;
	}

	time_t t = time ( NULL ) ;
	fprintf ( mcr_stdout, "Alarm begins at %s.\n", ctime ( & t ) ) ;

	t += difference ;
	struct tm * alPt = localtime ( & t ) ;
	mcr_Signal sig = { & mcr_iAlarm, {0} } ;
	sig.data.data = alPt ;
	mcr_send ( & sig ) ;

	t = time ( NULL ) ;
	fprintf ( mcr_stdout, "Alarm ends at %s.\n", ctime ( & t ) ) ;
}

void echo ( )
{
	fprintf ( mcr_stdout, "\nEcho test.\n" ) ;
	mcr_HIDEcho echo ;
	mcr_Echo_init ( & echo ) ;
	mcr_Signal sig = { & mcr_iHIDEcho, { & echo, 0 } } ;
	size_t count = mcr_Echo_count ( ) ;

	mcr_Key kk ;
	mcr_Key_init_with ( & kk, BTN_LEFT, 0, MCR_DOWN ) ;
	mcr_Signal sig2 = { & mcr_iKey, { & kk, 0} } ;
	mcr_send ( & sig2 ) ;
	MCR_KEY_SET_UP_TYPE ( & kk, MCR_UP ) ;
	int tmp ;
	UNUSED ( tmp ) ;
	tmp = MCR_NOOP_SEND ( & delay ) ;
	mcr_send ( & sig2 ) ;
	for ( size_t i = 0 ; i < count ; i++ )
	{
		// Set current.
		mcr_Echo_set ( & echo, i ) ;

		// Send current.
		fprintf ( mcr_stdout, "Send echo %d.\n", mcr_Echo_get ( & echo ) ) ;
		mcr_send ( & delaySig ) ;
		mcr_send ( & sig ) ;
	}
}

void keyPress ( )
{
	fprintf ( mcr_stdout, "\nTesting key press and release.\nEnter integer key code: " ) ;
	SAFESCAN ( buffer ) ;
	int key = atoi ( buffer ) ;
	fprintf ( mcr_stdout, "Enter integer scan code: " ) ;
	SAFESCAN ( buffer ) ;
	int scan = atoi ( buffer ) ;

	mcr_Key mKey ;
	mcr_Signal sig = { & mcr_iKey, { & mKey, 0} } ;
	mcr_Key_init_with ( & mKey, key, scan, MCR_DOWN ) ;

	fprintf ( mcr_stdout, "\nPressing key...\n" ) ;
	mcr_send ( & delaySig ) ;
	mcr_send ( & sig ) ;

	fprintf ( mcr_stdout, "\nKey is pressed. Releasing key...\n" ) ;
	mcr_Key_set_up_type ( & mKey, MCR_UP ) ;
	mcr_send ( & delaySig ) ;
	mcr_send ( & sig ) ;

	fprintf ( mcr_stdout, "\nNow testing both down and up together.\n" ) ;
	mcr_Key_set_up_type ( & mKey, MCR_BOTH ) ;
	mcr_send ( & delaySig ) ;
	mcr_send ( & sig ) ;
}

void justify ( )
{
//	char buffer [ 64 ] ;
	fprintf ( mcr_stdout, "\nTest justified cursor.\nEnter amount to justify: " ) ;
	SAFESCAN ( buffer ) ;
	int displacement = atoi ( buffer ) ;

	mcr_MoveCursor just ;
	mcr_Signal sig = { & mcr_iMoveCursor, { & just, 0} } ;
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
		fprintf ( mcr_stdout, "Move cursor: x=%lld, y=%lld, z=%lld\n",
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
	fprintf ( mcr_stdout, "\nTest absolute cursor.\n" ) ;

	mcr_MoveCursor abs ;
	mcr_MoveCursor_init ( & abs ) ;
	mcr_Signal sig = { & mcr_iMoveCursor, { & abs, 0} } ;
	mcr_MoveCursor_enable_justify ( & abs, 0 ) ;

	// Get and set x, y, z.
	fprintf ( mcr_stdout, "Enter x value: " ) ;
	SAFESCAN ( buffer ) ;
	long long p = atoll ( buffer ) ;
	mcr_MoveCursor_set_position ( & abs, MCR_X, p ) ;
	fprintf ( mcr_stdout, "Enter y value: " ) ;
	SAFESCAN ( buffer ) ;
	p = atoll ( buffer ) ;
	mcr_MoveCursor_set_position ( & abs, MCR_Y, p ) ;
	fprintf ( mcr_stdout, "Enter z value: " ) ;
	SAFESCAN ( buffer ) ;
	p = atoll ( buffer ) ;
	mcr_MoveCursor_set_position ( & abs, MCR_Z, p ) ;

	// Send current.
	fprintf ( mcr_stdout, "Move cursor: x=%lld, y=%lld, z=%lld\n",
			mcr_MoveCursor_get_position ( & abs, MCR_X ),
			mcr_MoveCursor_get_position ( & abs, MCR_Y ),
			mcr_MoveCursor_get_position ( & abs, MCR_Z ) ) ;
	mcr_send ( & delaySig ) ;
	mcr_send ( & sig ) ;
}

void scroll ( )
{
	fprintf ( mcr_stdout, "\nTest scrolling.\nEnter amount to scroll: " ) ;
	SAFESCAN ( buffer ) ;
	long long scrolling = atoll ( buffer ) ;

	mcr_Scroll scr ;
	mcr_Signal sig = { & mcr_iScroll, { & scr, 0} } ;
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
		fprintf ( mcr_stdout, "Scrolling: x=%lld, y=%lld, z=%lld\n",
				mcr_Scroll_get_dimension ( & scr, MCR_X ),
				mcr_Scroll_get_dimension ( & scr, MCR_Y ),
				mcr_Scroll_get_dimension ( & scr, MCR_Z ) ) ;
		mcr_send ( & delaySig ) ;
		mcr_send ( & sig ) ;

		// Reset to 0.
		mcr_Scroll_set_dimension ( & scr, dimI, 0 ) ;
	}
}
