/*! \file hotkey/keymod.h
 * \brief Set modifiers, using key signals.
 */

# ifndef MCR_KEYMOD_H
# define MCR_KEYMOD_H

# include "hotkey/mods.h"

namespace macro
{
	// ! \brief Set modifiers, using key signals.
	class KeyMod : public ISignal
	{
	public :
		typedef BiMap<int, int> map_t ;
// ! \brief Map a keyboard key to modifier, and vice versa.
		static MCR_API map_t KeyToMod ;
// ! \brief Defaults for \ref KeyMod::KeyToMod
		static MCR_API map_t defaultKeyToMod ( ) ;

		//
		// Members
		//
		Mods::mod_t mods ;

		//
		// Functions
		//
// ! \brief ctor
		inline KeyMod ( const unsigned int values = 0, KeyUpType keyUp = DOWN ) ;
// ! \brief ctor
		inline KeyMod ( const KeyMod & copytron ) ;
// ! \brief assignment
		inline KeyMod & operator = ( const KeyMod & copytron ) ;

		//
		// ISignal
		//
		MCR_ISIGNAL_COMMONS ( KeyMod )
		/*! \brief Simulate keyboard keys. */
		virtual inline bool send ( ) ;

		// ! \brief This will only give a signal with oposite key up type.
		inline KeyMod operator - ( ) const ;

		MCR_EQUALITY_OPERATORS ( KeyMod )
//		public ISignal ( KeyMod )
//		inline bool operator == ( const KeyMod & rhs ) const ;
//		inline bool operator != ( const KeyMod & rhs ) const ;
//		inline bool operator < ( const KeyMod & rhs ) const ;
//		inline bool operator > ( const KeyMod & rhs ) const ;
//		inline bool operator <= ( const KeyMod & rhs ) const ;
//		inline bool operator >= ( const KeyMod & rhs ) const ;
	} ;
}

# include "hotkey/inline/keymod.h"

# endif // MCR_KEYMOD_H
