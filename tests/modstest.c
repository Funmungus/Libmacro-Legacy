/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


# include "hotkey/hotkey.h"
# include "hotkey/mods.h"
# include <assert.h>

# define SIZE 0xFF

mcr_Mod data ;
mcr_Signal mod ;

void cleanup ( void )
{
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( cleanup ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
	mod.type = & mcr_iMod ;
	mod.data.data = & data ;
	mod.data.is_heap = 0 ;
}

void test_Mod_send ( )
{
	data.up_type = MCR_DOWN ;
	for ( unsigned int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_internalMods = 0 ;
		data.modifiers = i ;
		mcr_send ( & mod ) ;
		assert ( mcr_internalMods == i ) ;
	}

	data.up_type = MCR_UP ;
	for ( unsigned int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_internalMods = ~ 0 ;
		data.modifiers = i ;
		mcr_send ( & mod ) ;
		assert ( mcr_internalMods == ~ i ) ;
	}

	fprintf ( mcr_out, "mcr_Mod_send - OK.\n" ) ;
}

void test_Mod_names ( )
{
	char name [ SIZE ] ;
	mcr_Mod_clear_names ( ) ;
	for ( unsigned int i = 0 ; i < SIZE ; i ++ )
	{
		snprintf ( name, SIZE - 1, "0x%x", i ) ;
		mcr_Mod_set_name ( i, name ) ;
		assert ( mcr_Mod_from ( name ) == i ) ;
		assert ( ! strncmp ( mcr_Mod_name ( i ),
				name, SIZE - 1 ) ) ;
		assert ( mcr_Mod_count ( ) == i + 1 ) ;
	}
	unsigned int all [ SIZE ] ;
	mcr_Mod_all ( all, SIZE ) ;
	for ( unsigned int i = 0 ; i < SIZE ; i ++ )
	{
		assert ( all [ i ] == i ) ;
	}

	mcr_Mod_clear_names ( ) ;
	assert ( mcr_Mod_count ( ) == 0 ) ;

	fprintf ( mcr_out, "mcr_Mod_get - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_get_name - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_count - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_get_all - OK.\n" ) ;
}

void test_Mod_echo ( )
{
	mcr_Mod tmpMod ;
	for ( unsigned int i = 0 ; i < SIZE ; i++ )
	{
		data.modifiers = i ;
		mcr_Mod_set_echo ( data, ( int ) i ) ;
		tmpMod = mcr_Mod_from_echo ( ( int ) i ) ;
		assert ( ! memcmp ( & tmpMod, & data,
				sizeof ( mcr_Mod ) ) ) ;
		assert ( mcr_Mod_echo ( data ) == ( int ) i ) ;
		assert ( mcr_Mod_echo_count ( ) == i + 1 ) ;
	}
	int all [ SIZE ] ;
	mcr_Mod_echo_all ( all, SIZE ) ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		assert ( all [ i ] == i ) ;
	}

	mcr_Mod_clear_echo ( ) ;
	assert ( mcr_Mod_echo_count ( ) == 0 ) ;

	fprintf ( mcr_out, "mcr_Mod_echo_get - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_echo_get_echo - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_echo_echo_count - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_echo_get_all - OK.\n" ) ;
}

void test_Mod_key ( )
{
	for ( unsigned int i = 0 ; i < SIZE ; i++ )
	{
		mcr_Mod_set_key ( i, ( int ) i ) ;
		assert ( mcr_Mod_from_key ( ( int ) i ) == i ) ;
		assert ( mcr_Mod_key_key ( i ) == ( int ) i ) ;
		assert ( mcr_Mod_key_count ( ) == i + 1 ) ;
	}
	int all [ SIZE ] ;
	mcr_Mod_key_all ( all, SIZE ) ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		assert ( all [ i ] == i ) ;
	}

	mcr_Mod_clear_key ( ) ;
	assert ( mcr_Mod_key_count ( ) == 0 ) ;

	fprintf ( mcr_out, "mcr_Mod_key_get - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_key_get_key - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_key_key_count - OK.\n" ) ;
	fprintf ( mcr_out, "mcr_Mod_key_get_all - OK.\n" ) ;
}

void test_HIDEcho_modify ( )
{
	mcr_Signal sig ;
	mcr_HIDEcho echo ;
	mcr_Echo_init ( & echo ) ;
	mcr_Signal_init_with ( & sig, & mcr_iHIDEcho, & echo, 0, 1 ) ;
	for ( unsigned int mod = 0 ; mod < SIZE ; mod ++ )
	{
		data.modifiers = mod ;
		for ( int i = 0 ; i < ( int ) mcr_Echo_count ( ) ; i ++ )
		{
			mcr_Echo_set_echo ( & echo, i ) ;
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

	fprintf ( mcr_out, "mcr_HIDEcho_modify - OK.\n" ) ;
}

void test_Key_modify ( )
{
	mcr_Signal sig ;
	mcr_Key key ;
	mcr_Key_init_with ( & key, 0, 0, MCR_DOWN ) ;
	mcr_Signal_init_with ( & sig, & mcr_iKey, & key, 0, 1 ) ;
	for ( unsigned int mod = 0 ; mod < SIZE ; mod ++ )
	{
		for ( int i = 0 ; i < 0x42 ; i ++ )
		{
			mcr_Key_set_key ( & key, i ) ;
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

	fprintf ( mcr_out, "mcr_Key_modify - OK.\n" ) ;
}

void test_Mod_compare ( )
{
	mcr_Mod lhs ;
	lhs.modifiers = data.modifiers = 0 ;
	lhs.up_type = data.up_type = MCR_DOWN ;
	assert ( ! mcr_Mod_compare ( & lhs, & data ) ) ;
	lhs.modifiers = 1 ;
	assert ( mcr_Mod_compare ( & lhs, & data ) > 0 ) ;

	fprintf ( mcr_out, "mcr_Mod_compare - OK.\n" ) ;
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

	fprintf ( mcr_out, "Mods test completed without assertion.\n" ) ;
	return 0 ;
}
