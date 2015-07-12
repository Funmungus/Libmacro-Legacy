/* include/util/def.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file util/def.h
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
# include <ctype.h>		// islower/tolower
# include <time.h>
# ifdef DEBUG
	# include <assert.h>
	# include <errno.h>
# endif

/* From
 * http://goodliffe.blogspot.com/2009/07/c-how-to-say-warning-to-visual-studio-c.html
 * Year 2014 */
# define STRINGIFY_HELPER( x ) #x
# define STRINGIFY( x ) STRINGIFY_HELPER ( x )
/* TOKENPASTE found at
 * http://stackoverflow.com/questions/16522341/pseudo-generics-in-c
 * Year 2014 */
# define TOKENPASTE_HELPER( x, y ) x ## y
# define SNAKEPASTE_HELPER( x, y ) x ## _ ## y
# define TOKENPASTE( x, y ) TOKENPASTE_HELPER ( x, y )
# define SNAKEPASTE( x, y ) SNAKEPASTE_HELPER ( x, y )

# ifndef WARNING
# define WARNING( desc ) message ( "warning:" __FILE__ "(" \
		STRINGIFY ( __LINE__ ) " ) : " #desc )
# endif
// usage:
//#pragma WARNING ( FIXME: Code removed because... )

# ifndef MCR_NATIVE_DIR
# pragma WARNING(MCR_NATIVE_DIR is undefined, defaulting to nativeless.\
Native functions will be unusable. )
# define MCR_NATIVE_DIR nativeless
# endif

# ifndef UNUSED
# define UNUSED( x ) ( void ) ( x )
# endif

# define MCR_CLOCKINTEREST 0x0200

# ifdef DEBUG
# define dmsg \
	fprintf ( mcr_stderr, "%s: Error %d:" __FILE__ " ( " \
			STRINGIFY ( __LINE__ ) \
			" ): %s.\n", mcr_timestring ( ), errno, strerror ( errno ) )
# define dassert( expression ) assert ( expression )
# define mcr_dclock( assignVar ) \
	assignVar = clock ( )
# define mcr_profile( startClock ) \
	startClock = clock ( ) - startClock ; \
	if ( startClock > MCR_CLOCKINTEREST ) \
	{ fprintf ( mcr_stderr, "%s: Profile:" __FILE__ " ( " \
				STRINGIFY ( __LINE__ ) \
				" ): %li clocks.\n", mcr_timestring ( ), startClock ) ; }
# else
# ifndef dmsg
# define dmsg {}
# endif
# ifndef dassert
# define dassert( expression ) {}
# endif
# ifndef mcr_dclock
# define mcr_dclock( assignVar ) {}
# endif
# ifndef mcr_profile
# define mcr_profile( startClock ) {}
# endif
# endif

/*! \brief
 * Function to call for elements of iteration.
 * */
typedef void ( * mcr_iterate_fnc ) ( void *, va_list ) ;

# include STRINGIFY(util/MCR_NATIVE_DIR/def.h)

# ifndef MCR_API
// The result is as if MCR_API is not there if not yet given a value
# define MCR_API
# endif

MCR_API extern FILE * mcr_stdout ;
MCR_API extern FILE * mcr_stdin ;
MCR_API extern FILE * mcr_stderr ;

//! \brief time(NULL) and ctime.
MCR_API const char * mcr_timestring ( ) ;

# endif
