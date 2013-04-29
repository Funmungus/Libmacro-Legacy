
# include "hotkey/cursordispatch.h"

namespace macro
{
	CursorDispatch::map_t CursorDispatch::HotJustified ;
	CursorDispatch::map_t CursorDispatch::HotAbsolute ;

	void CursorDispatch::reset ( )
	{
		release ( ) ;
		Dispatch<MoveCursor>::reset ( ) ;
	}

	void CursorDispatch::release ( )
	{
		HotJustified.clear ( ) ;
		HotAbsolute.clear ( ) ;
		Dispatch<MoveCursor>::release ( ) ;
	}

	void CursorDispatch::addSpecific ( Hot<MoveCursor> & newHotkey,
									MoveCursor *, int * modsPt )
	{
		// Values of interceptPt not used.
		// Unspecific modifier is -1.
		int mapping = modsPt == NULL ? -1 : * modsPt ;

		// If completely unspecific, do not use specific dispatch.
		if ( mapping == -1 )
		{
			Dispatch<MoveCursor>::add ( newHotkey ) ;
			return ;
		}

		addVectorItem < int, Hot<MoveCursor> * > ( HotSpecified,
												mapping, & newHotkey ) ;
	}

	void CursorDispatch::removeSpecific ( Hot<MoveCursor> & delHotkey )
	{
		removeVectorItem < int, Hot<MoveCursor> * > ( HotSpecified, & delHotkey ) ;
		Dispatch<MoveCursor>::remove ( delHotkey ) ;
	}
}
