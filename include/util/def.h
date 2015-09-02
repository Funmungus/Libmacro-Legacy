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

/*! \file util/def.h
 * \brief Utility definitions
 * */

# ifndef MCR_UTIL_DEFINES_H
# define MCR_UTIL_DEFINES_H

# ifdef _MSC_VER
# define _CRT_SECURE_NO_WARNINGS
# endif

# include <stdarg.h>	// Variadics
# include <stdio.h>
# include <string.h>	// c library memcpy/strncpy and concats
# include <stdlib.h>
# include <ctype.h>	// islower/tolower
# include <time.h>
# ifdef DEBUG
	# include <assert.h>
	# include <errno.h>
# endif

/* From
 * http://goodliffe.blogspot.com/2009/07/c-how-to-say-warning-to-visual-studio-c.html
 * Year 2014 */
# define MCR_STR_HELPER( x ) #x
/*! \brief Stringify, or create a string.
 * e.g. MCR_STR ( myString ) => "myString"
 * */
# define MCR_STR( x ) MCR_STR_HELPER ( x )
# define MCR_EXP_HELPER( x ) x
//! \brief Expand macros: e.g. MCR_EXP ( myMacro ) => myMacroExpanded
# define MCR_EXP( x ) MCR_EXP_HELPER ( x )
/* TOKENPASTE found at
 * http://stackoverflow.com/questions/16522341/pseudo-generics-in-c
 * Year 2014 */
# define MCR_TOKEN_HELPER( x, y ) x ## y
# define MCR_SNAKE_HELPER( x, y ) x ## _ ## y
/*! \brief MCR_TOKEN ( MyString, MyString2 ) => MyStringMyString2
  * */
# define MCR_TOKEN( x, y ) MCR_TOKEN_HELPER ( x, y )
/*! \brief MCR_SNAKE ( myString, myString2 ) => myString_myString2
  * */
# define MCR_SNAKE( x, y ) MCR_SNAKE_HELPER ( x, y )

/*! \brief Display warning to the compiler.
  * */
# ifndef MCR_WARN
# define MCR_WARN( desc ) message ( "Warning:" __FILE__ "(" \
		MCR_STR ( __LINE__ ) " ) : " #desc )
# endif
// usage:
//#pragma MCR_WARN ( FIXME: Code removed because... )

# ifndef MCR_NATIVE_DIR
# pragma MCR_WARN ( MCR_NATIVE_DIR is undefined, defaulting to nativeless.\
Native functions will be unusable. )
# define MCR_NATIVE_DIR nativeless
# endif

# ifndef UNUSED
# define UNUSED( x ) ( void ) ( x )
# endif

# ifndef MCR_CLOCKINTEREST
//! \brief The number of clock ticks to print a profiling message
# define MCR_CLOCKINTEREST ( 999 )
# endif

# ifdef DEBUG
# ifndef mcr_dmsg
//! \brief Automated error message based on errno
# define mcr_dmsg \
	fprintf ( mcr_err, "%s: Error %d:" __FILE__ " ( " \
			MCR_STR ( __LINE__ ) " ): %s.\n", \
			mcr_timestring ( ), errno, strerror ( errno ) )
# endif
# ifndef mcr_dassert
//! \brief Assertion only for debug builds
# define mcr_dassert( expression ) assert ( expression )
# endif
# ifndef mcr_dclock
//! \brief Initialize \ref clock_t with \ref clock, only for debug builds.
# define mcr_dclock( assignVar ) \
	assignVar = clock ( )
# endif
# ifndef mcr_profile
/*! \brief If \ref MCR_CLOCKINTEREST clock ticks have passed
 *  since startClock has been initialized, then a message will print
 * to mcr_err */
# define mcr_profile( startClock ) \
	startClock = clock ( ) - startClock ; \
	if ( startClock > MCR_CLOCKINTEREST ) \
	{ fprintf ( mcr_err, "%s: Profile:" __FILE__ " ( " \
				MCR_STR ( __LINE__ ) " ): %li clocks.\n", \
				mcr_timestring ( ), startClock ) ; }
# endif
# else
# ifndef mcr_dmsg
# define dmsg {}
# endif
# ifndef mcr_dassert
# define dassert( expression ) {}
# endif
# ifndef mcr_dclock
# define mcr_dclock( assignVar ) {}
# endif
# ifndef mcr_profile
# define mcr_profile( startClock ) {}
# endif
# endif

# ifndef dmsg
//! \ref mcr_dmsg
# define dmsg mcr_dmsg
# endif
# ifndef dassert
//! \ref mcr_dassert
# define dassert( expr ) mcr_dassert ( expr )
# endif
# ifndef dclock
//! \ref mcr_dclock
# define dclock( assignVar ) mcr_dclock ( assignVar )
# endif
# ifndef profile
//! \ref mcr_profile
# define profie( startClock ) mcr_profile ( startClock )
# endif

/*! \brief
 * Function to call for elements of iteration.
 * */
typedef void ( * mcr_iterate_fnc ) ( void *, va_list ) ;

# include MCR_STR(util/MCR_NATIVE_DIR/def.h)

# ifndef MCR_API
// The result is as if MCR_API is not there if not yet given a value
# define MCR_API
# endif

MCR_API extern FILE * mcr_out ;
MCR_API extern FILE * mcr_in ;
MCR_API extern FILE * mcr_err ;

//! \brief time(NULL) and ctime.
MCR_API const char * mcr_timestring ( ) ;

# endif
