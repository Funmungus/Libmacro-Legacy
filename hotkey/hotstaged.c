/* hotkey/hotstaged.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "hotkey/hotstaged.h"

void mcr_HotStaged_init ( mcr_HotStaged * hotPt )
{
	if ( ! hotPt )
		return ;
	mcr_Hot_init ( ( mcr_Hot * ) hotPt ) ;
	hotPt->hot.trigger = mcr_HotStaged_trigger ;
	mcr_Array_init ( & hotPt->stages, sizeof ( mcr_Stage ) ) ;
	hotPt->on_complete = NULL ;
	hotPt->style = MCR_UNMANAGED ;
}

void mcr_HotStaged_init_with ( mcr_HotStaged * hotPt,
		void * data, mcr_trigger_fnc onComplete,
		mcr_BlockStyle style )
{
	if ( ! hotPt )
		return ;
	mcr_Hot_init_with ( ( mcr_Hot * ) hotPt, 0, mcr_HotStaged_trigger, data ) ;
	mcr_Array_init ( & hotPt->stages, sizeof ( mcr_Stage ) ) ;
	hotPt->on_complete = onComplete ;
	hotPt->style = style ;
}

void mcr_HotStaged_free ( mcr_HotStaged * hotPt )
{
	dassert ( hotPt ) ;
	mcr_Array_free ( & hotPt->stages ) ;
}

void mcr_HotStaged_set_style ( mcr_HotStaged * hotPt,
		mcr_BlockStyle style )
{
	dassert ( hotPt ) ;
	hotPt->style = style ;
	mcr_HotStaged_enable_blocking ( hotPt, 1 ) ;
}

int mcr_HotStaged_is_blocking ( mcr_HotStaged * hotPt )
{
	dassert ( hotPt ) ;
	if ( ! hotPt->stages.used )
		return 0 ;
	mcr_Stage * it = MCR_ARR_AT ( & hotPt->stages, 0 ),
			 * end = MCR_ARR_END ( & hotPt->stages ) ;
	switch ( hotPt->style )
	{
	case MCR_UNMANAGED :
		for ( ; it < end ; it = MCR_ARR_NEXT ( & hotPt->stages, it ) )
		{
			if ( it->blocking )
				return 1 ;
		}
		return 0 ;
	case MCR_NOBLOCK :
		return 0 ;
	case MCR_BLOCK_FINAL :
		it = MCR_ARR_PREV ( & hotPt->stages, end ) ;
		return it->blocking ;
	case MCR_BLOCK_ALL :
		for ( ; it < end ; it = MCR_ARR_NEXT ( & hotPt->stages, it ) )
		{
			if ( ! it->blocking )
				return 0 ;
		}
		return 1 ;
	}
	return 0 ;
}

void mcr_HotStaged_enable_blocking ( mcr_HotStaged * hotPt,
		int blocking )
{
	dassert ( hotPt ) ;
	if ( ! hotPt->stages.used )
		return ;
	mcr_Stage * it = MCR_ARR_AT ( & hotPt->stages, 0 ),
			 * end = MCR_ARR_END ( & hotPt->stages ) ;
	switch ( hotPt->style )
	{
	case MCR_UNMANAGED :
		break ;
		// No blocking always disables all blocking.
	case MCR_NOBLOCK :
		for ( ; it < end ; it = MCR_ARR_NEXT ( & hotPt->stages, it ) )
		{
			it->blocking = 0 ;
		}
		break ;
	case MCR_BLOCK_FINAL :
		it = MCR_ARR_PREV ( & hotPt->stages, end ) ;
		it->blocking = blocking ;
		break ;
	case MCR_BLOCK_ALL :
		for ( ; it < end ; it = MCR_ARR_NEXT ( & hotPt->stages, it ) )
		{
			it->blocking = blocking ;
		}
		break ;
	}
}

int mcr_HotStaged_push ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers )
{
	dassert ( hotPt ) ;
	mcr_Stage p ;
	memset ( & p, 0, sizeof ( mcr_Stage ) ) ;
	mcr_Stage_set ( & p, interceptPt, modifiers ) ;
	return mcr_Array_push ( & hotPt->stages, & p ) ;
}

int mcr_HotStaged_push_with ( mcr_HotStaged * hotPt, int blocking,
		mcr_Signal * interceptPt, size_t measurement_error,
		unsigned int modifiers )
{
	dassert ( hotPt ) ;
	mcr_Stage p ;
	mcr_Stage_init_with ( & p, blocking, interceptPt, measurement_error, modifiers ) ;
	return mcr_Array_push ( & hotPt->stages, & p ) ;
}

int mcr_HotStaged_insert ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers, size_t index )
{
	dassert ( hotPt ) ;
	mcr_Stage any, i ;
	memset ( & any, 0, sizeof ( mcr_Stage ) ) ;
	memset ( & i, 0, sizeof ( mcr_Stage ) ) ;
	mcr_Stage_set ( & any, NULL, MCR_ANY_MOD ) ;
	mcr_Stage_set ( & i, interceptPt, modifiers ) ;
	return mcr_Array_insert_filled ( & hotPt->stages, index, & i, & any ) ;
}

int mcr_HotStaged_insert_with ( mcr_HotStaged * hotPt,
		int blocking, mcr_Signal * interceptPt, size_t measurement_error,
		unsigned int modifiers, size_t index )
{
	dassert ( hotPt ) ;
	mcr_Stage any, i ;
	mcr_Stage_init_with ( & any, 0, NULL, 0, MCR_ANY_MOD ) ;
	mcr_Stage_init_with ( & i, blocking, interceptPt, measurement_error, modifiers ) ;
	return mcr_Array_insert_filled ( & hotPt->stages, index, & i, & any ) ;
}

int mcr_HotStaged_set ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers, size_t index )
{
	dassert ( hotPt ) ;
	mcr_Stage s ;
	memset ( & s, 0, sizeof ( mcr_Stage ) ) ;
	mcr_Stage_set ( & s, interceptPt, modifiers ) ;
	return mcr_Array_set ( & hotPt->stages, index, & s ) ;
}

int mcr_HotStaged_set_with ( mcr_HotStaged * hotPt,
		int blocking, mcr_Signal * interceptPt, size_t measurement_error,
		unsigned int modifiers, size_t index )
{
	dassert ( hotPt ) ;
	mcr_Stage s ;
	mcr_Stage_init_with ( & s, blocking, interceptPt, measurement_error, modifiers ) ;
	return mcr_Array_set ( & hotPt->stages, index, & s ) ;
}

void mcr_HotStaged_trigger ( mcr_Hot * hotPt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	dassert ( hotPt ) ;
	mcr_HotStaged * shotPt = ( mcr_HotStaged * ) hotPt ;
	shotPt->hot.block = 0 ;
	// No elements? unblock, then trigger.
	if ( ! shotPt->stages.used )
	{
		MCR_HOTSTAGED_ONCOMPLETE ( shotPt, interceptPt, mods ) ;
		return ;
	}
	mcr_Stage * first = MCR_ARR_AT ( & shotPt->stages, 0 ), // First.
			 * rit = MCR_ARR_PREV ( & shotPt->stages,
			( MCR_ARR_END ( & shotPt->stages ) ) ), 	// At last element.
			 * prev = MCR_ARR_PREV ( & shotPt->stages, rit ) ;
			// One before last.
	// One element or previous, this may activate.
	// Currently triggering only blocks for the trigger stage.
	if ( shotPt->stages.used == 1 || prev->activated )
	{
		dassert ( rit->isme != NULL ) ;
		if ( rit->isme ( rit, interceptPt, mods ) )
		{
			MCR_HOTSTAGED_ONCOMPLETE ( shotPt, interceptPt, mods ) ;
			hotPt->block = rit->blocking ;
			mcr_deactivate_stages ( & shotPt->stages ) ;
			return ;
		}
	}
	if ( shotPt->stages.used == 1 )
		return ;

	/* Do not check first item for conditional activation, but
	 * always check it at the end. */
	do
	{
		dassert ( rit->isme != NULL ) ;
		/* Activated signals only need to have same type to
		 * stay activated and possibly block. */
		if ( rit->activated )
		{
			if ( MCR_STAGE_ISTYPE ( rit, interceptPt ) )
			{
				if ( rit->blocking )
					hotPt->block = 1 ;
			}
			else
				rit->activated = 0 ;
		}
		/* Non-activated signals can only be activated if previous
		 * has already been activated. */
		else if ( prev->activated )
		{
			if ( rit->isme ( rit, interceptPt, mods ) )
			{
				rit->activated = 1 ;
				if ( rit->blocking )
					hotPt->block = 1 ;
			}
		}
		rit = prev ;
		prev = MCR_ARR_PREV ( & shotPt->stages, prev ) ;
	} while ( rit != first ) ;
	if ( rit->isme ( rit, interceptPt, mods ) )
	{
		rit->activated = 1 ;
		if ( rit->blocking )
			hotPt->block = 1 ;
	}
}

static void deactivate_redirect ( mcr_Stage * stagePt,... )
{
	stagePt->activated = 0 ;
}

void mcr_deactivate_stages ( mcr_Array * stagesPt )
{
	MCR_ARR_FOR_EACH ( stagesPt, deactivate_redirect, 0 ) ;
}
