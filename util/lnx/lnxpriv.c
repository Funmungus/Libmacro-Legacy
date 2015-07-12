/* util/lnx/lnxpriv.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

// For seteuid
# ifndef _GNU_SOURCE
# define _GNU_SOURCE
# endif

# include "util/lnx/priv.h"

static int _uid = 1000 ;
void mcr_setuid ( int uid )
{
	if ( uid )
		_uid = uid ;
	else
		dmsg ;
}

int mcr_deactivate_root ( )
{
	// Full user, attempt set lower permission user,
	// do not fail on higher permission user.
	if ( getuid ( ) && geteuid ( ) )
	{
		if ( setuid ( _uid ) == -1 )
		{ dmsg ; }
	}
	// Full root, use _uid in memory, and fail if not able.
	else if ( ! getuid ( ) && ! geteuid ( ) )
	{
		if ( setuid ( _uid ) == -1 )
		{
			dmsg ;
			return 0 ;
		}
	}
	// We have a user, so try to set to that user, fail if not able.
	else if ( setuid ( getuid ( ) ? getuid ( ) : geteuid ( ) ) == -1 )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

int mcr_activate_root ( )
{
	if ( getuid ( ) && geteuid ( ) )
	{
		if ( seteuid ( 0 ) == -1 )
		{
			dmsg ;
			return 0 ;
		}
	}
	else if ( geteuid ( ) )
	{
		if ( setreuid ( geteuid ( ), getuid ( ) ) == -1 )
		{
			dmsg ;
			return 0 ;
		}
	}
	// Either already root, or seteuid success.
	return 1 ;
}

int mcr_activate_user ( )
{
	// Have a user, switch effective user.
	if ( getuid ( ) )
	{
		if ( setreuid ( geteuid ( ), getuid ( ) ) == -1 )
		{
			dmsg ;
			return 0 ;
		}
	}
	// User root, effective user root, set effective to inter memory,
	// fail if not able.
	else if ( ! geteuid ( ) )
	{
		if ( seteuid ( _uid ) == -1 )
		{
			dmsg ;
			return 0 ;
		}
	}
	return 1 ;
}
