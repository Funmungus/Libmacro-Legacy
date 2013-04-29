
# ifndef MCR_INLINE_MODS_H
# define MCR_INLINE_MODS_H

# include "hotkey/mods.h"

namespace macro
{
	void Mods::modify ( unsigned int & container, const KeyUpType containerKeyUp, const unsigned int modifier, const KeyUpType modifierKeyUp )
	{
		if ( containerKeyUp == DOWN )
			modify ( container, modifier, modifierKeyUp ) ;
		else
			modify ( container, modifier, ( modifierKeyUp == DOWN ) ? UP : DOWN ) ;
	}
	void Mods::modify ( unsigned int & container, const unsigned int modifier, const KeyUpType keyUp )
	{
		if ( keyUp == DOWN )
			add ( container, modifier ) ;
		else
			remove ( container, modifier ) ;
	}
	void Mods::modify ( unsigned int & container, const unsigned int modifier )
	{
		add ( container, modifier ) ;
	}

	bool Mods::hasMod ( const unsigned int container, const unsigned int modVal )
	{
		return ( container & modVal ) == modVal ;
	}

	void Mods::add ( unsigned int & container, const unsigned int newMod )
	{
		container |= newMod ;
	}

	void Mods::remove ( unsigned int & container, const unsigned int delMod )
	{
		container & = ( ~delMod ) ;
	}
}

# endif
