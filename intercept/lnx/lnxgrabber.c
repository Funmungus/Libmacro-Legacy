/* intercept/lnx/lnxgrabber.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "intercept/lnx/grabber.h"

void mcr_Grabber_init ( mcr_Grabber * grabPt )
{
	if ( ! grabPt )
	{
		dmsg ;
		return ;
	}
	memset ( grabPt, 0, sizeof ( mcr_Grabber ) ) ;
	grabPt->fd = -1 ;
	mcr_String_init ( & grabPt->path ) ;
}

void mcr_Grabber_init_with ( mcr_Grabber * grabPt,
		const char * path, int enable )
{
	if ( ! grabPt )
	{
		dmsg ;
		return ;
	}
	mcr_Grabber_init ( grabPt ) ;
	if ( path )
	{
		mcr_String_from_string ( & grabPt->path, path ) ;
		mcr_Grabber_enable ( grabPt, enable ) ;
	}
}

void mcr_Grabber_free ( mcr_Grabber * grabPt )
{
	if ( ! grabPt )
	{
		dmsg ;
		return ;
	}
	if ( grabPt->fd != -1 )
	{
		ioctl ( grabPt->fd, EVIOCGRAB, 0 ) ;
		close ( grabPt->fd ) ;
	}
	mcr_Array_free ( & grabPt->path ) ;
}

const char * mcr_Grabber_get_path ( mcr_Grabber * grabPt )
{
	dassert ( grabPt ) ;
	if ( MCR_STR_ISEMPTY ( & grabPt->path ) )
		return NULL ;
	return grabPt->path.array ;
}

void mcr_Grabber_set_path ( mcr_Grabber * grabPt,
		const char * path )
{
	int wasEnabled = mcr_Grabber_is_enabled ( grabPt ) ;
	if ( wasEnabled )
	{
		mcr_Grabber_enable ( grabPt, 0 ) ;
	}
	mcr_String_from_string ( & grabPt->path, path ) ;
	if ( wasEnabled )
	{
		mcr_Grabber_enable ( grabPt, 1 ) ;
	}
}

int mcr_Grabber_is_enabled ( mcr_Grabber * grabPt )
{
	return MCR_GRABBER_ENABLED ( grabPt ) &&
			! MCR_STR_ISEMPTY ( & grabPt->path ) ;
}

int mcr_Grabber_enable ( mcr_Grabber * grabPt, int enable )
{
	if ( ! mcr_activate_root ( ) )
	{
		dmsg ;
		return 0 ;
	}
	int result ;
	if ( grabPt->fd != -1 )
	{
		// Valgrind false positive error.
		if ( ioctl ( grabPt->fd, EVIOCGRAB, 0 ) == -1 )
			dmsg ;
		if ( close ( grabPt->fd ) == -1 )
			dmsg ;
		grabPt->fd = -1 ;
		// We are now no longer grabbing
	}

	if ( ! enable )
	{
		mcr_activate_user ( ) ;
		return 1 ;
	}

	dassert ( ! MCR_STR_ISEMPTY ( & grabPt->path ) ) ;

	grabPt->fd = open ( grabPt->path.array,
			O_RDONLY /*| O_NONBLOCK*/ ) ;
	if ( grabPt->fd == -1 )
	{
		dmsg ;
		mcr_activate_user ( ) ;
		return 0 ;
	}

	result = ioctl ( grabPt->fd, EVIOCGRAB, 1 ) ;
	if ( result < 0 )
	{
		dmsg ;
		close ( grabPt->fd ) ;
		grabPt->fd = -1 ;
		mcr_activate_user ( ) ;
		return 0 ;
	}

	mcr_NoOp separator ;
	separator.tv_sec = 0 ;
	separator.tv_nsec = 1000 /* 1 micro */ * 1000 /* 1 milli */
			* 10 /* 10 milli */ ;
	MCR_NOOP_QUICKSEND ( & separator ) ;

	// We are now grabbing exclusively.
	if ( ! mcr_activate_user ( ) )
		dmsg ;
	return 1 ;
}

void mcr_Grabber_state ( mcr_Grabber * grabPt,
		char * buffer, const unsigned int size )
{
	dassert ( grabPt ) ;
	dassert ( buffer ) ;

	int fd = grabPt->fd ;
	if ( fd == -1 && ! MCR_STR_ISEMPTY ( & grabPt->path ) )
	{
		if ( ! mcr_activate_root ( ) )
		{
			dmsg ;
			return ;
		}
		fd = open ( grabPt->path.array, O_RDONLY | O_NONBLOCK ) ;
	}
	if ( fd == -1 )
	{
		dmsg ;
		mcr_activate_user ( ) ;
		return ;
	}

	if ( ioctl ( fd, EVIOCGKEY ( size ), buffer ) < 0 )
	{
		dmsg ;
	}

	// If grabber was not enabled, we need to return it back.
	if ( grabPt->fd == -1 )
	{
		close ( fd ) ;
		mcr_activate_user ( ) ;
	}
}
