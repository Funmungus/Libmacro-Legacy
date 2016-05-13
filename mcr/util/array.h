/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

/*! \file
 * \brief Dynamic resizing array, with utility functions
 */

#ifndef MCR_ARRAY_H
#define MCR_ARRAY_H

#include "mcr/util/interface.h"

/*!
 * \brief Dynamic resizing array
 *
 * Byte size of the whole array = elementSize * size <br/>
 * Important: char array is first element for string compare
 */
struct mcr_Array {
	/*! \brief Resizing array pointer */
	char *array;
	/*! \brief Number of elements assignable (allocated area) */
	size_t size;
	/*! \brief Number of elements assigned */
	size_t used;
	/* Internal */
	/*! \brief Optional comparison to sort and find elements.
	 *
	 * If NULL, all sorting and finding will use memcmp */
	mcr_compare_fnc compare;
	/*! \brief Size of one element, in bytes */
	size_t element_size;
};
/*! \brief Data interface for a sorted, generic array of references */
MCR_API const struct mcr_Interface *mcr_Array_ref_iface();

/*!
 * \ref mcr_Array ctor
 *
 * Element size will be 1, with no array allocated
 * \param dataPt (opt)
 */
MCR_API void mcr_Array_init(void *dataPt);
/*! \brief \ref mcr_Array_init for ordered array of pointers */
MCR_API void mcr_Array_init_ref(void *data);
/*!
 * \brief \ref mcr_Array dtor
 *
 * Element size is preserved.
 * \param dataPt (opt)
 */
MCR_API void mcr_Array_free(void *dataPt);
/*! See \ref mcr_Array_free */
#define mcr_Array_free_foreach(arrPt, ignore) mcr_Array_free(arrPt)
/*!
 * \brief Set values needed to be useful.
 *
 * Will free anything that is allocated
 * \param compare \ref mcr_Array.compare
 * \param elementSize \ref mcr_Array.element_size
 */
MCR_API void mcr_Array_set_all(struct mcr_Array *arrPt,
	mcr_compare_fnc compare, size_t elementSize);
/* Allocation control */
MCR_API int mcr_Array_minused(struct mcr_Array *arrPt, size_t minimumUsed);
MCR_API int mcr_Array_minsize(struct mcr_Array *arrPt, size_t minimumSize);
MCR_API int mcr_Array_smartsize(struct mcr_Array *arrPt,
	size_t increasingCount);
MCR_API int mcr_Array_minfill(struct mcr_Array *arrPt, size_t minimumUsed,
	const void *fillerElementPt);
/*! \brief Minimize array allocation. */
MCR_API void mcr_Array_trim(struct mcr_Array *arrPt);
/*!
 * \brief Reallocate array to given size. Actual byte size will
 * be this size * element size.
 *
 * \return \ref reterr
 */
MCR_API int mcr_Array_resize(struct mcr_Array *arrPt, size_t newSize);
MCR_API void mcr_Array_clear(struct mcr_Array *arrPt);
/* Position */
/*!
 * \brief Get a reference to a position in the array.
 *
 * \param pos Index of element to return
 * \return If pos is within used range, this will be the address
 * of that position. Otherwise this is NULL.
 */
MCR_API void *mcr_Array_element(const struct mcr_Array *arrPt, size_t pos);
/*!
 * \brief Get a position reference after the given position.
 *
 * \param posPt (opt) Base position to increase from
 * \return Increase the number of bytes equal to
 * mcr_Array#element_size.
 */
MCR_API void *mcr_Array_next(const struct mcr_Array *arrPt, const void *posPt);
/*!
 * \brief Get a position reference before the given position.
 *
 * \param posPt (opt) Base position to decrease from
 * \return Decrease number of bytes equal to
 * mcr_Array#element_size.
 */
MCR_API void *mcr_Array_prev(const struct mcr_Array *arrPt, const void *posPt);
/*!
 * \brief Get a position reference exactly one byte after
 * last element in the array.
 *
 * \param arrPt (opt)
 * \return This will be NULL if there are no elements.  Otherwise this
 * will be the byte right after the last element.
 */
MCR_API void *mcr_Array_end(const struct mcr_Array *arrPt);
/*!
 * \brief Get a reference to the first element.
 *
 * \param arrPt (opt)
 */
MCR_API void *mcr_Array_first(const struct mcr_Array *arrPt);
/*!
 * \brief Get a reference to the last element.
 *
 * \param arrPt (opt)
 */
MCR_API void *mcr_Array_last(const struct mcr_Array *arrPt);
/*!
 * \brief Get the index of the last element.
 *
 * \param arrPt (opt)
 */
MCR_API size_t mcr_Array_last_index(const struct mcr_Array *arrPt);
/*!
 * \brief Get the position for a pointer to an element in the array.
 *
 * \param elPt (opt) Pointer to an element in the array
 * \return \ref retind
 */
MCR_API size_t mcr_Array_index(const struct mcr_Array *arrPt, const void *elPt);
/* Add/remove */
/*!
 * \brief Place an element at the given position. Byte position is
 * determined by the element size.
 *
 * This will displace elements after position. If position is
 * out of current range, the extra elements will be filled with
 * 0's.
 *
 * \param pos Index to place into
 * \param elementArr (opt) Array of elements to copy from
 * \param len Number of elements to copy
 * \param fillerElementPt (opt) Fill empty elements with this value,
 * or if NULL fill with 0's
 * \return \ref reterr
 */
MCR_API int mcr_Array_insert(struct mcr_Array *arrPt, size_t pos,
	const void *elementArr, size_t count);
MCR_API int mcr_Array_insert_filled(struct mcr_Array *arrPt, size_t pos,
	const void *elementArr, size_t count, const void *fillerPt);
/*!
 * \brief Remove elements from given index, and adjust the rest of
 * the array.
 *
 * \param pos Index to remove from
 * \param count Number of elements to remove
 */
MCR_API void mcr_Array_remove_index(struct mcr_Array *arrPt, size_t pos,
	size_t count);
/*!
 * \brief Place elements at the end of the array.
 *
 * \param elementArr (opt) Array of elements to copy from
 * \param len Number of elements to copy
 * \param flagUnique If true elements will only be added if they
 * do not already exist.
 * \return \ref reterr
 */
MCR_API int mcr_Array_append(struct mcr_Array *arrPt, const void *elementArr,
	size_t len, bool flagUnique);
/*! \brief \ref mcr_Array_append with one element */
MCR_API int mcr_Array_push(struct mcr_Array *arrPt, const void *elementPt,
	bool flagUnique);
/*!
 * \brief Decrease used amount by one.
 *
 * \param arrPt (opt)
 */
MCR_API void mcr_Array_pop(struct mcr_Array *arrPt);
/* Replace current elements */
/*!
 * \brief Replace entire array with given array source.
 *
 * \param arraySource (opt) Values are copied from this
 * array.
 * \param count Number of values to copy
 * \return \ref reterr
 */
MCR_API int mcr_Array_replace(struct mcr_Array *arrPt,
	const void *arraySource, size_t count);
/*!
 * \brief Replace a subset of elements at a position.
 *
 * Size will be increased if needed.
 * \param srcArray (opt) Array to copy from; If NULL, memset will be used.
 */
MCR_API int mcr_Array_copy(struct mcr_Array *dstPt, size_t dstPos,
	const void *srcArray, size_t count);
/*!
 * \brief Replace a single element at a position.
 *
 * \param pos Position of the element to replace
 * \param elementPt (opt) Pointer to the object to copy from
 * \return \ref reterr
 */
MCR_API int mcr_Array_set(struct mcr_Array *arrPt, size_t pos,
	const void *elementPt);
/*!
 * \brief Replace a subset of elements by copying a single element.
 *
 * \param copyElementPt (opt) Copy this into subset.  If NULL, fill
 * elements with 0's.
 */
MCR_API int mcr_Array_fill(struct mcr_Array *arrPt, size_t pos,
	size_t count, const void *copyElementPt);
/*!
 * \brief Move elements within an array, or from one
 * to the other.
 *
 * If source and destination are the same, memmove will be used,
 * otherwise memcpy will be used.
 */
MCR_API int mcr_Array_move(struct mcr_Array *dstPt, size_t dstPos,
	const struct mcr_Array *srcPt, size_t srcPos, size_t count);
/*
 * Sortable functions: If no compare function available, memcmp will be
 * used on an unsorted array, with a slow iteration.
 */
/*!
 * \brief Sort array elements ordered with the comparison function.
 *
 * Sortable function
 */
MCR_API void mcr_Array_sort(struct mcr_Array *arrPt);
/*!
 * \brief Get a reference to an element in the array.
 *
 * Sortable function
 * \param elementPt Comparison element to find something equivalent
 * in the array
 * \return If an equivalent element is found this will be the address
 * of that element. Otherwise this is NULL.
 */
MCR_API void *mcr_Array_find(const struct mcr_Array *arrPt,
	const void *elementPt);
/*!
 * \brief Place an element into a sorted array
 *
 * Sortable function, will push elements if not sorted.
 * \param elementArr (opt) Array of elements to copy from
 * \param len Number of elements to copy
 * \param flagUnique If true elements will only be added if they
 * do not already exist.
 * \return \ref reterr
 */
MCR_API int mcr_Array_add(struct mcr_Array *arrPt,
	const void *elementArr, size_t len, bool flagUnique);
/*!
 * \brief Remove all elements that match the given element.
 *
 * Sortable function
 * \param removeElementPt Element to remove
 */
MCR_API void mcr_Array_remove(struct mcr_Array *arrPt,
	const void *removeElementPt);

/* Macro utils */
/*! See \ref mcr_Array_element */
#define MCR_ARR_ELEMENT(arr, pos) \
((void *)((arr).array + ((pos) * (arr).element_size)))
/*! See \ref mcr_Array_next */
#define MCR_ARR_NEXT(arr, curPt) \
((void *)((curPt) ? \
	((const char *)(curPt)) + (arr).element_size : \
NULL))
/*! See \ref mcr_Array_prev */
#define MCR_ARR_PREV(arr, curPt) \
((void *)((curPt) ? \
	((const char *)(curPt)) - (arr).element_size : \
NULL))
/*! See \ref mcr_Array_first */
#define MCR_ARR_FIRST(arr) ((void *)((arr).array))
/*! See \ref mcr_Array_end */
#define MCR_ARR_END(arr) \
((void *)((arr).used ? \
	MCR_ARR_ELEMENT (arr, (arr).used) : \
NULL))
/*! See \ref mcr_Array_last */
#define MCR_ARR_LAST(arr) \
((void *)((arr).used ? \
	MCR_ARR_ELEMENT(arr, (arr).used - 1) : \
NULL))
#define MCR_ARR_LAST_INDEX(arr) ((arr).used - 1)
/*! See \ref mcr_Array_indexof */
#define MCR_ARR_INDEX(arr, elementPt) \
((size_t)((arr).array && elementPt && (const char *) \
		(elementPt) >= (arr).array ? \
	(((const char *)(elementPt)) - \
			(arr).array) / (arr).element_size : \
(size_t)~0))
/*! */
#define MCR_ARR_ITER(arr, itHolder, endHolder, bytesHolder) \
bytesHolder = (arr).element_size; \
itHolder = (char *)MCR_ARR_FIRST(arr); \
endHolder = (char *)MCR_ARR_END(arr);
/*! */
#define MCR_ARR_ITER_RANGE(arr, itHolder, endHolder, bytesHolder, \
		firstIndex, lastIndex) \
bytesHolder = (arr).element_size; \
itHolder = (char *)MCR_ARR_ELEMENT(arr, firstIndex); \
endHolder = (char *)((lastIndex) + 1 < (arr).used ? \
		MCR_ARR_ELEMENT(arr, (lastIndex) + 1) : \
		MCR_ARR_END(arr));
/*!
 * \brief For every element in the array call a function with arguments
 * starting with a pointer, which is an element in the array, and
 * ending with all other arguments in __VA_ARGS__.
 *
 * This will not create va_list or do any type checking on variable
 * arguments.\n
 * It was found that variadic functions have slower performance than
 * regular functions.  Tests done by Jonathan Pelletier determined
 * non-variadic functions call in roughly 3/4 the time to call a
 * variadic function.  Please now use MCR_EXP with variadic macros.
 *
 * \param arr \ref mcr_Array
 * \param iterateFnc void (*) (void *, ...) This function must be able
 * to accept both the array member reference, and all variadic
 * parameters given to this macro.\n
 * If this is a variadic macro then an expansion helper \ref MCR_EXP
 * may be needed. For example, \ref mcr_Array_free_foreach needs to be
 * expanded, but ignores extra arguments:\n
 * \c MCR_ARR_FOR_EACH(arr, MCR_EXP(mcr_Array_free_foreach),)
 */
#define MCR_ARR_FOR_EACH(arr, iterateFnc, ...) \
{ \
	MCR_ARR_ITER (arr, char *local_it, char *local_end, \
			size_t local_bytes); \
	while (local_it < local_end) { \
		iterateFnc ((void *)local_it, __VA_ARGS__); \
		local_it += local_bytes; \
	} \
}
/*!
 * \brief Similar to \ref MCR_ARR_FOR_EACH, but iterates through a
 * set of indices.
 *
 * Indices include all beginning with \c firstIndex, through \c lastIndex.
 * That means \c lastIndex is included in the range \c iterateFnc acts on.
 */
#define MCR_ARR_RANGE(arr, firstIndex, lastIndex, iterateFnc, ...) \
{ \
	MCR_ARR_ITER_RANGE (arr, char *local_it, char *local_end, \
			size_t local_bytes, firstIndex, lastIndex); \
	while (local_it < local_end) { \
		iterateFnc ((void *)local_it, __VA_ARGS__); \
		local_it += local_bytes; \
	} \
}
/*
 * Sortable array macros; If no compare function available, then array
 * functions will be used.
 */
/*! See \ref mcr_Array_sort */
#define MCR_ARR_SORT(arr) \
if ((arr).used) { \
	if ((arr).compare) \
		qsort ((arr).array, (arr).used, (arr).element_size, \
				(arr).compare); \
	else \
		mcr_Array_sort (&(arr)); \
}
/*! See \ref mcr_Array_find */
#define MCR_ARR_FIND(arr, elPt) \
((arr).used ? \
	(arr).compare ? \
		bsearch (elPt, (arr).array, (arr).used, \
				(arr).element_size, (arr).compare) : \
	mcr_Array_find (&(arr), elPt) : \
NULL)
/*! \brief Number of bytes allocated. */
#define mcr_Array_bytes(arrPt) \
((arrPt) ? (arrPt)->size * (arrPt)->element_size : 0)
/*! \brief Number of bytes allocated. */
#define MCR_ARR_BYTES(arr) \
((arr).size * (arr).element_size)

#endif
