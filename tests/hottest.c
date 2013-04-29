
# include "hotkey/hotkey.h"
# include <assert.h>

# define SIZE 0x10

int obj = 42 ;
mcr_Hot hotty ;
mcr_Signal sig ;
mcr_ISignal isig ;

void onComplete ( void )
{
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_reg_cleanup_filed ( onComplete, __FILE__ ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;

	printf ( "Setup - OK\n" ) ;
}

void test_Hot_init ( )
{
	memset ( & hotty, 1, sizeof ( hotty ) ) ;
	mcr_Hot_init ( & hotty ) ;
	assert ( ! memcmp ( & hotty, & mcr_Hot_Initial, sizeof ( hotty ) ) ) ;

	printf ( "mcr_Hot_init - OK\n" ) ;
}
void test_Hot_init_id ( )
{
	// Unknown signal type should be initialized regularly.
	mcr_Hot nullSig ;
	memset ( & hotty, 1, sizeof ( hotty ) ) ;
	mcr_Hot_init_id ( & hotty, -1 ) ;
	mcr_Hot_init ( & nullSig ) ;
	assert ( ! memcmp ( & hotty, & nullSig, sizeof ( hotty ) ) ) ;
	memset ( & hotty, 1, sizeof ( hotty ) ) ;
	mcr_Hot_init_id ( & hotty, mcr_Hot_count ( ) ) ;
	assert ( ! memcmp ( & hotty, & nullSig, sizeof ( hotty ) ) ) ;

	// Ensure known signal is registered.
	nullSig.intercept.type = & mcr_IAlarm ;
	nullSig.intercept_mods = 42 ;
	nullSig.use_specific = 1 ;
	nullSig.block = 1 ;
	mcr_Hot_register ( & nullSig, 0 ) ;
	mcr_Hot_init_id ( & hotty, 0 ) ;
	assert ( ! memcmp ( & hotty, & nullSig, sizeof ( hotty ) ) ) ;

	printf ( "mcr_Hot_init_id - OK\n" ) ;
}
int sCalled = 0, soCalled = 0, tCalled = 0, toCalled = 0, sigCalled = 0 ;
void simpleFnc ( )
{
	sCalled = 1 ;
}
int sigCalledFnc ( mcr_Signal * siggy )
{
	UNUSED ( siggy ) ;
	sigCalled = 1 ;
	assert ( siggy == & sig ) ;
}
void test_Hot_set_simple ( )
{
	memset ( & hotty, 1, sizeof ( hotty ) ) ;
	mcr_Hot_init ( & hotty ) ;
	hotty.callback_object = & obj ;
	mcr_Hot_set_simple ( & hotty, simpleFnc ) ;
	assert ( hotty.callback_type == MCR_CALL_SIMPLE ) ;
	assert ( hotty.call_function == simpleFnc ) ;
	assert ( hotty.callback_object == & obj ) ;

	mcr_Hot_set_simple ( & hotty, NULL ) ;
	assert ( hotty.callback_type == MCR_CALL_NONE ) ;
	assert ( ! hotty.call_function ) ;
	assert ( hotty.callback_object == & obj ) ;

	printf ( "mcr_Hot_set_simple - OK\n" ) ;
}
void simpleObjFnc ( void * objCalled )
{
	soCalled = 1 ;
	UNUSED ( objCalled ) ;
	assert ( objCalled == & obj ) ;
}
void test_Hot_set_simple_object ( )
{
	memset ( & hotty, 1, sizeof ( hotty ) ) ;
	// case 1.1 both null
	mcr_Hot_set_simple_object ( & hotty, NULL, NULL ) ;
	assert ( hotty.callback_type == MCR_CALL_NONE ) ;
	assert ( ! hotty.call_function ) ;
	assert ( ! hotty.callback_object ) ;
	// case 1.2 function null, object not
	mcr_Hot_set_simple_object ( & hotty, NULL, & obj ) ;
	assert ( hotty.callback_type == MCR_CALL_NONE ) ;
	assert ( ! hotty.call_function ) ;
	assert ( hotty.callback_object == & obj ) ;
	// case 2.1 function exists, object does not
	mcr_Hot_set_simple_object ( & hotty, simpleObjFnc, NULL ) ;
	assert ( hotty.callback_type == MCR_CALL_OBJECT ) ;
	assert ( hotty.call_function == simpleObjFnc ) ;
	assert ( ! hotty.callback_object ) ;
	// case 2.2 both exist
	mcr_Hot_set_simple_object ( & hotty, simpleObjFnc, & obj ) ;
	assert ( hotty.callback_type == MCR_CALL_OBJECT ) ;
	assert ( hotty.call_function == simpleObjFnc ) ;
	assert ( hotty.callback_object == & obj ) ;

	printf ( "mcr_Hot_set_simple_object - OK\n" ) ;
}
void triggerFnc ( mcr_Signal * sigPt, unsigned int * modsPt )
{
	tCalled = 1 ;
	UNUSED ( sigPt ) ;
	UNUSED ( modsPt ) ;
}
void test_Hot_set_trigger ( )
{
	mcr_Hot_init ( & hotty ) ;
	hotty.callback_object = & obj ;
	mcr_Hot_set_trigger ( & hotty, triggerFnc ) ;
	assert ( hotty.callback_type == MCR_CALL_TRIGGER ) ;
	assert ( hotty.call_function == triggerFnc ) ;
	assert ( hotty.callback_object == & obj ) ;

	mcr_Hot_set_trigger ( & hotty, NULL ) ;
	assert ( hotty.callback_type == MCR_CALL_NONE ) ;
	assert ( ! hotty.call_function ) ;
	assert ( hotty.callback_object == & obj ) ;

	printf ( "mcr_Hot_set_trigger - OK\n" ) ;
}
void triggerObjFnc ( void * obj, mcr_Signal * sigPt, unsigned int * modsPt )
{
	toCalled = 1 ;
	UNUSED ( obj ) ;
	UNUSED ( sigPt ) ;
	UNUSED ( modsPt ) ;
}
void test_Hot_set_trigger_object ( )
{
	memset ( & hotty, 1, sizeof ( hotty ) ) ;
	// case 1.1 both null
	mcr_Hot_set_trigger_object ( & hotty, NULL, NULL ) ;
	assert ( hotty.callback_type == MCR_CALL_NONE ) ;
	assert ( ! hotty.call_function ) ;
	assert ( ! hotty.callback_object ) ;
	// case 1.2 function null, object not
	mcr_Hot_set_trigger_object ( & hotty, NULL, & obj ) ;
	assert ( hotty.callback_type == MCR_CALL_NONE ) ;
	assert ( ! hotty.call_function ) ;
	assert ( hotty.callback_object == & obj ) ;
	// case 2.1 function exists, object does not
	mcr_Hot_set_trigger_object ( & hotty, triggerObjFnc, NULL ) ;
	assert ( hotty.callback_type == MCR_CALL_TRIGGER_OBJECT ) ;
	assert ( hotty.call_function == triggerObjFnc ) ;
	assert ( ! hotty.callback_object ) ;
	// case 2.2 both exist
	mcr_Hot_set_trigger_object ( & hotty, triggerObjFnc, & obj ) ;
	assert ( hotty.callback_type == MCR_CALL_TRIGGER_OBJECT ) ;
	assert ( hotty.call_function == triggerObjFnc ) ;
	assert ( hotty.callback_object == & obj ) ;

	printf ( "mcr_Hot_set_trigger_object - OK\n" ) ;
}
void test_Hot_set_signal ( )
{
	memset ( & hotty, 1, sizeof ( hotty ) ) ;
	sig.type = & mcr_IAlarm ;
	sig.data = & obj ;
	mcr_Hot_set_signal ( & hotty, & sig ) ;
	assert ( hotty.callback_type == MCR_CALL_SIGNAL ) ;
	assert ( ! hotty.call_function ) ;
	assert ( hotty.callback_object == & sig ) ;

	mcr_Hot_set_signal ( & hotty, NULL ) ;
	assert ( hotty.callback_type == MCR_CALL_NONE ) ;
	assert ( ! hotty.call_function ) ;
	assert ( ! hotty.callback_object ) ;

	printf ( "mcr_Hot_set_signal - OK\n" ) ;
}
void regFnc ( void * container, mcr_Hot * newHotkey )
{
	UNUSED ( container ) ;
	UNUSED ( newHotkey ) ;
	assert ( newHotkey == & hotty ) ;
	assert ( container == hotty.registrar_object ) ;
}
void regSpecFnc ( void * container,
				mcr_Hot * newHotkey,
				mcr_Signal * signalPt,
				unsigned int mods )
{
	UNUSED ( container ) ;
	UNUSED ( newHotkey ) ;
	UNUSED ( signalPt ) ;
	UNUSED ( mods ) ;
}
void unregFnc ( void * container,
				mcr_Hot * delHotkey )
{
	UNUSED ( container ) ;
	UNUSED ( delHotkey ) ;
}

void test_Hot_set_registrar ( )
{
	mcr_Hot_init ( & hotty ) ;
	mcr_Hot_set_registrar ( & hotty, regFnc, regSpecFnc, unregFnc, & obj ) ;
	assert ( hotty.registrar == regFnc ) ;
	assert ( hotty.registrar_specific == regSpecFnc ) ;
	assert ( hotty.unregistrar == unregFnc ) ;
	assert ( hotty.registrar_object == & obj ) ;
	mcr_Hot_init ( & hotty ) ;
	mcr_Hot_set_registrar ( & hotty, regFnc, regSpecFnc, unregFnc, NULL ) ;
	assert ( hotty.registrar == regFnc ) ;
	assert ( hotty.registrar_specific == regSpecFnc ) ;
	assert ( hotty.unregistrar == unregFnc ) ;
	assert ( ! hotty.registrar_object ) ;

	printf ( "mcr_Hot_set_registrar - OK\n" ) ;
}

void test_Hot_register ( )
{
	mcr_Hot reggi, reg2 ;
	memset ( & reggi, 1, sizeof ( reggi ) ) ;
	memset ( & reg2, 2, sizeof ( reg2 ) ) ;
	mcr_Hot_register ( & reggi, 0 ) ;
	mcr_Hot_register ( & reg2, 1 ) ;
	mcr_Hot_init_id ( & hotty, 0 ) ;
	assert ( ! memcmp ( & hotty, & reggi, sizeof ( mcr_Hot ) ) ) ;
	mcr_Hot_init_id ( & hotty, 1 ) ;
	assert ( ! memcmp ( & hotty, & reg2, sizeof ( mcr_Hot ) ) ) ;

	printf ( "mcr_Hot_register - OK\n" ) ;
}
void test_Hot_trigger ( )
{
	mcr_ISignal isig ;
	mcr_Signal sig ;
	sig.type = & isig ;
	isig.send = sigCalledFnc ;

	mcr_Hot_init ( & hotty ) ;
	mcr_Hot_set_simple ( & hotty, simpleFnc ) ;
	mcr_Hot_trigger ( & hotty, & sig, & mcr_InternalMods ) ;
	assert ( sCalled ) ;
	mcr_Hot_set_simple_object ( & hotty, simpleObjFnc, NULL ) ;
	mcr_Hot_trigger ( & hotty, & sig, & mcr_InternalMods ) ;
	assert ( soCalled ) ;
	mcr_Hot_set_trigger ( & hotty, triggerFnc ) ;
	mcr_Hot_trigger ( & hotty ) ;
	assert ( tCalled ) ;
	mcr_Hot_set_trigger_object ( & hotty, triggerObjFnc, NULL ) ;
	mcr_Hot_trigger ( & hotty ) ;
	assert ( toCalled ) ;
	mcr_Hot_set_signal ( & hotty, & sig ) ;
	mcr_Hot_trigger ( & hotty ) ;
	assert ( sigCalled ) ;

	printf ( "mcr_Hot_trigger - OK\n" ) ;
}
int calledCount = 0 ;
void countFnc ( )
{
	++ calledCount ;
}
void test_Hot_trigger_array ( )
{
	mcr_Hot mSet [ SIZE ] ;
	for ( int i = 0 ; i < SIZE ; i++ )
	{
		mcr_Hot_init ( mSet + i ) ;
		mcr_Hot_set_simple ( mSet + i, countFnc ) ;
	}
	mcr_Hot_trigger_array ( mSet, SIZE ) ;
	assert ( calledCount == SIZE ) ;

	printf ( "mcr_Hot_trigger_array - OK\n" ) ;
}

int main ( void )
{
	setup ( ) ;

	test_Hot_init ( ) ;
	test_Hot_init_id ( ) ;
	test_Hot_set_simple ( ) ;
	test_Hot_set_simple_object ( ) ;
	test_Hot_set_trigger ( ) ;
	test_Hot_set_trigger_object ( ) ;
	test_Hot_set_signal ( ) ;
	test_Hot_set_registrar ( ) ;
	test_Hot_register ( ) ;
	test_Hot_trigger ( ) ;
	test_Hot_trigger_array ( ) ;

	printf ( "Test complete without assertion error.\n" ) ;

	return 0 ;
}
