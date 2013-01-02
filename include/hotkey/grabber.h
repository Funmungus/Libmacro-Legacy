/*! \file grabber.h
 * \brief Grabber, Take exclusive access to a /dev/input event.
 */

# ifndef FUN_AGRABBER_H
# define FUN_AGRABBER_H

# include "hotkey/def.h"

# ifdef LNX

# ifndef FUN_GRAB_SET_LENGTH
# define FUN_GRAB_SET_LENGTH 16
# endif

namespace funlibrary
{
	//! \brief Take exclusive access to a /dev/input event.
	struct Grabber	{
	public :
		//! \brief ctor, path is the /dev/input event path.
		Grabber ( std::string path = "", bool beginEnabled = false ) ;
		//! \brief Move ctor, copy not available.
		Grabber ( Grabber && copytron ) ;
		//! \brief Move assignment, copy not available.
		Grabber & operator= ( Grabber && copytron ) ;
		//! \brief dtor
		~Grabber ( ) ;
		//! \brief The constant size of one input_event
		static const int oneEventSize ;
		//! \brief The byte size of grabSet, and also the size read from device.
		static const unsigned int readSize ;
		//! \brief The path to grab from.
		std::string path ;
		
		//! \brief The set of input_events from the last read operation.
		input_event grabSet [ FUN_GRAB_SET_LENGTH ] ;
		/*! \brief Quick-access variable for the /dev/input event file descriptor.
		 * 
		 * Not used by this struct in any function, and only modified in enable.
		 * Changing this value will do nothing in this object,
		 * but will very likely disable anything that uses this object.
		 */
		int fd_accessor ;
		/*! \brief Quick-access variable for enabled state of this Grabber.
		 * 
		 * Not used by this struct in any function, and only modified in enable.
		 * Changing this value will do nothing in this object,
		 * but will very likely disable anything that uses this object.
		 */
		bool enable_accessor ;

		//! \brief Allow or disallow this object to function.
		bool enable ( bool enable ) ;
		/*! \brief Sets grabSet with the values of the next read operation
		 * to the /dev/input device.
		 */
		int read ( ) ;
		//! \brief Obtain the current key state of this device.  size is the bytesize of buffer.
		void keyState ( uint8_t * buffer, const unsigned int size ) const ;
	private :
		FileSafety _fs ;
		bool _enable ;
	} ;
}

# endif

# endif

