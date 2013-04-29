
# include "util/mcrstrings.h"

// stri implemented on win
int strcasecmp ( const char * s1, const char * s2 )
{
	return _stricmp ( s1, s2 ) ;
}
int strncasecmp ( const char * s1, const char * s2, size_t n )
{
	return _strnicmp ( s1, s2, n ) ;
}
