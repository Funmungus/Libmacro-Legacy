/*! \file dispatcher.h
 * \brief Dispatcher, Interpret hardware signals and dispatch them.
 */

# ifndef FUN_DISPATCHER_H
# define FUN_DISPATCHER_H

# ifdef LNX
# include "hotkey/grabber.h"
# endif
# ifdef WIN
# include "hotkey/hook.h"
# endif

namespace funlibrary
{

# ifdef LNX
	/*! \brief To find the index of a key code in a /dev/input device key state.
	 *
	 * \return The indux that key code exists in.  Please also find the bit in that item
	 * for this key code.
	 */
	unsigned int eventState_index ( const int keyValue ) ;
	/*! \brief To find the bit number of a key code in a /dev/input device key state.
	 *
	 * \return The bit of an item for a key code.  If this bit is on, the key is pressed.
	 * If this bit is off, the key is released.
	 */
	uint8_t eventState_bit ( const int keyValue ) ;

	//! \brief \ref Dispatcher::setGrabEvents from values found in the grab_events file.
	void setGrabFromFile ( ) ;
# endif
	/*! \brief Interprets values from \ref Grabber or \ref Hook, and sends them to given callbacks for that type.
	 *
	 * If the callback returns true, then the signal is dropped and has no more effect.<br>
	 * If the callback returns false, then the signal is sent with Device,
	 * to be passed on to the operating system.
	 */
	class Dispatcher
	{
	public:
# ifdef LNX
		/*! \brief Create Grabbers from events given from the strings in paths.
		 *
		 * This will always reset Dispatcher.
		 */
		static void setGrabEvents ( std::vector < std::string > paths ) ;
# endif
		//! \brief Return the real-time modifiers of known subsystems.
		static unsigned int getMods ( ) ;

		//! \brief Enabling or disabling stops grabbing, but does not change callbacks.
		static void enable ( bool enable ) ;
		//! \brief True if grabbing events.
		static bool isEnabled ( ) ;
		//! \brief Destroy threads and Grabbers, and restarts them for previous event paths.
		static void reset ( ) ;

		//! \brief Set callback for key signals.  May activate grabbing if already enabled.
		static void setKeyCallback ( const KeyCallback newCallback ) ;
		//! \brief Set callback for space signals.  May activate grabbing if already enabled.
		static void setSpaceCallback ( const SpaceCallback newCallback ) ;

		static void release ( )	//!< \brief TODO decide if also releasing grabdata
			{ _keyback = NULL ; _spaceback = NULL ; enable ( false ) ; }

	protected:
	private:
		// Only needed for destruction and releasing threads.
		static Dispatcher _instance ;
		Dispatcher ( ) ;
		~Dispatcher ( ) ;
		/*
		~Dispatcher ( ) ;
		Dispatcher ( const Dispatcher & copytron ) = default ;
		Dispatcher & operator= ( const Dispatcher & rhs ) = default ;
		*/
		// End destruction and thread release.

		static KeyCallback _keyback ;
		static SpaceCallback _spaceback ;

		//Utility
		static bool call ( KeySignal * key, Mods * mod ) ;
		static bool call ( SpaceSignal * space, Mods * mod ) ;
# ifdef LNX
		struct GrabThread
		{
			std::thread trd ;
			std::string grabPath ;
		} ;

		// grabPath is made unique in the setGrabEvents
		static std::vector < GrabThread > _threads ;
		static std::map < std::string, Grabber * > _threadGrabs ;
		static std::mutex _threadInitLock ; // Also used to lock the above _threadGrabs
//		static std::mutex _resetLock ;
		static std::thread _enableThread ;
		static std::mutex _enableLock ;
		static NoOp _delay ;

		// Utility
		static void setGrabFromFile ( ) ;

		// Assumes Modified is populated, no NULL values.
		// Assumes both pointers are not NULL.
		static void interceptingFunction ( std::string grabPath ) ;
		static void threadSetGrabEvents ( std::vector < std::string > paths ) ;
		static void threadReset ( ) ;
		static void threadEnable ( bool enable ) ;
		static void enable_impl ( bool enable ) ;
# endif
# ifdef WIN
		static bool _enable ;
		static const int _KEY_TYPE ;
		static const int _SPACE_TYPE ;
		static const int _WHEEL_TYPE ;
		static Hook _keyHook ;
		static Hook _spaceHook ;
		static Hook _wheelHook ;
		static LRESULT CALLBACK keyCallback ( int nCode, WPARAM wParam, LPARAM lParam ) ;
		static LRESULT CALLBACK spaceCallback ( int nCode, WPARAM wParam, LPARAM lParam ) ;
		static LRESULT CALLBACK wheelCallback ( int nCode, WPARAM wParam, LPARAM lParam ) ;
# endif
	} ;
}

# endif

/*
on key,

4 assignments
{ specified }'s #mods * ( 1-3 ) ( binary checks )

if does not have, continue with minimal checks
if found existing
create key and set

call from code...

return result, or block


call from code :
set * 5
{
set call func
check
callInterceptFunction
}   * 1 - 6

callInterceptFunction:
call user function ( may block for indeterminant time )
check null then done
else
assign * 2


at most ( ( 3 + INF ) * 6 + 1 + 5 ) + 7 + 4

not including inf that is 24 + 11 = 35 calls.

Conclusion, Linear, possibly blocking * 6, but never exponential.
*/
