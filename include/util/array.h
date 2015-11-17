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

/*! \file util/array.h
 * \brief Dynamic resizing array, with utility functions
 * */

# ifndef MCR_ARRAY_H
# define MCR_ARRAY_H

# include "util/def.h"

/*! \brief Comparison function for objects that are to be dereferenced
 *
 * \return Value < 0 if left is less than right, value 0 if both are equal,
 * value > 0 if left is greater than right
 * */
typedef int ( * mcr_compare_fnc ) ( const void *, const void * ) ;

/*! \brief Dynamic resizing array
 *
 * Byte size of the whole array = elementSize * size <br>
 * Important: char array is first element for string compare
 * */
typedef struct mcr_Array
{
	//! \brief Resizing array pointer
	char * array ;
	//! \brief Number of elements assignable ( allocated zone )
	size_t size ;
	//! \brief Number of elements assigned
	size_t used ;
	//
	// Internal
	//
	//! \brief Size of one element, in bytes
	size_t element_size ;
} mcr_Array ;

/*! \brief ctor: Element size is required
 *
 * \param elementSize Size in bytes for every element
 * */
MCR_API void mcr_Array_init ( mcr_Array * arrPt, size_t elementSize ) ;
/*! \brief Free dynamic memory and empty. Element size is preserved.
 * */
MCR_API void mcr_Array_free ( mcr_Array * arrPt ) ;
//! \brief Variadic \ref mcr_Array_free, unused extra arguments
# define mcr_Array_free_foreach( arrPt, ... ) mcr_Array_free ( arrPt )
/*! \brief Get a reference to a position in the array.
 *
 * \param pos Index of element to return
 * \return If pos is within used range, this will be the address
 * of that position. Otherwise this is NULL.
 * */
MCR_API void * mcr_Array_at ( mcr_Array * arrPt, size_t pos ) ;
/*! \brief Get a position reference after the given position.
 *
 * \param posPt Base position to increase from
 * \return Increase the number of bytes equal to
 * mcr_Array#element_size.
 * */
MCR_API void * mcr_Array_next ( mcr_Array * arrPt, void * posPt ) ;
/*! \brief Get a position reference before the given position.
 *
 * \param posPt Base position to decrease from
 * \return Decrease number of bytes equal to
 * mcr_Array#element_size.
 * */
MCR_API void * mcr_Array_prev ( mcr_Array * arrPt, void * posPt ) ;
/*! \brief Get a position reference exactly one byte after
 * last element in the array.
 *
 * \return This will be NULL if there are no elements.  Otherwise this
 * will be the byte right after the last element.
 * */
MCR_API void * mcr_Array_end ( mcr_Array * arrPt ) ;
/*! \brief Get the position for a pointer to an element in the array.
 *
 * \param elPt Pointer to an element in the array
 * \return Position of elPt if it is an element of the array, otherwise -1
 * */
MCR_API size_t mcr_Array_indexof ( mcr_Array * arrPt,
		const void * elPt ) ;
/*! \brief Sort array elements ordered with the comparison function.
 *
 * \param cmp Comparison function to order elements
 * */
MCR_API void mcr_Array_sort ( mcr_Array * arrPt, mcr_compare_fnc cmp ) ;
/*! \brief Get a reference to an element in the array.
 *
 * \param elementPt Comparison element to find something equivalent
 * in the array
 * \param cmp Comparison function to find an element equal to
 * elementPt
 * \return If an equivalent element is found this will be the address
 * of that element. Otherwise this is NULL.
 * */
MCR_API void * mcr_Array_find ( mcr_Array * arrPt, const void * elementPt,
		mcr_compare_fnc cmp ) ;
/*! \brief For every member of the array, call the iteration
 * function with that member, and the va_list from variadic args.
 * */
MCR_API void mcr_Array_for_each ( const mcr_Array * arrPt,
		mcr_iterate_fnc iterateFnc, ... ) ;
/*! \brief Place an element at the end of the array.
 *
 * \param elementPt Pointer to the element to copy from
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_push ( mcr_Array * arrPt, const void * elementPt ) ;
/*! \brief Place an element at the end of the array, but only
 * if it does not already exist previously.
 *
 * \param elementPt Pointer to the element to copy from
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_push_unique ( mcr_Array * arrPt,
		const void * elementPt ) ;
/*! \brief Place an element at the end of the array, but only
 * if it does not already exist previously.
 *
 * The array is assumed to be sorted with the given comparison function.
 *
 * \param elementPt Pointer to the element to copy from
 * \param cmp Comparison function to find elements, and sort after
 * insertion
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_add_sorted ( mcr_Array * arrPt,
		const void * elementPt, mcr_compare_fnc cmp ) ;
/*! \brief Set and replace element at position,
 * without changing other elements.
 *
 * \param pos Position of the element to replace
 * \param elementPt Pointer to the object to copy from
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_set ( mcr_Array * arrPt, size_t pos,
		const void * elementPt ) ;
/*! \brief Place an element at the given position. Byte position is
 * determined by the element size.
 *
 * This will displace elements after position. If position is
 * out of current range, the extra elements will be filled with
 * 0's.
 *
 * \param pos Index to place into
 * \param elementPt Pointer to the object to copy from
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_insert ( mcr_Array * arrPt, size_t pos,
		const void * elementPt ) ;
/*! \brief Filled version of \ref mcr_Array_insert
 *
 * \param pos Index to place into
 * \param elementPt Pointer to the object to copy from
 * \param fillerElementPt Pointer to the object to copy from for filled spaces
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_insert_filled ( mcr_Array * arrPt, size_t pos,
		const void * elementPt, const void * fillerElementPt ) ;
/*! \brief Copy values from arraySource.
 *
 * \param arraySource Values are copied from this array.
 * \param count Number of values to copy
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_from_array ( mcr_Array * arrPt,
		const void * arraySource, size_t count ) ;
/*! \brief Add a number of objects to the end of used array
 * from arraySource.
 *
 * \param arraySource Values are copied from this array.
 * \param count Number of values to copy
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_append ( mcr_Array * arrPt,
		const void * arraySource, size_t count ) ;
/*! \brief Decrease used amount by one.
 *
 * \return Address of removed space, or NULL if array is empty
 * */
MCR_API void * mcr_Array_pop ( mcr_Array * arrPt ) ;
/*! \brief Remove given index, and adjust the rest of the array.
 *
 * \param pos Index to remove from
 * */
MCR_API void mcr_Array_remove ( mcr_Array * arrPt, size_t pos ) ;
/*! \brief Remove all elements that match the given element.
 *
 * \param removeElementPt Element to remove
 * */
MCR_API void mcr_Array_remove_all ( mcr_Array * arrPt,
		const void * removeElementPt ) ;
/*! \brief Remove all elements that match the given element.
 *
 * \param removeElementPt Element to remove
 * \param cmp Comparison function to find elements, and sort after
 * removal
 * */
MCR_API void mcr_Array_remove_sorted ( mcr_Array * arrPt,
		const void * removeElementPt, mcr_compare_fnc cmp ) ;
/*! \brief Minimize array allocation.
 * */
MCR_API void mcr_Array_trim ( mcr_Array * arrPt ) ;
/*! \brief Reallocate array to given size. Actual byte size will
 * be this size * element size.
 *
 * \return 0 if unsuccessful, otherwise successful
 * */
MCR_API int mcr_Array_resize ( mcr_Array * arrPt, size_t newSize ) ;
//! \brief Print values from array to mcr_stdout.
MCR_API void mcr_Array_print ( const mcr_Array * arrPt ) ;

//
// Macro utils
//
/*! \brief \ref mcr_Array_at
 *
 * \param arr const \ref mcr_Array
 * \param pos size_t
 * \return void *
 * */
# define MCR_ARR_AT( arr, pos ) \
( ( void * ) ( ( pos ) >= ( arr ).used ? \
	NULL : \
( arr ).array + ( ( pos ) * ( arr ).element_size ) ) )
/*! \brief \ref mcr_Array_next
 *
 * \param arr const \ref mcr_Array
 * \param curPt const void *
 * \return void *
 * */
# define MCR_ARR_NEXT( arr, curPt ) \
( ( void * ) ( ( ( const char * ) ( curPt ) ) + ( arr ).element_size ) )
/*! \brief \ref mcr_Array_prev
 *
 * \param arr const \ref mcr_Array
 * \param curPt const void *
 * \return void *
 * */
# define MCR_ARR_PREV( arr, curPt ) \
( ( void * ) ( ( ( const char * ) ( curPt ) ) - ( arr ).element_size ) )
/*! \brief \ref mcr_Array_end
 *
 * \param arr const \ref mcr_Array
 * \return void *
 * */
# define MCR_ARR_END( arr ) \
( ( void * ) ( ( arr ).used ? \
	( ( char * ) ( arr ).array + \
			( ( arr ).used * ( arr ).element_size ) ) : \
NULL ) )
/*! \brief \ref mcr_Array_indexof
 *
 * \param arr const \ref mcr_Array
 * \param valPt const void *
 * \return size_t
 * */
# define MCR_ARR_INDEXOF( arr, valPt ) \
( ( size_t ) ( ( arr ).used && ( const char * ) \
		( valPt ) >= ( arr ).array && \
		( void * ) ( valPt ) < MCR_ARR_END ( arr ) ? \
	( ( ( const char * ) ( valPt ) ) - \
			( arr ).array ) / ( arr ).element_size : \
( size_t ) -1 ) )
/*! \brief \ref mcr_Array_sort
 *
 * \param arr \ref mcr_Array
 * \param cmp \ref mcr_compare_fnc
 * */
# define MCR_ARR_SORT( arr, cmp ) \
if ( ( arr ).used ) \
	qsort ( ( arr ).array, ( arr ).used, ( arr ).element_size, cmp ) ;
/*! \brief \ref mcr_Array_find
 *
 * \param arr const \ref mcr_Array
 * \param elPt const void *
 * \param cmp \ref mcr_compare_fnc
 * \return void *
 * */
# define MCR_ARR_FIND( arr, elPt, cmp ) \
( ( void * ) ( ( arr ).used ? \
	bsearch ( elPt, ( arr ).array, ( arr ).used, \
			( arr ).element_size, cmp ) : \
NULL ) )
/*! \brief \ref mcr_Array_for_each
 *
 * This will not create va_list or do any type checking on variable
 * arguments. <br>
 * It was found that variadic functions have slower performance than
 * regular functions.  Tests done by Jonathan Pelletier determined
 * non-variadic functions call in roughly 3/4 the time to call a
 * variadic function.  Please now use MCR_EXP with variadic macros.
 *
 * \param arr \ref mcr_Array
 * \param iterateFnc void ( * ) ( void *, ... ) This function must be able
 * to accept both the array member reference, and all variadic
 * parameters given to this macro. <br>
 * If this is a variadic macro then an expansion helper \ref MCR_EXP
 * may be needed. For example, \ref mcr_Array_free_foreach needs to be
 * expanded, but ignores extra arguments:
 * MCR_ARR_FOR_EACH( arr, MCR_EXP(mcr_Array_free_foreach),)
 * */
# define MCR_ARR_FOR_EACH( arr, iterateFnc, ... ) \
{ \
	void * _itPt_ = MCR_ARR_AT ( arr, 0 ) ; \
	void * _endPt_ = MCR_ARR_END ( arr ) ; \
	while ( _itPt_ < _endPt_ ) \
	{ \
		iterateFnc ( _itPt_, __VA_ARGS__ ) ; \
		_itPt_ = MCR_ARR_NEXT ( arr, _itPt_ ) ; \
	} \
}

# endif
