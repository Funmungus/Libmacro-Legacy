/*! \file mods.h
 * \brief Mods, To handle known modifiers.
 *
 * TODO Enable user to add/remove known modifiers.
 */

# ifndef FUN_MODS_H
# define FUN_MODS_H

# include "signal/keysignal.h"

/*! \def FUN_ALT
 * \brief alt, The alt modifier.
 */
/*! \def FUN_CTRL
 * \brief ctrl, The control modifier.
 */
/*! \def FUN_SHIFT
 * \brief shift, The shift modifier.
 */
/*! \def FUN_META
 * \brief meta, The meta modifier.
 */

# define FUN_ALT "alt"
# define FUN_CTRL "ctrl"
# define FUN_SHIFT "shift"
# define FUN_META "meta"

namespace funlibrary
{
	class Mods ;
	enum KeyUpType ;

	//! \brief Modifiers to be used internally as "current modifier."
	extern Mods InternalMods ;

	// Modifies InternalMods with given values.
	// KeyUpType DOWN inserts mod, KeyUpType UP removes that mod.
//	SIGNAL_API void modify ( const Mods & values, KeyUpType keyUp ) ;

	/*! \brief Modifiers, such as control, alt, menu, super, shift, meta...
	 *
	 * This does not define all possibilities.  Mods are implemented as bits,
	 * so other mods may be currently undefined bits.
	 */
	class Mods : public ISignal, public ICallNamedUnsigned,
			public ICallNamedInt
	{
	public:
		/*! \brief "mods"
		 *
		 * \ref ISignal::name
		 */
		static SIGNAL_API const std::string name ;
		//! \brief The set of known mod names, as strings.
		static SIGNAL_API const std::string MODSET [ ] ;
		//! \brief The number of elements in MODSET
		static SIGNAL_API const unsigned int MODSET_LENGTH ;

		//! \brief ctor
		SIGNAL_API Mods ( const unsigned int values = 0, const KeyUpType keyUp = KeyUpType::DOWN ) ;
		// ctor
		// "copytron" Values are copied from here.
//		SIGNAL_API Mods ( const Mods & copytron ) ;

		/*! \brief Get the name of modVal.
		 *
		 * Checks from lower bits up, and only returns one string
		 * for the first found.
		 */
		static SIGNAL_API std::string getModName ( const unsigned int modVal ) ;
		//! \brief Get the bit value of modName.
		static SIGNAL_API unsigned int getMod ( std::string modName ) ;
		//! \brief Get the mod bit value of a key code.
		static SIGNAL_API unsigned int modFromKey ( const int key ) ;

		//! \brief This mod as an unsigned int (bit values)
		SIGNAL_API unsigned int get ( ) const ;
		//! \brief Assign a set of bits as this mod.
		SIGNAL_API void set ( const unsigned int newMod ) ;
		//! \brief Assign a set of bits as this mod.
		SIGNAL_API void set ( const long newMod ) ;
		/*! \brief Assign a set of bits as this mod, from a set of mod values.
		 *
		 * \param newMods An array of bit values which are all assigned to
		 * this mod. ( bitwise OR, then set this mod as that value )
		 * \param length The number of values to OR together.
		 */
		SIGNAL_API void set ( const unsigned int * newMods, const unsigned int length ) ;
		//! \brief newMod is valued with this mod ( bitwise OR )
		SIGNAL_API void add ( const unsigned int newMod ) ;
		//! \brief hasMod returns true if this mod contains all bits from modVal.
		SIGNAL_API bool hasMod ( const unsigned int modVal ) const ;
		//! \brief Removes all values in delMod from this mod.
		SIGNAL_API void remove ( const unsigned int delMod ) ;

		//! \brief Get the current key up type.
		SIGNAL_API KeyUpType getKeyUp ( ) const ;
		//! \brief Set a new key up type as current.
		SIGNAL_API void setKeyUp ( KeyUpType keyUp ) ;

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
		/*! \brief Receive named unsinged int value.
		 * \ref ICallNamedUnsigned::call(char *, unsigned int)
		 */
		virtual SIGNAL_API void call ( char * name, unsigned int value ) ;
		/*! \brief Receive named int value.
		 * \ref ICallNamedInt::call(char *, int)
		 */
		virtual SIGNAL_API void call ( char * name, int value ) ;

		// Members are native, no operator = needed.
		// True if rhs has the same value as this.
		// "rhs" The value to check against.
		// Return True if rhs mods are the same as this.
		inline bool operator == ( const Mods & rhs ) const
		{
			bool outVal = get ( ) == rhs.get ( ) ;
			if ( ! outVal )
				return false ;
			outVal = getKeyUp ( ) == rhs.getKeyUp ( ) ;
			return ( get ( ) == rhs.get ( ) ) ;

		}
		// True if rhs does not have the same value as this.
		// "rhs">The value to check against.
		// Return True if rhs mods are not the same as this.
		inline bool operator != ( const Mods & rhs ) const
		{
			return !( *this == rhs ) ;

		}
		// True if rhs values are less than this.
		// "rhs" The value to check against.
		// Return True if rhs mods are less than this.
		inline bool operator < ( const Mods & rhs ) const
		{
			bool modori ;

			modori = get ( ) < rhs.get ( ) ;
			if ( ! modori )
				return false ;
			modori = getKeyUp ( ) < rhs.getKeyUp ( ) ;

			return modori ;
		}
		// True if rhs values are greater than this.
		// "rhs" The value to check against.
		// returns True if rhs mods are greater than this.
		inline bool operator > ( const Mods & rhs ) const
		{
			bool modori ;

			modori  = get ( ) > rhs.get ( ) ;
			if ( ! modori )
				return false ;
			modori = getKeyUp ( ) > rhs.getKeyUp ( ) ;

			return modori ;
		}
		// True if rhs values are less than or equal to this.
		// "rhs" The value to check against.
		// returns True if rhs mods are less than or equal to this.
		inline bool operator <= ( const Mods & rhs ) const
		{
			bool modori ;

			modori = get ( ) <= rhs.get ( ) ;

			return modori ;
		}
		// True if rhs values are greater than or equal to this.
		// "rhs" The value to check against.
		// returns True if rhs mods are greater than or equal to this.
		inline bool operator >= ( const Mods & rhs ) const
		{
			bool modori ;

			modori = get ( ) >= rhs.get ( ) ;

			return modori ;
		}

	protected:
	private:
		// Bit value representation of this mod.
		std::uint32_t _values ;
		KeyUpType _keyUp ;

		static std::map < std::string, unsigned int > _modVals ;
		static std::map < unsigned int, std::string > _modNames ;

		static std::map < std::string, unsigned int > initVals ( ) ;
		static std::map < unsigned int, std::string > initNames ( ) ;
	};
}

#endif

