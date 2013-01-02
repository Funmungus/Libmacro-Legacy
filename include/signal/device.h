/*! \file device.h
 * \brief Device is a wrapper for creating uinput devices.
 *
 * The source file contains predefined Devices for keyboard, absolute, and relative.
 * TODO Dynamic directory search, or configuration value for uinput node.
 */

# ifndef FUN_DEVICE_H
# define FUN_DEVICE_H

# ifdef LNX

# include "signal/isignal.h"

namespace funlibrary
{
	class KeyDevice ;
	class AbsDevice ;
	class RelDevice ;

	//! \brief Public KeyDevice instance to send KeySignals.
	extern KeyDevice KeyDev ;
	//! \brief Public AbsDevice instance to send SpaceSignals unjustified.
	extern AbsDevice AbsDev ;
	//! \brief Public RelDevice instance to send SpaceSignals justified.
	extern RelDevice RelDev ;

	//! \brief Wrapper for uinput devices.
	class Device
	{
	public :
		//! \brief ctor. type_value_map is used to set the input bits.
		SIGNAL_API Device ( const std::map < int, std::vector < int > > & type_value_map
												= std::map < int, std::vector < int > > ( ),
												const uinput_user_dev * device = NULL, const bool enable = false ) ;
		//! \brief dtor
		SIGNAL_API ~Device ( ) ;

		//! \brief Call this if you do not know what's going on.
		static SIGNAL_API void usage ( ) ;

		//! \brief Get the current uinput_user_dev device.
		SIGNAL_API uinput_user_dev getDev ( ) const ;
		//! \brief Copy values from device as the new current uinput_user_dev device.
		SIGNAL_API void setDev ( const uinput_user_dev * device ) ;

		//! \brief Get the set of all input bits to be set.
		SIGNAL_API std::map < int, std::vector < int > > getInputBits ( ) const ;
		//! \brief Set input bit values for a single bit type.
		SIGNAL_API void setInputBits ( const int bitType, const int * bits, const unsigned int bit_len ) ;
		//! \brief Set input bit values from a set of [bit type, values] pairs.
		SIGNAL_API void setInputBits ( const std::map < int, std::vector < int > > & type_value_map
																	 = std::map < int, std::vector < int > > ( ) ) ;

		//! \brief If enabled, this device should be able to send input_events for the input bits provided.
		SIGNAL_API bool isEnabled ( ) const ;
		//! \brief If all other values are set correctly, this device can be enabled.
		SIGNAL_API bool enable ( const bool enable ) ;

		//! \brief Get the file descriptor associated with the device.
		SIGNAL_API int getFd ( ) const ;
		//! \brief Get the file descriptor associated with the event node in /dev/input.
		SIGNAL_API int getEventFd ( ) const ;

		//! \brief Writes the given eventObj to device's fd.
		SIGNAL_API bool send ( const input_event * eventObj ) const ;
		/*! \brief Writes the given set of event objects to this device's fd.
		 * Size is the byte size of eventObjs.
		 */
		SIGNAL_API bool send ( const input_event * eventObjs, const unsigned int size ) const ;
		//! \brief Send a syncing event to this device.
		bool inline sync ( ) const
		{
			return send ( & _syncer ) ;
		}
	private :
		FileSafety _fs ;
		FileSafety _eventFs ;
		//TODO performance may be increased with unique_ptr < vector < int > >
		std::map < int, std::vector < int > > _type_value_map ;
		uinput_user_dev _device ;
		bool _enable ;

		bool open () ;
		bool setBits ( const int bitType, const std::vector<int> & bits ) ;
		bool create () ;
		void setEventFile ( ) ;

		static const input_event _syncer ;
		static std::mutex _enableLock ;
		static input_event initSync ( ) ;
	} ;

	//! \brief Sends events for keyboard.
	class KeyDevice : public Device
	{
	public :
		SIGNAL_API KeyDevice ( const char * name = NULL, const bool enable = true ) ;
	private :
		void prepBits ( ) ;
	} ;

	//! \brief Sends events for absolute value coordinates.
	class AbsDevice : public Device
	{
	public :
		SIGNAL_API AbsDevice ( const char * name = NULL, const bool enable = true ) ;
	private :
		void prepBits ( ) ;
	} ;

	//! \brief Sends events for cursor-justified coordinates. (vector-offset)
	class RelDevice : public Device
	{
	public :
		SIGNAL_API RelDevice ( const char * name = NULL, const bool enable = true ) ;
	private :
		void prepBits ( ) ;
	} ;
}
# endif

# endif

