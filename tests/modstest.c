
# include "hotkey/hotkey.h"
# include "hotkey/mods.h"
# include <assert.h>

# define SIZE 0x10

mcr_Mod data ;
mcr_Signal mod ;

void cleanup ( void )
{
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_reg_cleanup ( cleanup ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
	mod.type = & mcr_IMod ;
	mod.data = & data ;
}

void test_Mod_send ( )
{
	data.up_type = MCR_DOWN ;
	for ( unsigned int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_InternalMods = 0 ;
		data.modifiers = i ;
		mcr_send ( & mod ) ;
		assert ( mcr_InternalMods == i ) ;
	}

	data.up_type = MCR_UP ;
	for ( unsigned int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_InternalMods = ~ 0 ;
		data.modifiers = i ;
		mcr_send ( & mod ) ;
		assert ( mcr_InternalMods == ~ i ) ;
	}

	printf ( "mcr_Mod_send - OK.\n" ) ;
}

void test_Mod_names ( )
{
	char name [ SIZE ] ;
	mcr_Mod_clear_names ( ) ;
	for ( unsigned int i = 0 ; i < SIZE ; i ++ )
	{
		snprintf ( name, SIZE - 1, "0x%x", i ) ;
		mcr_Mod_set_name ( i, name ) ;
		assert ( mcr_Mod_get ( name ) == i ) ;
		assert ( ! strncmp ( mcr_Mod_get_name ( i ),
				name, SIZE - 1 ) ) ;
		assert ( mcr_Mod_count ( ) == i + 1 ) ;
	}
	unsigned int all [ SIZE ] ;
	mcr_Mod_get_all ( all, SIZE ) ;
	for ( unsigned int i = 0 ; i < SIZE ; i ++ )
	{
		assert ( all [ i ] == i ) ;
	}

	mcr_Mod_clear_names ( ) ;
	assert ( mcr_Mod_count ( ) == 0 ) ;

	printf ( "mcr_Mod_get - OK.\n" ) ;
	printf ( "mcr_Mod_get_name - OK.\n" ) ;
	printf ( "mcr_Mod_count - OK.\n" ) ;
	printf ( "mcr_Mod_get_all - OK.\n" ) ;
}

void test_Mod_echo ( )
{
	mcr_Mod tmpMod ;
	for ( unsigned int i = 0 ; i < SIZE ; i++ )
	{
		data.modifiers = i ;
		mcr_Mod_set_echo ( data, ( int ) i ) ;
		tmpMod = mcr_Mod_echo_get ( ( int ) i ) ;
		assert ( ! memcmp ( & tmpMod, & data,
				sizeof ( mcr_Mod ) ) ) ;
		assert ( mcr_Mod_echo_get_echo ( data ) == ( int ) i ) ;
		assert ( mcr_Mod_echo_count ( ) == i + 1 ) ;
	}
	int all [ SIZE ] ;
	mcr_Mod_echo_get_all ( all, SIZE ) ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		assert ( all [ i ] == i ) ;
	}

	mcr_Mod_clear_echo ( ) ;
	assert ( mcr_Mod_echo_count ( ) == 0 ) ;

	printf ( "mcr_Mod_echo_get - OK.\n" ) ;
	printf ( "mcr_Mod_echo_get_echo - OK.\n" ) ;
	printf ( "mcr_Mod_echo_echo_count - OK.\n" ) ;
	printf ( "mcr_Mod_echo_get_all - OK.\n" ) ;
}

void test_Mod_key ( )
{
	for ( unsigned int i = 0 ; i < SIZE ; i++ )
	{
		mcr_Mod_set_key ( i, ( int ) i ) ;
		assert ( mcr_Mod_key_get ( ( int ) i ) == i ) ;
		assert ( mcr_Mod_key_get_key ( i ) == ( int ) i ) ;
		assert ( mcr_Mod_key_count ( ) == i + 1 ) ;
	}
	int all [ SIZE ] ;
	mcr_Mod_key_get_all ( all, SIZE ) ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		assert ( all [ i ] == i ) ;
	}

	mcr_Mod_clear_key ( ) ;
	assert ( mcr_Mod_key_count ( ) == 0 ) ;

	printf ( "mcr_Mod_key_get - OK.\n" ) ;
	printf ( "mcr_Mod_key_get_key - OK.\n" ) ;
	printf ( "mcr_Mod_key_key_count - OK.\n" ) ;
	printf ( "mcr_Mod_key_get_all - OK.\n" ) ;
}

void test_HIDEcho_modify ( )
{
	mcr_Signal sig ;
	mcr_HIDEcho echo ;
	mcr_Echo_init ( & echo ) ;
	mcr_Signal_init_with ( & sig, & mcr_IHIDEcho, & echo ) ;
	for ( unsigned int mod = 0 ; mod < SIZE ; mod ++ )
	{
		data.modifiers = mod ;
		for ( int i = 0 ; i < ( int ) mcr_Echo_count ( ) ; i ++ )
		{
			mcr_Echo_set ( & echo, i ) ;
			data.up_type = MCR_DOWN ;
			mcr_Mod_set_echo ( data, i ) ;
			for ( unsigned int j = 0 ; j < SIZE ; j ++ )
			{
				assert ( mcr_HIDEcho_modify ( & sig, j )
						== mcr_Mod_add ( j, mod ) ) ;
			}
			data.up_type = MCR_UP ;
			mcr_Mod_set_echo ( data, i ) ;
			for ( unsigned int j = 0 ; j < SIZE ; j ++ )
			{
				assert ( mcr_HIDEcho_modify ( & sig, j )
						== mcr_Mod_remove ( j, mod ) ) ;
			}
		}
	}
	mcr_Mod_clear_echo ( ) ;

	printf ( "mcr_HIDEcho_modify - OK.\n" ) ;
}

void test_Key_modify ( )
{
	mcr_Signal sig ;
	mcr_Key key ;
	mcr_Key_init_with ( & key, 0, 0, MCR_DOWN ) ;
	mcr_Signal_init_with ( & sig, & mcr_IKey, & key ) ;
	for ( unsigned int mod = 0 ; mod < SIZE ; mod ++ )
	{
		for ( int i = 0 ; i < 0x42 ; i ++ )
		{
			mcr_Key_set ( & key, i ) ;
			mcr_Key_set_up_type ( & key, MCR_DOWN ) ;
			mcr_Mod_set_key ( mod, i ) ;
			for ( unsigned int j = 0 ; j < SIZE ; j ++ )
			{
				assert ( mcr_Key_modify ( & sig, j )
						== mcr_Mod_add ( j, mod ) ) ;
			}
			mcr_Key_set_up_type ( & key, MCR_UP ) ;
			mcr_Mod_set_key ( mod, i ) ;
			for ( unsigned int j = 0 ; j < SIZE ; j ++ )
			{
				assert ( mcr_Key_modify ( & sig, j )
						== mcr_Mod_remove ( j, mod ) ) ;
			}
		}
	}
	mcr_Mod_clear_key ( ) ;

	printf ( "mcr_Key_modify - OK.\n" ) ;
}

void test_Mod_compare ( )
{
	mcr_Mod lhs ;
	lhs.modifiers = data.modifiers = 0 ;
	lhs.up_type = data.up_type = MCR_DOWN ;
	assert ( ! mcr_Mod_compare ( & lhs, & data ) ) ;
	lhs.modifiers = 1 ;
	assert ( mcr_Mod_compare ( & lhs, & data ) > 0 ) ;

	printf ( "mcr_Mod_compare - OK.\n" ) ;
}

int main ( )
{
	setup ( ) ;

	test_Mod_send ( ) ;
	test_Mod_names ( ) ;
	test_Mod_echo ( ) ;
	test_Mod_key ( ) ;
	test_HIDEcho_modify ( ) ;
	test_Key_modify ( ) ;
	test_Mod_compare ( ) ;

	printf ( "Mods test completed without assertion.\n" ) ;
	return 0 ;
}
