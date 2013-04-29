/*! \file signal/keystring.h
 * \brief KeyString, for converting an ascii string into keyboard keys.
 */

# ifndef MCR_KEYSTRING_H
# define MCR_KEYSTRING_H

# include "signal/isignal.h"
# include "signal/noop.h"
# include "signal/key.h"
//# include "signal/mods.h"

namespace macro
{
	// ! \brief Convert a unicode string into keyboard keys.
	class KeyString : public ISignal
	{
	public:
		/*! \brief KeyString
		*
		* \ref ISignal::name
		 */
		static MCR_API const std::string name ;
		// ! \brief CharMap, Map key+shifted value to character index.
		typedef MappedVector<std::pair<int, bool>,
			std::pair<int, bool> ( 0, false ) > charmap_t ;
		// ! \brief The value returned for map type when no mapping is found.
		static MCR_API const std::pair<int, bool> NO_MAPPING ;
		// ! \brief The value returned for character index when no mapping is found.
		static MCR_API const unsigned int NO_CHARACTER ;

		// ! \brief \ref ISignal#dispatch ( )
		static MCR_API bool ( * dispatcher ) ( KeyString & ) ;
		/*! \brief Map is <int, bool>, vector is int.
		* Mapping key and shifted value to character as index
		 **/
		static MCR_API charmap_t CharMap ;
		/*! \brief The starting off charmap.
		* Important, implement in native directory.
		 **/
		static MCR_API charmap_t defaultCharMap ( ) ;

		// ! \brief ctor
		inline KeyString ( const std::string & text = "", const unsigned long long delay = 0,
													const unsigned long long interval = 0 )
			: _delay ( delay ), _interval ( interval )
		{
			setText ( text ) ;
		}
		// ! \brief Set text.
		inline KeyString & operator = ( const std::string & text )
		{
			setText ( text ) ;
			return * this ;
		}

		// static
		/*! \brief Convert a set of signals into a KeyString object.
		*
		* Any signal that is not a "shift" Mods, NoOp, or key signal are ignored.<br>
		* A NoOp between a key press and release is interpreted as a delay. Between separate keys is an interval.<br>
		* The average of all delays will be the final delay value used. The same is true for all intervals.
		 */
		static MCR_API KeyString fromKeys ( std::vector < std::unique_ptr < ISignal > > * keySet ) ;
		/*! \brief Convert a KeyString object into a set of signals.
		* \param retVals This vector is first cleared, and then values are added to it based on the string in container.
		 */
		MCR_API void toKeys ( std::vector < std::unique_ptr < ISignal > > * retVals ) ;

		// static util
		/*! \brief Convert a key value into an ascii character.
		*
		* \param keyval The key code value to convert.
		* \param shifted A shifted key may change the resulting ascii code.
		 */
		static inline int charFromKey ( const int keyVal, const bool shifted )
		{
			return CharMap.index ( std::make_pair ( keyVal, shifted ) ) ;
		}
		// ! \brief Convert an ascii character into a key code.
		static inline int keyFromChar ( const int character )
		{
			auto mPair = CharMap.first ( character ) ;
			return mPair.first ;
		}
		// ! \brief Get the shifted value of an ascii character.
		static inline bool needShift ( const int character )
		{
			auto mPair = CharMap.first ( character ) ;
			return mPair.second ;
		}

		// KeyString specific
		// ! \brief Get the string to be converted into key presses.
		inline std::string getText ( ) const
		{
			return _text ;
		}
		// ! \brief Set the string to be converted into key presses.
		inline void setText ( const std::string & text )
		{
			_text = text ;
		}
		// ! \brief Get the delay between a key press and release..
		inline unsigned long long getDelay ( ) const
		{
			return _delay.get ( ) ;
		}
		// ! \brief Set the delay between a key press and release..
		inline void setDelay ( unsigned long long millis )
		{
			_delay.set ( millis ) ;
		}
		// ! \brief Get the interval between separate keys.
		inline unsigned long long getInterval ( ) const
		{
			return _interval.get ( ) ;
		}
		// ! \brief Set the interval between separate keys.
		inline void setInterval ( unsigned long long millis )
		{
			_interval.set ( millis ) ;
		}

		/*! \brief \ref ISignal#type ( ) const */
		virtual MCR_API std::string type ( ) const ;
		/*! \brief Send text as a set of signals. */
		virtual MCR_API bool send ( ) ;
		// ! \brief \ref ISignal#dispatch ( )
		virtual inline bool	dispatch ( )
		{
			if ( dispatcher != NULL )
			{
				if ( dispatcher ( * this ) )
					return true ;
			}
			return ISignal::dispatch ( ) ;
		}

		// Members are native, no operator = needed.
		// 1 ) string, 2 ) delay, 3 ) interval
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
			bool modori = ! ( * this == rhs ) ;

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
		{
			return ! ( * this > rhs ) ;
		}
		inline bool operator >= ( const KeyString & rhs ) const
		{
			return ! ( * this < rhs ) ;
		}
	protected:
	private:
		std::string _text ;
		NoOp _delay ;
		NoOp _interval ;

		inline bool sendWithDelay ( Key & key )
		{
			if ( getDelay ( ) == 0 )
			{
				key.setKeyUp ( BOTH ) ;
				if ( ! key.send ( ) ) return false ;
			}
			else
			{
				key.setKeyUp ( DOWN ) ;
				if ( ! key.send ( ) ) return false ;
				_delay.send ( ) ;
				key.setKeyUp ( UP ) ;
				if ( ! key.send ( ) ) return false ;
			}

			if ( getInterval ( ) != 0 )
				_interval.send ( ) ;
			return true ;
		}
	} ;
}

# endif
