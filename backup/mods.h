/*! \file hotkey/mods.h
 * \brief Mods, To handle known modifiers.
 */

# ifndef MCR_MODS_H
# define MCR_MODS_H

# include "hotkey/def.h"

namespace macro
{
	/*! \brief Modifiers, alt, ctrl, shift, meta, windows...
	 *
	 * This does not define all possibilities. Mods are implemented as bits,
	 * so other mods may be currently undefined bits.
	 * <br>
	 * It is suggested to have a BiMap to and from signal type to bit value.
	 * By default alt = 1, ctrl = 2, shift = 4, meta/win = 8
	 */
	struct Mods // : public ISignal
	{
	public:
		// ! \brief Alt
		static MCR_API const std::string ALT ;
		// ! \brief Control
		static MCR_API const std::string CTRL ;
		// ! \brief Shift
		static MCR_API const std::string SHIFT ;
		// ! \brief Meta, also logically the windows key
		static MCR_API const std::string META ;

		typedef std::pair<int, KeyUpType> mod_t ;
		static const mod_t DEFAULT_MOD ;

		/*! \brief Assign a set of bits as this mod, from a set of mod values.
		 *
		 * \param newMods An array of bit values which are all assigned to
		 * the container. ( bitwise OR, then set this mod as that value )
		 * \param length The number of values to OR together.
		 */
		static void set ( unsigned int & container, const unsigned int * newMods, const unsigned int length ) ;
		/*!
		 * \brief modifier is added to or removed from container
		 * \param containerKeyUp If DOWN values from modifier are added or removed as normal. Otherwise adding or removing is reversed.
		 * \param modifierKeyUp If DOWN modFrom is added, else modFrom is removed.
		 */
		static inline void modify ( unsigned int & container, const KeyUpType containerKeyUp, const unsigned int modifier, const KeyUpType modifierKeyUp ) ;
		/*!
		 * \brief modifier is added to or removed from container
		 * \param keyUp If DOWN modFrom is added, else modFrom is removed.
		 */
		static inline void modify ( unsigned int & container, const unsigned int modifier, const KeyUpType keyUp ) ;
		/*!
		 * \brief modifier is added to container.
		 */
		static inline void modify ( unsigned int & container, const unsigned int modifier ) ;
		// ! \brief hasMod returns true if this mod contains all bits from modVal.
		static inline bool hasMod ( const unsigned int container, const unsigned int modVal ) ;
		// ! \brief newMod is valued with this mod ( bitwise OR ), inline for key sending.
		static inline void add ( unsigned int & container, const unsigned int newMod ) ;
		// ! \brief Removes all values in delMod from this mod, inline for key sending.
		static inline void remove ( unsigned int & container, const unsigned int delMod ) ;
	} ;

	//
	// Modifier mappings that do not have a signal associated with them.
	//
	// ! \brief Map event code to mod+keyUp
	extern BiMap < int, Mods::mod_t > EchoToMod ;
	// Scroll and cursor deal with multidimensional inexact matching.
	// Cannot map directly with inexact matching.
//	// ! \brief measurementError for matching dimensions modifiers.
//	extern int ModifierMeasurementError ;
	// Edit: Spatial signals are too complex to create modifiers from,
	// and are not expected to be needed. If modifiers are desired from
	// these types, then a new dispatch function will be required.
//	/*! \brief Set of matching Scroll signal to modifier type. Matching is determined by
//	 * \ref macro::ResemblesFromZero ( const Dimensions, const Dimensions )
//	 * and cannot be mapped directly.
//	 **/
//	extern std::vector < std::pair < Scroll, Mods::mod_t > > ScrollToMod ; 	// x, y, z
//	/*! \brief Set of matching cursor signal to modifier type. Matching is determined by
//	 * \ref macro::ResemblesAbsolute ( const Dimensions, const Dimensions, int )
//	 * and cannot be mapped directly.
//	 **/
//	extern std::vector < std::pair < MoveCursor, Mods::mod_t > > ScrollToMod ; 	// x, y, z
}

# include "hotkey/inline/mods.h"

# endif
