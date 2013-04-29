
# include "util/mcrstrings.h"

// case implemented on lnx
int stricmp ( const char * lhs, const char * rhs )
{
	return strcasecmp ( lhs, rhs ) ;
}
int strnicmp ( const char * lhs, const char * rhs, size_t count )
{
	return strncasecmp ( lhs, rhs, count ) ;
}
