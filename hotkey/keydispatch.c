
# include "hotkey/keydispatch.h"

namespace macro
{
	KeyDispatch::codemap_t KeyDispatch::HotByKey [ 2 ] ;
	KeyDispatch::codemap_t KeyDispatch::HotByScan [ 2 ] ;

	void KeyDispatch::reset ( )
	{
		release ( ) ;
		Dispatch<Key>::reset ( ) ;
	}

	void KeyDispatch::release ( )
	{
		HotByKey [ 0 ].clear ( ) ;
		HotByKey [ 1 ].clear ( ) ;
		HotByScan [ 0 ].clear ( ) ;
		HotByScan [ 1 ].clear ( ) ;
		Dispatch<Key>::release ( ) ;
	}

	void KeyDispatch::addSpecific ( Hot<Key> & newHotkey, Key * interceptPt,
									int * modsPt )
	{
		void ( & addFunction ) ( codemap_t &, code_mod_t &, Hot<Key> * ) =
			addVectorItem < code_mod_t, Hot<Key> * > ;
		// Unspecific code is 0, unspecific modifier is -1.
		code_mod_t keyed ( interceptPt == NULL ? 0 : interceptPt->getKey ( ),
						modsPt == NULL ? -1 : * modsPt ) ;
		code_mod_t scanned ( interceptPt == NULL ? 0 : interceptPt->getScan ( ),
						modsPt == NULL ? -1 : * modsPt ) ;

		// If completely unspecific, do not use specific dispatch.
		// Unspecific intercept if pointer is NULL, or key and scan are both 0.
		bool unspecific = keyed.first == 0 && scanned.first == 0 ;
		if ( unspecific && keyed.second == -1 )
		{
			Dispatch<Key>::add ( newHotkey ) ;
			return ;
		}

		KeyUpType keyUp = interceptPt == NULL ? BOTH : interceptPt->getKeyUp ( ) ;
		// We are sure to be mapping specific in some way. If unspecific code, add
		// to keyed mapping, modifier specified.
		if ( unspecific )
		{
			// keyed is unspecific code, specific modifier.
			if ( keyUp != UP )
			{
				addFunction ( HotByKey [ 0 ], keyed, & newHotkey ) ;
			}
			if ( keyUp != DOWN )
			{
				addFunction ( HotByKey [ 1 ], keyed, & newHotkey ) ;
			}
			return ;
		}

		// Specific key, may or may not be modified.
		if ( keyed.first != 0 )
		{
			if ( keyUp != UP )
				addFunction ( HotByKey [ 0 ], keyed, & newHotkey ) ;
			if ( keyUp != DOWN )
				addFunction ( HotByKey [ 1 ], keyed, & newHotkey ) ;
		}
		// Specific scan. Only add one of either key or scan.
		else if ( scanned.first != 0 )
		{
			if ( keyUp != UP )
				addFunction ( HotByScan [ 0 ], scanned, & newHotkey ) ;
			if ( keyUp != DOWN )
				addFunction ( HotByScan [ 1 ], scanned, & newHotkey ) ;
		}
	}

	void KeyDispatch::removeSpecific ( Hot<Key> & delHotkey )
	{
		void ( & removeFunction ) ( codemap_t &, Hot<Key> * ) =
			removeVectorItem < code_mod_t, Hot<Key> * > ;
		removeFunction ( HotByKey [ 0 ], & delHotkey ) ;
		removeFunction ( HotByKey [ 1 ], & delHotkey ) ;
		removeFunction ( HotByScan [ 0 ], & delHotkey ) ;
		removeFunction ( HotByScan [ 1 ], & delHotkey ) ;
		Dispatch<Key>::remove ( delHotkey ) ;
	}
}
