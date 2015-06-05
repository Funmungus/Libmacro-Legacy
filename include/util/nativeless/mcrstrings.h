/* include/util/nativeless/mcrstrings.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "util/nativeless/def.h"

// case functions implemented in util/nativeless/mcrstrings.c
MCR_API int strcasecmp ( const char * s1, const char * s2 ) ;
MCR_API int strncasecmp ( const char * s1, const char * s2, size_t n ) ;
MCR_API int stricmp ( const char * lhs, const char * rhs ) ;
MCR_API int strnicmp ( const char * lhs, const char * rhs, size_t count ) ;
