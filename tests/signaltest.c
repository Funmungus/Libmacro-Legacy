
# include "signal/signal.h"
# include <assert.h>

# define SIZE 0xFF
# define MID ( ( int ) SIZE / ( int ) 2 )
# define HEXLEN 16
# define LEN (sizeof (nameBase) + HEXLEN + 1)

typedef struct
{
	char bytes [ MID ] ;
} s1 ;

# define SIZEOF ( sizeof ( s1 ) )

const char * nameBase = "ISIG0x" ;
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
void mkname ( char * container, unsigned int id ) ;
void reset ( ) ;

int send_s1 ( mcr_Signal * s1sig )
{
	verifySig ( s1sig ) ;
	assert ( s1sig->type == curI ) ;
	assert ( s1sig->data == curData ) ;
	return sendreturn ;
}

void verifysend ( mcr_Signal * s1sig )
{
	verifySig ( s1sig ) ;
	curI = s1sig->type ;
	curData = s1sig->data ;
	assert ( mcr_send ( s1sig ) ? sendreturn : ! sendreturn ) ;
	verifySig ( s1sig ) ;
	assert ( s1sig->type == curI ) ;
	assert ( s1sig->data == curData ) ;
}

void verifySig ( mcr_Signal * s1sig )
{
	assert ( s1sig != NULL ) ;
	assert ( s1sig->type->dispatch == s1disp || ! s1sig->type->dispatch ) ;
	assert ( s1sig->type->send == send_s1 ) ;
	assert ( s1sig->type->data_size == SIZEOF ) ;
	char naming [ LEN ] ;
	mkname ( naming, s1sig->type->id - isigs [ 0 ].id ) ;
	assert ( ! strncmp ( s1sig->type->name, naming, LEN - 1 ) ) ;
}

void onComplete ( void )
{
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_reg_cleanup_filed ( onComplete, __FILE__ ) ;
	mcr_signal_initialize ( ) ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		isigs [ i ].dispatch = s1disp ;
		mkname ( names [ i ], i ) ;
		mcr_ISignal_init ( isigs + i, names [ i ], send_s1, SIZEOF ) ;
		size_t retId = mcr_ISignal_register ( isigs + i ) ;
		assert ( retId != ( size_t ) -1 ) ;
		assert ( retId == isigs [ i ].id ) ;
		sigs [ i ].type = isigs + i ;
		verifySig ( sigs + i ) ;
	}

	printf ( "Setup - OK\n" ) ;
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

void mkname ( char * container, unsigned int id )
{
	snprintf ( container, LEN, "%s%x", nameBase, id ) ;
}

void test_AllDispatch ( )
{
	curI = isigs ;
	curData = datas ;
	// Case 1 With blocking
	dispatchBlocking = 1 ;
	// Case 1.1 Both exist with blocking, all disp not called.
	mcr_AllDispatch = s1alldisp ;
	// Blocked, send always true.
	assert ( mcr_send ( sigs ) ) ;
	verifyDispatched ( ) ;
	assert ( ! alldispatchCalled ) ;
	// Case 1.2 AllDisp null, regular exists, all disp not called.
	mcr_AllDispatch = NULL ;
	// Blocked, send always true.
	assert ( mcr_send ( sigs ) ) ;
	verifyDispatched ( ) ;
	assert ( ! alldispatchCalled ) ;
	// Case 1.3 Specific does not exist, but alldispatch does.
	// specific not called.
	isigs [ 0 ].dispatch = NULL ;
	mcr_AllDispatch = s1alldisp ;
	// Blocked, send always true.
	assert ( mcr_send ( sigs ) ) ;
	assert ( ! dispatchCalled ) ;
	verifyAllDispatched ( ) ;
	// Case 1.4 neither exist, not blocked
	mcr_AllDispatch = NULL ;
	verifysend ( sigs ) ;
	assert ( ! dispatchCalled ) ;
	assert ( ! alldispatchCalled ) ;

	// Case 2 Not blocking
	dispatchBlocking = 0 ;
	// Case 2.1 Both exist, both dispatched
	mcr_AllDispatch = s1alldisp ;
	isigs [ 0 ].dispatch = s1disp ;
	verifysend ( sigs ) ;
	verifyDispatched ( ) ;
	verifyAllDispatched ( ) ;
	// Case 2.2 AllDisp null, all disp not called
	mcr_AllDispatch = NULL ;
	verifysend ( sigs ) ;
	verifyDispatched ( ) ;
	assert ( ! alldispatchCalled ) ;
	// Case 2.3 Spec null, spec not called
	isigs [ 0 ].dispatch = NULL ;
	mcr_AllDispatch = s1alldisp ;
	verifysend ( sigs ) ;
	assert ( ! dispatchCalled ) ;
	verifyAllDispatched ( ) ;
	// Case 2.4 both null
	mcr_AllDispatch = NULL ;
	verifysend ( sigs ) ;
	assert ( ! dispatchCalled ) ;
	assert ( ! alldispatchCalled ) ;

	// Reset
	mcr_AllDispatch = s1alldisp ;
	isigs [ 0 ].dispatch = s1disp ;

	printf ( "mcr_AllDispatch - OK\n" ) ;
}

void test_Signal_init ( )
{
	mcr_Signal sig = {0} ;
	mcr_Signal_init ( & sig, NULL ) ;
	assert ( sig.type == NULL ) ;
	assert ( sig.data == NULL ) ;
	mcr_Signal_init ( & sig, isigs ) ;
	assert ( sig.type == isigs ) ;
	assert ( sig.data == NULL ) ;
	verifySig ( & sig ) ;

	printf ( "mcr_Signal_init - OK\n" ) ;
}
void test_Signal_init_with ( )
{
	mcr_Signal sig ;
	mcr_Signal_init_with ( & sig, NULL, NULL ) ;
	assert ( sig.data == NULL ) ;
	assert ( sig.type == NULL ) ;
	mcr_Signal_init_with ( & sig, isigs, NULL ) ;
	assert ( sig.data == NULL ) ;
	assert ( sig.type == isigs ) ;
	mcr_Signal_init_with ( & sig, NULL, datas ) ;
	assert ( sig.data == datas ) ;
	assert ( sig.type == NULL ) ;
	mcr_Signal_init_with ( & sig, isigs, datas ) ;
	assert ( sig.data == datas ) ;
	assert ( sig.type == isigs ) ;

	printf ( "mcr_Signal_init_with - OK\n" ) ;
}
void test_send ( )
{
	dispatchBlocking = 0 ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		verifysend ( sigs + i ) ;
		sendreturn = 0 ;
		verifysend ( sigs + i ) ;
		sendreturn = 1 ;
	}
	printf ( "mcr_send - OK\n" ) ;
}

void test_ISignal_init ( )
{
	mcr_ISignal isig = {0} ;
	const char * noname = "noname" ;
	mcr_ISignal_init ( & isig, noname, send_s1, SIZEOF ) ;
	assert ( isig.data_size == SIZEOF ) ;
	assert ( isig.dispatch == NULL ) ;
	assert ( isig.id == 0 ) ;
	assert ( ! strcmp ( isig.name, noname ) ) ;
	assert ( isig.send == send_s1 ) ;

	printf ( "mcr_ISignal_init - OK\n" ) ;
}
mcr_ISignal reggi = {0} ;
void test_ISignal_register ( )
{
	const char * noname = "noname" ;
	mcr_ISignal_init ( & reggi, noname, send_s1, SIZEOF ) ;
	size_t ret = mcr_ISignal_register ( & reggi ) ;
	assert ( ret != ( size_t ) -1 ) ;
	assert ( ret > 0 ) ;
	assert ( ret == reggi.id ) ;

	printf ( "mcr_ISignal_register - OK\n" ) ;
}
void test_ISignal_get ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		mcr_ISignal * got = mcr_ISignal_get ( isigs [ i ].id ) ;
		assert ( got == isigs + i ) ;
	}

	printf ( "mcr_ISignal_get - OK\n" ) ;
}
void test_ISignal_get_id ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		size_t got = mcr_ISignal_get_id ( isigs [ i ].name ) ;
		assert ( got == isigs [ i ].id ) ;
	}

	printf ( "mcr_ISignal_get_id - OK\n" ) ;
}
void test_ISignal_from_name ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		mcr_ISignal * got = mcr_ISignal_from_name ( isigs [ i ].name ) ;
		assert ( got == isigs + i ) ;
	}

	printf ( "mcr_ISignal_from_name - OK\n" ) ;
}
void test_ISignal_count ( )
{
	assert ( mcr_ISignal_count ( ) == reggi.id + 1 ) ;

	printf ( "mcr_ISignal_count - OK\n" ) ;
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

	printf ( "mcr_ISignal_get_all - OK\n" ) ;
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

	printf ( "mcr_name_compare - OK\n" ) ;
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

	printf ( "mcr_int_compare - OK\n" ) ;
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

	printf ( "mcr_unsigned_compare - OK\n" ) ;
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

	printf ( "mcr_size_t_compare - OK\n" ) ;
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

	printf ( "mcr_ref_compare - OK\n" ) ;
}


// 1 global variable, 1 init fnc, 1 clean fnc, 3 signal functions,
// 7 isignal functions, 5 comparators
int main ( void )
{
	setup ( ) ;
	test_AllDispatch ( ) ;
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

	printf ( "Test complete without assertion error.\n" ) ;

	return 0 ;
}
