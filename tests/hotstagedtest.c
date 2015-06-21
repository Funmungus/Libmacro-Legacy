/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

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

	fprintf ( mcr_stdout, "test setup - OK\n" ) ;
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
		mcr_HotStaged_push_with ( & hs, 0, sigs + i, 0, ( unsigned int ) i ) ;
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
	fprintf ( mcr_stdout, "Hotstaged test completed without assertion.\n" ) ;
	return 0 ;
}
