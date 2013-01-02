/*! \file keystring.h
 * \brief KeyString, for converting an ascii string into keyboard keys.
 */

# ifndef FUN_KEYSTRING_H
# define FUN_KEYSTRING_H

# include "signal/isignal.h"
# include "signal/noop.h"
# include "signal/keysignal.h"
# include "signal/mods.h"

namespace funlibrary
{
	//! \brief Convert an ascii string into keyboard keys.
	class KeyString : public ISignal, public ICallNamedLong, public ICallNamedCString
	{
	public:
		/*! \brief keystring
		 * 
		 * \ref ISignal::name
		 */
		static SIGNAL_API const std::string name ;
		//! \brief ctor
		SIGNAL_API KeyString ( std::string text = "", const unsigned long long delay = 0,
													 const unsigned long long interval = 0 ) ;
//		SIGNAL_API KeyString ( const KeyString & copytron ) ;
//		SIGNAL_API KeyString & operator = ( const KeyString & copytron ) ;

		// static
		/*! \brief Convert a set of signals into a KeyString object.
		 * 
		 * Any signal that is not a "shift" Mods, NoOp, or key signal are ignored.<br>
		 * A NoOp between a key press and release is interpreted as a delay.  Between separate keys is an interval.<br>
		 * The average of all delays will be the final delay value used.  The same is true for all intervals.
		 */
		static KeyString fromKeys ( std::vector < std::unique_ptr < ISignal > > * keySet ) ;
		/*! \brief Convert a KeyString object into a set of signals.
		 * \param container The KeyString to convert into a set of signals.
		 * \param retVals This vector is first cleared, and then values are added to it based on the string in container.
		 */
		static void toKeys ( const KeyString * container,
												 std::vector < std::unique_ptr < ISignal > > * retVals ) ;
		// static util
		/*! \brief Convert a key value into an ascii character.
		 * 
		 * \param keyval The key code value to convert.
		 * \param shifted A shifted key may change the resulting ascii code.
		 */
		static int asciiFromKey ( const int keyVal, const bool shifted ) ;
		//! \brief Convert an ascii character into a key code. 
		static int keyFromAscii ( const int asciiVal ) ;
		//! \brief Get the shifted value of an ascii character. 
		static bool needShift ( const int asciiVal ) ;

		// KeyString specific
		//! \brief Get the string to be converted into key presses.
		SIGNAL_API std::string getText ( ) const ;
		//! \brief Set the string to be converted into key presses.
		SIGNAL_API void setText ( std::string text ) ;
		//! \brief Set the delay between a key press and release..
		SIGNAL_API unsigned long long getDelay ( ) const ;
		//! \brief Set the delay between a key press and release..
		SIGNAL_API void setDelay ( unsigned long long millis ) ;
		//! \brief Get the interval between separate keys.
		SIGNAL_API unsigned long long getInterval ( ) const ;
		//! \brief Set the interval between separate keys.
		SIGNAL_API void setInterval ( unsigned long long millis ) ;

		/*! \brief \ref ISignal#type() const */
		virtual std::string type ( ) const ;
		/*! \brief \ref ISignal#send() */
		virtual SIGNAL_API bool send ( ) ;
		/*! \brief \ref ISignal#xml() const */
		virtual SIGNAL_API std::string xml ( ) const ;
		/*! \brief \ref ISignal#serialize() const */
		virtual SIGNAL_API std::string serialize ( ) const ;
		/*! \brief \ref ISignal#deserialize(std::string, unsigned int) */
		virtual SIGNAL_API std::string deserialize ( std::string container, unsigned int pos = 0 ) ;

		/*! \brief Receive named long value.
		 * \ref ICallNamedLong::call(char *, long)
		 */
		virtual SIGNAL_API void call ( char * name, long value ) ;
		/*! \brief Receive named C-string value.
		 * \ref ICallNamedCString::call(char *, char *)
		 */
		virtual SIGNAL_API void call ( char * name, char * value ) ;

		// Members are native, no operator = needed.
		// 1) string, 2) delay, 3) interval
		inline bool operator == ( const KeyString & rhs ) const
		{
			if ( _text != rhs._text )
				return false ;

			if ( _delay != rhs._delay )
				return false ;

			if ( _interval != rhs._interval )
				return false ;

			return true ;
		}
		inline bool operator != ( const KeyString & rhs ) const
		{
			bool modori = ! ( *this == rhs ) ;

			return modori ;
		}
		inline bool operator < ( const KeyString & rhs ) const
		{
			if ( _text < rhs._text )
				return true ;

			if ( _delay < rhs._delay )
				return true ;

			if ( _interval < rhs._interval )
				return true ;

			return false ;
		}
		inline bool operator > ( const KeyString & rhs ) const
		{
			if ( _text > rhs._text )
				return true ;

			if ( _delay > rhs._delay )
				return true ;

			if ( _interval > rhs._interval )
				return true ;

			return false ;
		}
		inline bool operator <= ( const KeyString & rhs ) const
		{			return ! ( *this > rhs ) ;
		}
		inline bool operator >= ( const KeyString & rhs ) const
		{
			return ! ( *this < rhs ) ;
		}
	protected:
	private:
		static const int _keyVals [ ] ;
		static const bool _needShift [ ] ;
		// Map a key code and shift to ascii code
		static const std::map < std::pair < int, bool >, int > _asciiMap ;

		static std::map < std::pair < int, bool >, int > initAsciiMap ( ) ;

		std::string _text ;
		NoOp _delay ;
		NoOp _interval ;
		std::vector < KeySignal > _sigs ;

		void sendingShifted ( unsigned int * pos ) ;
		void sendingUnshifted ( unsigned int * pos ) ;
	};
}

# endif

