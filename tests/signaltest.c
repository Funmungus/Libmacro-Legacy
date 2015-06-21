/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */


# include "signal/signal.h"
# include <assert.h>

# define SIZE 0xFF
# define MID ( ( int ) SIZE / ( int ) 2 )
# define HEXLEN 16
# define LEN ( sizeof ( nameBase ) + HEXLEN + 1)

typedef struct
{
	char bytes [ MID ] ;
} s1 ;

# define SIZEOF ( sizeof ( s1 ) )

const char * nameBase = ( const char * ) "ISIG0x" ;
// Each name may be nameBase plus 16 digits of a hex number
char names [ SIZE ] [ LEN ] ;
s1 datas [ SIZE ] ;
mcr_ISignal isigs [ SIZE ] ;
mcr_Signal sigs [ SIZE ] ;


int i ;
mcr_ISignal * curI = NULL ;
s1 * curData = NULL ;
int sendreturn = 1 ;

void verifySig ( mcr_Signal * s1sig ) ;
int s1disp ( mcr_Signal * s1sig ) ;
void mkname ( char * container, int id ) ;
void reset ( ) ;

int send_s1 ( mcr_Signal * s1sig )
{
	verifySig ( s1sig ) ;
	assert ( s1sig->type == curI ) ;
	assert ( s1sig->data.data == curData ) ;
	return sendreturn ;
}

void verifysend ( mcr_Signal * s1sig )
{
	verifySig ( s1sig ) ;
	curI = s1sig->type ;
	curData = s1sig->data.data ;
	assert ( mcr_send ( s1sig ) ? sendreturn : ! sendreturn ) ;
	verifySig ( s1sig ) ;
	assert ( s1sig->type == curI ) ;
	assert ( s1sig->data.data == curData ) ;
}

void verifySig ( mcr_Signal * s1sig )
{
	assert ( s1sig != NULL ) ;
	assert ( s1sig->type->dispatch == s1disp || ! s1sig->type->dispatch ) ;
	assert ( s1sig->type->send == send_s1 ) ;
	assert ( s1sig->type->interface.data_size == SIZEOF ) ;
}

void onComplete ( void )
{
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( onComplete ) ;
	mcr_signal_initialize ( ) ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		isigs [ i ].dispatch = s1disp ;
		mkname ( names [ i ], i ) ;
		mcr_ISignal_init ( isigs + i, send_s1, SIZEOF ) ;
		size_t retId = mcr_ISignal_register ( isigs + i ) ;
		mcr_ISignal_set_name ( isigs + i, names [ i ] ) ;
		assert ( retId != ( size_t ) -1 ) ;
		assert ( retId == isigs [ i ].interface.id ) ;
		sigs [ i ].type = isigs + i ;
		verifySig ( sigs + i ) ;
	}

	fprintf ( mcr_stdout, "Setup - OK\n" ) ;
}

int alldispatchCalled = 0 ;
int dispatchCalled = 0 ;
int dispatchBlocking = 0 ;
int s1disp ( mcr_Signal * s1sig )
{
	verifySig ( s1sig ) ;
	dispatchCalled = 1 ;
	return dispatchBlocking ;
}
int s1alldisp ( mcr_Signal * s1sig )
{
	verifySig ( s1sig ) ;
	alldispatchCalled = 1 ;
	return dispatchBlocking ;
}

void verifyDispatched ( )
{
	assert ( dispatchCalled ) ;
	dispatchCalled = 0 ;
}
void verifyAllDispatched ( )
{
	assert ( alldispatchCalled ) ;
	alldispatchCalled = 0 ;
}

void mkname ( char * container, int id )
{
	snprintf ( container, LEN, "%s%x", nameBase, id ) ;
}

void test_Signal_init ( )
{
	mcr_Signal sig = {0} ;
	mcr_Signal_init ( & sig, NULL ) ;
	assert ( sig.type == NULL ) ;
	assert ( sig.data.data == NULL ) ;
	mcr_Signal_init ( & sig, isigs ) ;
	assert ( sig.type == isigs ) ;
	assert ( sig.data.data == NULL ) ;
	verifySig ( & sig ) ;

	fprintf ( mcr_stdout, "mcr_Signal_init - OK\n" ) ;
}
void test_Signal_init_with ( )
{
	mcr_Signal sig ;
	mcr_Signal_init_with ( & sig, NULL, NULL, 0 ) ;
	assert ( sig.data.data == NULL ) ;
	assert ( sig.type == NULL ) ;
	mcr_Signal_init_with ( & sig, isigs, NULL, 0 ) ;
	assert ( sig.data.data == NULL ) ;
	assert ( sig.type == isigs ) ;
	mcr_Signal_init_with ( & sig, NULL, datas, 0 ) ;
	assert ( sig.data.data == datas ) ;
	assert ( sig.type == NULL ) ;
	mcr_Signal_init_with ( & sig, isigs, datas, 0 ) ;
	assert ( sig.data.data == datas ) ;
	assert ( sig.type == isigs ) ;

	fprintf ( mcr_stdout, "mcr_Signal_init_with - OK\n" ) ;
}
void test_send ( )
{
	dispatchBlocking = 0 ;
	sendreturn = 1 ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		verifysend ( sigs + i ) ;
	}
	fprintf ( mcr_stdout, "mcr_send - OK\n" ) ;
}

void test_ISignal_init ( )
{
	mcr_ISignal isig ;
	mcr_ISignal_init ( & isig, send_s1, SIZEOF ) ;
	assert ( isig.interface.data_size == SIZEOF ) ;
	assert ( isig.dispatch == NULL ) ;
	assert ( isig.interface.id == ( size_t ) -1 ) ;
	assert ( isig.send == send_s1 ) ;

	fprintf ( mcr_stdout, "mcr_ISignal_init - OK\n" ) ;
}
mcr_ISignal reggi ;
void test_ISignal_register ( )
{
	mcr_ISignal_init ( & reggi, send_s1, SIZEOF ) ;
	size_t ret = mcr_ISignal_register ( & reggi ) ;
	assert ( ret != ( size_t ) -1 ) ;
	assert ( ret > 0 ) ;
	assert ( ret == reggi.interface.id ) ;

	fprintf ( mcr_stdout, "mcr_ISignal_register - OK\n" ) ;
}
void test_ISignal_get ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		mcr_ISignal * got = mcr_ISignal_get ( isigs [ i ].interface.id ) ;
		assert ( got == isigs + i ) ;
	}

	fprintf ( mcr_stdout, "mcr_ISignal_get - OK\n" ) ;
}
void test_ISignal_get_id ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		size_t got = mcr_ISignal_get_id ( names [ i ] ) ;
		assert ( got == isigs [ i ].interface.id ) ;
	}

	fprintf ( mcr_stdout, "mcr_ISignal_get_id - OK\n" ) ;
}
void test_ISignal_from_name ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		mcr_ISignal * got = mcr_ISignal_from_name (
				names [ i ] ) ;
		assert ( got == isigs + i ) ;
	}

	fprintf ( mcr_stdout, "mcr_ISignal_from_name - OK\n" ) ;
}
void test_ISignal_count ( )
{
	assert ( mcr_ISignal_count ( ) == reggi.interface.id + 1 ) ;

	fprintf ( mcr_stdout, "mcr_ISignal_count - OK\n" ) ;
}
void test_ISignal_get_all ( )
{
	mcr_ISignal ** set = NULL ;
	size_t count = mcr_ISignal_count ( ) ;
	assert ( count != 0 ) ;
	assert ( count != ( size_t ) -1 ) ;
	set = calloc ( count, sizeof ( mcr_ISignal * ) ) ;
	assert ( set ) ;
	mcr_ISignal_get_all ( set, count ) ;
	for ( i = 0 ; i < ( int ) count ; i++ )
	{
		mcr_ISignal * found = mcr_ISignal_get ( i ) ;
		assert ( set [ i ] == found ) ;
	}
	free ( set ) ;

	fprintf ( mcr_stdout, "mcr_ISignal_get_all - OK\n" ) ;
}
void test_name_compare ( )
{
	const char * con, * noc ;
	// Good name compares.
	for ( i = 0 ; i < SIZE ; i++ )
	{
		con = names [ i ] ;
		int ret = mcr_name_compare ( & con, & con ) ;
		assert ( ! ret ) ;
	}
	// Bad name compares.
	for ( i = 0 ; i < SIZE - 1 ; i++ )
	{
		con = names [ i ] ;
		noc = names [ i + 1 ] ;
		int ret = mcr_name_compare ( & con, & noc ) ;
		assert ( ret ) ;
	}

	fprintf ( mcr_stdout, "mcr_name_compare - OK\n" ) ;
}
void test_int_compare ( )
{
	int j = 0 ;
	for ( i = 0, j = 0 ; i < SIZE ; i++, j++ )
	{
		int ret = mcr_int_compare ( & i, & j ) ;
		assert ( ! ret ) ;
	}
	j = 1 ;
	for ( i = 0 ; i < SIZE ; i++, j++ )
	{
		int ret = mcr_int_compare ( & i, & j ) ;
		assert ( ret < 0 ) ;
	}
	j = -1 ;
	for ( i = 0 ; i < SIZE ; i++, j++ )
	{
		int ret = mcr_int_compare ( & i, & j ) ;
		assert ( ret > 0 ) ;
	}

	fprintf ( mcr_stdout, "mcr_int_compare - OK\n" ) ;
}
void test_unsigned_compare ( )
{
	unsigned int lhs = 0, rhs = 0 ;
	for ( lhs = 0, rhs = 0 ; lhs < SIZE ; lhs++, rhs++ )
	{
		int ret = mcr_unsigned_compare ( & lhs, & rhs ) ;
		assert ( ! ret ) ;
	}

	lhs = 0 ;
	rhs = 1 ;
	for ( ; lhs < SIZE ; lhs++, rhs++ )
	{
		int ret = mcr_unsigned_compare ( & lhs, & rhs ) ;
		assert ( ret < 0 ) ;
	}

	lhs = 1 ;
	rhs = 0 ;
	for ( ; lhs < SIZE ; lhs++, rhs++ )
	{
		int ret = mcr_unsigned_compare ( & lhs, & rhs ) ;
		assert ( ret > 0 ) ;
	}

	lhs = ( unsigned int ) -5 ;
	rhs = 5 ;
	assert ( mcr_unsigned_compare ( & lhs, & rhs ) > 0 ) ;
	lhs = 5 ;
	rhs = ( unsigned int ) -5 ;
	assert ( mcr_unsigned_compare ( & lhs, & rhs ) < 0 ) ;

	fprintf ( mcr_stdout, "mcr_unsigned_compare - OK\n" ) ;
}
void test_size_t_compare ( )
{
	size_t lhs = 0, rhs = 0 ;
	for ( lhs = 0, rhs = 0 ; lhs < SIZE ; lhs++, rhs++ )
	{
		int ret = mcr_size_t_compare ( & lhs, & rhs ) ;
		assert ( ! ret ) ;
	}

	lhs = 0 ;
	rhs = 1 ;
	for ( ; lhs < SIZE ; lhs++, rhs++ )
	{
		int ret = mcr_size_t_compare ( & lhs, & rhs ) ;
		assert ( ret < 0 ) ;
	}

	lhs = 1 ;
	rhs = 0 ;
	for ( ; lhs < SIZE ; lhs++, rhs++ )
	{
		int ret = mcr_size_t_compare ( & lhs, & rhs ) ;
		assert ( ret > 0 ) ;
	}

	lhs = ( size_t ) -5 ;
	rhs = 5 ;
	assert ( mcr_size_t_compare ( & lhs, & rhs ) > 0 ) ;
	lhs = 5 ;
	rhs = ( size_t ) -5 ;
	assert ( mcr_size_t_compare ( & lhs, & rhs ) < 0 ) ;

	fprintf ( mcr_stdout, "mcr_size_t_compare - OK\n" ) ;
}
void test_ref_compare ( )
{
	mcr_ISignal * lhs, * rhs ;
	for ( i = 0, lhs = isigs + i, rhs = isigs + i ;
		i < SIZE ; i++, lhs++, rhs++ )
	{
		int ret = mcr_ref_compare ( & lhs, & rhs ) ;
		assert ( ! ret ) ;
	}
	for ( i = 0, lhs = isigs + i, rhs = isigs + i + 1 ;
		i < SIZE - 1 ; i++, lhs++, rhs++ )
	{
		int ret = mcr_ref_compare ( & lhs, & rhs ) ;
		assert ( ret < 0 ) ;
	}
	for ( i = 0, lhs = isigs + i + 1, rhs = isigs + i ;
		i < SIZE - 1 ; i++, lhs++, rhs++ )
	{
		int ret = mcr_ref_compare ( & lhs, & rhs ) ;
		assert ( ret > 0 ) ;
	}

	fprintf ( mcr_stdout, "mcr_ref_compare - OK\n" ) ;
}


// 1 global variable, 1 init fnc, 1 clean fnc, 3 signal functions,
// 7 isignal functions, 5 comparators
int main ( void )
{
	setup ( ) ;
	test_Signal_init ( ) ;
	test_Signal_init_with ( ) ;
	test_send ( ) ;

	test_ISignal_init ( ) ;
	test_ISignal_register ( ) ;
	test_ISignal_get ( ) ;
	test_ISignal_get_id ( ) ;
	test_ISignal_from_name ( ) ;
	test_ISignal_count ( ) ;
	test_ISignal_get_all ( ) ;

	test_name_compare ( ) ;
	test_int_compare ( ) ;
	test_unsigned_compare ( ) ;
	test_size_t_compare ( ) ;
	test_ref_compare ( ) ;

	fprintf ( mcr_stdout, "Test complete without assertion error.\n" ) ;

	return 0 ;
}
