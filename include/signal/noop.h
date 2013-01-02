/*! \file noop.h
 * \brief NoOp, pause execution in milliseconds.
 */

# ifndef FUN_NOOP_H
# define FUN_NOOP_H

# include "signal/isignal.h"

namespace funlibrary
{
	//! \brief A signal to pause execution in milliseconds.
	class NoOp : public ISignal, public ICallNamedLong
	{
	public:
		/*! \brief noop
		 *
		 * \ref ISignal::name
		 */
		static SIGNAL_API const std::string name ;
		//! ctor
		SIGNAL_API NoOp ( const unsigned long long millis = 1 ) ;
		// ctor
		// "copytron" Beginning descriptor will come from copytron
//		SIGNAL_API NoOp ( const NoOp & copytron ) ;

		//! \brief Get the time in milliseconds to interrupt operation.
		SIGNAL_API unsigned long long getMillis ( ) const ;
		//! \brief Set the time in milliseconds to interrupt operation.
		SIGNAL_API void setMillis ( const unsigned long long millis ) ;

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
		/*! \brief Receive named long value.
		 * \ref ICallNamedLong::call(char *, long)
		 */
		virtual SIGNAL_API void call ( char * name, long value ) ;

		// Members are native, no operator = needed.
		inline bool operator == ( const NoOp & rhs ) const
		{
			bool modori = getMillis ( ) == rhs.getMillis ( ) ;

			return modori ;
		}
		inline bool operator != ( const NoOp & rhs ) const
		{
			bool modori = ! ( *this == rhs ) ;

			return modori ;
		}
		inline bool operator < ( const NoOp & rhs ) const
		{
			bool modori = getMillis ( ) < rhs.getMillis ( ) ;

			return modori ;
		}
		inline bool operator > ( const NoOp & rhs ) const
		{
			bool modori = getMillis ( ) > rhs.getMillis ( ) ;

			return modori ;
		}
		inline bool operator <= ( const NoOp & rhs ) const
		{			return ! ( *this > rhs ) ;
		}
		inline bool operator >= ( const NoOp & rhs ) const
		{
			return ! ( *this < rhs ) ;
		}
	protected:
	private:
		// Time in milliseconds to interrupt operation.
		std::uint64_t _millis ;
	};
}

# endif

