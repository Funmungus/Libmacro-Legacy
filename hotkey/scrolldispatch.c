
# include "hotkey/scrolldispatch.h"

namespace macro
{

	ScrollDispatch::map_t ScrollDispatch::HotSpecified ;

	void ScrollDispatch::reset ( )
	{
		release ( ) ;
		Dispatch<Scroll>::reset ( ) ;
	}

	void ScrollDispatch::release ( )
	{
		HotSpecified.clear ( ) ;
		Dispatch<Scroll>::release ( ) ;
	}

	void ScrollDispatch::addSpecific ( Hot<Scroll> & newHotkey,
									Scroll *, int * modsPt )
	{
		// interceptPt not used.
		// Unspecific code is 0, unspecific modifier is -1.
		int mapping = modsPt == NULL ? -1 : * modsPt ;

		// If completely unspecific, do not use specific dispatch.
		if ( mapping == -1 )
		{
			Dispatch<Scroll>::add ( newHotkey ) ;
			return ;
		}

		addVectorItem < int, Hot<Scroll> * > ( HotSpecified,
											mapping, & newHotkey ) ;
	}

	void ScrollDispatch::removeSpecific ( Hot<Scroll> & delHotkey )
	{
		removeVectorItem < int, Hot<Scroll> * > ( HotSpecified, & delHotkey ) ;
		Dispatch<Scroll>::remove ( delHotkey ) ;
	}

}
