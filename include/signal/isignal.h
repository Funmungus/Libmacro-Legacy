/*! \file isignal.h
 * \brief ISignal interface for signals.
 */

# ifndef FUN_ISignal_H
# define FUN_ISignal_H

# include "signal/def.h"

namespace funlibrary
{
	/*! \brief Superclass of all signals.
	 * 
	 * Logically a signal should be anything that can be "sent" out to another entity,
	 * or "sent" to cause an action.
	 */
	class ISignal : virtual public TemplateBase
	{
	public:
		/*! \brief isignal
		 * name is used in xml strings to describe and receive an object of that type.
		 */
		static SIGNAL_API const std::string name ;

		//! \brief Return the name of subclass. Should only be implemented by instantiable subclasses.
		virtual std::string type ( ) const = 0 ;

		//! \brief How a signal is sent depends on the type of signal.
		virtual SIGNAL_API bool send ( ) = 0 ;

		//! \brief Creates an xml descriptor from this signal.
		virtual SIGNAL_API std::string xml ( ) const = 0 ;

		/*! \brief This object, in bytes.
		 * 
		 * All serializations should start with a serialHeader of that type, and end with SerialSeparator.
		 */
		virtual SIGNAL_API std::string serialize ( ) const = 0 ;

		/*! \brief Set this signal from the output of serialize.
		 * 
		 * \param container The output of serialize to set this signal from.
		 * \param pos Position to start checking from.
		 * \return A substring excluding this signal.
		 */
		virtual SIGNAL_API std::string deserialize ( std::string container, unsigned int pos = 0 ) = 0 ;
	} ;
}

# endif

