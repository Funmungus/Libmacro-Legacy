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


# include "util/util.h"
# include <assert.h>
# include <stdarg.h>

# define SIZE 0xFF
# define MID ( ( int ) SIZE / ( int ) 2 )
# define STRSIZE MID

typedef struct {
	char bytes [ STRSIZE ] ;
} mstruct ;

# define STR mstruct
# define SIZEOF sizeof ( STR )

STR structset [ SIZE ] ;
STR * pt ;
STR val, lav ;
int i ;
mcr_Array arr ;

void reset ( ) ;

void fail ( int line )
{
	fprintf ( stderr, "Function failed at %d\n", line ) ;
	reset ( ) ;
}
# define FAILIF(expression) \
if ( expression ) \
{ \
	fail ( __LINE__ ) ; \
	return ; \
}

void on_complete ( void )
{
	mcr_Array_free ( & arr ) ;
}

void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( on_complete ) ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		memset ( structset + i, i, SIZEOF ) ;
	}
	memset ( & val, 42, SIZEOF ) ;
	memset ( & lav, 24, SIZEOF ) ;
}

void reset ( )
{
	if ( ! mcr_Array_from_array ( & arr, structset, SIZE ) )
	{
		fail ( __LINE__ ) ;
	}
	assert ( arr.array != NULL ) ;
	assert ( arr.element_size == SIZEOF ) ;
	assert ( arr.size != 0 ) ;
	assert ( arr.used == SIZE ) ;
}

void test_Array_init ( )
{
	mcr_Array_init ( & arr, SIZEOF ) ;
	assert ( arr.element_size == SIZEOF ) ;
	reset ( ) ;
}

void test_Array_free ( )
{
	mcr_Array_free ( & arr ) ;
	assert ( arr.array == NULL ) ;
	assert ( arr.used == 0 ) ;
	assert ( arr.size == 0 ) ;
	reset ( ) ;
}

void test_Array_push ( )
{
	STR zeros ;
	memset ( & zeros, 0, SIZEOF ) ;
	mcr_Array_free ( & arr ) ;
	assert ( arr.used == 0 ) ;
	assert ( arr.size == 0 ) ;
	assert ( arr.array == 0 ) ;
	assert ( arr.element_size == SIZEOF ) ;
	// push value
	FAILIF ( ! mcr_Array_push ( & arr, & val ) ) ;
	assert ( arr.used == 1 ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 0 ), & val, SIZEOF ) ) ;
	// push empty
	FAILIF ( ! mcr_Array_push ( & arr, NULL ) ) ;
	assert ( arr.used == 2 ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 1 ), & zeros, SIZEOF ) ) ;
	reset ( ) ;
}

void test_Array_push_unique ( )
{
	mcr_Array_free ( & arr ) ;
	FAILIF ( ! mcr_Array_push_unique ( & arr, structset + 0 ) ) ;
	assert ( arr.used == 1 ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 0 ), structset + 0, SIZEOF ) ) ;
	FAILIF ( ! mcr_Array_push_unique ( & arr, structset + 0 ) ) ;
	assert ( arr.used == 1 ) ;
	FAILIF ( ! mcr_Array_push_unique ( & arr, structset + 1 ) ) ;
	assert ( arr.used == 2 ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 1 ), structset + 1, SIZEOF ) ) ;
	FAILIF ( ! mcr_Array_push_unique ( & arr, structset + 1 ) ) ;
	assert ( arr.used == 2 ) ;
	reset ( ) ;
}

void test_Array_set ( )
{
	int success = mcr_Array_set ( & arr, 0, & val ) ;
	success = mcr_Array_set ( & arr, MID, & val ) ? success : 0 ;
	success = mcr_Array_set ( & arr, SIZE, & val ) ? success : 0 ;
	FAILIF ( ! success ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 0 ), & val, SIZEOF ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, MID ), & val, SIZEOF ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, SIZE ), & val, SIZEOF ) ) ;
	reset ( ) ;
}

void test_Array_insert ( )
{
	FAILIF ( ! mcr_Array_insert ( & arr, SIZE, & val ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, SIZE ), & val, SIZEOF ) ) ;

	FAILIF ( ! mcr_Array_insert ( & arr, MID, & val ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, MID ), & val, SIZEOF ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, MID + 1 ), structset + MID,
			SIZEOF ) ) ;

	FAILIF ( ! mcr_Array_insert ( & arr, 0, & val ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 0 ), & val, SIZEOF ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 1 ), structset + 0, SIZEOF ) ) ;
	reset ( ) ;
}

void test_Array_insert_filled ( )
{
	STR zeros ;
	memset ( & zeros, 0, SIZEOF ) ;

	// beginning
	mcr_Array_trim ( & arr ) ;
	FAILIF ( ! mcr_Array_insert_filled ( & arr, 0, & val, & lav ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 0 ), & val, SIZEOF ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 1 ), structset + 0, SIZEOF ) ) ;

	// mid
	reset ( ) ;
	mcr_Array_trim ( & arr ) ;
	FAILIF ( ! mcr_Array_insert_filled ( & arr, MID, & val, & lav ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, MID ), & val, SIZEOF ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, MID + 1 ), structset + MID,
			SIZEOF ) ) ;
	// end
	reset ( ) ;
	mcr_Array_trim ( & arr ) ;
	FAILIF ( ! mcr_Array_insert_filled ( & arr, SIZE, & val, & lav ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, SIZE ), & val, SIZEOF ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, SIZE - 1 ),
			structset + SIZE - 1, SIZEOF ) ) ;

	// check filling with no room left
	arr.used = SIZE ;
	mcr_Array_trim ( & arr ) ;
	FAILIF ( ! mcr_Array_insert_filled ( & arr, SIZE + MID, & val,
			& lav ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, SIZE + MID ), & val,
			SIZEOF ) ) ;
	for ( i = SIZE ; i < SIZE + MID ; i++ )
	{
		assert ( ! memcmp ( mcr_Array_at ( & arr, i ), & lav, SIZEOF ) ) ;
	}

	/* check changing end filling to zeros.
	 * This is also check filling with plenty of space. */
	arr.used = SIZE ;
	FAILIF ( ! mcr_Array_insert_filled ( & arr, SIZE + MID, & val,
			& zeros ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, SIZE + MID ), & val,
			SIZEOF ) ) ;
	for ( i = SIZE ; i < SIZE + MID ; i++ )
	{
		assert ( ! memcmp ( mcr_Array_at ( & arr, i ), & zeros, SIZEOF ) ) ;
	}

	reset ( ) ;
}

void test_Array_from_array ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		assert ( ! memcmp ( mcr_Array_at ( & arr, i ), structset + i,
				SIZEOF ) ) ;
	}
}

void test_Array_append ( )
{
	FAILIF ( ! mcr_Array_append ( & arr, structset, SIZE ) ) ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		assert ( ! memcmp ( mcr_Array_at ( & arr, i ), structset + i,
				SIZEOF ) ) ;
	}
	for ( i = 0 ; i < SIZE ; i++ )
	{
		assert ( ! memcmp ( mcr_Array_at ( & arr, SIZE + i ),
				structset + i, SIZEOF ) ) ;
	}
	reset ( ) ;
}

void test_Array_pop ( )
{
	pt = mcr_Array_pop ( & arr ) ;
	assert ( arr.used == SIZE - 1 ) ;
	assert ( ! memcmp ( pt, structset + SIZE - 1, SIZEOF ) ) ;
	reset ( ) ;
}

void test_Array_remove ( )
{
	// end
	mcr_Array_remove ( & arr, SIZE - 1 ) ;
	assert ( arr.used == SIZE - 1 ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, SIZE - 2 ), structset + SIZE - 2,
			SIZEOF ) ) ;
	// mid
	mcr_Array_remove ( & arr, MID ) ;
	assert ( arr.used == SIZE - 2 ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, MID ), structset + MID + 1,
			SIZEOF ) ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, MID - 1 ), structset + MID - 1,
			SIZEOF ) ) ;
	// beginning
	mcr_Array_remove ( & arr, 0 ) ;
	assert ( arr.used == SIZE - 3 ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 0 ), structset + 1, SIZEOF ) ) ;
	reset ( ) ;
}

void test_Array_remove_all ( )
{
	mcr_Array_remove_all ( & arr, structset+0 ) ;
	assert ( arr.used == SIZE - 1 ) ;
	assert ( ! memcmp ( mcr_Array_at ( & arr, 0 ), structset+1, SIZEOF ) ) ;
	reset ( ) ;
}

void test_Array_at ( )
{
	assert ( ! memcmp ( mcr_Array_at ( & arr, MID ), structset + MID,
			SIZEOF ) ) ;
	assert ( mcr_Array_at ( & arr, SIZE ) == NULL ) ;
}

void test_Array_next ( )
{
	pt = mcr_Array_at ( & arr, MID ) ;
	pt = mcr_Array_next ( & arr, pt ) ;
	assert ( ! memcmp ( pt, structset + MID + 1, SIZEOF ) ) ;
}

void test_Array_prev ( )
{
	pt = mcr_Array_at ( & arr, MID ) ;
	pt = mcr_Array_prev ( & arr, pt ) ;
	assert ( ! memcmp ( pt, structset + MID - 1, SIZEOF ) ) ;
}

void test_Array_end ( )
{
	pt = MCR_ARR_END ( arr ) ;
	assert ( pt == ( STR * ) ( ( ( char * )
			mcr_Array_at ( & arr, SIZE - 1 ) ) + SIZEOF ) ) ;
	mcr_Array_free ( & arr ) ;
	assert ( MCR_ARR_END ( arr ) == NULL ) ;
	reset ( ) ;
}

int reps ;

void inc_impl ( void * arrIt, va_list lst )
{
	UNUSED ( arrIt ) ;
	assert ( ( ( int ) va_arg ( lst, int ) ) == 42 ) ;
	++ reps ;
}

void inc ( void * arrIt, ... )
{
	va_list lst ;
	va_start ( lst, arrIt ) ;
	inc_impl ( arrIt, lst ) ;
	va_end ( lst ) ;
}

void test_Array_for_each ( )
{
	reps = 0 ;
	MCR_ARR_FOR_EACH ( arr, inc, 42 ) ;
	assert ( reps == SIZE ) ;
	reps = 0 ;
	mcr_Array_for_each ( & arr, inc_impl, 42 ) ;
}

void test_Array_trim ( )
{
	assert ( arr.size > MID ) ;
	arr.used = MID ;
	mcr_Array_trim ( & arr ) ;
	assert ( arr.used == MID ) ;
	assert ( arr.size == arr.used ) ;
	reset ( ) ;
}

void test_Array_resize ( )
{
	// resize from full to half
	FAILIF ( ! mcr_Array_resize ( & arr, MID ) ) ;
	assert ( arr.used == MID ) ;
	assert ( arr.size == MID ) ;
	// resize to empty
	FAILIF ( ! mcr_Array_resize ( & arr, 0 ) ) ;
	assert ( arr.used == 0 ) ;
	assert ( arr.size == 0 ) ;
	// resize empty to half
	FAILIF ( ! mcr_Array_resize ( & arr, MID ) ) ;
	assert ( arr.used == 0 ) ;
	assert ( arr.size == MID ) ;
	// resize half to full
	FAILIF ( ! mcr_Array_resize ( & arr, SIZE ) ) ;
	assert ( arr.used == 0 ) ;
	assert ( arr.size == SIZE ) ;
	reset ( ) ;
}

/*void test_Array_print ( )
{
	fprintf ( mcr_out, "Two is a manual test, please read below.\n" ) ;
	fprintf ( mcr_out, "Please verify array is printed correctly, from 0 to %d.\n",
			SIZE - 1 ) ;
	mcr_Array_print ( & arr ) ;
}*/

// 18 total mcr_Array functions, plus two macros without functions.
int main ( void )
{
	setup ( ) ;

	test_Array_init ( ) ;
	test_Array_free ( ) ;
	test_Array_push ( ) ;
	test_Array_push_unique ( ) ;
	test_Array_set ( ) ;
	test_Array_insert ( ) ;
	test_Array_insert_filled ( ) ;
	test_Array_from_array ( ) ;
	test_Array_append ( ) ;
	test_Array_pop ( ) ;
	test_Array_remove ( ) ;
	test_Array_remove_all ( ) ;
	test_Array_at ( ) ;
	test_Array_next ( ) ;
	test_Array_prev ( ) ;
	test_Array_end ( ) ;
	test_Array_for_each ( ) ;
	test_Array_trim ( ) ;
	test_Array_resize ( ) ;
//	test_Array_print ( ) ;

	fprintf ( mcr_out, "Test complete without assertion error.\n" ) ;

	return 0 ;
}
