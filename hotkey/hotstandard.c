/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

# include "hotkey/hotstandard.h"

MCR_API mcr_IHot mcr_iHot ;
MCR_API mcr_IHot mcr_iHotStaged ;

void mcr_Hot_load_contract ( )
{
	mcr_IHot * hots [ ] =
	{
		& mcr_iHot, & mcr_iHotStaged
	} ;
	const char * names [ ] =
	{
		"Hotkey", "StagedHotkey"
	} ;
	const char * addNames [ ] [ 5 ] =
	{
		{"Hot"}, {"staged hotkey", "staged_hotkey", "HotStaged",
				"hot staged", "hot_staged"}
	} ;
	const size_t addLens [ ] =
	{
		1, 5
	} ;
	int len = sizeof ( hots ) / sizeof ( mcr_ISignal * ) ;
	for ( int i = 0 ; i < len ; i ++ )
	{
		mcr_IHot_set_names ( hots [ i ], names [ i ], addNames [ i ],
				addLens [ i ] ) ;
	}
}

void mcr_HotStaged_set_style ( mcr_HotStaged * hotPt,
		mcr_BlockStyle style )
{
	dassert ( hotPt ) ;
	hotPt->style = style ;
	mcr_HotStaged_set_blocking ( hotPt, 1 ) ;
}

int mcr_HotStaged_blocking ( mcr_HotStaged * hotPt )
{
	dassert ( hotPt ) ;
	if ( ! hotPt->stages.used )
		return 0 ;
	mcr_Stage * it = MCR_ARR_AT ( hotPt->stages, 0 ),
			 * end = MCR_ARR_END ( hotPt->stages ) ;
	switch ( hotPt->style )
	{
	case MCR_UNMANAGED :
		for ( ; it < end ; it = MCR_ARR_NEXT ( hotPt->stages, it ) )
		{
			if ( it->blocking )
				return 1 ;
		}
		return 0 ;
	case MCR_NOBLOCK :
		return 0 ;
	case MCR_BLOCK_FINAL :
		it = MCR_ARR_PREV ( hotPt->stages, end ) ;
		return it->blocking ;
	case MCR_BLOCK_ALL :
		for ( ; it < end ; it = MCR_ARR_NEXT ( hotPt->stages, it ) )
		{
			if ( ! it->blocking )
				return 0 ;
		}
		return 1 ;
	}
	return 0 ;
}

void mcr_HotStaged_set_blocking ( mcr_HotStaged * hotPt,
		int blocking )
{
	dassert ( hotPt ) ;
	if ( ! hotPt->stages.used )
		return ;
	mcr_Stage * it = MCR_ARR_AT ( hotPt->stages, 0 ),
			 * end = MCR_ARR_END ( hotPt->stages ) ;
	switch ( hotPt->style )
	{
	case MCR_UNMANAGED :
		break ;
		// No blocking always disables all blocking.
	case MCR_NOBLOCK :
		for ( ; it < end ; it = MCR_ARR_NEXT ( hotPt->stages, it ) )
		{
			it->blocking = 0 ;
		}
		break ;
	case MCR_BLOCK_FINAL :
		it = MCR_ARR_PREV ( hotPt->stages, end ) ;
		it->blocking = blocking ;
		break ;
	case MCR_BLOCK_ALL :
		for ( ; it < end ; it = MCR_ARR_NEXT ( hotPt->stages, it ) )
		{
			it->blocking = blocking ;
		}
		break ;
	}
}

int mcr_HotStaged_push ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers,
		mcr_TriggerFor trigFor )
{
	dassert ( hotPt ) ;
	mcr_Stage p ;
	memset ( & p, 0, sizeof ( mcr_Stage ) ) ;
	mcr_Stage_set ( & p, interceptPt, modifiers, trigFor ) ;
	return mcr_Array_push ( & hotPt->stages, & p ) ;
}

int mcr_HotStaged_push_with ( mcr_HotStaged * hotPt, int blocking,
		mcr_Signal * interceptPt, unsigned int measurement_error,
		unsigned int modifiers,
		mcr_TriggerFor trigFor )
{
	dassert ( hotPt ) ;
	mcr_Stage p ;
	mcr_Stage_init_with ( & p, blocking, interceptPt, measurement_error,
			modifiers, trigFor ) ;
	return mcr_Array_push ( & hotPt->stages, & p ) ;
}

int mcr_HotStaged_insert ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers,
mcr_TriggerFor trigFor, size_t index )
{
	dassert ( hotPt ) ;
	mcr_Stage any, i ;
	memset ( & any, 0, sizeof ( mcr_Stage ) ) ;
	memset ( & i, 0, sizeof ( mcr_Stage ) ) ;
	mcr_Stage_set ( & any, NULL, MCR_ANY_MOD, trigFor ) ;
	mcr_Stage_set ( & i, interceptPt, modifiers, trigFor ) ;
	return mcr_Array_insert_filled ( & hotPt->stages, index, & i,
			& any ) ;
}

int mcr_HotStaged_insert_with ( mcr_HotStaged * hotPt,
		int blocking, mcr_Signal * interceptPt, unsigned int measurement_error,
		unsigned int modifiers,
mcr_TriggerFor trigFor, size_t index )
{
	dassert ( hotPt ) ;
	mcr_Stage any, i ;
	mcr_Stage_init_with ( & any, 0, NULL, 0, MCR_ANY_MOD, trigFor ) ;
	mcr_Stage_init_with ( & i, blocking, interceptPt, measurement_error,
			modifiers, trigFor ) ;
	return mcr_Array_insert_filled ( & hotPt->stages, index, & i,
			& any ) ;
}

int mcr_HotStaged_set ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers,
mcr_TriggerFor trigFor, size_t index )
{
	dassert ( hotPt ) ;
	mcr_Stage s ;
	memset ( & s, 0, sizeof ( mcr_Stage ) ) ;
	mcr_Stage_set ( & s, interceptPt, modifiers, trigFor ) ;
	return mcr_Array_set ( & hotPt->stages, index, & s ) ;
}

int mcr_HotStaged_set_with ( mcr_HotStaged * hotPt,
		int blocking, mcr_Signal * interceptPt, unsigned int measurement_error,
		unsigned int modifiers,
mcr_TriggerFor trigFor, size_t index )
{
	dassert ( hotPt ) ;
	mcr_Stage s ;
	mcr_Stage_init_with ( & s, blocking, interceptPt, measurement_error,
			modifiers, trigFor ) ;
	return mcr_Array_set ( & hotPt->stages, index, & s ) ;
}

static void deactivate_redirect ( mcr_Stage * stagePt, ... )
{
	stagePt->activated = 0 ;
}
void mcr_deactivate_stages ( mcr_Array * stagesPt )
{
	MCR_ARR_FOR_EACH ( * stagesPt, deactivate_redirect, 0 ) ;
}

int mcr_HotStaged_compare ( const void * lhs, const void * rhs )
{
	dassert ( lhs ) ;
	dassert ( rhs ) ;
	const mcr_HotStaged * lPt = lhs, * rPt = rhs ;
	if ( lPt->stages.used != rPt->stages.used )
		return lPt->stages.used < rPt->stages.used ? -1 : 1 ;
	if ( lPt->style != rPt->style )
		return lPt->style < rPt->style ? -1 : 1 ;
	int ret = 0 ;
	mcr_Stage * lSt = MCR_ARR_AT ( lPt->stages, 0 ),
			 * rSt = MCR_ARR_AT ( rPt->stages, 0 ),
			 * end = MCR_ARR_END ( lPt->stages ) ;
	while ( ! ret && lSt < end )
	{
		ret = mcr_Stage_compare ( lSt ++, rSt ++ ) ;
	}
	return ret ;
}

void mcr_HotStaged_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	mcr_HotStaged * dPt = dstPt, * sPt = srcPt ;
	dPt->style = sPt->style ;
	if ( dPt->stages.used )
		mcr_HotStaged_free ( dstPt ) ;
	if ( mcr_Array_from_array ( & dPt->stages, sPt->stages.array, sPt->stages.used ) )
	{
		mcr_Stage * dSt = MCR_ARR_AT ( dPt->stages, 0 ),
				 * sSt = MCR_ARR_AT ( sPt->stages, 0 ),
				 * end = MCR_ARR_END ( dPt->stages ) ;
		while ( dSt < end )
		{
			// copied reference, reset.
			dSt->intercept.data.data = NULL ;
			dSt->intercept.data.is_heap = 0 ;
			mcr_Signal_copy ( & ( dSt ++ )->intercept,
					& ( sSt ++ )->intercept ) ;
		}
	}
}

void mcr_HotStaged_init ( void * hotPt )
{
	if ( ! hotPt )
		return ;
	memset ( hotPt, 0, sizeof ( mcr_HotStaged ) ) ;
	mcr_Array_init ( & ( ( mcr_HotStaged * ) hotPt )->stages,
			sizeof ( mcr_Stage ) ) ;
}

void mcr_HotStaged_init_with ( mcr_HotStaged * hotPt,
		mcr_Stage * stages, size_t stageCount,
		mcr_BlockStyle style )
{
	if ( ! hotPt )
		return ;
	mcr_HotStaged_init ( hotPt ) ;
	hotPt->style = style ;
	if ( stages && stageCount )
		mcr_Array_from_array ( & hotPt->stages, stages, stageCount ) ;
}

void mcr_HotStaged_free ( void * hotPt )
{
	dassert ( hotPt ) ;
	MCR_ARR_FOR_EACH ( ( ( mcr_HotStaged * ) hotPt )->stages,
			mcr_Stage_free_foreach, ) ;
	mcr_Array_free ( & ( ( mcr_HotStaged * ) hotPt )->stages ) ;
}

void mcr_HotStaged_trigger ( mcr_Hot * hotPt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	dassert ( hotPt ) ;
	mcr_HotStaged * shotPt = hotPt->data.data ;
	hotPt->block = 0 ;
	// No elements? unblock, then trigger.
	if ( ! shotPt->stages.used )
	{
		hotPt->block = 0 ;
		mcr_HotStaged_on_complete ( hotPt, interceptPt, mods ) ;
		return ;
	}
	mcr_Stage * first = MCR_ARR_AT ( shotPt->stages, 0 ), // First.
			 * rit = MCR_ARR_PREV ( shotPt->stages,
			( MCR_ARR_END ( shotPt->stages ) ) ), // At last element.
			 * prev = MCR_ARR_PREV ( shotPt->stages, rit ) ;
			// One before last.
	// One element or previous, this may activate.
	// Currently triggering only blocks for the trigger stage.
	if ( shotPt->stages.used == 1 || prev->activated )
	{
		if ( MCR_STAGE_ISME ( * rit, interceptPt, mods ) )
		{
			hotPt->block = rit->blocking ;
			mcr_HotStaged_on_complete ( hotPt, interceptPt, mods ) ;
			mcr_deactivate_stages ( & shotPt->stages ) ;
			return ;
		}
		if ( shotPt->stages.used == 1 )
			return ;
		rit = prev ;
		prev = MCR_ARR_PREV ( shotPt->stages, prev ) ;
	}

	/* Do not check first item for conditional activation, but
	 * always check it at the end. */
	while ( rit != first )
	{
		if ( rit->activated )
		{
			if ( MCR_STAGE_RESEMBLES ( * rit, interceptPt, mods ) )
			{
				if ( rit->blocking )
					hotPt->block = 1 ;
			}
			else
				rit->activated = 0 ;
		}
		else if ( prev->activated )
		{
			if ( MCR_STAGE_ISME ( * rit, interceptPt, mods ) )
			{
				rit->activated = 1 ;
				if ( rit->blocking )
					hotPt->block = 1 ;
			}
		}
		rit = prev ;
		prev = MCR_ARR_PREV ( shotPt->stages, prev ) ;
	}
	// Should be at first element, and not previously checked.
	if ( MCR_STAGE_ISME ( * rit, interceptPt, mods ) )
	{
		rit->activated = 1 ;
		if ( rit->blocking )
			hotPt->block = 1 ;
	}
}

void mcr_hotstandard_initialize ( )
{
	mcr_IHot_init ( & mcr_iHot, 0 ) ;
	mcr_IHot_init_with ( & mcr_iHotStaged, mcr_HotStaged_compare,
			mcr_HotStaged_copy, sizeof ( mcr_HotStaged ),
			mcr_HotStaged_init, mcr_HotStaged_free, mcr_HotStaged_trigger ) ;
	if ( mcr_IHot_register ( & mcr_iHot ) == ( size_t ) -1 )
	{
		dmsg ;
	}
	if ( mcr_IHot_register ( & mcr_iHotStaged ) == ( size_t ) -1 )
	{
		dmsg ;
	}
}

void mcr_hotstandard_cleanup ( )
{

}
