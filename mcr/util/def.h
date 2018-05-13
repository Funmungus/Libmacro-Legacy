/* Libmacro - A multi-platform, extendable macro and hotkey C library
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
 * \brief Utility definitions
 */

#ifndef MCR_UTIL_DEF_H
#define MCR_UTIL_DEF_H

/* Needed for some functions on linux */
#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

/* Use standard cstring functions, not Windows-specific safeties */
#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <time.h>
#include <stdbool.h>
#ifdef DEBUG
	#include <assert.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Doxygen in-document references */
#ifndef reterr
	/*! Return 0 for success, otherwise standard error code
	* from \c errno.h.
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
	* or greater than 0 if left > right.
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
 * e.g. MCR_STR(myString) => "myString"
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
 * \param lhs Left side
 * \param rhs Right side
 * \return \ref retcmp
 */
#define MCR_CMP(lhs, rhs) \
((lhs) < (rhs) ? \
	-1 : \
(lhs) > (rhs))
#endif
#ifndef MCR_CMP_CAST
/*! Integral comparison with built-in casting
 *
 * \param casting Prefix used to cast variables before comparison
 * \param lhs Left side
 * \param rhs Right side
 * \return \ref retcmp
 */
#define MCR_CMP_CAST(casting, lhs, rhs) \
MCR_CMP(casting(lhs), casting(rhs))
#endif
/* Debugging definitions */
#ifndef mcr_ddo
	#ifdef DEBUG
		/*! Do the argument only for debug builds. */
		#define mcr_ddo(stuff) stuff
	#else
		#define mcr_ddo(stuff)
	#endif
#endif
#ifndef mcr_dprint
/*! If debug build, print to stdout. */
#define mcr_dprint(...) \
mcr_ddo(printf(__VA_ARGS__))
#endif
#ifndef mcr_dmsg
/*! Automated error message based on errno.
 *
 * Print to \c stderr for only debug builds.
 */
#define mcr_dmsg \
mcr_ddo(fprintf(stderr, "Error %d: " MCR_LINE ", %s: %s.\n", \
		mcr_err, mcr_timestamp(), strerror(mcr_err)))
#endif
#ifndef mcr_dassert
	/*! Assertion only for debug builds */
	#define mcr_dassert(expression) mcr_ddo(assert(expression))
#endif
#ifndef mcr_fixme
	#ifdef _MSC_VER
		#define mcr_fixme
	#else
		/*! Debug compile message indicating future changes */
		#define mcr_fixme \
		mcr_ddo(_Pragma("message MCR_LINE \": Fix Me!\""))
	#endif
#endif
#ifndef mcr_arrlen
/*! For a static array definition get the number of elements.
 *
 * \param arr Static array
 * \return size_t Number of elements
 */
#define mcr_arrlen(arr) \
(sizeof(arr) / sizeof(*(arr)))
#endif
#ifndef mcr_errno
/*! Set \ref mcr_err to \ref errno.
 *
 * Also \ref mcr_dmsg
 * \param fallbackError If errno is not an error, use this error code.
 */
#define mcr_errno(fallbackError) \
mcr_err = errno; \
if (!mcr_err) { \
	mcr_err = (fallbackError); \
} \
mcr_dmsg;
#endif

#ifndef mcr_mset_error
/*! Set \ref mcr_err and \ref mcr_dmsg */
#define mcr_mset_error(errorNumber) \
mcr_err = (errorNumber); \
mcr_dmsg;
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
#ifndef fixme
	#define fixme mcr_fixme
#endif
#ifndef arrlen
	/*! \ref mcr_arrlen */
	#define arrlen(arr) mcr_arrlen(arr)
#endif
#ifndef mset_error
	/*! \ref mcr_mset_error */
	#define mset_error(errorNumber) mcr_mset_error(errorNumber)
#endif

#ifndef MCR_API
	#ifdef MCR_STATIC
		#define MCR_API
	#else
		#ifdef MCR_EXPORTS
			#define MCR_API __declspec(dllexport)
		#else
			#define MCR_API __declspec(dllimport)
		#endif
	#endif
#endif

/* Used throughout in library functions */
struct mcr_context;

/* Thread-local storage not available in MSVC? */
MCR_API int *mcr_err_tls();
#ifdef _MSC_VER
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
 * Use this for data init and deinit, such as \ref mcr_Interface.init
 * and \ref mcr_Interface.deinit
 * \param dataPt \ref mcr_Data.data
 * \return \ref reterr
 */
typedef int (*mcr_data_fnc) (void *dataPt);
/*! Copy from one data or object another
 *
 * \param destinationPt Destination object
 * \param sourcePt \ref opt Source object
 * \return \ref reterr
 */
typedef int (*mcr_copy_fnc) (void *destinationPt, const void *sourcePt);
/*! Comparison function of two data or objects
 *
 * \param lhsPt \ref opt Left side of comparison
 * \param rhsPt \ref opt Right side of comparison
 * \return \ref retcmp
 */
typedef int (*mcr_compare_fnc) (const void *lhsPt, const void *rhsPt);

#ifndef MCR_UTIL_PLATFORM_INC
	/*! Include to access platform utilities */
	#define MCR_UTIL_PLATFORM_INC MCR_STR(mcr/util/MCR_PLATFORM/nutil.h)
#endif

/* Define string comparison aliases, because Windows has too many options */
#include MCR_STR(mcr/util/MCR_PLATFORM/ndef.h)

#ifdef __cplusplus
}
#endif

#endif
