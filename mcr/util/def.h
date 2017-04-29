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

/*!
 * \file
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

/* Doxygen in-document references */
#ifndef reterr
/*!
 * \brief Return 0 for success, otherwise standard error code
 * from \c errno.h.
 *
 * May also be a negative standard error code.
 */
#define reterr int
#endif

#ifndef retind
/*! \brief Return size_t index, or -1 on error. */
#define retind size_t
#endif

#ifndef retid
/*! \brief Return size_t id, or -1 on error. */
#define retid size_t
#endif

#ifndef retcmp
/*!
 * \brief Return 0 if comparison is equal, less than 0 if left < right,
 * or greater than 0 if left > right.
 */
#define retcmp int
#endif

#ifndef opt
/*! \brief Optional parameter */
#define opt
#endif

#ifndef mcr_is_platform
/*! \brief Required to be re-defined for each platform */
#define mcr_is_platform
#endif

#define MCR_STR_HELPER(x) #x
/*!
 * \brief Stringify, or create a string.
 * e.g. MCR_STR(myString) => "myString"
 */
#define MCR_STR(x) MCR_STR_HELPER(x)

#define MCR_EXP_HELPER(x) x
/*! \brief Expand macros: e.g. MCR_EXP(myMacro) => myMacroExpanded */
#define MCR_EXP(x) MCR_EXP_HELPER(x)

#define MCR_TOKEN_HELPER(x, y) x ##y
#define MCR_SNAKE_HELPER(x, y) x ##_ ##y
/*! \brief MCR_TOKEN(MyString, MyString2) => MyStringMyString2 */
#define MCR_TOKEN(x, y) MCR_TOKEN_HELPER(x, y)
/*! \brief MCR_SNAKE(myString, myString2) => myString_myString2 */
#define MCR_SNAKE(x, y) MCR_SNAKE_HELPER(x, y)

#ifndef MCR_LINE
#define MCR_LINE __FILE__ "(" MCR_STR(__LINE__) ")"
#endif

#ifndef MCR_WARN
/*!
 * \brief Display a compiler warning.
 *
 * See \ref MCR_FWARN for more information\n
 * "Warning: file(line)"
 */
#define MCR_WARN _Pragma("message \"Warning: \" MCR_LINE")
#endif

#ifndef MCR_ERR
/*!
 * \brief Display a compiler error.
 *
 * See \ref MCR_FERR for more information\n
 * "Error: file(line)"
 */
#define MCR_ERR _Pragma("message \"Error: \" MCR_LINE")
#endif

#ifndef MCR_FWARN
/*!
 * \brief Include file to raise a compiler warning.
 *
 * usage: \code{.c}
 * MCR_WARN
 * #include MCR_FWARN
 * \endcode
 */
#define MCR_FWARN "mcr/warn.h"
#endif

#ifndef MCR_FERR
/*!
 * \brief Include file to raise a compiler error.
 *
 * usage: \code{.c}
 * MCR_ERR
 * #include MCR_FERR
 * \endcode
 */
#define MCR_FERR "mcr/err.h"
#endif

#ifndef MCR_PLATFORM
#define MCR_PLATFORM none
MCR_WARN
#pragma message "MCR_PLATFORM is not defined.  Please provide \
MCR_PLATFORM as a preprocessor definition for your platform."
#include MCR_FERR
#endif
#ifndef UNUSED
/*! \brief Remove compiler warning for unused variable. */
#define UNUSED(x) ((void)(x))
#endif
#ifndef MCR_CMP_INTEGRAL
/*!
 * \brief Compare integral types.  Casting must be done manually.
 *
 * \param lhs Left side
 * \param rhs Right side
 * \return \ref retcmp
 */
#define MCR_CMP_INTEGRAL(lhs, rhs) \
((lhs) < (rhs) ? \
	-1 : \
(lhs) > (rhs))
#endif
#ifndef MCR_CMP_CAST
/*!
 * \brief Integral comparison with built-in casting
 *
 * \param casting Prefix used to cast variables before comparison
 * \param lhs Left side
 * \param rhs Right side
 * \return \ref retcmp
 */
#define MCR_CMP_CAST(casting, lhs, rhs) \
MCR_CMP_INTEGRAL(casting(lhs), casting(rhs))
#endif
/* Debugging definitions */
#ifndef mcr_ddo
#ifdef DEBUG
/*! \brief Do the argument only for debug builds. */
#define mcr_ddo(stuff) stuff
#else
#define mcr_ddo(stuff)
#endif
#endif
#ifndef mcr_dprint
/*! \brief If debug build, print to stdout. */
#define mcr_dprint(...) \
mcr_ddo(printf(__VA_ARGS__))
#endif
#ifndef mcr_dmsg
/*!
 * \brief Automated error message based on errno.
 *
 * Print to \c stderr for only debug builds.
 */
#define mcr_dmsg \
mcr_ddo(fprintf(stderr, "Error %d: " MCR_LINE ", %s: %s.\n", \
		mcr_error(), mcr_timestamp(), strerror(mcr_error())))
#endif
#ifndef mcr_dassert
/*! \brief Assertion only for debug builds */
#define mcr_dassert(expression) mcr_ddo(assert(expression))
#endif
#ifndef mcr_fixme
#define mcr_fixme \
mcr_ddo(_Pragma("message MCR_LINE \": Fix Me!\""))
#endif
#ifndef mcr_arrlen
/*!
 * \brief For a static array definition get the number of elements.
 *
 * \param arr Static array
 * \return size_t Number of elements
 */
#define mcr_arrlen(arr) \
(sizeof(arr) / sizeof(*(arr)))
#endif
#ifndef mcr_mset_error
/*! \brief \ref mcr_set_error and \ref mcr_dmsg */
#define mcr_mset_error(errorNumber) \
mcr_set_error(errorNumber); \
if (errorNumber) { \
	mcr_dmsg; \
}
#endif
#ifndef mcr_mset_errno
/*! \brief \ref mcr_set_errno with current file and line */
#define mcr_mset_errno(errorNumber) \
mcr_set_errno(errorNumber, __FILE__, __LINE__)
#endif
#ifndef WARN
/*! \brief \ref MCR_WARN */
#define WARN MCR_WARN
#endif
#ifndef ERR
/*! \brief \ref MCR_ERR */
#define ERR MCR_ERR
#endif
#ifndef FWARN
/*! \brief \ref MCR_FWARN */
#define FWARN MCR_FWARN
#endif
#ifndef FERR
/*! \brief \ref MCR_FERR */
#define FERR MCR_FERR
#endif
#ifndef ddo
/*! \brief \ref mcr_ddo */
#define ddo(stuff) mcr_ddo(stuff)
#endif
#ifndef dprint
/*! \brief \ref mcr_dprint */
#define dprint(...) mcr_dprint(__VA_ARGS__)
#endif
#ifndef dmsg
/*! \brief \ref mcr_dmsg */
#define dmsg mcr_dmsg
#endif
#ifndef dassert
/*! \brief \ref mcr_dassert */
#define dassert(expr) mcr_dassert(expr)
#endif
#ifndef fixme
#define fixme mcr_fixme
#endif
#ifndef arrlen
/*! \brief \ref mcr_arrlen */
#define arrlen(arr) mcr_arrlen(arr)
#endif
#ifndef mset_error
/*! \brief \ref mcr_mset_error */
#define mset_error(errorNumber) mcr_mset_error(errorNumber)
#endif
#ifndef mset_errno
/*! \brief \ref mcr_mset_errno */
#define mset_errno(errorNumber) mcr_mset_errno(errorNumber)
#endif
/* Used throughout in library functions */
struct mcr_context;
/*!
 * \brief Function on data or an object.
 *
 * Use this for data init and deinit, such as \ref mcr_Interface.init
 * and \ref mcr_Interface.deinit
 * \param dataPt \ref mcr_Data.data
 * \return \ref reterr
 */
typedef int (*mcr_data_fnc) (void *dataPt);
/*!
 * \brief Copy from one data or object another
 *
 * \param destinationPt Destination object
 * \param sourcePt \ref opt Source object
 * \return \ref reterr
 */
typedef int (*mcr_copy_fnc) (void *destinationPt, const void *sourcePt);
/*!
 * \brief Comparison function of two data or objects
 *
 * \param lhsPt \ref opt Left side of comparison
 * \param rhsPt \ref opt Right side of comparison
 * \return \ref retcmp
 */
typedef int (*mcr_compare_fnc) (const void *lhsPt, const void *rhsPt);

#ifndef MCR_UTIL_PLATFORM_INC
/*! \brief Include to access platform utilities */
#define MCR_UTIL_PLATFORM_INC MCR_STR(mcr/util/MCR_PLATFORM/nutil.h)
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

/* Define string comparison aliases, because Windows has too many options */
#include MCR_STR(mcr/util/MCR_PLATFORM/ndef.h)

#endif
