/*! \file spacesignal.h
 * \brief SpacePosition, a set of values representing spatial coordinates. <br>
 * SpaceSignal, a signal to send and manipulate spatial coordinates.
 */

# ifndef FUN_SPACESIGNAL_H
# define FUN_SPACESIGNAL_H

# include "signal/isignal.h"
# include "signal/device.h"

namespace funlibrary
{
	//! \brief A set of values representing spatial coordinates.
	struct SpacePosition : virtual public ICallNamedLong
	{
	public:
		/*! \brief spaceposition
		 * 
		 * \ref ISignal::name
		 */
		static SIGNAL_API const std::string name ;
		//! \brief X spatial coordinate.
		std::int64_t x ;
		//! \brief Y spatial coordinate.
		std::int64_t y ;
		//! \brief Z spatial coordinate.
		std::int64_t z ;

		//! \brief ctor
		SIGNAL_API SpacePosition ( const long long x = 0, const long long y = 0, const long long z = 0 ) ;

//		SIGNAL_API SpacePosition ( const SpacePosition & copytron ) ;

		/* ICall */
		/*! \brief Receive named long value.
		 * \ref ICallNamedLong::call(char *, long)
		 */
		virtual SIGNAL_API void call ( char * name, long value ) ;

		/*
		Order of importance for comparison.  x, y, then z.
		*/
//		inline SpacePosition & operator += ( const SpacePosition & rhs )
//		{
//			x += rhs.x ;
//			y += rhs.y ;
//			z += rhs.z ;
//			return *this ;
//		}

		// Members are native, no operator = needed.
		inline bool operator == ( const SpacePosition & rhs ) const
		{
			if ( x != rhs.x )
				return false ;

			if ( y != rhs.y )
				return false ;

			if ( z != rhs.z )
				return false ;

			return true ;

		}

		inline bool operator != ( const SpacePosition & rhs ) const
		{
			bool modori = !( *this == rhs ) ;

			return modori ;

		}
		inline bool operator < ( const SpacePosition & rhs ) const
		{
			if ( x != rhs.x )
				return x < rhs.x ;

			if ( y != rhs.y )
				return y < rhs.y ;

			if ( z != rhs.z )
				return z < rhs.z ;

			return false ;
		}
		inline bool operator > ( const SpacePosition & rhs ) const
		{
			if ( x != rhs.x )
				return x > rhs.x ;

			if ( y != rhs.y )
				return y > rhs.y ;

			if ( z != rhs.z )
				return z > rhs.z ;

			return false ;
		}
		inline bool operator <= ( const SpacePosition & rhs ) const
		{
			bool modori = !( *this > rhs ) ;

			return modori ;
		}
		inline bool operator >= ( const SpacePosition & rhs ) const
		{
			bool modori = !( *this < rhs ) ;

			return modori ;
		}
	};

	//! \brief A signal to send and manipulate spatial coordinates.
	class SpaceSignal : public ISignal, public ICallNamedLong,
			public ICallNamedBool, public ICallNamedInt, public ICallXmlNode
	{
	public:
		/*! \brief spacesignal
		 * 
		 * \ref ISignal::name
		 */
		static SIGNAL_API const std::string name ;
		/*! \brief ctor
		 *
		 * \param event The event type, or significance of this space signal.
		 * \param cursorJustify If true, spatial movement is the SpacePosition vector
		 * added to current position. If false, use SpacePosition as screen coordinates.
		 * \param eventData Use dependent on event type, and operating system.
		 * \param blip X, Y, Z spatial coordinates. 
		 */ 
		SIGNAL_API SpaceSignal ( const int event = 0, const SpacePosition & blip = SpacePosition ( ),
														 const bool cursorJustify = false, const int eventData = 0 ) ;
		// ctor
		// "copytron" All values taken directly from this
		// instance
		SIGNAL_API SpaceSignal ( const SpaceSignal & copytron ) ;
		SIGNAL_API SpaceSignal & operator = ( const SpaceSignal & copytron ) ;

		/*! \brief Get a string representation of an event from and integer.
		 * 
		 * \param event The event to turn into a string.
		 */
		static SIGNAL_API std::string getEventString ( const int event ) ;
		/*! \brief Get an integer representation of an event from a string.
		 *
		 * \param eventString The event to turn into an integer.
		 */
		static SIGNAL_API int getEventInt ( std::string eventString ) ;
		//! \brief Get the event value from an operating system specific key/btn
		static SIGNAL_API int getEventFromKey ( const int key ) ;
		//! \brief Get the current position of the user interface cursor.
		static SIGNAL_API SpacePosition currentPosition ( ) ;

		//! \brief An integer representation of the event type this
		// signal represents.
		SIGNAL_API int getEvent ( ) const ;
		//! \brief Set the event type for this SpaceSignal.
		SIGNAL_API void setEvent ( const int event ) ;
		//! \brief Get the X coordinate of this SpaceSignal.
		SIGNAL_API long long getX ( ) const ;
		//! \brief Set the X coordinate of this SpaceSignal.
		SIGNAL_API void setX ( const long long x ) ;
		//! \brief Get the Y coordinate of this SpaceSignal.
		SIGNAL_API long long getY ( ) const ;
		//! \brief Set the Y coordinate of this SpaceSignal.
		SIGNAL_API void setY ( const long long y ) ;
		//! \brief Get the Z coordinate of this SpaceSignal.
		SIGNAL_API long long getZ ( ) const ;
		//! \brief Set the Z coordinate of this SpaceSignal.
		SIGNAL_API void setZ ( const long long z ) ;
		//! \brief Get the whole SpacePosition of this SpaceSignal's coordinates.
		SIGNAL_API SpacePosition getPosition ( ) const ;
		//! \brief Set the whole SpacePosition of this SpaceSignal's coordinates.
		SIGNAL_API void setPosition ( const SpacePosition & blip ) ;
		//! \brief Get the cursor justification flag. True = cursor is justified.
		SIGNAL_API bool isCursorJustify ( ) const ;
		//! \brief Set the cursor justification flag. True = cursor is justified.
		SIGNAL_API void setEnableCursorJustify ( const bool cursorJustify ) ;
		//! \brief Get the extra data. How data is used depends on the event.
		SIGNAL_API int getEventData ( ) const ;
		//! \brief Set the extra data. How data is used depends on the event.
		SIGNAL_API void setEventData ( const int eventData ) ;

		/* ISignal implementation */
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

		/* ICall */
		/*! \brief Receive named long value
		 * \ref ICallNamedLong::call(char *, long)
		 */
		virtual SIGNAL_API void call ( char * name, long value ) ;
		/*! \brief Receive named boolean value
		 * \ref ICallNamedBool::call(char *, bool)
		 */
		virtual SIGNAL_API void call ( char * name, bool value ) ;
		/*! \brief Receive named integer value
		 * \ref ICallNamedInt::call(char *, int)
		 */
		virtual SIGNAL_API void call ( char * name, int value ) ;
		/*! \brief Receive an xml node, for object members.
		 * \ref ICallXmlNode::call(rapidxml::xml_node<char>* obj)
		 */
		virtual SIGNAL_API void call ( rapidxml::xml_node < char > * obj ) ;

		/*
		Order of importance for comparison.  Event, EventData, Position, then CursorJustify
		*/
//		inline SpaceSignal & operator = ( const SpaceSignal & rhs )
//		{
//			if ( rhs == *this )
//				return *this ;

//			setEvent ( rhs.getEvent ( ) ) ;
//			setEventData ( rhs.getEventData ( ) ) ;
//			setX ( rhs.getX ( ) ) ;
//			setY ( rhs.getY ( ) ) ;
//			setZ ( rhs.getZ ( ) ) ;
//			setEnableCursorJustify ( rhs.isCursorJustify ( ) ) ;

//			return *this;
//		}
		inline bool operator == ( const SpaceSignal & rhs ) const
		{
			if ( getEvent ( ) != rhs.getEvent ( ) )
				return false ;

			if ( getPosition ( ) != rhs.getPosition ( ) )
				return false ;

			if ( isCursorJustify ( ) != rhs.isCursorJustify ( ) )
				return false ;

			if ( getEventData ( ) != rhs.getEventData ( ) )
				return false ;

			return true ;

		}
		inline bool operator != ( const SpaceSignal & rhs ) const
		{
			bool modori = !( *this == rhs ) ;

			return modori ;

		}
		inline bool operator < ( const SpaceSignal & rhs ) const
		{
			if ( getEvent ( ) != rhs.getEvent ( ) )
				return getEvent ( ) < rhs.getEvent ( ) ;

			if ( getEventData ( ) != rhs.getEventData ( ) )
				return getEventData ( ) < rhs.getEventData ( ) ;

			if ( getPosition ( ) != rhs.getPosition ( ) )
				return getPosition ( ) < rhs.getPosition ( ) ;

			if ( isCursorJustify ( ) != rhs.isCursorJustify ( ) )
				return isCursorJustify ( ) < rhs.isCursorJustify ( ) ;

			return false ;
		}
		inline bool operator > ( const SpaceSignal & rhs ) const
		{
			if ( getEvent ( ) != rhs.getEvent ( ) )
				return getEvent ( ) > rhs.getEvent ( ) ;

			if ( getEventData ( ) != rhs.getEventData ( ) )
				return getEventData ( ) > rhs.getEventData ( ) ;

			if ( getPosition ( ) != rhs.getPosition ( ) )
				return getPosition ( ) > rhs.getPosition ( ) ;

			if ( isCursorJustify ( ) != rhs.isCursorJustify ( ) )
				return isCursorJustify ( ) > rhs.isCursorJustify ( ) ;

			return false ;
		}
		inline bool operator <= ( const SpaceSignal & rhs ) const
		{
			bool modori = !( *this > rhs ) ;

			return modori ;
		}
		inline bool operator >= ( const SpaceSignal & rhs ) const
		{
			bool modori = !( *this < rhs ) ;

			return modori ;
		}
	private:
		static const int _eventBtnMap [ ] ;
		static const std::string  _eventName [ ] ;
		static const unsigned int FUN_SPACE_COUNT ;
		// Map from event strings to integers.
		static std::map < std::string, int > _eventInts ;
		static std::map < int, int > _btnEvents ;

		static std::map < std::string, int > initVals ( ) ;
		static std::map < int, int > initBtnEvent ( ) ;

		// The type of SpaceSignal.
		std::int32_t _event ;
		// The three dimensional position of this SpaceSignal.
		SpacePosition _blip ;
		// True to move based on previous position.
		bool _cursorJustify ;
		// event data depends on the event type.
		std::int32_t _eventData ;

# ifdef LNX
		struct input_event _spacevent [ 2 ], _relvent [ 4 ], _absvent [ 4 ] ;
		void initVent ( ) ;
# endif

	};
}

# endif

