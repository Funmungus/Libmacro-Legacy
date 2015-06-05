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

int mcr_deactivate_root ( )
{
	if ( getuid ( ) && geteuid ( ) )
		return 1 ;
	if ( ! getuid ( ) && ! geteuid ( ) )
	{
		dmsg ;
		return 0 ;
	}
//	if ( getuid ( ) )
//		return seteuid ( getuid ( ) ) != -1 ;
	if ( setuid ( getuid ( ) ? getuid ( ) : geteuid ( ) ) == -1 )
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
	// Only set user to non-root.
	if ( getuid ( ) )
	{
		if ( setreuid ( geteuid ( ), getuid ( ) ) == -1 )
		{
			dmsg ;
			return 0 ;
		}
	}
	return 1 ;
}
