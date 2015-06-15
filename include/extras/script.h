/* include/extras/script.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef MCR_SCRIPT_H
# define MCR_SCRIPT_H

# include "extras/safestring.h"

typedef struct mcr_Script
{
	mcr_Array scripts ;
} mcr_Script ;

typedef int ( * mcr_option_fnc ) ( int argc, char ** argv,
		int index ) ;

extern mcr_ISignal mcr_iScript ;
extern const char * mcr_helpOptionName ;
extern mcr_option_fnc mcr_defaultOption ;

MCR_API void mcr_Script_init ( void * scriptPt ) ;
MCR_API void mcr_Script_free ( void * scriptPt ) ;
MCR_API void mcr_Script_copy ( void * dstPt, void * srcPt ) ;
MCR_API int mcr_Script_compare ( const void * lhs, const void * rhs ) ;
MCR_API int mcr_Script_send ( mcr_Signal * sigPt ) ;

MCR_API void mcr_Script_option ( int argc, char ** argv ) ;
MCR_API int mcr_Script_do_option ( int argc, char ** argv, int index ) ;
MCR_API void mcr_Script_set_option ( const char * key,
		mcr_option_fnc optFnc ) ;
MCR_API int mcr_Script_short_help ( int argc, char ** argv, int index ) ;
MCR_API void mcr_Script_parse_error ( ) ;

MCR_API void mcr_script_initialize ( ) ;
MCR_API void mcr_script_cleanup ( ) ;

# define MCR_OPTION_MIN_OPTS( argc, argv, index, minOpts ) \
	if ( index + minOpts >= argc ) \
	{ \
		dmsg ; \
		fprintf ( stderr, "Not enough options.\n" ) ; \
		mcr_Script_parse_error ( ) ; \
		return index ; \
	}

# endif
