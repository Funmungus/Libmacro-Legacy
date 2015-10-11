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

// For seteuid
# ifndef _GNU_SOURCE
# define _GNU_SOURCE
# endif

# include "util/lnx/priv.h"
# include "util/priv.h"

static int _uid = 1000 ;
void mcr_setuid ( int uid )
{
	if ( uid )
		_uid = uid ;
	else
		dmsg ;
}

int mcr_privilege_deactivate ( )
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

static int privswap ( )
{
	if ( setreuid ( geteuid ( ), getuid ( ) ) == -1 )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

int mcr_privileged ( )
{
	return ! geteuid ( ) ;
}

int mcr_set_privileged ( int enable )
{
	if ( enable )
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
		{ return privswap ( ) ; }
	}
	else
	{
		// Have a user, switch effective user.
		if ( getuid ( ) )
		{ return privswap ( ) ; }
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
	}
	return 1 ;
}
