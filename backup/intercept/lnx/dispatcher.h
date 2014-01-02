
# include "hotkey/native/dispatcher.h"
# include "hotkey/lnx/grabber.h"

namespace macro
{
	/*! \brief To find the index of a key code in a /dev/input device key state.
	 *
	 * \return The index that key code exists in an array. Please also find
	 * the bit in that item for this key code.
	 */
	unsigned int eventState_index ( const int keyValue ) ;
	/*! \brief To find the bit number of a key code in a /dev/input device key state.
	 *
	 * \return The bit of an item for a key code within one byte.
	 * If this bit is on, the key is pressed.
	 * If this bit is off, the key is released.
	 */
	unsigned char eventState_bit ( const int keyValue ) ;

	/*! \brief Each path opens a grabber, each grabber runs on its own thread, and each thread must be lock-protected
	 * \pre paths should be unique before putting into GrabThread
	 **/
	struct GrabThread
	{
		std::string path ;
		std::shared_ptr < Grabber > grabber ;
		std::thread trd ;
		std::shared_ptr < std::mutex > lock ;
	} ;

	struct NativeDispatcher
	{
		std::vector < GrabThread > threads ;
		std::mutex enableLock ;
		std::mutex initLock ;
		NativeDispatcher ( ) ;
		~NativeDispatcher ( ) ;
	} ;

	void threadIntercepting ( std::shared_ptr < Grabber > grabbi, std::shared_ptr < std::mutex > mutti ) ;

	// ! ctor has no arguments
	struct NativeDispatcher ;
	// ! \brief Enable the native dispatcher.
	void enable ( NativeDispatcher &, bool enable ) ;
		// ! \brief True if intercepting events natively.
	bool isEnabled ( NativeDispatcher & ) ;
		// ! \brief Destroy resources and restarts intercepting if enabled.
	void reset ( NativeDispatcher & ) ;

	inline unsigned int eventState_index ( const int keyValue )
{
return keyValue / 8 ;
}

	inline unsigned char eventState_bit ( const int keyValue )
	{
		return 1 << keyValue % 8 ;
	}
}
