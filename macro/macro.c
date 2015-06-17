/* macro/macro.c - Trigger a set of signals from a hotkey.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "macro/macro.h"

static int thread_macro ( void * ) ;
static int thread_wait_reset ( void * ) ;

# define set_macro_trigger( mcrPt ) \
	( mcrPt )->hot.trigger = mcr_Macro_trigger ; \
	( mcrPt )->hot.trigger_data = ( mcrPt ) ;

# define resetthread( mcrPt ) \
{ \
	thrd_t trd ; \
	if ( thrd_create ( & trd, thread_wait_reset, mcrPt ) == \
			thrd_success ) \
	{ \
		if ( thrd_detach ( trd ) != thrd_success ) \
		{ \
			dmsg ; \
		} \
		( mcrPt )->queued = 0 ; \
	} \
}

void mcr_Macro_init ( mcr_Macro * mcrPt )
{
	if ( ! mcrPt )
	{
		dmsg ;
		return ;
	}
	memset ( mcrPt, 0, sizeof ( mcr_Macro ) ) ;
	mtx_init ( & mcrPt->lock, mtx_plain ) ;
	cnd_init ( & mcrPt->cnd ) ;
	mcr_Hot_init ( & mcrPt->hot, & mcr_iHot ) ;
	set_macro_trigger ( mcrPt ) ;
	mcr_Array_init ( & mcrPt->signal_set, sizeof ( mcr_Signal ) ) ;
	mcrPt->thread_max = 1 ;
}

void mcr_Macro_init_with ( mcr_Macro * mcrPt,
		int sticky, unsigned int threadMax, mcr_Hot * hotPt,
		mcr_Signal * signalSet, size_t signalCount, int enable )
{
	if ( ! mcrPt )
	{
		dmsg ;
		return ;
	}
	mcr_Macro_init ( mcrPt ) ;
	mcrPt->sticky = sticky ;
	mcrPt->thread_max = threadMax ;
	mcr_Macro_set_hotkey ( mcrPt, hotPt ) ;
	mcr_Macro_set_signals ( mcrPt, signalSet, signalCount ) ;
	mcr_Macro_enable ( mcrPt, enable ) ;
}

void mcr_Macro_free ( mcr_Macro * mcrPt )
{
	if ( ! mcrPt )
	{
		dmsg ;
		return ;
	}
	mtx_lock ( & mcrPt->lock ) ;
	mcrPt->interruptor = MCR_INTERRUPT_DISABLE ;
	while ( mcrPt->threads )
	{
		cnd_wait ( & mcrPt->cnd, & mcrPt->lock ) ;
		mcrPt->interruptor = MCR_INTERRUPT_DISABLE ;
	}
	MCR_ARR_FOR_EACH ( & mcrPt->signal_set, mcr_Signal_free_foreach,
			0 ) ;
	mcr_Array_free ( & mcrPt->signal_set ) ;
	mtx_unlock ( & mcrPt->lock ) ;
	cnd_destroy ( & mcrPt->cnd ) ;
	mtx_destroy ( & mcrPt->lock ) ;
}

void mcr_Macro_set_hotkey ( mcr_Macro * mcrPt, mcr_Hot * hotPt )
{
	dassert ( mcrPt ) ;
	mtx_lock ( & mcrPt->lock ) ;
	int enableRet = mcrPt->interruptor != MCR_INTERRUPT_DISABLE ;
	mcrPt->interruptor = MCR_INTERRUPT_DISABLE ;
	mcr_Hot_copy ( & mcrPt->hot, hotPt ) ;
	set_macro_trigger ( mcrPt ) ;
	if ( enableRet )
	{
		mcrPt->interruptor = MCR_INTERRUPT_ALL ;
		resetthread ( mcrPt ) ;
	}
	mtx_unlock ( & mcrPt->lock ) ;
}

void mcr_Macro_interrupt ( mcr_Macro * mcrPt,
		mcr_Interrupt interruptType )
{
	dassert ( mcrPt ) ;
	mtx_lock ( & mcrPt->lock ) ;
	// Enable or disable, just make it happen.
	// Interrupt one or all, do not change from disabled.
	mcr_Interrupt prev = mcrPt->interruptor ;
	if ( ! interruptType || interruptType == MCR_INTERRUPT_DISABLE ||
			prev != MCR_INTERRUPT_DISABLE )
	{
		mcrPt->interruptor = interruptType ;
		/* Only start up a reset thread if not previously
		 * waiting for all signals to be interrupted, and
		 * we need now to wait for all to be interrupted.
		 **/
		if ( interruptType == MCR_INTERRUPT_ALL &&
				prev != MCR_INTERRUPT_ALL )
		{
			resetthread ( mcrPt ) ;
		}
	}
	mtx_unlock ( & mcrPt->lock ) ;
}

void mcr_Macro_get_signals ( mcr_Macro * mcrPt,
		mcr_Signal * signalbuffer, size_t bufferCount )
{
	dassert ( mcrPt ) ;
	dassert ( signalbuffer ) ;
	mtx_lock ( & mcrPt->lock ) ;
	mcr_Signal * arr = ( mcr_Signal * ) mcrPt->signal_set.array ;
	for ( size_t i = 0 ; i < mcrPt->signal_set.used &&
			i < bufferCount ; i ++ )
	{
		signalbuffer [ i ] = arr [ i ] ;
	}
	mtx_unlock ( & mcrPt->lock ) ;
}

void mcr_Macro_set_signals ( mcr_Macro * mcrPt,
		mcr_Signal * signalSet, size_t signalCount )
{
	dassert ( mcrPt ) ;
	mtx_lock ( & mcrPt->lock ) ;
	int enableRet = mcrPt->interruptor != MCR_INTERRUPT_DISABLE ;
	mcrPt->interruptor = MCR_INTERRUPT_DISABLE ;
	MCR_ARR_FOR_EACH ( & mcrPt->signal_set, mcr_Signal_free_foreach,
			0 ) ;
	mcrPt->signal_set.used = 0 ;
	if ( mcr_Array_resize ( & mcrPt->signal_set, signalCount ) )
	{
		mcr_Signal newSig ;
		for ( size_t i = 0 ; i < signalCount ; i ++ )
		{
			mcr_Signal_init ( & newSig, signalSet [ i ].type ) ;
			mcr_Signal_copy ( & newSig, signalSet + i ) ;
			if ( ! mcr_Array_push ( & mcrPt->signal_set,
					& newSig ) )
			{
				dmsg ;
				mcr_Signal_free ( & newSig ) ;
			}
		}
	}
	else
		dmsg ;
	if ( enableRet )
	{
		mcrPt->interruptor = MCR_INTERRUPT_ALL ;
		resetthread ( mcrPt ) ;
	}
	mtx_unlock ( & mcrPt->lock ) ;
}

void mcr_Macro_enable ( mcr_Macro * mcrPt, int enable )
{
	dassert ( mcrPt ) ;
	mcr_Macro_interrupt ( mcrPt, enable ? MCR_CONTINUE :
			MCR_INTERRUPT_DISABLE ) ;
}

void mcr_Macro_trigger ( mcr_Hot * hotPt, mcr_Signal * sigPt,
		unsigned int mods )
{
	dassert ( hotPt ) ;
	UNUSED ( sigPt ) ;
	UNUSED ( mods ) ;
	mcr_Macro * mcrPt = hotPt->trigger_data ;
	mtx_lock ( & mcrPt->lock ) ;
	/* If interrupting one, then we just do not increment this
	 * time. We are already locked for it. */
	if ( mcrPt->interruptor == MCR_INTERRUPT )
	{
		mcrPt->interruptor = MCR_CONTINUE ;
		mtx_unlock ( & mcrPt->lock ) ;
		return ;
	}
	if ( ! mcrPt->interruptor )
	{
		++ mcrPt->queued ;
		if ( ! mcrPt->thread_max ||
				mcrPt->thread_max == ( unsigned int ) -1 )
		{
			dmsg ;
			mcrPt->thread_max = 1 ;
		}
		// Thread max not reached.
		if ( mcrPt->threads < mcrPt->thread_max )
		{
			// TODO: Possible unused threads if only one queued item.
			thrd_t trd ;
			if ( thrd_create ( & trd, thread_macro, mcrPt ) ==
					thrd_success )
			{
				++ mcrPt->threads ;
				if ( thrd_detach ( trd ) != thrd_success )
				{
					dmsg ;
				}
			}
			else
				dmsg ;
		}
	}
	mtx_unlock ( & mcrPt->lock ) ;
}

# define ceptloop( mcrPt ) \
	( ! ( mcrPt )->interruptor || ( mcrPt )->interruptor == \
			MCR_INTERRUPT )

static int thread_macro ( void * data )
{
	mcr_Macro * mcrPt = data ;
	mcr_Signal * it, * end ;
	dassert ( mcrPt ) ;
	mtx_lock ( & mcrPt->lock ) ;
	// Loop only while interceptor states continue, or only one.
	// Loop only while triggers queued, or last thread is sticky.
	while ( ceptloop ( mcrPt ) && ( mcrPt->queued ||
			( mcrPt->sticky && mcrPt->threads == 1 ) ) )
	{
		if ( mcrPt->queued )
			-- mcrPt->queued ;
		mtx_unlock ( & mcrPt->lock ) ;
		// If sig set is changed, all threads should end.
		it = MCR_ARR_AT ( & mcrPt->signal_set, 0 ) ;
		end = MCR_ARR_END ( & mcrPt->signal_set ) ;
		while ( ceptloop ( mcrPt ) && it < end )
		{
			if ( mcrPt->interruptor == MCR_INTERRUPT &&
					mtx_trylock ( & mcrPt->lock ) )
			{
				/* Interrupt may change before locking. */
				if ( mcrPt->interruptor == MCR_INTERRUPT )
				{
					mcrPt->interruptor = MCR_CONTINUE ;
					mtx_unlock ( & mcrPt->lock ) ;
					break ;
				}
			}
			MCR_SEND ( it ) ;
		}
		mtx_lock ( & mcrPt->lock ) ;
	}
	// Is locked.
	-- mcrPt->threads ;
	cnd_broadcast ( & mcrPt->cnd ) ;
	mtx_unlock ( & mcrPt->lock ) ;
	return thrd_success ;
}

static int thread_wait_reset ( void * data )
{
	mcr_Macro * mcrPt = data ;
	dassert ( mcrPt ) ;
	mtx_lock ( & mcrPt->lock ) ;
	while ( mcrPt->threads && mcrPt->interruptor == MCR_INTERRUPT_ALL )
	{
		cnd_wait ( & mcrPt->cnd, & mcrPt->lock ) ;
	}
	// Either no threads or interrupt has changed.
	if ( mcrPt->interruptor == MCR_INTERRUPT_ALL )
	{
		mcrPt->interruptor = MCR_CONTINUE ;
	}
	mtx_unlock ( & mcrPt->lock ) ;
	return thrd_success ;
}
