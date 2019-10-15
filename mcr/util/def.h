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
 *  \brief Utility definitions
 */

#ifndef MCR_UTIL_DEF_H_
#define MCR_UTIL_DEF_H_

/* Needed for some functions with GCC */
#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

/* Use standard cstring functions, not Windows-specific safeties */
#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef __cplusplus
#include <ctime>
#include <cstdbool>
#include <cstdint>
#ifdef MCR_DEBUG
	#include <cassert>
#endif
extern "C" {
#else
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#ifdef MCR_DEBUG
	#include <assert.h>
#endif
#endif

/* Doxygen in-document references */
#ifndef reterr
	/*! Return 0 for success, otherwise standard error code
	*   from \c errno.h.
	*/
	#define reterr int
#endif

#ifndef retind
	/*! Return size_t index, or -1 on error. */
	#define retind size_t
#endif

#ifndef retid
	/*! Return size_t id, or -1 on error. */
	#define retid size_t
#endif

#ifndef retcmp
	/*! Return 0 if comparison is equal, less than 0 if left < right,
	*   or greater than 0 if left > right.
	*/
	#define retcmp int
#endif

#ifndef opt
	/*! Optional parameter */
	#define opt
#endif

#ifndef mcr_is_platform
	/*! Required to be re-defined for each platform */
	#define mcr_is_platform
#endif

#define MCR_STR_HELPER(x) #x
/*! Stringify, or create a string.
 *  e.g. MCR_STR(myString) => "myString"
 */
#define MCR_STR(x) MCR_STR_HELPER(x)

#define MCR_EXP_HELPER(x) x
/*! Expand macros: e.g. MCR_EXP(myMacro) => myMacroExpanded */
#define MCR_EXP(x) MCR_EXP_HELPER(x)

#define MCR_TOKEN_HELPER(x, y) x ##y
#define MCR_SNAKE_HELPER(x, y) x ##_ ##y
/*! MCR_TOKEN(MyString, MyString2) => MyStringMyString2 */
#define MCR_TOKEN(x, y) MCR_TOKEN_HELPER(x, y)
/*! MCR_SNAKE(myString, myString2) => myString_myString2 */
#define MCR_SNAKE(x, y) MCR_SNAKE_HELPER(x, y)

/*! Constant string "file(line)" */
#ifndef MCR_LINE
	#define MCR_LINE __FILE__ "(" MCR_STR(__LINE__) ")"
#endif

#ifndef MCR_PLATFORM
	#define MCR_PLATFORM none
	#pragma message "Warning: MCR_PLATFORM is not defined.  Please provide MCR_PLATFORM as a preprocessor definition for your platform."
	#include "mcr/warn.h"
#endif
#ifndef UNUSED
	/*! Remove compiler warning for unused variable. */
	#define UNUSED(x) ((void)(x))
#endif
#ifndef MCR_CMP
	/*! Compare integral types.  Casting must be done manually.
	*
	*  \param lhs Left side
	*  \param rhs Right side
	*  \return \ref retcmp
	*/
	#define MCR_CMP(lhs, rhs) (((lhs) > (rhs)) - ((lhs) < (rhs)))
#endif
#ifndef MCR_CMP_CAST
	/*! Integral comparison with built-in casting
	*
	*  \param casting Prefix used to cast variables before comparison
	*  \param lhs Left side
	*  \param rhs Right side
	*  \return \ref retcmp
	*/
	#define MCR_CMP_CAST(casting, lhs, rhs) MCR_CMP(casting(lhs), casting(rhs))
#endif
#ifndef MCR_CMP_PTR
/*! Integral comparison of pointers with built-in casting
*
*  If either pointer is null the pointer addresses are compared.
*  \param T Native type each pointer is pointing to.  e.g. if comparing
*  pointers to integers T should be \c int.  If either pointer is const
*  then T must also be const.  e.g. \c const \c int
*  \param lhsPtr Pointer to left side comparison
*  \param rhsPtr Pointer to right side comparison
*  \return \ref retcmp
*/
#define MCR_CMP_PTR(T, lhsPtr, rhsPtr) (lhsPtr && rhsPtr ? \
		MCR_CMP(*mcr_castpt(T, lhsPtr), *mcr_castpt(T, rhsPtr)) : \
		MCR_CMP(lhsPtr, rhsPtr))
#endif
#ifndef mcr_cast
	#ifdef __cplusplus
		#define mcr_cast(T, obj) static_cast<T>(obj)
		#define mcr_castpt(T, obj) reinterpret_cast<T *>(obj)
		#define mcr_null nullptr
	#else
		/*! static_cast for C++ and regular cast for C.  Macros are not affected
		*   by extern "C". Designed for macros C/C++ interoperability. */
		#define mcr_cast(T, obj) ((T)(obj))
		/*! reinterpret_cast as a pointer for C++ and regular cast for C.
		*   Macros are not affected by extern "C". Designed for macros C/C++
		*   interoperability. */
		#define mcr_castpt(T, obj) ((T *)(obj))
		/*! nullptr for C++ and NULL for C */
		#define mcr_null NULL
	#endif
#endif
/* Debugging definitions */
#ifndef mcr_ddo
	#ifdef MCR_DEBUG
		/*! Do the argument only for debug builds. */
		#define mcr_ddo(stuff) stuff
	#else
		/* Empty blocks.  Non-empty interferes with one-liners. */
		#define mcr_ddo(stuff) {}
	#endif
#endif
#ifndef mcr_dprint
	/*! If debug build, print to stdout. */
	#define mcr_dprint(...) mcr_ddo(printf(__VA_ARGS__))
#endif
#ifndef mcr_dexit
	/*! Only when debugging exit with \ref mcr_err error code. */
	#define mcr_dexit mcr_ddo({ mcr_dmsg; exit(mcr_err); })
#endif
#ifndef mcr_dmsg
/*! Automated error message of \ref mcr_errno.
 *
 *  Print to \c stderr for only debug builds.
 */
#define mcr_dmsg \
mcr_ddo(fprintf(stderr, "Error %d: " MCR_LINE ", %s: %s.\n", \
		mcr_err, mcr_timestamp(), strerror(mcr_err)))
#endif
#ifndef mcr_dassert
	/*! Assertion only for debug builds */
	#define mcr_dassert(expression) mcr_ddo(assert(expression))
#endif
#ifndef mcr_arrlen
	/*! For a static array definition get the number of elements.
	*
	*  \param arr Static array
	*  \return size_t Number of elements
	*/
	#define mcr_arrlen(arr) (sizeof(arr) / sizeof(*(arr)))
#endif
#ifndef mcr_errno
/*! Set \ref mcr_err to \ref errno.
 *
 *  Also \ref mcr_dmsg
 *  \param fallbackError If errno is not an error, use this error code.
 */
#define mcr_errno(fallbackError) { \
	mcr_err = errno; \
	if (!mcr_err) \
		mcr_err = (fallbackError); \
	mcr_dmsg; \
}
#endif
#ifndef mcr_err_return
/*! Immediately \ref mcr_dmsg and return \ref mcr_err.
 *
 *  Only use in a function that returns \ref reterr
 */
#define mcr_err_return { \
	mcr_dmsg; \
	return mcr_err; \
}
#endif
#ifndef mcr_mset_error
/*! Set \ref mcr_err and \ref mcr_dmsg */
#define mcr_mset_error(errorNumber) { \
	mcr_err = (errorNumber); \
	mcr_dmsg; \
}
#endif
#ifndef mcr_mset_error_return
/*! Set \ref mcr_err, \ref mcr_dmsg and return \ref mcr_err.
 *
 *  Only use in a function that returns \ref reterr
 */
#define mcr_mset_error_return(errorNumber) { \
	mcr_err = (errorNumber); \
	mcr_dmsg; \
	return errorNumber; \
}
#endif

#ifndef WARN
	/*! \ref MCR_WARN */
	#define WARN MCR_WARN
#endif
#ifndef ERR
	/*! \ref MCR_ERR */
	#define ERR MCR_ERR
#endif
#ifndef FWARN
	/*! \ref MCR_FWARN */
	#define FWARN MCR_FWARN
#endif
#ifndef FERR
	/*! \ref MCR_FERR */
	#define FERR MCR_FERR
#endif
#ifndef ddo
	/*! \ref mcr_ddo */
	#define ddo(stuff) mcr_ddo(stuff)
#endif
#ifndef dexit
	/*! \ref mcr_dexit */
	#define dexit mcr_dexit
#endif
#ifndef dprint
	/*! \ref mcr_dprint */
	#define dprint(...) mcr_dprint(__VA_ARGS__)
#endif
#ifndef dmsg
	/*! \ref mcr_dmsg */
	#define dmsg mcr_dmsg
#endif
#ifndef dassert
	/*! \ref mcr_dassert */
	#define dassert(expr) mcr_dassert(expr)
#endif
#ifndef arrlen
	/*! \ref mcr_arrlen */
	#define arrlen(arr) mcr_arrlen(arr)
#endif
#ifndef err_return
	/*! \ref mcr_err_return */
	#define err_return mcr_err_return
#endif
#ifndef mset_error
	/*! \ref mcr_mset_error */
	#define mset_error(errorNumber) mcr_mset_error(errorNumber)
#endif
#ifndef mset_error_return
	/*! \ref mcr_mset_error_return */
	#define mset_error_return(errorNumber) mcr_mset_error_return(errorNumber)
#endif

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define MCR_EXPORT __declspec(dllexport)
	#define MCR_IMPORT __declspec(dllimport)
#else
	#define MCR_EXPORT __attribute__((visibility("default")))
	#define MCR_IMPORT __attribute__((visibility("default")))
#endif

#ifndef MCR_API
	#ifdef MCR_STATIC
		#define MCR_API
	#else
		#ifdef LIBMACRO_LIBRARY
			#define MCR_API MCR_EXPORT
		#else
			#define MCR_API MCR_IMPORT
		#endif
	#endif
#endif

/* Used throughout in library functions */
struct mcr_context;

#ifdef _MSC_VER
	/*! Thread-local storage not available in MSVC? */
	MCR_API int *mcr_err_tls();
	#define mcr_err (*mcr_err_tls())
#else
	extern MCR_API
	#ifdef __GNUC__
		__thread
	#else
		thread_local
	#endif
	int mcr_err;
#endif

/*! Function on data or an object.
 *
 *  Use this for data init and deinit, such as \ref mcr_Interface.init
 *  and \ref mcr_Interface.deinit
 *  \param dataPt \ref mcr_Data.data
 *  \return \ref reterr
 */
typedef int (*mcr_data_fnc) (void *dataPt);
/*! Copy from one data or object another
 *
 *  \param destinationPt Destination object
 *  \param sourcePt \ref opt Source object
 *  \return \ref reterr
 */
typedef int (*mcr_copy_fnc) (void *destinationPt, const void *sourcePt);
/*! Comparison function of two data or objects
 *
 *  \param lhsPt \ref opt Left side of comparison
 *  \param rhsPt \ref opt Right side of comparison
 *  \return \ref retcmp
 */
typedef int (*mcr_compare_fnc) (const void *lhsPt, const void *rhsPt);

/* Machine, platform, and some alias definitions.
 *
 * If linux is defined as 1 MCR_PLATFORM will expand to 1.
 */
#ifdef linux
#include "mcr/util/linux/p_def.h"
#else
#include MCR_STR(mcr/util/MCR_PLATFORM/p_def.h)
#endif

#ifdef __cplusplus
}
#endif

#endif
