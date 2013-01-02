/*! \file scriptsignal.h
 * \brief ScriptSignal, to send commands to the shell/command processor.
 */

# ifndef FUN_SCRIPTSIGNAL_H
# define FUN_SCRIPTSIGNAL_H

# include "signal/isignal.h"

namespace funlibrary
{
	//! \brief Send commands to the shell/command processor.
	class ScriptSignal : public ISignal, public ICallNamedCString
	{
	public:
		/*! \brief scriptsignal
		 * 
		 * \ref ISignal::name
		 */
		static SIGNAL_API const std::string name ;
		//! \brief ctor
		SIGNAL_API ScriptSignal ( std::string script = "" ) ;
		// ctor
		// "copytron" Values are copied from this ScriptSignal.
//		SIGNAL_API ScriptSignal ( const ScriptSignal & copytron ) ;

		//! \brief returns The system command for this ScriptSignal
		SIGNAL_API std::string getScript ( ) const ;
		//! \brief Set the system command for this ScriptSignal
		SIGNAL_API void setScript ( std::string script ) ;

		/* ISignal implementation */
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

		/* ICall */
		/*! \brief Receive named C-String
		 * \ref ICallNamedCString::call(char *, char *)
		 */
		virtual SIGNAL_API void call ( char * name, char * value ) ;

		// Members are native, no operator = needed.
		inline bool operator == ( const ScriptSignal & rhs ) const
		{
			bool modori = getScript ( ) == rhs.getScript ( ) ;

			return modori ;
		}
		inline bool operator != ( const ScriptSignal & rhs ) const
		{
			bool modori = ! ( *this == rhs ) ;

			return modori ;
		}
		inline bool operator < ( const ScriptSignal & rhs ) const
		{
			bool modori = getScript ( ) < rhs.getScript ( ) ;

			return modori ;
		}
		inline bool operator > ( const ScriptSignal & rhs ) const
		{
			bool modori = getScript ( ) > rhs.getScript ( ) ;

			return modori ;
		}
		inline bool operator <= ( const ScriptSignal & rhs ) const
		{
			bool modori = ! ( *this > rhs ) ;

			return modori ;
		}
		inline bool operator >= ( const ScriptSignal & rhs ) const
		{
			bool modori = ! ( *this < rhs ) ;

			return modori ;
		}
	protected:
	private:
		// The command to execute with systen.
		std::string _script ;
	};
}

#endif

