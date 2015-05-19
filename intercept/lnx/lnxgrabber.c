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
		return ;
	memset ( grabPt, 0, sizeof ( mcr_Grabber ) ) ;
	grabPt->fd = -1 ;
	mcr_Array_init ( & grabPt->path, sizeof ( char ) ) ;
}

void mcr_Grabber_init_with ( mcr_Grabber * grabPt,
		const char * path, int enable )
{
	if ( ! grabPt )
		return ;
	mcr_Grabber_init ( grabPt ) ;
	if ( path )
	{
		mcr_String_from_string ( & grabPt->path, path,
				( size_t ) -1 ) ;
		mcr_Grabber_enable ( grabPt, enable ) ;
	}
}

void mcr_Grabber_free ( mcr_Grabber * grabPt )
{
	if ( ! grabPt )
		return ;
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
	return ( const char * ) grabPt->path.array ;
}

void mcr_Grabber_set_path ( mcr_Grabber * grabPt,
		const char * path )
{
	int wasEnabled = mcr_Grabber_is_enabled ( grabPt ) ;
	if ( wasEnabled )
	{
		mcr_Grabber_enable ( grabPt, 0 ) ;
	}
	mcr_String_from_string ( & grabPt->path, path,
			( size_t ) -1 ) ;
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
	int result ;
	if ( grabPt->fd != -1 )
	{
		// Valgrind false positive error.
		result = ioctl ( grabPt->fd, EVIOCGRAB, 0 ) ;
		close ( grabPt->fd ) ;
		grabPt->fd = -1 ;
		// We are now no longer grabbing
	}

	if ( ! enable )
		return 1 ;

	dassert ( ! MCR_STR_ISEMPTY ( & grabPt->path ) ) ;

	grabPt->fd = open ( ( const char * ) grabPt->path.array, O_RDONLY ) ;
	if ( grabPt->fd == -1 )
	{
		dmsg ( "Grabber open : %s.\n", strerror ( errno ) ) ;
		grabPt->fd = -1 ;
		return 0 ;
	}

	result = ioctl ( grabPt->fd, EVIOCGRAB, 1 ) ;
	if ( result < 0 )
	{
		dmsg ( "Grabber EVIOCGRAB, path '%s', fd '%d': %s.\n",
				grabPt->path.array, grabPt->fd,
				strerror ( errno ) ) ;
		close ( grabPt->fd ) ;
		grabPt->fd = -1 ;
		return 0 ;
	}

	mcr_NoOp separator ;
	separator.tv_sec = 0 ;
	separator.tv_nsec = 1000 /* 1 micro */ * 1000 /* 1 milli */
			* 10 /* 10 milli */ ;
	MCR_NOOP_SEND ( & separator, result ) ;

	// We are now grabbing exclusively.
	return 1 ;
}

void mcr_Grabber_state ( mcr_Grabber * grabPt,
		unsigned char * buffer, const unsigned int size )
{
	dassert ( grabPt ) ;
	dassert ( buffer ) ;

	int fd = grabPt->fd ;
	if ( fd == -1 && ! MCR_STR_ISEMPTY ( & grabPt->path ) )
		fd = open ( ( const char * ) grabPt->path.array, O_RDONLY ) ;
	if ( fd == -1 )
	{
		dmsg ( "Grabber open '%s': %s.\n", grabPt->path.array,
				strerror ( errno ) ) ;
		return ;
	}

	if ( ioctl ( fd, EVIOCGKEY ( size ), buffer ) < 0 )
	{
		dmsg ( "Grabber EVIOCGKEY, path %s, fd %d : %s.\n",
				grabPt->path.array, fd, strerror ( errno ) ) ;
	}

	// If grabber was not enabled, we need to return it back.
	if ( grabPt->fd == -1 )
		close ( fd ) ;
}
