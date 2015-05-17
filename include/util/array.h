/* include/util/array.h - Dynamic, resizing array.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file util/array.h
 * \brief Dynamic resizing array, with utility functions.
 * */

# ifndef MCR_ARRAY_H
# define MCR_ARRAY_H

# include "util/def.h"

/*! \brief Dynamic resizing array.
 *
 * Byte size of the whole array = elementSize * size.
 * */
typedef struct mcr_Array {
	//! \brief Resizing array pointer.
	unsigned char * array ;
	//! \brief Number of elements assigned.
	size_t used ;
	//! \brief Number of elements assignable ( allocated zone ).
	size_t size ;
	//! \brief Size of one element, in bytes.
	size_t element_size ;
} mcr_Array ;

/*! \brief ctor, Element size required.
 *
 * \param elementSize Size in bytes for every element.
 * */
MCR_API void mcr_Array_init ( mcr_Array * arrPt, size_t elementSize ) ;
/*! \brief Free memory and empty. Element size is preserved.
 * */
MCR_API void mcr_Array_free ( mcr_Array * arrPt ) ;
/*! \brief Place element at the end of the array.
 *
 * \param elementPt Pointer to element to copy from.
 * \return 0 if unsuccessful, otherwise successful.
 * */
MCR_API int mcr_Array_push ( mcr_Array * arrPt, const void * elementPt ) ;
/*! \brief Place element at the end of the array, but only
 * if it does not already exist previously.
 *
 * \param elementPt Element to copy from.
 * \return 0 if unsuccessful, otherwise successful.
 * */
MCR_API int mcr_Array_push_unique ( mcr_Array * arrPt,
		const void * elementPt ) ;
/*! \brief Set and replace element at position,
 * without changing other elements.
 *
 * \param pos Position of element to replace.
 * \param elementPt Pointer to object to copy from.
 * */
MCR_API int mcr_Array_set ( mcr_Array * arrPt, size_t pos,
		const void * elementPt ) ;
/*! \brief Place element at position. Byte position determined by
 * element size.
 *
 * This will displace elements after position. If position is
 * out of current range, the extra elements will be filled with
 * 0's.
 *
 * \param pos Index to place into.
 * \param elementPt Pointer to object to copy from.
 * \return 0 if unsuccessful, otherwise successful.
 * */
MCR_API int mcr_Array_insert ( mcr_Array * arrPt, size_t pos,
		const void * elementPt ) ;
/*! \brief Filled version of \ref mcr_Array_insert.
 *
 * \param pos Index to place into.
 * \param elementPt Pointer to object to copy from.
 * \param fillerElementPt Pointer to object to copy from for filled spaces.
 * \return 0 if unsuccessful, otherwise successful.
 * */
MCR_API int mcr_Array_insert_filled ( mcr_Array * arrPt, size_t pos,
		const void * elementPt, const void * fillerElementPt ) ;
/*! \brief Copy values from arraySource. Element size is assumed to
 * be in the array object already.
 *
 * \param arraySource Values copied from this array.
 * \param count Number of values to copy.
 * \return 0 if unsuccessful.
 * */
MCR_API int mcr_Array_from_array ( mcr_Array * arrPt,
		const void * arraySource, size_t count ) ;
/*! \brief Add a number of objects to the end of used array,
 * from arraySource.
 *
 * \param arraySource Values copied from this array.
 * \param count Number of values to copy.
 * \return 0 if unsuccessful.
 * */
MCR_API int mcr_Array_append ( mcr_Array * arrPt, const void * arraySource,
		size_t count ) ;
/*! \brief Decreased used amount by one.
 *
 * \return Address of removed space, or NULL if array is empty.
 * */
MCR_API void * mcr_Array_pop ( mcr_Array * arrPt ) ;
/*! \brief Remove given index, and adjust the rest of the array.
 *
 * \param pos Index to remove from.
 * */
MCR_API void mcr_Array_remove ( mcr_Array * arrPt, size_t pos ) ;
/*! \brief Remove all elements that match the given element.
 *
 * \param removeElementPt Element to remove.
 * */
MCR_API void mcr_Array_remove_all ( mcr_Array * arrPt,
		const void * removeElementPt ) ;
/*! \brief Get a reference to a position in the array.
 *
 * \param pos Index of element to return.
 * \return If pos is within used range, this will be the address
 * of that position. Else this is NULL.
 * */
MCR_API void * mcr_Array_at ( mcr_Array * arrPt, size_t pos ) ;
/*! \brief Get a position reference after the given position.
 *
 * \param posPt Base position to increase from.
 * \return Increase number of bytes equal to
 * element size.
 * */
MCR_API void * mcr_Array_next ( mcr_Array * arrPt, void * posPt ) ;
/*! \brief Get a position reference before the given position.
 *
 * \param posPt Base position to decrease from.
 * \return Decrease number of bytes equal to
 * element size.
 * */
MCR_API void * mcr_Array_prev ( mcr_Array * arrPt, void * posPt ) ;
/*! \brief Get a position reference exactly one byte after
 * last element in the array.
 *
 * \return This will be NULL if there are no elements.
 * */
MCR_API void * mcr_Array_end ( mcr_Array * arrPt ) ;
/*! \brief For every member of the array, call the iteration
 * function with that member, and the va_list from variadic args.
 * */
MCR_API void mcr_Array_for_each ( const mcr_Array * arrPt,
		mcr_iterate_fnc iterateFnc,... ) ;
/*! \brief Minimize array allocation.
 * */
MCR_API void mcr_Array_trim ( mcr_Array * arrPt ) ;
/*! \brief Reallocate array to given size. Actual byte size will
 * be this size * element size.
 *
 * \return 0 if unsuccessful, otherwise successful.
 * */
MCR_API int mcr_Array_resize ( mcr_Array * arrPt, size_t newSize ) ;
//! \brief Print values from array.
MCR_API void mcr_Array_print ( const mcr_Array * arrPt ) ;

//
// Macro utilities lack sanity checks.
//
/*! \brief \ref mcr_Array_at without sanity check.
 *
 * \param pos size_t
 * \return ( void * )
 * */
# define MCR_ARR_AT( arrPt, pos ) \
	( void * ) ( ( pos ) >= ( arrPt )->used ? NULL : ( arrPt )->array + \
			( ( pos ) * ( arrPt )->element_size ) )

/*! \brief \ref mcr_Array_next without sanity check.
 *
 * \return ( void * )
 * */
# define MCR_ARR_NEXT( arrPt, curPt ) \
	( void * ) ( ( unsigned char * ) ( curPt ) + ( arrPt )->element_size )

/*! \brief \ref mcr_Array_prev without sanity check.
 *
 * \return ( void * )
 * */
# define MCR_ARR_PREV( arrPt, curPt ) \
	( void * ) ( ( unsigned char * ) ( curPt ) - ( arrPt )->element_size )

/*! \brief Exactly one byte after last used element.
 * NULL if used is 0. No sanity check.
 *
 * \return ( void * ) just after last element.
 * */
# define MCR_ARR_END( arrPt ) \
	( void * ) ( ( arrPt )->used ? ( ( unsigned char * ) \
			( arrPt )->array + \
			( ( arrPt )->used * ( arrPt )->element_size ) ) : \
			NULL )

/*! \brief \ref mcr_Array_for_each without sanity check.
 *
 * This will not create va_list or do any type checking on variable
 * arguments.
 *
 * \param iterateFnc void ( * ) ( void *,... ) This function must be able
 * to accept both the array member reference, and all variadic
 * parameters given to this macro.
 * */
# define MCR_ARR_FOR_EACH( arrPt, iterateFnc,... ) \
{\
	void * _itPt_ = MCR_ARR_AT ( arrPt, 0 ) ; \
	void * _endPt_ = MCR_ARR_END ( arrPt ) ; \
	while ( _itPt_ < _endPt_ ) \
	{\
		iterateFnc ( _itPt_, __VA_ARGS__ ) ; \
		_itPt_ = MCR_ARR_NEXT ( arrPt, _itPt_ ) ; \
	}\
}

//
// Array string helper
//
MCR_API int mcr_String_insert ( mcr_Array * arrPt, size_t index,
		const char * str, size_t len ) ;
MCR_API int mcr_String_insert_char ( mcr_Array * arrPt, size_t index,
		const char c ) ;
MCR_API int mcr_String_push ( mcr_Array * arrPt, const char c ) ;
MCR_API char mcr_String_pop ( mcr_Array * arrPt ) ;
MCR_API int mcr_String_from_string ( mcr_Array * arrPt,
		const char * str, size_t len ) ;
MCR_API int mcr_String_append ( mcr_Array * arrPt,
		const char * str, size_t len ) ;

# define MCR_STR_ISEMPTY( arrPt ) \
	( ! ( arrPt )->used || ( arrPt )->array [ 0 ] == '\0' )

# endif
