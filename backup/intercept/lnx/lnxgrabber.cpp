
# include "hotkey/lnx/grabber.h"

namespace funlibrary
{
	const int Grabber::oneEventSize = sizeof ( input_event ) ;
	const unsigned int Grabber::readSize = oneEventSize * FUN_GRAB_SET_LENGTH ;

	Grabber::Grabber ( string path, bool beginEnabled )
		: path ( path ), fd_accessor ( -1 ), enable_accessor ( false ), _fs ( -1 ),
			_enable ( false )
	{
		enable ( beginEnabled ) ;
	}
	
	Grabber::Grabber ( Grabber && copytron )
		: path ( copytron.path ),
			fd_accessor ( copytron._fs.fd ), enable_accessor ( copytron._enable ),
			_fs ( -1 ), _enable ( copytron._enable )
	{	
		copytron._enable = copytron.enable_accessor = false ;
		_fs = std::move ( copytron._fs ) ;
		copytron.fd_accessor = -1 ;
	}
	
	Grabber & Grabber::operator= ( Grabber && copytron )
	{
		if ( & copytron != this )
		{
			path = copytron.path ;
			enable_accessor = _enable = copytron._enable ;
			_fs = move ( copytron._fs ) ;
			fd_accessor = _fs.fd ;
		}
		return * this ;
	}

	Grabber::~Grabber ( )
	{
		if ( _fs.fd != -1 )
		{
			ioctl ( _fs.fd, EVIOCGRAB, 0 ) ;
			_fs.close ( ) ;
		}
	}

	bool Grabber::enable ( bool enable )
	{
		int result ;
		if ( _fs.fd != -1 )
		{
			result = ioctl ( _fs.fd, EVIOCGRAB, 0 ) ;
			_fs.close ( ) ;
			// We are now no longer grabbing
		}

		_enable = false ;
		// _fs already -1
		enable_accessor = false ;
		fd_accessor = -1 ;

		if ( ! enable )
			return true ;

		if ( path.empty ( ) )
		{
			fprintf ( stderr, "Grabber : No path to grab.\n" ) ;
			return false ;
		}

		_fs.fd = open ( path.c_str ( ), O_RDONLY ) ;
		if ( _fs.fd == -1 )
		{
			fprintf ( stderr, "Grabber open : %s\n", strerror ( errno ) ) ;
_fs.fd = -1 ;
			return false ;
}

		result = ioctl ( _fs.fd, EVIOCGRAB, 1 ) ;
		if ( result < 0 )
		{
			fprintf ( stderr, "Grabber EVIOCGRAB, path '%s', fd '%d': %s\n",
					path.c_str ( ), _fs.fd, strerror ( errno ) ) ;
_fs.close ( ) ;
			return false ;
}

static NoOp separator ( 10 ) ;
separator.send ( ) ;

		fd_accessor = _fs.fd ;
		enable_accessor = true ;
_enable = true ;
		// We are now grabbing exclusively.
		return _enable ;
	}

int Grabber::read ( )
	{
		if ( _fs.fd == -1 ) return -1 ;
//			fprintf ( stderr, "Grabber read : Grabbing device is not enabled.\n" ) ;

		return ::read ( _fs.fd, grabSet, readSize ) ;

		/*if ( ret < oneEventSize )
			fprintf ( stderr, "Grabber read : %s\n", strerror ( errno ) ) ;

		return ret ; */
	}

// TODO config file of extra event paths to read mods from.
	void Grabber::keyState ( unsigned char * buffer, const unsigned int size ) const
	{
		if ( buffer == NULL )
			return ;

		int fd = _fs.fd ;
		if ( fd == -1 && ! path.empty ( ) )
			fd = open ( path.c_str ( ), O_RDONLY ) ;
		if ( fd == -1 )
		{
			fprintf ( stderr, "Grabber open '%s': %s.\n", path.c_str ( ), strerror ( errno ) ) ;
			return ;
		}

		if ( ioctl ( fd, EVIOCGKEY ( size ), buffer ) < 0 )
			fprintf ( stderr, "Grabber EVIOCGKEY, path %s, fd %d : %s.\n", path.c_str ( ), fd,
					strerror ( errno ) ) ;

		// If grabber was not enabled, we need to return it back.
		if ( _fs.fd == -1 )
			close ( fd ) ;
	}
}
