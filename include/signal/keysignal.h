/*! \file keysignal.h
 * \brief KeySignal, for keyboard keys.
 */

# ifndef FUN_KEYSIGNAL_H
# define FUN_KEYSIGNAL_H

# include "signal/isignal.h"
# include "signal/device.h"

namespace funlibrary
{
	/*! \brief To signify a key is being set to either pressed down,
	 * released, or both pressed and released.
	 */
	enum KeyUpType
	{
		DOWN = 0,
		UP = 1,
		BOTH = 2
	};

	// Hardware signal specific to keyboards.
	// Please note that in the future scan codes will be sent by a keyboard driver.
	
	/*! \brief Send or receive keyboard keys.
	 *
	 * Keyboard signals have both a scan code, specified by the hardware,
	 * or a key code, specified by the operating system.  One or both may be specified.
	 * <br>
	 * A 0 value of both key code and scan code should be logically interpreted as either,
	 * "no key available, do not send," or "either a key code or scan code of any value."
	 * <br>
	 * Also in the case of 0 value key and scan, if the key up type is UP,
	 * then sending this signal should have the added effect of releasing all known keys.
	 */
	class KeySignal : public ISignal, public ICallNamedInt
	{
	public:
		/*! \brief keysignal
		 * 
		 * \ref ISignal::name
		 */
		static SIGNAL_API const std::string name ;
		/*! ctor
		 * \param key Integer representation of a keyboard key.
		 * \param scan True if this is a scancode.
		 * \param keyUp Key down, up, or both.
		 */
		SIGNAL_API KeySignal ( const int key = 1, const int scan = 0,
													 const KeyUpType keyUp = KeyUpType::DOWN ) ;
		//! \brief ctor
		SIGNAL_API KeySignal ( const KeySignal & copytron ) ;
		//! \brief Values are copied from this copytron.
		SIGNAL_API KeySignal & operator = ( const KeySignal & copytron ) ;


		//! \brief Get a string representation of keyInt.
		static SIGNAL_API std::string getKeyString ( const int keyInt ) ;
		//! \brief Get a key code representation of keyString.
		static SIGNAL_API int getKeyInt ( std::string keyString ) ;

		//! \brief Get This item's key code.
		SIGNAL_API int getKey ( ) const ;
		//! \brief Set This item's key code.
		SIGNAL_API void setKey ( const int key ) ;
		//! \brief Get This item's scan code.
		SIGNAL_API int getScan ( ) const ;
		//! \brief Set This item's scan code. 
		SIGNAL_API void setScan ( const int scan ) ;
		//! \brief Get This item's key up type.
		SIGNAL_API KeyUpType getKeyUp ( ) const ;
		//! \brief Set This item's key up type. 
		SIGNAL_API void setKeyUp ( const KeyUpType keyUp ) ;

		/*! \brief \ref ISignal#type() const */
		virtual std::string type ( ) const ;
		/*! \ref ISignal#send() */
		virtual SIGNAL_API bool send ( ) ;
		/*! \brief \ref ISignal#xml() const */
		virtual SIGNAL_API std::string xml ( ) const ;
		/*! \brief \ref ISignal#serialize() const */
		virtual SIGNAL_API std::string serialize ( ) const ;
		/*! \brief \ref ISignal#deserialize(std::string, unsigned int) */
		virtual SIGNAL_API std::string deserialize ( std::string container, unsigned int pos = 0 ) ;

		/*! \brief Receive named int value.
		 * \ref ICallNamedInt::call(char *, int)
		 */
		virtual SIGNAL_API void call ( char * name, int value ) ;

		/*
		Order of importance for comparison.  Key, scan, then keyUp.
		*/

		// Members are native, no operator = needed.
		inline bool operator == ( const KeySignal & rhs ) const
		{
			if ( getKey ( ) != rhs.getKey ( ) )
				return false ;
			if ( getScan ( ) != rhs.getScan ( ) )
				return false ;
			if ( getKeyUp ( ) != rhs.getKeyUp ( ) )
				return false ;

			return true ;
		}
		inline bool operator != ( const KeySignal & rhs ) const
		{
			bool modori = !( *this == rhs ) ;

			return modori ;

		}
		inline bool operator < ( const KeySignal & rhs ) const
		{
			if ( getKey ( ) != rhs.getKey ( ) )
				return getKey ( ) < rhs.getKey ( ) ;

			if ( getScan ( ) != rhs.getScan ( ) )
				return getScan ( ) < rhs.getScan ( ) ;
			
			if ( getKeyUp ( ) != rhs.getKeyUp ( ) )
				return getKeyUp ( ) < rhs.getKeyUp ( ) ;


			return false ;
		}
		inline bool operator > ( const KeySignal & rhs ) const
		{
			if ( getKey ( ) != rhs.getKey ( ) )
				return getKey ( ) > rhs.getKey ( ) ;

			if ( getScan ( ) != rhs.getScan ( ) )
				return getScan ( ) > rhs.getScan ( ) ;

			if ( getKeyUp ( ) != rhs.getKeyUp ( ) )
				return getKeyUp ( ) > rhs.getKeyUp ( ) ;


			return false ;
		}
		inline bool operator <= ( const KeySignal & rhs ) const
		{
			bool modori = !( *this > rhs ) ;

			return modori ;
		}
		inline bool operator >= ( const KeySignal & rhs ) const
		{
			bool modori = !( *this < rhs ) ;

			return modori ;
		}
	private:
		// Map from keyString to keycode.
		static std::map < std::string, int > _keyInts ;
		const static std::string _keyNames [ ] ;
		const static unsigned int KEY_COUNT ;

		static std::map < std::string, int>  initKeys ( ) ;

		// The keycode of this KeySignal.
		std::int32_t _key ;
		// True means this KeySignal sends a scancode.
		std::int32_t _scan ;
		// True to release a keycode.
		KeyUpType _keyUp ;
# ifdef LNX
		// scan, key, and sync
		input_event _keyvent [ 3 ] ;
		void initVent ( ) ;
# endif
	};
}

# endif

