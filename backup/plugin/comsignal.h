/*! \file signal/comsignal.h
 * \brief ComSignal communicates signals accross language barriers.
 */

# ifndef MCR_COMSIGNAL_H
# define MCR_COMSIGNAL_H

# include "signal/isignal.h"

/// TODO Decide if an interface is needed for factories, and if ComSignal should have a factory ( s ) reference.
namespace macro
{
	/*!
	 * \brief ComSignal communicates signals accross language barriers.
	 *
	 * Planned communications are CLI, JNI, and P/Invoke.
	 */
	class ComSignal : public ISignal
	{
	public:
		/*! \brief Comsignal
		 *
		 * \ref ISignal::name
		 */
		static MCR_API const std::string name ;
		// ! \brief \ref ISignal#dispatch ( )
		static MCR_API bool ( * dispatcher ) ( ComSignal & ) ;

		/*! \brief \ref ISignal#type ( ) const */
		virtual MCR_API std::string		type ( ) const ;
		/*! \brief \ref ISignal#send ( ) */
		virtual MCR_API bool			send ( ) ;
		// ! \brief \ref ISignal#dispatch ( )
		virtual inline bool			dispatch ( )
		{
			if ( dispatcher != NULL )
			{
				if ( dispatcher ( * this ) )
					return true ;
			}
			return ISignal::dispatch ( ) ;
		}

		/*! \brief ctor
		 * \param newMessageDescriptor Descriptor of contained signal.
		 */
		inline ComSignal ( const std::string & newMessageDescriptor )
			: _messageDescriptor ( newMessageDescriptor )
		{	}
		// ! \brief Set message.
		inline ComSignal & operator = ( const std::string & message )
		{
			setMessage ( message ) ;
			return * this ;
		}

		/*! \brief Get the xml descriptor of contained signal. */
		inline std::string getMessage ( ) const
		{
			return _messageDescriptor ;
		}
		// ! \brief Set the xml descriptor of contained signal.
		inline void setMessage ( const std::string & newMessageDescriptor )
		{
			_messageDescriptor = newMessageDescriptor ;
		}

		inline bool operator == ( const ComSignal & rhs ) const
		{
			bool modori = getMessage ( ) == rhs.getMessage ( ) ;

			return modori ;
		}
		inline bool operator != ( const ComSignal & rhs ) const
		{
			bool modori = ! ( * this == rhs ) ;

			return modori ;
		}
		inline bool operator < ( const ComSignal & rhs ) const
		{
			bool modori = getMessage ( ) < rhs.getMessage ( ) ;

			return modori ;
		}
		inline bool operator > ( const ComSignal & rhs ) const
		{
			bool modori = getMessage ( ) > rhs.getMessage ( ) ;

			return modori ;
		}
		inline bool operator <= ( const ComSignal & rhs ) const
		{
			return ! ( * this > rhs ) ;
		}
		inline bool operator >= ( const ComSignal & rhs ) const
		{
			return ! ( * this < rhs ) ;
		}
	protected:
	private:
		// xml representation of a contained signal to communicate.
		std::string _messageDescriptor ;
	} ;
}

# endif

///TODO
///Add JNI function as native callback
