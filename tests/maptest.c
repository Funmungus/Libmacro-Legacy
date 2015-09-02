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
# define SIZE1 ( MID / 2 )
# define SIZE2 ( MID + SIZE1 )
# define SIZEBOTH ( SIZE1 + SIZE2 )
# define SIZEOF(obj) ( sizeof ( obj.bytes ) )

typedef struct {
	char bytes [ SIZE1 ] ;
} s1 ;

typedef struct {
	char bytes [ SIZE2 ] ;
} s2 ;

typedef struct {
	char bytes [ SIZE1 + SIZE2 ] ;
} sboth ;

s1 set1 [ SIZE ] ;
s2 set2 [ SIZE ] ;
sboth setboth [ SIZE ] ;
s1 val1, lav1 ;
s2 val2, lav2 ;
int i ;
mcr_Map map ;

int set1compare ( const void * lhs, const void * rhs )
{
	assert ( lhs != NULL ) ;
	assert ( rhs != NULL ) ;
	return memcmp ( lhs, rhs, SIZE1 ) ;
}

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
	mcr_Map_free ( & map ) ;
}

void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( on_complete ) ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		memset ( set1 + i, i, SIZEOF ( set1 [ 0 ] ) ) ;
		memset ( set2 + i, i, SIZEOF ( set2 [ 0 ] ) ) ;
		memset ( setboth + i, i, SIZEOF ( setboth [ 0 ] ) ) ;
	}
	memset ( & val1, 42, SIZEOF ( val1 ) ) ;
	memset ( & lav1, 24, SIZEOF ( lav1 ) ) ;
	memset ( & val2, 42, SIZEOF ( val2 ) ) ;
	memset ( & lav2, 24, SIZEOF ( lav2 ) ) ;
}

void reset ( )
{
	for ( i = 0 ; i < SIZE ; i ++ )
	{
		FAILIF ( ! mcr_Map_map ( & map, set1 + i, set2 + i ) ) ;
		assert ( map.set.used > ( size_t ) i ) ;
		assert ( map.set.size > ( size_t ) i ) ;
	}
	assert ( map.compare == set1compare ) ;
	assert ( map.sizeof_first == SIZE1 ) ;
	assert ( map.sizeof_second == SIZE2 ) ;
	assert ( map.set.array != NULL ) ;
	assert ( map.set.element_size == SIZEBOTH ) ;
	assert ( map.set.size != 0 ) ;
	assert ( map.set.used == SIZE ) ;
}

void test_map_init ( )
{
	mcr_Map_init ( & map, SIZE1, SIZE2 ) ;
	map.compare = set1compare ;
	assert ( map.sizeof_first == SIZE1 ) ;
	assert ( map.sizeof_second == SIZE2 ) ;
	assert ( map.set.element_size == SIZEBOTH ) ;
	reset ( ) ;
	fprintf ( mcr_out, "init OK\n" ) ;
}

void test_map_free ( )
{
	mcr_Map_free ( & map ) ;
	assert ( map.compare == set1compare ) ;
	assert ( map.sizeof_first == SIZE1 ) ;
	assert ( map.sizeof_second == SIZE2 ) ;
	assert ( map.set.array == NULL ) ;
	assert ( map.set.element_size == SIZEBOTH ) ;
	assert ( map.set.size == 0 ) ;
	assert ( map.set.used == 0 ) ;
	reset ( ) ;
	fprintf ( mcr_out, "free OK\n" ) ;
}

# define CHKMAP \
	for ( i = 0 ; i < SIZE ; i++ ) \
	{ \
		void * it = mcr_Array_at ( & map.set, i ) ; \
		assert ( ! memcmp ( it, setboth + i, SIZEBOTH ) ) ; \
	}

void test_map_map ( )
{
	// If mapping was correct in reset, then each s1+s2 should be
	// the same sboth.
	CHKMAP ;
	// We also want to check sorting, so try mapping backwards.
	mcr_Map_free ( & map ) ;
	for ( i = SIZE - 1 ; i >= 0 ; i-- )
	{
		FAILIF ( ! mcr_Map_map ( & map, set1 + i, set2 + i ) ) ;
	}
	CHKMAP ;
	// Check entry without sorting
	mcr_Map_free ( & map ) ;
	for ( i = SIZE - 1 ; i >= 0 ; i-- )
	{
		FAILIF ( ! mcr_Map_map ( & map, set1 + i, set2 + i ) ) ;
		assert ( ! memcmp ( mcr_Map_get ( & map, set1 + i ), setboth + i,
				SIZEBOTH ) ) ;
	}
	map.compare = set1compare ;
	mcr_Map_sort ( & map ) ;
	reset ( ) ;
	CHKMAP ;
	fprintf ( mcr_out, "map OK\n" ) ;
}

void test_map_remap ( )
{
	for ( i = 0 ; i < SIZE - 1 ; i ++ )
	{
		FAILIF ( ! mcr_Map_remap ( & map, set1 + i, set1 + i + 1 ) ) ;
		// At i is removed, i + 1 has { set1 + i + 1, set2 + i }.
		assert ( ! mcr_Map_get ( & map, set1 + i ) ) ;
		void * getPt = mcr_Map_get ( & map, set1 + i + 1 ) ;
		assert ( getPt ) ;
		// Lowest number was removed, i + 1 is new lowest.
		void * it = mcr_Array_at ( & map.set, 0 ) ;
		assert ( getPt == it ) ;
		assert ( ! memcmp ( it, set1 + i + 1, SIZE1 ) ) ;
		it = MCR_MAP_VALUE ( map, it ) ;
		assert ( ! memcmp ( it, set2 + i, SIZE2 ) ) ;
		// Revert value to old value.
		memcpy ( it, set2 + i + 1, SIZE2 ) ;
	}

	reset ( ) ;
	CHKMAP ;

	for ( i = SIZE - 1 ; i > 0 ; i -- )
	{
		FAILIF ( ! mcr_Map_remap ( & map, set1 + i, set1 + i - 1 ) ) ;
		// At i is removed, i - 1 has { set1 + i - 1, set2 + i }.
		assert ( ! mcr_Map_get ( & map, set1 + i ) ) ;
		void * getPt = mcr_Map_get ( & map, set1 + i - 1 ) ;
		assert ( getPt ) ;
		// Highest number was removed, i - 1 is new highest.
		void * it = mcr_Array_at ( & map.set, map.set.used - 1 ) ;
		assert ( getPt == it ) ;
		assert ( ! memcmp ( it, set1 + i - 1, SIZE1 ) ) ;
		it = ( ( unsigned char * ) it ) + SIZE1 ;
		assert ( ! memcmp ( it, set2 + i, SIZE2 ) ) ;
		// Revert value to old value.
		memcpy ( it, set2 + i - 1, SIZE2 ) ;
	}

	reset ( ) ;
	CHKMAP ;

	fprintf ( mcr_out, "remap OK\n" ) ;
}

void test_map_map_pair ( )
{
	mcr_Map_free ( & map ) ;
	// Same as reset, but using sboth.
	for ( i = 0 ; i < SIZE ; i++ )
	{
		FAILIF ( ! mcr_Map_map_pair ( & map, setboth + i ) ) ;
	}
	CHKMAP ;
	// Now with sorting from backwards.
	for ( i = SIZE - 1 ; i >= 0 ; i-- )
	{
		FAILIF ( ! mcr_Map_map_pair ( & map, setboth + i ) ) ;
	}
	CHKMAP ;
	// Check remapping.
	for ( i = SIZE - 1 ; i >= 0 ; i-- )
	{
		FAILIF ( ! mcr_Map_map_pair ( & map, setboth + i ) ) ;
	}
	CHKMAP ;
	// entry without sorting
	mcr_Map_free ( & map ) ;
	reset ( ) ;
	CHKMAP ;
	fprintf ( mcr_out, "map_pair OK\n" ) ;
}

void test_map_get ( )
{
	CHKMAP ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		assert ( ! memcmp ( mcr_Map_get ( & map, set1 + i ), setboth + i,
				SIZEBOTH ) ) ;
	}
	// get from empty map always null
	mcr_Map_free ( & map ) ;
	assert ( mcr_Map_get ( & map, set1 + 0 ) == NULL ) ;
	assert ( mcr_Map_get ( & map, set1 + 1 ) == NULL ) ;
	mcr_Map_map_pair ( & map, setboth + 0 ) ;
	assert ( ! memcmp ( mcr_Map_get ( & map, set1 + 0 ), setboth + 0,
			SIZEBOTH ) ) ;
	assert ( mcr_Map_get ( & map, set1 + 1 ) == NULL ) ;
	reset ( ) ;
	CHKMAP ;

	fprintf ( mcr_out, "get OK\n" ) ;
}

void test_map_get_slow ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		assert ( ! memcmp ( mcr_Map_get_slow ( & map, set1 + i ), setboth + i,
				SIZEBOTH ) ) ;
	}
	CHKMAP ;
	fprintf ( mcr_out, "get_slow OK\n" ) ;
}

void test_map_get_ensured ( )
{
	mcr_Map_free ( & map ) ;
	assert ( map.set.used == 0 ) ;
	assert ( map.set.size == 0 ) ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		void * it = mcr_Map_get_ensured ( & map, set1 + i, set2 + i ) ;
		assert ( it != NULL ) ;
		assert ( map.set.used == ( size_t ) i + 1 ) ;
		assert ( map.set.size > ( size_t ) i ) ;
		assert ( ! memcmp ( it, setboth + i, SIZEBOTH ) ) ;
		// check remapping also
		it = mcr_Map_get_ensured ( & map, set1 + i, set2 + 0 ) ;
		assert ( it != NULL ) ;
		assert ( map.set.used == ( size_t ) i + 1 ) ;
		assert ( map.set.size > ( size_t ) i ) ;
		assert ( ! memcmp ( it, setboth + i, SIZEBOTH ) ) ;
	}
	// check remapping null
	mcr_Map_free ( & map ) ;
	sboth nullcheck ;
	memset ( & nullcheck, 0, sizeof ( nullcheck ) ) ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		// nullcheck's s1 == set1 + i, nullcheck's s2 is all 0's
		memset ( & nullcheck, i, SIZE1 ) ;
		void * it = mcr_Map_get_ensured ( & map, set1 + i, NULL ) ;
		assert ( it != NULL ) ;
		assert ( map.set.used == ( size_t ) i + 1 ) ;
		assert ( map.set.size > ( size_t ) i ) ;
		assert ( ! memcmp ( it, & nullcheck, SIZEBOTH ) ) ;
		// check remapping also
		it = mcr_Map_get_ensured ( & map, set1 + i, NULL ) ;
		assert ( it != NULL ) ;
		assert ( map.set.used == ( size_t ) i + 1 ) ;
		assert ( map.set.size > ( size_t ) i ) ;
		assert ( ! memcmp ( it, & nullcheck, SIZEBOTH ) ) ;
	}
	reset ( ) ;
	CHKMAP ;
	fprintf ( mcr_out, "get_ensured OK\n" ) ;
}

void test_map_index_of ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		assert ( mcr_Map_index_of ( & map, set1 + i ) == ( size_t ) i ) ;
	}
	mcr_Map_free ( & map ) ;
	assert ( mcr_Map_index_of ( & map, set1 + 0 ) == ( size_t ) -1 ) ;
	assert ( mcr_Map_index_of ( & map, set1 + 1 ) == ( size_t ) -1 ) ;
	mcr_Map_map_pair ( & map, setboth + 0 ) ;
	assert ( mcr_Map_index_of ( & map, set1 + 0 ) == 0 ) ;
	assert ( mcr_Map_index_of ( & map, set1 + 1 ) == ( size_t ) -1 ) ;
	reset ( ) ;
	CHKMAP ;
	fprintf ( mcr_out, "index_of OK\n" ) ;
}

void test_map_unmap ( )
{
	for ( i = 0 ; i < SIZE ; i++ )
	{
		mcr_Map_unmap ( & map, set1 + i ) ;
		assert ( mcr_Map_get ( & map, set1 + i ) == NULL ) ;
		if ( i < SIZE - 1 )
		{
			assert ( mcr_Map_index_of ( & map, set1 + i + 1 ) == 0 ) ;
		}
		assert ( map.set.used == SIZE - 1 - ( size_t ) i ) ;
	}
	assert ( map.set.used == 0 ) ;
	reset ( ) ;
	// unmap backwards
	for ( i = SIZE - 1 ; i >= 0 ; i-- )
	{
		mcr_Map_unmap ( & map, set1 + i ) ;
		assert ( mcr_Map_get ( & map, set1 + i ) == NULL ) ;
		if ( i > 0 )
		{
			assert ( mcr_Map_index_of ( & map, set1 + i - 1 ) ==
					map.set.used - 1 ) ;
		}
		assert ( map.set.used == ( size_t ) i ) ;
	}
	assert ( map.set.used == 0 ) ;
	// unmap empty
	mcr_Map_unmap ( & map, set1 + 0 ) ;
	// unmap empty and not existing
	mcr_Map_map_pair ( & map, setboth + 0 ) ;
	mcr_Map_unmap ( & map, set1 + 1 ) ;
	reset ( ) ;
	CHKMAP ;
	fprintf ( mcr_out, "unmap OK\n" ) ;
}

void test_map_sort ( )
{
	// sort already sorted
	mcr_Map_sort ( & map ) ;
	CHKMAP ;
	// sort empty, just make sure no runtime error happens
	mcr_Map_free ( & map ) ;
	mcr_Map_sort ( & map ) ;
	// check sort without comparator
	for ( i = SIZE - 1 ; i >= 0 ; i-- )
	{
		mcr_Map_map_pair ( & map, setboth + i ) ;
	}
	mcr_Map_sort ( & map ) ;
	CHKMAP ;
	fprintf ( mcr_out, "sort OK\n" ) ;
}

void test_map_clear ( )
{
	mcr_Map_clear ( & map ) ;
	assert ( map.compare == set1compare ) ;
	assert ( map.sizeof_first == SIZE1 ) ;
	assert ( map.sizeof_second == SIZE2 ) ;
	assert ( map.set.array != NULL ) ;
	assert ( map.set.element_size == SIZEBOTH ) ;
	assert ( map.set.size != 0 ) ;
	assert ( map.set.used == 0 ) ;
	reset ( ) ;
	fprintf ( mcr_out, "clear OK\n" ) ;
}

void test_map_trim ( )
{
	mcr_Map_trim ( & map ) ;
	assert ( map.set.used == map.set.size ) ;
	assert ( map.set.size == SIZE ) ;
	map.set.used = MID ;
	mcr_Map_trim ( & map ) ;
	assert ( map.set.used == map.set.size ) ;
	assert ( map.set.size == MID ) ;
	map.set.used = 0 ;
	mcr_Map_trim ( & map ) ;
	assert ( map.set.used == map.set.size ) ;
	assert ( map.set.size == 0 ) ;
	reset ( ) ;
	fprintf ( mcr_out, "trim OK\n" ) ;
}

/*void test_map_print ( )
{
	fprintf ( mcr_out, "Print test is manual. Please read below, and make sure "
				"this prints from 0 to 0x%x.\n", SIZE - 1 ) ;
	mcr_Map_print ( & map ) ;
	fprintf ( mcr_out, "print OK\n" ) ;
}*/

void test_map_value ( )
{
	assert ( MCR_MAP_VALUE ( map, NULL ) == NULL ) ;
	for ( i = 0 ; i < SIZE ; i++ )
	{
		assert ( MCR_MAP_VALUE ( map,
				mcr_Map_get ( & map, set1 + i ) ) ==
						( void * ) ( ( char * )
						mcr_Map_get ( & map, set1 + i ) + SIZE1 ) ) ;
	}
	fprintf ( mcr_out, "value OK\n" ) ;
}

void chk_redirect_impl ( void * itPt, va_list lst )
{
	typedef int * intPt ;
	assert ( itPt == mcr_Map_get ( & map, set1 + i ) ) ;
	assert ( * va_arg ( lst, intPt ) == i ) ;
	++ i ;
}

void chk_redirect ( void * itPt, ... )
{
	va_list lst ;
	va_start ( lst, itPt ) ;
	chk_redirect_impl ( itPt, lst ) ;
	va_end ( lst ) ;
}

void test_map_for_each ( )
{
	i = 0 ;
	MCR_MAP_FOR_EACH ( map, chk_redirect, & i ) ;
	i = 0 ;
	mcr_Map_for_each ( & map, chk_redirect_impl, & i ) ;

	fprintf ( mcr_out, "for_each OK\n" ) ;
}

void chk_value_redirect_impl ( void * itPt, va_list lst )
{
	typedef int * intPt ;
	assert ( itPt == MCR_MAP_VALUE ( map, mcr_Map_get (
			& map, set1 + i ) ) ) ;
	assert ( * va_arg ( lst, intPt ) == i ) ;
	++ i ;
}

void chk_value_redirect ( void * itPt, ... )
{
	va_list lst ;
	va_start ( lst, itPt ) ;
	chk_value_redirect_impl ( itPt, lst ) ;
	va_end ( lst ) ;
}

void test_map_for_each_value ( )
{
	i = 0 ;
	MCR_MAP_FOR_EACH_VALUE ( map, chk_value_redirect, & i ) ;
	fprintf ( mcr_out, "for_each_value OK\n" ) ;
}

// 12 total mcr_Map functions, plus three macros without functions.
int main ( void )
{
	setup ( ) ;

	test_map_init ( ) ;
	test_map_free ( ) ;
	test_map_map ( ) ;
	test_map_remap ( ) ;
	test_map_map_pair ( ) ;
	test_map_get ( ) ;
	test_map_get_slow ( ) ;
	test_map_get_ensured ( ) ;
	test_map_index_of ( ) ;
	test_map_unmap ( ) ;
	test_map_sort ( ) ;
	test_map_clear ( ) ;
	test_map_trim ( ) ;
//	test_map_print ( ) ;

	test_map_value ( ) ;
	test_map_for_each ( ) ;
	test_map_for_each_value ( ) ;

	fprintf ( mcr_out, "Test complete without assertion error.\n" ) ;

	return 0 ;
}
