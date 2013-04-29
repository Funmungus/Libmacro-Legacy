/*! \file signal/command.h
 * \brief Command, to send commands to the shell/command processor.
 */

# ifndef MCR_COMMAND_H
# define MCR_COMMAND_H

# include "signal/isignal.h"

namespace macro
{
	// ! \brief Send commands to the shell/command processor.
	class Command : public ISignal
	{
	public:
		/*! \brief Command
		*
		* \ref ISignal::name
		 */
		static MCR_API const std::string name ;
		// ! \brief \ref ISignal#dispatch ( )
		static MCR_API bool ( * dispatcher ) ( Command & ) ;
		// ! \brief ctor
		inline Command ( std::string script = "" )
			: _script ( script )
		{
		}
		// ! \brief Set script.
		inline Command & operator = ( std::string script )
		{
			setScript ( script ) ;
			return * this ;
		}

		// ! \brief returns The system command for this Command
		inline std::string get ( ) const
		{
			return _script ;
		}
		// ! \brief Set the system command for this Command
		inline void set ( const std::string & script )
		{
			_script = script ;
		}

		/* ISignal implementation */
		/*! \brief \ref Command::name */
		virtual MCR_API std::string type ( ) const ;
		/*! \ref Execute system command. */
		virtual inline bool send ( )
		{
			if ( dispatch ( ) ) return true ;
			if ( _script.empty ( ) ) return false ;
			std::system ( _script.c_str ( ) ) ;
			return true ;
		}
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

		// Members are native, no operator = needed.
		inline bool operator == ( const Command & rhs ) const
		{
			bool modori = getScript ( ) == rhs.getScript ( ) ;
			return modori ;
		}
		inline bool operator != ( const Command & rhs ) const
		{
			bool modori = ! ( * this == rhs ) ;

			return modori ;
		}
		inline bool operator < ( const Command & rhs ) const
		{
			bool modori = getScript ( ) < rhs.getScript ( ) ;

			return modori ;
		}
		inline bool operator > ( const Command & rhs ) const
		{
			bool modori = getScript ( ) > rhs.getScript ( ) ;

			return modori ;
		}
		inline bool operator <= ( const Command & rhs ) const
		{
			bool modori = ! ( * this > rhs ) ;

			return modori ;
		}
		inline bool operator >= ( const Command & rhs ) const
		{
			bool modori = ! ( * this < rhs ) ;

			return modori ;
		}
	protected:
	private:
		// The command to execute with systen.
		std::string _script ;
	} ;
}

# endif
