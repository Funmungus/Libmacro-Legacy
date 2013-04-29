
# include "signal/signal.h"
# include <assert.h>

mcr_ISignal * knowns [ ] =
{
	& mcr_IAlarm, & mcr_IHIDEcho, & mcr_IKey,
	& mcr_IMoveCursor, & mcr_INoOp, & mcr_IScroll
} ;

void onComplete ( void )
{
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_reg_cleanup_filed ( onComplete, __FILE__ ) ;
	mcr_signal_initialize ( ) ;

	size_t len = sizeof ( knowns ) / sizeof ( mcr_ISignal * ) ;
	// Check registered for each
	for ( size_t i = 0 ; i < len ; i++ )
	{
		assert ( mcr_ISignal_from_name ( knowns [ i ]->name ) == knowns [ i ] ) ;
		assert ( mcr_ISignal_get ( knowns [ i ]->id ) == knowns [ i ] ) ;
		assert ( mcr_ISignal_get_id ( knowns [ i ]->name ) == knowns [ i ]->id ) ;
	}

	printf ( "Setup - OK\n" ) ;
}

// Echo codes
void test_Echo_code ( )
{
	// The two HIDEchos that are sure to be on every system are
	// LeftDown and LeftUp
	int ret = mcr_Echo_code ( "LeftDown" ) ;
	assert ( ret != -1 ) ;
	ret = mcr_Echo_code ( "LeftUp" ) ;
	assert ( ret != -1 ) ;
	// Null, empty, and simply wrong should all return -1
	ret = mcr_Echo_code ( NULL ) ;
	assert ( ret == -1 ) ;
	ret = mcr_Echo_code ( "" ) ;
	assert ( ret == -1 ) ;
	ret = mcr_Echo_code ( "jJoeiNbFI09aDdENn" ) ;
	assert ( ret == -1 ) ;
	// Try some symbols
	ret = mcr_Echo_code ( "+_ ) ( * & ^%$#@ ! ~=-` [ ] \\{}| ; ':\",./<>?" ) ;
	assert ( ret == -1 ) ;
	printf ( "mcr_Echo_code - OK\n" ) ;
}

// TODO: dynamically find set of all echo names
const char * names [ ] =
{
	"LeftDown", "LeftUp", "MiddleDown", "MiddleUp", "RightDown", "RightUp"
} ;

void test_Echo_name ( )
{
	for ( size_t i = 0 ; i < mcr_Echo_count ( ) ; i++ )
	{
		const char * ret = mcr_Echo_name ( i ) ;
		assert ( ! strcmp ( ret, names [ i ] ) ) ;
	}

	printf ( "mcr_Echo_name - OK\n" ) ;
}
void test_Echo_count ( )
{
	size_t len = sizeof ( names ) / sizeof ( const char * ) ;
	assert ( len == mcr_Echo_count ( ) ) ;
	printf ( "mcr_Echo_count - OK\n" ) ;
}
const char * testEchoName = "TestEchoName" ;
int testEchoValue = -1 ;
void test_Echo_register ( )
{
	// good check
	testEchoValue = mcr_Echo_register ( testEchoName ) ;
	assert ( testEchoValue == ( int ) mcr_Echo_count ( ) - 1 ) ;
	const char * retName = mcr_Echo_name ( testEchoValue ) ;
	assert ( ! strcmp ( retName, testEchoName ) ) ;
	int retVal = mcr_Echo_code ( testEchoName ) ;
	assert ( retVal == testEchoValue ) ;

	// bad check
	retVal = mcr_Echo_register ( NULL ) ;
	assert ( retVal == -1 ) ;
	retVal = mcr_Echo_register ( "" ) ;
	assert ( retVal == -1 ) ;

	printf ( "mcr_Echo_register - OK\n" ) ;
}

void test_Echo_inits ( )
{
	mcr_HIDEcho echo = {0} ;
	mcr_Echo_init ( & echo ) ;
	assert ( mcr_Echo_get ( & echo ) ==0 ) ;
	mcr_Echo_init_with ( & echo, 1 ) ;
	assert ( mcr_Echo_get ( & echo ) ==1 ) ;
	mcr_Echo_init ( & echo ) ;
	assert ( mcr_Echo_get ( & echo ) ==0 ) ;
	mcr_Echo_init_with ( & echo, -1 ) ;
	assert ( mcr_Echo_get ( & echo ) ==-1 ) ;
//	mcr_Echo_init_with ( & echo, 42 ) ;
//	assert ( mcr_Echo_get ( & echo ) == ( int ) mcr_Echo_count ( ) -1 ) ;

	printf ( "mcr_Echo_init - OK\n" ) ;
	printf ( "mcr_Echo_init_with - OK\n" ) ;
}

void test_Key_inits ( )
{
	mcr_Key k ;
	mcr_Key_init ( & k ) ;
	assert ( mcr_Key_get ( & k ) ==0 ) ;
	assert ( mcr_Key_get_scan ( & k ) ==0 ) ;
	assert ( mcr_Key_get_up_type ( & k ) == MCR_BOTH ) ;
	mcr_Key_init_with ( & k, 0, 0, MCR_DOWN ) ;
	assert ( mcr_Key_get ( & k ) ==0 ) ;
	assert ( mcr_Key_get_scan ( & k ) ==0 ) ;
	assert ( mcr_Key_get_up_type ( & k ) ==MCR_DOWN ) ;
	mcr_Key_init_with ( & k, -1, -1, MCR_UP ) ;
	assert ( mcr_Key_get ( & k ) ==0 ) ;
	assert ( mcr_Key_get_scan ( & k ) ==-1 ) ;
	assert ( mcr_Key_get_up_type ( & k ) ==MCR_UP ) ;
	mcr_Key_init_with ( & k, 500, 500, MCR_BOTH ) ;
	assert ( mcr_Key_get ( & k ) ==500 ) ;
	assert ( mcr_Key_get_scan ( & k ) ==500 ) ;
	assert ( mcr_Key_get_up_type ( & k ) ==MCR_BOTH ) ;
	mcr_Key_init ( & k ) ;
	assert ( mcr_Key_get ( & k ) ==0 ) ;
	assert ( mcr_Key_get_scan ( & k ) ==0 ) ;
	assert ( mcr_Key_get_up_type ( & k ) == MCR_BOTH ) ;

	mcr_Key_init_with ( & k, 0, 0, -1 ) ;
	assert ( mcr_Key_get ( & k ) ==0 ) ;
	assert ( mcr_Key_get_scan ( & k ) ==0 ) ;
	assert ( mcr_Key_get_up_type ( & k ) == MCR_BOTH ) ;

	printf ( "mcr_Key_init - OK\n" ) ;
	printf ( "mcr_Key_init_with - OK\n" ) ;
}

void space_equal ( const mcr_SpacePosition lhs, const mcr_SpacePosition rhs )
{
	for ( int i = 0 ; i < MCR_DIMENSION_CNT ; i++ )
	{
		assert ( lhs [ i ] == rhs [ i ] ) ;
	}
}

void test_MoveCursor_inits ( )
{
	mcr_MoveCursor mc ;
	mcr_SpacePosition sp, mem ;

	mcr_MoveCursor_init ( & mc ) ;
	mcr_MoveCursor_get ( & mc, sp ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;
	space_equal ( sp, mem ) ;
	assert ( mcr_MoveCursor_is_justify ( & mc ) ) ;

	for ( int ju = 0 ; ju < 2 ; ju++ )
	{
		mcr_MoveCursor_init_with ( & mc, sp, ju ) ;
		mcr_MoveCursor_get ( & mc, sp ) ;
		space_equal ( sp, mem ) ;
		assert ( mcr_MoveCursor_is_justify ( & mc ) ? ju : ! ju ) ;
	}

	memset ( sp, 0, sizeof ( sp ) ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;
	for ( int ju = 0 ; ju < 2 ; ju++ )
	{
		for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
		{
			// w/0
			mcr_MoveCursor_init_with ( & mc, sp, ju ) ;
			mcr_MoveCursor_get ( & mc, sp ) ;
			space_equal ( sp, mem ) ;
			assert ( mcr_MoveCursor_is_justify ( & mc ) ? ju : ! ju ) ;
			// w/-1
			sp [ xyz ] = mem [ xyz ] = -1 ;
			mcr_MoveCursor_init_with ( & mc, sp, ju ) ;
			memset ( sp, 0, sizeof ( sp ) ) ;
			mcr_MoveCursor_get ( & mc, sp ) ;
			space_equal ( sp, mem ) ;
			assert ( mcr_MoveCursor_is_justify ( & mc ) ? ju : ! ju ) ;
			// w/1
			sp [ xyz ] = mem [ xyz ] = 1 ;
			mcr_MoveCursor_init_with ( & mc, sp, ju ) ;
			memset ( sp, 0, sizeof ( sp ) ) ;
			mcr_MoveCursor_get ( & mc, sp ) ;
			space_equal ( sp, mem ) ;
			assert ( mcr_MoveCursor_is_justify ( & mc ) ? ju : ! ju ) ;
			// reset
			sp [ ju ] = mem [ ju ] = 0 ;
		}
		mcr_MoveCursor_init ( & mc ) ;
		memset ( sp, 0, sizeof ( sp ) ) ;
		mcr_MoveCursor_get ( & mc, sp ) ;
		memset ( mem, 0, sizeof ( mem ) ) ;
		space_equal ( sp, mem ) ;
		assert ( mcr_MoveCursor_is_justify ( & mc ) ) ;
	}

	printf ( "mcr_MoveCursor_init - OK\n" ) ;
	printf ( "mcr_MoveCursor_init_with - OK\n" ) ;
}

void test_Scroll_inits ( )
{
	mcr_Scroll scr ;
	mcr_Dimensions dim, mem ;
	memset ( dim, 0, sizeof ( dim ) ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;
	mcr_Scroll_init ( & scr ) ;
	mcr_Scroll_get ( & scr, dim ) ;
	space_equal ( dim, mem ) ;

	memset ( dim, 0, sizeof ( dim ) ) ;
	for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
	{
		// w/0
		mcr_Scroll_init_with ( & scr, dim ) ;
		mcr_Scroll_get ( & scr, dim ) ;
		space_equal ( dim, mem ) ;
		// w/-1
		dim [ xyz ] = mem [ xyz ] = -1 ;
		mcr_Scroll_init_with ( & scr, dim ) ;
		memset ( dim, 0, sizeof ( dim ) ) ;
		mcr_Scroll_get ( & scr, dim ) ;
		space_equal ( dim, mem ) ;
		// w/1
		dim [ xyz ] = mem [ xyz ] = 1 ;
		mcr_Scroll_init_with ( & scr, dim ) ;
		memset ( dim, 0, sizeof ( dim ) ) ;
		mcr_Scroll_get ( & scr, dim ) ;
		space_equal ( dim, mem ) ;
		// reset
		dim [ xyz ] = mem [ xyz ] = 0 ;
	}
	mcr_Scroll_init ( & scr ) ;
	memset ( dim, 0, sizeof ( dim ) ) ;
	mcr_Scroll_get ( & scr, dim ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;
	space_equal ( dim, mem ) ;

	printf ( "mcr_Scroll_init - OK\n" ) ;
	printf ( "mcr_Scroll_init_with - OK\n" ) ;
}

void test_Echo_getset ( )
{
	mcr_HIDEcho echo ;
	mcr_Echo_init ( & echo ) ;
	assert ( mcr_Echo_get ( & echo ) == 0 ) ;
	mcr_Echo_set ( & echo, -1 ) ;
	assert ( mcr_Echo_get ( & echo ) == -1 ) ;
	for ( int i = 0 ; ( unsigned int ) i < mcr_Echo_count ( ) ; i++ )
	{
		mcr_Echo_set ( & echo, i ) ;
		assert ( mcr_Echo_get ( & echo ) == i ) ;
	}
	mcr_Echo_init ( & echo ) ;
	assert ( mcr_Echo_get ( & echo ) == 0 ) ;

	printf ( "mcr_Echo_set - OK\n" ) ;
	printf ( "mcr_Echo_get - OK\n" ) ;
}

void test_Key_getset ( )
{
	mcr_Key k ;
	mcr_Key_init ( & k ) ;
	assert ( mcr_Key_get ( & k ) == 0 ) ;
	mcr_Key_set ( & k, -1 ) ;
	assert ( mcr_Key_get ( & k ) == 0 ) ;
	for ( int i = 0 ; i < 42 ; i++ )
	{
		mcr_Key_set ( & k, i ) ;
		assert ( mcr_Key_get ( & k ) == i ) ;
	}

	printf ( "mcr_Key_set - OK\n" ) ;
	printf ( "mcr_Key_get - OK\n" ) ;
}
void test_Key_getset_scan ( )
{
	mcr_Key k ;
	mcr_Key_init ( & k ) ;
	assert ( mcr_Key_get_scan ( & k ) == 0 ) ;
	for ( int i = -42 ; i < 42 ; i++ )
	{
		mcr_Key_set_scan ( & k, i ) ;
		assert ( mcr_Key_get_scan ( & k ) == i ) ;
	}

	printf ( "mcr_Key_set_scan - OK\n" ) ;
	printf ( "mcr_Key_get_scan - OK\n" ) ;
}
void test_Key_getset_up_type ( )
{
	mcr_Key k ;
	mcr_Key_init ( & k ) ;
	assert ( mcr_Key_get_up_type ( & k ) == MCR_BOTH ) ;

	mcr_Key_set_up_type ( & k, 5 ) ;
	assert ( mcr_Key_get_up_type ( & k ) == MCR_BOTH ) ;
	mcr_Key_set_up_type ( & k, MCR_DOWN ) ;
	assert ( mcr_Key_get_up_type ( & k ) == MCR_DOWN ) ;
	mcr_Key_set_up_type ( & k, MCR_UP ) ;
	assert ( mcr_Key_get_up_type ( & k ) == MCR_UP ) ;
	mcr_Key_set_up_type ( & k, MCR_BOTH ) ;
	assert ( mcr_Key_get_up_type ( & k ) == MCR_BOTH ) ;

	printf ( "mcr_Key_set_up_type - OK\n" ) ;
	printf ( "mcr_Key_get_up_type - OK\n" ) ;
}

void test_MoveCursor_getset ( )
{
	mcr_MoveCursor mc ;
	mcr_SpacePosition sp, mem ;
	mcr_MoveCursor_init ( & mc ) ;
	memset ( sp, 0, sizeof ( sp ) ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;
	mcr_MoveCursor_get ( & mc, sp ) ;
	space_equal ( sp, mem ) ;

	for ( int i = -1 ; i < 2 ; i++ )
	{
		for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
		{
			sp [ xyz ] =mem [ xyz ] =i ;
			mcr_MoveCursor_set ( & mc, sp ) ;
			memset ( sp, 0, sizeof ( sp ) ) ;
			mcr_MoveCursor_get ( & mc, sp ) ;
			space_equal ( sp, mem ) ;
			// reset
			sp [ xyz ] =mem [ xyz ] =0 ;
		}
	}

	printf ( "mcr_MoveCursor_get - OK\n" ) ;
	printf ( "mcr_MoveCursor_set - OK\n" ) ;
}
void test_MoveCursor_getset_position ( )
{
	mcr_MoveCursor mc ;
	mcr_MoveCursor_init ( & mc ) ;

	for ( int i = -1 ; i < 2 ; i++ )
	{
		for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
		{
			mcr_MoveCursor_set_position ( & mc, xyz, i ) ;
			assert ( mcr_MoveCursor_get_position ( & mc, xyz ) == i ) ;
		}
	}

	printf ( "mcr_MoveCursor_get_position - OK\n" ) ;
	printf ( "mcr_MoveCursor_set_position - OK\n" ) ;
}
void test_MoveCursor_justifies ( )
{
	mcr_MoveCursor mc ;
	mcr_MoveCursor_init ( & mc ) ;
	assert ( mcr_MoveCursor_is_justify ( & mc ) ) ;

	mcr_MoveCursor_enable_justify ( & mc, 0 ) ;
	assert ( ! mcr_MoveCursor_is_justify ( & mc ) ) ;

	mcr_MoveCursor_enable_justify ( & mc, 1 ) ;
	assert ( mcr_MoveCursor_is_justify ( & mc ) ) ;

	printf ( "mcr_MoveCursor_is_justify - OK\n" ) ;
	printf ( "mcr_MoveCursor_enable_justify - OK\n" ) ;
}

void test_Scroll_getset ( )
{
	mcr_Scroll scr ;
	mcr_SpacePosition sp, mem ;
	mcr_Scroll_init ( & scr ) ;
	memset ( sp, 0, sizeof ( sp ) ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;
	mcr_Scroll_get ( & scr, sp ) ;
	space_equal ( sp, mem ) ;

	for ( int i = -1 ; i < 2 ; i++ )
	{
		for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
		{
			sp [ xyz ] =mem [ xyz ] =i ;
			mcr_Scroll_set ( & scr, sp ) ;
			memset ( sp, 0, sizeof ( sp ) ) ;
			mcr_Scroll_get ( & scr, sp ) ;
			space_equal ( sp, mem ) ;
			// reset
			sp [ xyz ] =mem [ xyz ] =0 ;
		}
	}

	printf ( "mcr_Scroll_get - OK\n" ) ;
	printf ( "mcr_Scroll_set - OK\n" ) ;
}
void test_Scroll_getset_dimension ( )
{
	mcr_Scroll scr ;
	mcr_Scroll_init ( & scr ) ;

	for ( int i = -1 ; i < 2 ; i++ )
	{
		for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
		{
			mcr_Scroll_set_dimension ( & scr, xyz, i ) ;
			assert ( mcr_Scroll_get_dimension ( & scr, xyz ) == i ) ;
		}
	}

	printf ( "mcr_Scroll_get_dimension - OK\n" ) ;
	printf ( "mcr_Scroll_set_dimension - OK\n" ) ;
}


void test_resembles_justified ( )
{
	mcr_Dimensions dim, mem ;
	memset ( dim, 0, sizeof ( dim ) ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;
	// Assert accept any for either of lhs/rhs.
	assert ( mcr_resembles_justified ( dim, mem ) ) ;
	memset ( mem, 1, sizeof ( mem ) ) ;
	assert ( mcr_resembles_justified ( dim, mem ) ) ;
	memset ( mem, -1, sizeof ( mem ) ) ;
	assert ( mcr_resembles_justified ( dim, mem ) ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;
	memset ( dim, 1, sizeof ( dim ) ) ;
	assert ( mcr_resembles_justified ( dim, mem ) ) ;
	memset ( dim, -1, sizeof ( dim ) ) ;
	assert ( mcr_resembles_justified ( dim, mem ) ) ;
	// Assert similarity on either side, or not similar opposite side.
	for ( int i = -1 ; i < 2 ; i++ )
	{
		// Already tested 0.
		if ( i == 0 )
			++ i ;
		for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
		{
			dim [ xyz ] = i ;
			mem [ xyz ] = i * 5 ;
			assert ( mcr_resembles_justified ( dim, mem ) ) ;
			mem [ xyz ] = ( -i ) * 5 ;
			assert ( ! mcr_resembles_justified ( dim, mem ) ) ;
			// reset
			dim [ xyz ] = mem [ xyz ] = 0 ;
		}
	}

	printf ( "mcr_resembles_justified - OK\n" ) ;
}
void test_resembles_absolute ( )
{
	int positions [ ] = { -42, -1, 0, 1, 42 } ;
	size_t len = sizeof ( positions ) / sizeof ( int ) ;

	mcr_Dimensions dim, mem ;
	memset ( dim, 0, sizeof ( dim ) ) ;
	memset ( mem, 0, sizeof ( mem ) ) ;

	// Assert similarity on either side, or not for bad values.
	for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
	{
		for ( size_t pos = 0 ; pos < len ; pos++ )
		{
			int pp = positions [ pos ] ;
			dim [ xyz ] = pp ;
			for ( int sign = -1 ; sign < 2 ; sign += 2 )
			{
				mem [ xyz ] = pp + ( 42 * sign ) ;
				assert ( mcr_resembles_absolute ( dim, mem, 42 ) ) ;
				assert ( ! mcr_resembles_absolute ( dim, mem, 41 ) ) ;
				assert ( ! mcr_resembles_absolute ( dim, mem, 0 ) ) ;
				mem [ xyz ] = pp + ( 1 * sign ) ;
				assert ( mcr_resembles_absolute ( dim, mem, 42 ) ) ;
				assert ( mcr_resembles_absolute ( dim, mem, 1 ) ) ;
				assert ( ! mcr_resembles_absolute ( dim, mem, 0 ) ) ;
				mem [ xyz ] = pp ;
				assert ( mcr_resembles_absolute ( dim, mem, 42 ) ) ;
				assert ( mcr_resembles_absolute ( dim, mem, 1 ) ) ;
				assert ( mcr_resembles_absolute ( dim, mem, 0 ) ) ;
			}
		}
		// reset
		dim [ xyz ] = mem [ xyz ] = 0 ;
	}

	printf ( "mcr_resembles_absolute - OK\n" ) ;
}
void test_resembles ( )
{
	mcr_MoveCursor lhs, rhs ;
	mcr_MoveCursor_init ( & lhs ) ;
	mcr_MoveCursor_init ( & rhs ) ;

	for ( int ju = 0 ; ju < 2 ; ju++ )
	{
		// Make sure different justification is always false.
		mcr_MoveCursor_enable_justify ( & lhs, ju ) ;
		mcr_MoveCursor_enable_justify ( & rhs, ! ju ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 42 ) ) ;
		mcr_MoveCursor_enable_justify ( & lhs, ! ju ) ;
		mcr_MoveCursor_enable_justify ( & rhs, ju ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 42 ) ) ;
		mcr_MoveCursor_enable_justify ( & lhs, ju ) ;

		// Both equal w/same justification always true.
		assert ( mcr_resembles ( & lhs, & rhs, 0 ) ) ;
		assert ( mcr_resembles ( & lhs, & rhs, 1 ) ) ;
		assert ( mcr_resembles ( & lhs, & rhs, 42 ) ) ;
		for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
		{
			mcr_MoveCursor_set_position ( & lhs, xyz, 42 ) ;
			mcr_MoveCursor_set_position ( & rhs, xyz, 42 ) ;
			assert ( mcr_resembles ( & lhs, & rhs, 0 ) ) ;
			assert ( mcr_resembles ( & lhs, & rhs, 1 ) ) ;
			assert ( mcr_resembles ( & lhs, & rhs, 42 ) ) ;
			assert ( mcr_resembles ( & lhs, & rhs, 43 ) ) ;
			mcr_MoveCursor_set_position ( & lhs, xyz, 0 ) ;
			mcr_MoveCursor_set_position ( & rhs, xyz, 0 ) ;
		}
	}
	// zero/equal already checked.
	// justified, same sign/different sign
	mcr_MoveCursor_enable_justify ( & lhs, 1 ) ;
	mcr_MoveCursor_enable_justify ( & rhs, 1 ) ;
	for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
	{
		mcr_MoveCursor_set_position ( & lhs, xyz, -42 ) ;
		mcr_MoveCursor_set_position ( & rhs, xyz, -1 ) ;
		assert ( mcr_resembles ( & lhs, & rhs, 1 ) ) ;
		assert ( mcr_resembles ( & rhs, & lhs, 1 ) ) ;
		mcr_MoveCursor_set_position ( & rhs, xyz, 1 ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 1 ) ) ;
		assert ( ! mcr_resembles ( & rhs, & lhs, 1 ) ) ;
		mcr_MoveCursor_set_position ( & lhs, xyz, 42 ) ;
		assert ( mcr_resembles ( & lhs, & rhs, 1 ) ) ;
		assert ( mcr_resembles ( & rhs, & lhs, 1 ) ) ;
		//reset
		mcr_MoveCursor_set_position ( & lhs, xyz, 0 ) ;
		mcr_MoveCursor_set_position ( & rhs, xyz, 0 ) ;
	}
	// absolute, inside/outside range
	mcr_MoveCursor_enable_justify ( & lhs, 0 ) ;
	mcr_MoveCursor_enable_justify ( & rhs, 0 ) ;
	for ( int xyz = 0 ; xyz < MCR_DIMENSION_CNT ; xyz++ )
	{
		mcr_MoveCursor_set_position ( & lhs, xyz, -42 ) ;
		assert ( mcr_resembles ( & lhs, & rhs, 43 ) ) ;
		assert ( mcr_resembles ( & rhs, & lhs, 43 ) ) ;
		assert ( mcr_resembles ( & lhs, & rhs, 42 ) ) ;
		assert ( mcr_resembles ( & rhs, & lhs, 42 ) ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 41 ) ) ;
		assert ( ! mcr_resembles ( & rhs, & lhs, 41 ) ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 1 ) ) ;
		assert ( ! mcr_resembles ( & rhs, & lhs, 1 ) ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 0 ) ) ;
		assert ( ! mcr_resembles ( & rhs, & lhs, 0 ) ) ;
		mcr_MoveCursor_set_position ( & lhs, xyz, 42 ) ;
		assert ( mcr_resembles ( & lhs, & rhs, 43 ) ) ;
		assert ( mcr_resembles ( & rhs, & lhs, 43 ) ) ;
		assert ( mcr_resembles ( & lhs, & rhs, 42 ) ) ;
		assert ( mcr_resembles ( & rhs, & lhs, 42 ) ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 41 ) ) ;
		assert ( ! mcr_resembles ( & rhs, & lhs, 41 ) ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 1 ) ) ;
		assert ( ! mcr_resembles ( & rhs, & lhs, 1 ) ) ;
		assert ( ! mcr_resembles ( & lhs, & rhs, 0 ) ) ;
		assert ( ! mcr_resembles ( & rhs, & lhs, 0 ) ) ;
		//reset
		mcr_MoveCursor_set_position ( & lhs, xyz, 0 ) ;
	}

	printf ( "mcr_resembles - OK\n" ) ;
}




// 6 known ISignal types, 4 native types, 2 pause execution types,
// 4 echo code functions, 8 initializers for native types, 18 native
// type modifiers, and 3 mcr_SpacePosition/mcr_Dimensions functions
// send functions tested in sendtest.c
int main ( void )
{
	setup ( ) ;

	test_Echo_code ( ) ;
	test_Echo_name ( ) ;
	test_Echo_count ( ) ;
	test_Echo_register ( ) ;

	test_Echo_inits ( ) ;

	test_Key_inits ( ) ;

	test_MoveCursor_inits ( ) ;

	test_Scroll_inits ( ) ;

	test_Echo_getset ( ) ;

	test_Key_getset ( ) ;
	test_Key_getset_scan ( ) ;
	test_Key_getset_up_type ( ) ;

	test_MoveCursor_getset ( ) ;
	test_MoveCursor_getset_position ( ) ;
	test_MoveCursor_justifies ( ) ;

	test_Scroll_getset ( ) ;
	test_Scroll_getset_dimension ( ) ;

	test_resembles_justified ( ) ;
	test_resembles_absolute ( ) ;
	test_resembles ( ) ;



	printf ( "Test complete without assertion error.\n" ) ;

	return 0 ;
}
