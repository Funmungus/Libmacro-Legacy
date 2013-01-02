/*! \file util.h
 * \brief Utilities for funlibrary.
 */

# ifndef FUN_UTIL_H
# define FUN_UTIL_H


//# include "util/event.h"
# include "util/xml.h"
# include "util/serial.h"

/*! \namespace funlibrary
 * \brief funlibrary is created by Funmungus.  License to be decided.
 *
 * funlibrary was originally created for Macropus.  Macropus was
 * originally written purely in C#, but has been rewritten with this
 * C++ library to be portable.  The Macropus front-end may be any
 * language as a daemon, service, or user interface that uses this
 * library.
 */
namespace funlibrary
{

	
	
	


	/*! \brief Contains a file descriptor which is closed upon object destruction.
	 *
	 * The file descriptor is publicly accessible, and unmanaged until object destruction.
	 */
	struct FileSafety
	{
	public :
# ifdef WIN
		/*! \brief The object file descriptor.
		 *
		 * An empty descriptor is NULL.
		 */
		FILE * fd ;

		/*! \brief ctor */
		FileSafety ( FILE * fd = NULL )
		{
			this->fd = fd ;
		}

		/*! \brief Dereferences fd from copytron.
		 *
		 * There is no copy constructor, only this move constructor.
		 */
		FileSafety ( FileSafety && copytron )
			: fd ( NULL )
		{
			fd = copytron.fd ;
			copytron.fd = NULL ;
		}

		/*! \brief fclose fd, then NULLify it */
		void close ( )
		{
			if ( fd != NULL )
			{
				fclose ( fd ) ;
				fd = NULL ;
			}
		}
# endif
# ifdef LNX
		/*! \brief The object file descriptor.
		 *
		 * An empty descriptor is -1.
		 */
		int fd ;

		/*! \brief ctor */
		FileSafety ( int fd = -1 )
		{
			this->fd = fd ;
		}

		/*! \brief Dereferences fd from copytron.
		 *
		 * There is no copy constructor, only this move constructor.
		 */
		FileSafety ( FileSafety && copytron )
			: fd ( -1 )
		{
			fd = copytron.fd ;
			copytron.fd = -1 ;
		}

		/*! \brief Dereferences fd from copytron.
		 *
		 * There is no copy assignment, only this move assignment.
		 */
		inline FileSafety & operator= ( FileSafety && other)
		{
			if ( this != &other )
			{
				if ( fd != other.fd )
					this->close ( ) ;
				fd = other.fd ;
				other.fd = -1 ;
			}

			return *this ;
		}

		/*! \brief ::close fd, then NULLify it */
		void close ( )
		{
			if ( fd != -1 )
			{
				::close ( fd ) ;
				fd = -1 ;
			}
		}
# endif
		/*! \brief dtor, fd will be closed. */
		~FileSafety ( )
		{
			this->close ( ) ;
		}

	};

}

# endif

