
# ifndef MCR_INLINE_KEYMOD_H
# define MCR_INLINE_KEYMOD_H

# include "hotkey/keymod.h"

namespace macro
{
	KeyMod::KeyMod ( const unsigned int values, KeyUpType keyUp )
		: mods ( values, keyUp )
	{}
	KeyMod::KeyMod ( const KeyMod & copytron )
		: mods ( copytron.mods )
	{}
	KeyMod & KeyMod::operator = ( const KeyMod & copytron )
	{
		mods = copytron.mods ;
		return * this ;
	}

	// ISignal
	bool KeyMod::send ( )
	{
		if ( dispatch ( ) ) return true ;
		bool error = false ;
		static Key key ( 0, 0, mods.second ) ;
		static std::mutex kl ;
		kl.lock ( ) ;
		// Go through all keys.
		for ( auto it = KeyToMod.firstMap ( ).begin ( ) ;
			it != KeyToMod.firstMap ( ).end ( ) ;
			it ++ )
		{
			if ( Mods::hasMod ( mods.first, it->second ) )
			{
				key.setKey ( it->first ) ;
				error = key ( ) ? error : true ;
			}
		}
		kl.unlock ( ) ;
		return ! error ;
	}

	// operators
	KeyMod KeyMod::operator - ( ) const
	{
		return KeyMod ( mods.first, ( mods.second == DOWN ) ? UP : DOWN ) ;
	}
	bool KeyMod::operator == ( const KeyMod & rhs ) const
	{
		return mods == rhs.mods ;
	}
	bool KeyMod::operator < ( const KeyMod & rhs ) const
	{
		return mods < rhs.mods ;
	}
//	bool KeyMod::operator != ( const KeyMod & rhs ) const
//	{
//		return mods != rhs.mods ;
//	}
//	bool KeyMod::operator > ( const KeyMod & rhs ) const
//	{
//		return mods > rhs.mods ;
//	}
//	bool KeyMod::operator <= ( const KeyMod & rhs ) const
//	{
//		return mods <= rhs.mods ;
//	}
//	bool KeyMod::operator >= ( const KeyMod & rhs ) const
//	{
//		return mods >= rhs.mods ;
//	}
}

# endif // MCR_INLINE_KEYMOD_H
