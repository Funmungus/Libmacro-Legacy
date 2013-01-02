/*! \file comsignal.h
 * \brief ComSignal communicates signals accross language barriers.
 */

# ifndef FUN_COMSIGNAL_H
# define FUN_COMSIGNAL_H

# include "signal/isignal.h"

/// TODO Decide if an interface is needed for factories, and if ComSignal should have a factory(s) reference.
namespace funlibrary
{
	/*! \file comsignal.h
	 * \brief ComSignal communicates signals accross language barriers.
	 * 
	 * Planned communications are CLI, JNI, and P/Invoke.
	 */
	class ComSignal : public ISignal, public ICallNamedCString
	{
	public:
		/*! \brief "comsignal" 
		 * 
		 * \ref ISignal::name
		 */
		static SIGNAL_API const std::string name ;
		/*! \brief ctor
		 * \param newMessageDescriptor Beginning descriptor of contained signal.
		 */
		SIGNAL_API ComSignal ( std::string newMessageDescriptor = "" ) ;
		/* \brief ctor
		 * \param copytron Beginning descriptor will come from copytron.
		 */
//		SIGNAL_API ComSignal ( const ComSignal & copytron ) ;

		/*! \brief Get the xml descriptor of contained signal. */
		SIGNAL_API std::string getMessage ( ) const ;
		// Set the xml descriptor of contained signal.
		SIGNAL_API void setMessage ( std::string newMessageDescriptor ) ;

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

		/*! \brief Receive named C-String
		 * \ref ICallNamedCString::call(char *, char *)
		 */
		virtual SIGNAL_API void call ( char * name, char * value ) ;

		// Members are native, no operator = needed.

		inline bool operator == ( const ComSignal & rhs ) const
		{
			bool modori = getMessage ( ) == rhs.getMessage ( ) ;

			return modori ;
		}
		inline bool operator != ( const ComSignal & rhs ) const
		{
			bool modori = ! ( *this == rhs ) ;

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
			return ! ( *this > rhs ) ;
		}
		inline bool operator >= ( const ComSignal & rhs ) const
		{
			return ! ( *this < rhs ) ;
		}

	protected:
	private:
		// xml representation of a contained signal to communicate.
		std::string _messageDescriptor ;
	};
}

# endif

///TODO
///Add JNI function as native callback

