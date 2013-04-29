/*! \file hotkey/signalmod.h
 * \brief Set modifiers, using the addresses of signals mapped to modifier values.
 */

# ifndef MCR_SIGNALMOD_H
# define MCR_SIGNALMOD_H

# include "hotkey/mods.h"

namespace macro
{
	// ! \brief Set modifiers, using signal pointers ( specific referenced signal ).
	class SignalMod : public ISignal
	{
	public :
		typedef BiMap < ISignal *, Mods::mod_t > map_t ;
		// ! \brief Map a signal pointer to modifier, and vice versa.
		static MCR_API map_t SigToMod ;

		//
		// Members
		//
		Mods::mod_t mods ;

// ! \brief ctor
		inline SignalMod ( const unsigned int values = 0, const KeyUpType keyUp = DOWN ) ;
// ! \brief ctor
		inline SignalMod ( const SignalMod & copytron ) ;
// ! \brief assignment
		inline SignalMod & operator = ( const SignalMod & copytron ) ;

		MCR_ISIGNAL_COMMONS ( SignalMod )
		/*! \brief \ref ISignal#send ( ) */
		inline bool send ( ) ;

// ! \brief This will only give a signal with oposite key up type.
		inline SignalMod operator - ( ) const ;
		MCR_EQUALITY_OPERATORS ( SignalMod )
//		inline bool operator == ( const SignalMod & rhs ) const ;
//		inline bool operator != ( const SignalMod & rhs ) const ;
//		inline bool operator < ( const SignalMod & rhs ) const ;
//		inline bool operator > ( const SignalMod & rhs ) const ;
//		inline bool operator <= ( const SignalMod & rhs ) const ;
//		inline bool operator >= ( const SignalMod & rhs ) const ;
	} ;
}

# include "hotkey/inline/signalmod.h"

# endif // MCR_SIGNALMOD_H
