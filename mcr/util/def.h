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
 * \brief Utility definitions
 */

#ifndef MCR_UTIL_DEF_H
#define MCR_UTIL_DEF_H

/* Needed for some functions on linux */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <time.h>
#include <stdbool.h>
#ifdef DEBUG
#include <assert.h>
#endif

/* Doxygen helpers */
/*!
 * \brief Return 0 for success, otherwise standard error code
 * from \c errno.h.
 *
 * May also be a negative standard error code.
 */
#define reterr int
/*!
 * \brief Return size_t index, or -1 on error.
 */
#define retind size_t
/*!
 * \brief Return size_t id, or -1 on error.
 */
#define retid size_t
/*!
 * \brief Return 0 if comparison is equal, less than 0 if left < right,
 * or greater than 0 if left > right.
 */
#define retcmp int

/*
 * From
 * http://goodliffe.blogspot.com/2009/07/c-how-to-say-warning-to-visual-studio-c.html
 * (2014)
 */
#define MCR_STR_HELPER(x) #x
/*!
 * \brief Stringify, or create a string.
 * e.g. MCR_STR(myString) => "myString"
 */
#define MCR_STR(x) MCR_STR_HELPER(x)
#define MCR_EXP_HELPER(x) x
/*! \brief Expand macros: e.g. MCR_EXP(myMacro) => myMacroExpanded */
#define MCR_EXP(x) MCR_EXP_HELPER(x)
/*!
 * \brief Expand the first argument as an argument, and the second
 * argument as a function
 *
 * Use with \ref MCR_EXP
 */
#define MCR_ARGSWAP(itPt, functionName) functionName(itPt)
/*!
 * \brief Expand the variadic arguments, ignoring the first
 *
 * Use with \ref MCR_EXP
 */
#define MCR_IGNORE_ARG(itPt, ...) __VA_ARGS__
/*
 * Token pasting found at
 * http://stackoverflow.com/questions/16522341/pseudo-generics-in-c
 * (2014) */
#define MCR_TOKEN_HELPER(x, y) x ##y
#define MCR_SNAKE_HELPER(x, y) x ##_ ##y
/*!
 * \brief MCR_TOKEN(MyString, MyString2) => MyStringMyString2
 */
#define MCR_TOKEN(x, y) MCR_TOKEN_HELPER(x, y)
/*!
 * \brief MCR_SNAKE(myString, myString2) => myString_myString2
 */
#define MCR_SNAKE(x, y) MCR_SNAKE_HELPER(x, y)

#ifndef MCR_WARN
/*!
 * \brief Display warning to the compiler.
 *
 * "file(line) : Warning Msg: <description>"
 * usage: \code{.c}
 * #pragma message MCR_WARN("message")
 * \endcode
 * \param description String which is added to the warning message
 */
#define MCR_WARN(description) __FILE__ "(" MCR_STR(__LINE__) ") : " \
	"Warning Msg: " description
#endif

#ifndef MCR_NATIVE_DIR
#define MCR_NATIVE_DIR nativeless
#ifndef MCR_FWARN
#define MCR_FWARN MCR_STR(mcr/util/MCR_NATIVE_DIR/nwarn.h)
#endif
#include MCR_FWARN
#pragma message MCR_WARN("MCR_NATIVE_DIR is undefined, defaulting to " \
MCR_STR(MCR_NATIVE_DIR)". Native functions will be unusable.")
#else
#ifndef MCR_FWARN
/*!
 * \brief Native file which raises a warning to the compiler.
 *
 * Message is "Warning message below:"\n
 * In linux this line will raise the warning.  In windows the message itself
 * will raise the warning.\n
 * usage: \code{.c}
 * #include MCR_FWARN
 * #pragma message MCR_WARN("description")
 * \endcode
 */
#define MCR_FWARN MCR_STR(mcr/util/MCR_NATIVE_DIR/nwarn.h)
#endif
#endif

#ifndef UNUSED
/*! \brief Remove compiler warning for unused variable. */
#define UNUSED(x) (void)(x)
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

/*
 * Debugging definitions
 */
#ifdef DEBUG
#ifndef mcr_ddo
/*! \brief Do the argument only for debug builds. */
#define mcr_ddo(stuff) stuff
#endif
#else
#define mcr_ddo(stuff)
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
 * Print to \c stderr for only debug builds. */
#define mcr_dmsg \
mcr_ddo(fprintf(stderr, "Error %d: " __FILE__ "(" \
		MCR_STR(__LINE__) "), %s: %s.\n", \
		mcr_error(), mcr_timestamp(), strerror(mcr_error())))
#endif
#ifndef mcr_dassert
/*! \brief Assertion only for debug builds */
#define mcr_dassert(expression) mcr_ddo(assert(expression))
#endif
#ifndef mcr_fixme
#define mcr_fixme \
mcr_ddo(_Pragma("message \"Fix Me!\""))
#endif
#ifndef mcr_arrlen
/*!
 * \brief For a static array definition get the number of elements.
 *
 * \param arr Static array
 * \return size_t Number of elements */
#define mcr_arrlen(arr) \
(sizeof(arr) / sizeof(*(arr)))
#endif
#ifndef mcr_mset_error
/*!
 * \brief \ref mcr_set_error and \ref mcr_dmsg
 */
#define mcr_mset_error(errorNumber) \
mcr_set_error(errorNumber); \
mcr_dmsg;
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

/*! \brief Function pointer with no arguments */
typedef void (*mcr_fnc) ();
/*! \brief Function on a data object. */
typedef void (*mcr_data_fnc) (void *dataPt);
/*!
 * \brief Copy from another object
 *
 * \param dst Destination object
 * \param src Source object
 * \return \ref reterr
 */
typedef int (*mcr_copy_fnc) (void *destinaitonPt, void *sourcePt);
/*!
 * \brief Comparison function of two objects
 *
 * \return \ref retcmp
 */
typedef int (*mcr_compare_fnc) (const void *lhsPt, const void *rhsPt);

/* Used throughout in library functions */
struct mcr_context;

#ifndef MCR_UTIL_NATIVE_INC
/*! \brief Include to access native utilities */
#define MCR_UTIL_NATIVE_INC MCR_STR(mcr/util/MCR_NATIVE_DIR/nutil.h)
#endif

/* Define MCR_API and strcasecmp/stricmp */
#include MCR_STR(mcr/util/MCR_NATIVE_DIR/ndef.h)
/* If no MCR_API, default to do nothing */
#ifndef MCR_API
#define MCR_API
#endif
#endif
