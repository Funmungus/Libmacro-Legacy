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
# include "hotkey/hotstandard.h"
# include <assert.h>

# define SIZE 0x10

void cleanup ( )
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

	fprintf ( mcr_out, "test setup - OK\n" ) ;
}

mcr_Hot hot ;
mcr_HotStaged hs ;
mcr_Signal sigs [ SIZE ] ;
mcr_Key keys [ SIZE ] ;
int fncCalled = 0 ;
void chkfnc ( mcr_Hot * hotPt, mcr_Signal * sigPt, unsigned int mods )
{
	assert ( hotPt == & hot ) ;
	UNUSED ( sigPt ) ;
	UNUSED ( mods ) ;
	++ fncCalled ;
}

int main ( )
{
	int i ;
	setup ( ) ;
	mcr_HotStaged_init_with ( & hs, NULL, 0, MCR_BLOCK_ALL ) ;
	mcr_Hot_init_with ( & hot, & mcr_iHotStaged, & hs, 0, 0, chkfnc,
			NULL ) ;
	for ( i = 0 ; i < SIZE ; i ++ )
	{
		sigs [ i ].type = & mcr_iKey ;
		sigs [ i ].data.data = keys + i ;
		mcr_Key_init_with ( keys + i, i, i, ( i % 3 ) + MCR_DOWN ) ;
		mcr_HotStaged_push_with ( & hs, 0, sigs + i, 0, ( unsigned int ) i, MCR_ALL ) ;
	}
	for ( i = 0 ; i < SIZE ; i ++ )
	{
		assert ( ! ( ( mcr_Stage * ) mcr_Array_at ( & hs.stages, i ) )->activated ) ;
		assert ( ( ( mcr_Stage * ) mcr_Array_at ( & hs.stages, i ) )->isme ==
				mcr_Stage_iskey ) ;
		assert ( hot.type->trigger == mcr_HotStaged_trigger ) ;
		mcr_Hot_trigger ( & hot, sigs + i, i ) ;
		if ( i != SIZE - 1 ) 	// Last activation triggers and deactivates.
		{
			assert ( ( ( mcr_Stage * ) mcr_Array_at ( & hs.stages, i ) )->activated ) ;
		}
	}
	assert ( fncCalled == 1 ) ;
	for ( i = 0 ; i < SIZE ; i ++ )
	{
		assert ( ! ( ( mcr_Stage * ) mcr_Array_at ( & hs.stages, i ) )->activated ) ;
	}

	fncCalled = 0 ;
	for ( i = SIZE - 1 ; i >= 0 ; i -- )
	{
		assert ( ! ( ( mcr_Stage * ) mcr_Array_at ( & hs.stages, i ) )->activated ) ;
		mcr_Hot_trigger ( & hot, sigs + i, i ) ;
	}
	assert ( ! fncCalled ) ;

	mcr_Hot_free ( & hot ) ;
	fprintf ( mcr_out, "Hotstaged test completed without assertion.\n" ) ;
	return 0 ;
}
