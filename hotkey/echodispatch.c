
# include "hotkey/echodispatch.h"

namespace macro
{
	EchoDispatch::codemap_t EchoDispatch::HotSpecified ;

	void EchoDispatch::reset ( )
	{
		release ( ) ;
		Dispatch<HIDEcho>::reset ( ) ;
	}

	void EchoDispatch::release ( )
	{
		HotSpecified.clear ( ) ;
		Dispatch<HIDEcho>::release ( ) ;
	}

	void EchoDispatch::addSpecific ( Hot<HIDEcho> & newHotkey,
									HIDEcho * interceptPt, int * modsPt )
	{
		// Unspecific code is 0, unspecific modifier is -1.
		code_mod_t mapping ( interceptPt == NULL ? -1 : interceptPt->getEvent ( ),
						modsPt == NULL ? -1 : * modsPt ) ;

		// If completely unspecific, do not use specific dispatch.
		if ( mapping.first == -1 && mapping.second == -1 )
		{
			Dispatch<HIDEcho>::add ( newHotkey ) ;
			return ;
		}

		addVectorItem < code_mod_t, Hot<HIDEcho> * > ( HotSpecified, mapping,
													& newHotkey ) ;
	}

	void EchoDispatch::removeSpecific ( Hot<HIDEcho> & delHotkey )
	{
		removeVectorItem < code_mod_t, Hot<HIDEcho> * > ( HotSpecified,
														& delHotkey ) ;
		Dispatch<HIDEcho>::remove ( delHotkey ) ;
	}
}
