
# ifndef MCR_INLINE_SIGNALMOD_H
# define MCR_INLINE_SIGNALMOD_H

# include "hotkey/signalmod.h"

namespace macro
{
	SignalMod::SignalMod ( const unsigned int values, const KeyUpType keyUp )
		: mods ( values, keyUp )
	{}
	SignalMod::SignalMod ( const SignalMod & copytron )
		: mods ( copytron.mods )
	{}
	SignalMod & SignalMod::operator = ( const SignalMod & copytron )
	{
		mods = copytron.mods ;
		return * this ;
	}

	// ISignal
	bool SignalMod::send ( )
	{
		if ( dispatch ( ) ) return true ;
		bool error = false ;
		// Go through all keys.
		for ( auto it = SigToMod.firstMap ( ).begin ( ) ;
			it != SigToMod.firstMap ( ).end ( ) ;
			it ++ )
		{
			if ( Mods::hasMod ( mods.first, it->second.first ) &&
				mods.second == it->second.second )
			{
				error = it->first->send ( ) ? error : true ;
			}
		}
		return ! error ;
	}
//	bool SignalMod::operator ( ) ( )
//	{
//		return send ( ) ;
//	}
//	bool SignalMod::dispatch ( )
//	{
//		return Signal < SignalMod >::Dispatcher ( * this ) || ISignal::dispatch ( ) ;
//	}

	// operators
	SignalMod SignalMod::operator - ( ) const
	{
		return SignalMod ( mods.first, mods.second == DOWN ? UP : DOWN ) ;
	}
	bool SignalMod::operator == ( const SignalMod & rhs ) const
	{
		return mods == rhs.mods ;
	}
	bool SignalMod::operator < ( const SignalMod & rhs ) const
	{
		return mods < rhs.mods ;
	}
//	bool SignalMod::operator != ( const SignalMod & rhs ) const
//	{
//		return mods != rhs.mods ;
//	}
//	bool SignalMod::operator > ( const SignalMod & rhs ) const
//	{
//		return mods > rhs.mods ;
//	}
//	bool SignalMod::operator <= ( const SignalMod & rhs ) const
//	{
//		return mods <= rhs.mods ;
//	}
//	bool SignalMod::operator >= ( const SignalMod & rhs ) const
//	{
//		return mods >= rhs.mods ;
//	}
}

# endif // MCR_INLINE_SIGNALMOD_H
