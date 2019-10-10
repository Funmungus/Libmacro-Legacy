/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

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

/*! \file
 *  \brief \ref mcr_Array Dynamic resizing array, with utility functions
 */

#ifndef MCR_UTIL_ARRAY_H_
#define MCR_UTIL_ARRAY_H_

#include "mcr/util/interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Dynamic resizing array
 *
 *  Byte size of the whole array = elementSize * size\n
 *  Important: byte array is first element for string comparison
 */
struct mcr_Array {
	/*! Resizing array pointer */
	void *array;
	/*! Number of elements assignable (allocated area) */
	size_t size;
	/*! Number of elements assigned */
	size_t used;
	/* Internal */
	/*! Optional comparison to sort and find elements.
	 *
	 *  If null, all sorting and finding will use memcmp */
	mcr_compare_fnc compare;
	/*! Size of one element, in bytes */
	size_t element_size;
};
/*! Interface for a sorted, generic array of references */
MCR_API const struct mcr_Interface *mcr_Array_ref_interface();

/*! \ref mcr_Array ctor
 *
 *  Element size will be 1, with no array allocated
 *  \param arrPt \ref opt \ref mcr_Array *
 *  \return 0
 */
MCR_API int mcr_Array_init(void *arrPt);
/*! \ref mcr_Array_init and \ref mcr_Array_set_all
 *
 *  \param compare \ref opt \ref mcr_Array.compare
 *  \param elementSize \ref mcr_Array.element_size
 *  \return Empty \ref mcr_Array
 */
MCR_API struct mcr_Array mcr_Array_new(mcr_compare_fnc compare,
									   size_t elementSize);
/*! \ref mcr_Array_init for ordered array of pointers
 *
 *  Initializer for instances of \ref mcr_Array_ref_interface
 *  \param arrPt \ref opt \ref mcr_Array *
 *  \return 0
 */
MCR_API int mcr_Array_init_ref(void *arrPt);
/*! \post The array object will be ready to use again, with no change to the
 *  element size and comparison function.
 *  \brief \ref mcr_Array dtor
 *
 *  Element size is preserved
 *  \param arrPt \ref opt \ref mcr_Array *
 *  \return 0
 */
MCR_API int mcr_Array_deinit(void *arrPt);
/*! \post Any allocated array will be deallocated.
 *  \brief Set initial values
 *
 *  Will deinitialize anything that is allocated
 *  \param compare \ref opt \ref mcr_Array.compare
 *  \param elementSize \ref mcr_Array.element_size
 */
MCR_API void mcr_Array_set_all(struct mcr_Array *arrPt,
							   mcr_compare_fnc compare, size_t elementSize);

/* Allocation control */
/*! Set a minimum number of used elements and
 *  \ref mcr_Array_resize if needed.
 *
 *  \param minUsed Minimum number of used elements
 *  \return \ref reterr
 */
MCR_API int mcr_Array_minused(struct mcr_Array *arrPt, size_t minUsed);
/*! Set a minimum size, and \ref mcr_Array_resize if needed.
 *
 *  \param minSize Minimum number of allocated elements
 *  \return \ref reterr
 */
MCR_API int mcr_Array_minsize(struct mcr_Array *arrPt, size_t minSize);
/*! Apply a smart resizing algorithm for adding a number of
 *  elements.
 *
 *  \param increasingCount Number of elements to add
 *  \return \ref reterr
 */
MCR_API int mcr_Array_smartsize(struct mcr_Array *arrPt,
								size_t increasingCount);
/*! Set a minimum number of used elements and fill new elements.
 *
 *  \param minUsed Minimum number of used elements
 *  \param fillerElementPt \ref opt Reference to copy into new elements at the
 *  end of the array
 *  \return \ref reterr
 */
MCR_API int mcr_Array_minfill(struct mcr_Array *arrPt, size_t minUsed,
							  const void *fillerElementPt);
/*! Minimize array allocation. */
MCR_API void mcr_Array_trim(struct mcr_Array *arrPt);
/*! Reallocate array to given size. Actual byte size will
 *  be this size * element size.
 *
 *  \param newSize New allocated number of elements
 *  \return \ref reterr
 */
MCR_API int mcr_Array_resize(struct mcr_Array *arrPt, size_t newSize);
/*! Set \ref mcr_Array.used to 0. */
MCR_API void mcr_Array_clear(struct mcr_Array *arrPt);

/* Position and elements */
/*! Get a reference to a position in the array.
 *
 *  \param arrPt \ref opt
 *  \param pos Index of element to return
 *  \return If pos is within used range, this will be the address
 *  of that position. Otherwise this is null.
 */
MCR_API void *mcr_Array_element(const struct mcr_Array *arrPt, size_t pos);
/*! Get a reference to the first element.
 *
 *  \param arrPt \ref opt
 *  \return Reference to first element, or null if empty
 */
MCR_API void *mcr_Array_first(const struct mcr_Array *arrPt);
/*! Get a reference to the last element.
 *
 *  \param arrPt \ref opt
 *  \return Reference to last element, or null if empty
 */
MCR_API void *mcr_Array_last(const struct mcr_Array *arrPt);
/*! Get a position reference exactly one byte after
 *  last element in the array.
 *
 *  \param arrPt \ref opt
 *  \return One byte right after the last element. null if array is null or
 *  empty
 */
MCR_API void *mcr_Array_end(const struct mcr_Array *arrPt);
/*! Get a position reference after the given position.
 *
 *  \param arrPt \ref opt
 *  \param posPt \ref opt Base position to increase from
 *  \return Increase the number of bytes equal to
 *  \ref mcr_Array.element_size. null if array or position are null
 */
MCR_API void *mcr_Array_next(const struct mcr_Array *arrPt, void *posPt);
/*! Get a position reference before the given position.
 *
 *  \param arrPt \ref opt
 *  \param posPt \ref opt Base position to decrease from
 *  \return Decrease number of bytes equal to
 *  \ref mcr_Array.element_size. null if array or position are null
 */
MCR_API void *mcr_Array_prev(const struct mcr_Array *arrPt, void *posPt);
/*! Get the position for a pointer to an element in the array.
 *
 *  \param arrPt \ref opt
 *  \param elPt \ref opt Pointer to an element in the array
 *  \return \ref retind Index of reference to element. -1 if array is null or
 *  element not found
 */
MCR_API size_t mcr_Array_index(const struct mcr_Array *arrPt,
							   const void *elPt);
/*! Get the index of the last element.
 *
 *  \param arrPt \ref opt
 *  \return \ref retind Index of last element. -1 if array is null or
 *  element not found
 */
MCR_API size_t mcr_Array_last_index(const struct mcr_Array *arrPt);
/*! Create an iterator.
 *
 *  If iterator is null, end will be null, and vise versa.  Null checks
 *  are suggested for equality(e.g. !=) comparisons.\n
 *  usage: \code{.c}
 *  void *elementPt;
 *  size_t itPt, endPt;
 *  size_t bytes;
 *  for (mcr_Array_iter(arrPt, &itPt, &endPt, &bytes); itPt < endPt;
 *  		itPt += bytes) {
 *  	// Cast iterator to use object reference
 *  	elementPt = (void *)itPt;
 *  }
 *  \endcode
 *  \param arrPt \ref opt
 *  \param iterPt \ref opt Set to first element
 *  \param endPt \ref opt Set to one after last element
 *  \param bytesPt \ref opt \ref mcr_Array.element_size
 */
MCR_API void mcr_Array_iter(const struct mcr_Array *arrPt, char **iterPt,
                            char **endPt, size_t *bytesPt);
/*! Create a range iterator.
 *
 *  Depending on the indices used, the iterator or last element might be null.
 *  A null check is suggested, and a for-loop is not suggested.\n
 *  usage(example skips first and last element): \code{.c}
 *  void *elementPt;
 *  size_t itPt, lastPt;
 *  size_t bytes, firstIndex = 1, lastIndex = arrPt->used - 2;
 *  mcr_Array_iter_range(arrPt, &itPt, &lastPt, &bytes, firstIndex, lastIndex);
 *  if (itPt && lastPt) {
 *  	while (itPt <= lastPt) {
 *  		// Cast iterator to use object reference
 *  		elementPt = (void *)itPt;
 *  		itPt += bytes;
 *  	}
 *  }
 *  \endcode
 *  \param arrPt \ref opt
 *  \param iterPt \ref opt Set to first element in range
 *  \param lastPt \ref opt Set to last element in range
 *  \param bytesPt \ref opt \ref mcr_Array.element_size
 *  \param firstIndex Index of first element in range
 *  \param lastIndex Index of last element in range
 */
MCR_API void mcr_Array_iter_range(const struct mcr_Array *arrPt,
                                  char **iterPt,
                                  char **lastPt, size_t *bytesPt, size_t firstIndex,
								  size_t lastIndex);

/* Add/remove */
/*! Place elements at the given position.
 *
 *  This will displace elements after position.
 *  \param pos Index to place into, within the array's used
 *  elements or one after
 *  \param elementArr \ref opt Array of elements to copy from
 *  \param count Number of elements to copy
 *  \return \ref reterr
 */
MCR_API int mcr_Array_insert(struct mcr_Array *arrPt, size_t pos,
							 const void *elementArr, size_t count);
/*! Remove elements from given index, and adjust the rest of
 *  the array.
 *
 *  \param pos Index to remove from, within range of the array's used
 *  elements
 *  \param count Number of elements to remove
 */
MCR_API void mcr_Array_remove_index(struct mcr_Array *arrPt, size_t pos,
									size_t count);
/*! Place elements at the end of the array.
 *
 *  \param elementArr \ref opt Array of elements to copy from
 *  \param count Number of elements to copy
 *  \param flagUnique If true elements will only be added if they
 *  do not already exist.
 *  \return \ref reterr
 */
MCR_API int mcr_Array_append(struct mcr_Array *arrPt, const void *elementArr,
							 size_t count);
/*! \ref mcr_Array_append with one element
 *
 *  \param elementPt \ref opt Reference to copy from
 *  \param flagUnique If true elements will only be added if they
 *  do not already exist.
 *  \return \ref reterr
 */
MCR_API int mcr_Array_push(struct mcr_Array *arrPt, const void *elementPt);
/*! Decrease used amount by one. */
MCR_API void mcr_Array_pop(struct mcr_Array *arrPt);

/* Replace current elements */
/*! Replace entire array with given array source.
 *
 *  \param arraySource \ref opt Values are copied from this array.
 *  \param count Number of values to copy
 *  \return \ref reterr
 */
MCR_API int mcr_Array_replace(struct mcr_Array *arrPt,
							  const void *arraySource, size_t count);
/*! Replace a subset of elements at a position.
 *
 *  Size will be increased if needed.
 *  \param dstPos Destination index to start replacing from
 *  \param srcArray \ref opt Array to copy from, if null memset will be used
 *  \param count Number of elements to replace
 *  \return \ref reterr
 */
MCR_API int mcr_Array_copy(struct mcr_Array *dstPt, size_t dstPos,
						   const void *srcArray, size_t count);
/*! Replace a single element at a position.
 *
 *  \param pos Position of the element to replace
 *  \param elementPt \ref opt Pointer to the object to copy from
 *  \return \ref reterr
 */
MCR_API int mcr_Array_set(struct mcr_Array *arrPt, size_t pos,
						  const void *elementPt);
/*! Replace a subset of elements by copying a single element.
 *
 *  \param dstPos Destination index to start replacing from
 *  \param copyElementPt \ref opt Copy this into subset.  If null, fill
 *  elements with 0's.
 *  \param count Number of elements to replace
 *  \return \ref reterr
 */
MCR_API int mcr_Array_fill(struct mcr_Array *arrPt, size_t dstPos,
						   const void *copyElementPt, size_t count);
/*! Move elements within an array, or from one
 *  to the other.
 *
 *  If source and destination are the same, memmove will be used,
 *  otherwise memcpy will be used.
 *  \param dstPos Destination index to move elements into
 *  \param srcPt \ref opt Array to move elements from.  If null, the
 *  destination array is assumed
 *  \param srcPos Source index to move elements from
 *  \param count Number of elements to move
 *  \return \ref reterr
 */
MCR_API int mcr_Array_move(struct mcr_Array *dstPt, size_t dstPos,
						   const struct mcr_Array *srcPt, size_t srcPos, size_t count);

/* Sorted functions: If no compare function is available, memcmp will be
 * used.
 */
/*! \pre Array may be sorted already or not
 *  \post Array will be sorted
 *  \brief Sort array elements
 *
 *  Sorted function, if \ref mcr_Array.compare is available, it will be used.
 */
MCR_API void mcr_Array_sort(struct mcr_Array *arrPt);
/*! \pre Array is sorted
 *  \post Array will be sorted
 *  \brief Get a reference to an element in the array.
 *
 *  Sorted function, if \ref mcr_Array.compare is available, it will be used.
 *  \param arrPt \ref opt
 *  \param elementPt \ref opt Comparison element to find something equivalent
 *  in the array
 *  \return If an equivalent element is found this will be the address
 *  of that element. Otherwise this is null.
 */
MCR_API void *mcr_Array_find(const struct mcr_Array *arrPt,
							 const void *elementPt);
/*! \pre Array is sorted
 *  \post Array will be sorted
 *  \brief Place an element into a sorted array
 *
 *  Sorted function, if \ref mcr_Array.compare is available, it will be used.
 *  \param elementArr \ref opt Array of elements to copy from
 *  \param count Number of elements to copy
 *  \param flagUnique If true elements will only be added if they
 *  do not already exist.
 *  \return \ref reterr
 */
MCR_API int mcr_Array_add(struct mcr_Array *arrPt,
						  const void *elementArr, size_t count, bool flagUnique);
/*! \pre Array is sorted
 *  \post Array will be sorted
 *  \brief Remove all elements that match the given element.
 *
 *  Sorted function, if \ref mcr_Array.compare is available, it will be used.
 *  \param removeElementPt \ref opt Element to remove
 */
MCR_API void mcr_Array_remove(struct mcr_Array *arrPt,
							  const void *removeElementPt);

/* Macro utils, array is checked for existing elements, but indices are
 * not checked for a valid range */
/*! See \ref mcr_Array_element */
#define MCR_ARR_ELEMENT(arr, index) \
mcr_castpt(void, (arr).used ? \
	mcr_castpt(char, (arr).array) + (index) * (arr).element_size : \
mcr_null)

/*! See \ref mcr_Array_first */
#define MCR_ARR_FIRST(arr) MCR_ARR_ELEMENT(arr, 0)

/*! See \ref mcr_Array_last */
#define MCR_ARR_LAST(arr) MCR_ARR_ELEMENT(arr, (arr).used - 1)

/*! See \ref mcr_Array_end */
#define MCR_ARR_END(arr) MCR_ARR_ELEMENT(arr, (arr).used)

/*! See \ref mcr_Array_next */
#define MCR_ARR_NEXT(arr, elementPt) \
mcr_castpt(void, (arr).used && elementPt ? \
	mcr_castpt(char, elementPt) + (arr).element_size : \
mcr_null)

/*! See \ref mcr_Array_prev */
#define MCR_ARR_PREV(arr, elementPt) \
mcr_castpt(void, (arr).used && elementPt ? \
	mcr_castpt(char, elementPt) - (arr).element_size : \
mcr_null)

/*! See \ref mcr_Array_index
 *  element - array = offset, offset / element size will be the index
 */
#define MCR_ARR_INDEX(arr, elementPt) \
((arr).used && elementPt ? \
	mcr_cast(size_t, (mcr_castpt(char, elementPt) - \
			mcr_castpt(char, (arr).array)) / (arr).element_size) : \
mcr_cast(size_t, -1))

/*! See \ref mcr_Array_last_index */
#define MCR_ARR_LAST_INDEX(arr) ((arr).used - 1)

/*! For every element in the array call a function with one argument,
 *  which will be a void * to each element in the array.
 *
 *  It was found that variadic functions have slower performance than
 *  regular functions.  Tests done by Jonathan Pelletier determined
 *  non-variadic functions call in roughly 3/4 the time to call a
 *  variadic function.
 *
 *  \param arr \ref mcr_Array
 *  \param iterateFnc void (*) (void *)  Return value is ignored.\n
 *  If this is a variadic macro then an expansion helper \ref MCR_EXP
 *  may be needed.
 */
#define MCR_ARR_FOR_EACH(arr, iterateFnc) \
{ \
	char *local_it, *local_end; \
	size_t local_bytes; \
	mcr_Array_iter(&(arr), &local_it, &local_end, \
			&local_bytes); \
	while (local_it < local_end) { \
		iterateFnc(mcr_castpt(void, local_it)); \
		local_it += local_bytes; \
	} \
}

#ifdef __cplusplus
}
#endif

#endif
