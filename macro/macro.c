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

// Will attempt to gently detach all threads, but force termination
// if timed out.
static void clear_threads ( mcr_Macro * mcrPt, mcr_Interrupt clearType,
	int stickyInterrupt ) ;
//static void finish_threads ( mcr_Macro * mcrPt ) ;
//static void finish_delete_threads ( mcr_Macro * mcrPt ) ;

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
	mcrPt->thread_max = 1 ;
	cnd_init ( & mcrPt->cnd ) ;
	mtx_init ( & mcrPt->lock, mtx_plain ) ;
	mcr_Hot_init ( & mcrPt->hot, & mcr_iHot ) ;
	set_macro_trigger ( mcrPt ) ;
	mcr_Array_init ( & mcrPt->signal_set, sizeof ( mcr_Signal ) ) ;
	mcr_Array_init ( & mcrPt->signal_isdispatch, sizeof ( char ) ) ;
//	mcr_Map_init ( & mcrPt->thread_finish, sizeof ( thrd_t ),
//			sizeof ( char ) ) ;
//	mcrPt->thread_finish.compare = mcr_ref_compare ;
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

void mcr_Macro_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	mcr_Macro * dPt = dstPt, * sPt = srcPt ;
	dPt->sticky = sPt->sticky ;
	dPt->thread_max = sPt->thread_max ;
	mcr_Macro_set_hotkey ( dPt, & sPt->hot ) ;
	mcr_Macro_enable ( dPt, MCR_MACRO_ENABLED ( sPt ) ) ;
	mcr_Macro_set_signals ( dPt, ( mcr_Signal * ) sPt->signal_set.array,
			sPt->signal_set.used ) ;
	mcr_Array_from_array ( & dPt->signal_isdispatch, sPt->
			signal_isdispatch.array, sPt->signal_isdispatch.used ) ;
}

void mcr_Macro_free ( mcr_Macro * mcrPt )
{
	if ( ! mcrPt )
	{
		dmsg ;
		return ;
	}
	// TODO lock here?
	mtx_lock ( & mcrPt->lock ) ;
	// Absolutey disable.
	clear_threads ( mcrPt, MCR_INTERRUPT_DISABLE, 1 ) ;
	MCR_ARR_FOR_EACH ( & mcrPt->signal_set, mcr_Signal_free_foreach,
			0 ) ;
	mcr_Array_free ( & mcrPt->signal_set ) ;
	mcr_Array_free ( & mcrPt->signal_isdispatch ) ;
//	mcr_Map_free ( & mcrPt->thread_finish ) ;
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
		mcrPt->interruptor = MCR_CONTINUE ;
	mtx_unlock ( & mcrPt->lock ) ;
}

void mcr_Macro_set_trigger ( mcr_Macro * mcrPt,
		mcr_Hot * hotPt )
{
	dassert ( mcrPt ) ;
	dassert ( hotPt ) ;
	hotPt->trigger = mcr_Macro_trigger ;
	hotPt->trigger_data = mcrPt ;
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
	// When setting signals absolute disable.
	clear_threads ( mcrPt, MCR_INTERRUPT_DISABLE, 1 ) ;
	MCR_ARR_FOR_EACH ( & mcrPt->signal_set, mcr_Signal_free_foreach,
			0 ) ;
	mcrPt->signal_set.used = 0 ;
	if ( mcr_Array_resize ( & mcrPt->signal_set, signalCount ) &&
			mcr_Array_resize ( & mcrPt->signal_isdispatch,
			signalCount ) )
	{
		mcrPt->signal_isdispatch.used = mcrPt->signal_isdispatch.size ;
		mcr_Signal newSig ;
		for ( size_t i = 0 ; i < signalCount ; i ++ )
		{
			mcr_Signal_init ( & newSig, signalSet [ i ].type ) ;
			mcr_Signal_copy ( & newSig, signalSet + i ) ;
			mcr_Array_push ( & mcrPt->signal_set, & newSig ) ;
			mcrPt->signal_isdispatch.array [ i ] = 1 ;
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

mcr_Signal * mcr_Macro_get_signal ( mcr_Macro * mcrPt,
		size_t index )
{
	dassert ( mcrPt ) ;
	return MCR_ARR_AT ( & mcrPt->signal_set, index ) ;
}

void mcr_Macro_set_signal ( mcr_Macro * mcrPt,
		mcr_Signal * copySig, size_t index ) 
{
	dassert ( mcrPt ) ;
	dassert ( copySig ) ;
	mcr_Signal * sigPt = mcr_Macro_get_signal ( mcrPt, index ) ;
	if ( sigPt )
		mcr_Signal_copy ( sigPt, copySig ) ;
	else
		mcr_Macro_push_signal ( mcrPt, copySig ) ;
}

void mcr_Macro_insert_signal ( mcr_Macro * mcrPt,
		mcr_Signal * copySig, size_t index ) 
{
	dassert ( mcrPt ) ;
	dassert ( copySig ) ;
	if ( index >= mcrPt->signal_set.used )
	{
		mcr_Macro_push_signal ( mcrPt, copySig ) ;
	}
	else
	{
		mcr_Signal addSig = { 0 } ;
		mcr_Signal_copy ( & addSig, copySig ) ;
		if ( ! mcr_Array_insert ( & mcrPt->signal_set, index,
				& addSig ) )
		{
			dmsg ;
			mcr_Signal_free ( & addSig ) ;
		}
	}
}

void mcr_Macro_remove_signal ( mcr_Macro * mcrPt,
		size_t index ) 
{
	dassert ( mcrPt ) ;
	mcr_Signal * sigPt = mcr_Macro_get_signal ( mcrPt, index ) ;
	if ( sigPt )
	{
		mcr_Signal_free ( sigPt ) ;
		mcr_Array_remove ( & mcrPt->signal_set, index ) ;
	}
}

void mcr_Macro_push_signal ( mcr_Macro * mcrPt,
		mcr_Signal * newSig )
{
	dassert ( mcrPt ) ;
	dassert ( newSig ) ;
	mtx_lock ( & mcrPt->lock ) ;
	mcr_Signal addSig = { 0 } ;
	mcr_Signal_copy ( & addSig, newSig ) ;
	if ( mcr_Array_push ( & mcrPt->signal_set, & addSig ) )
	{
		if ( mcr_Array_resize ( & mcrPt->signal_isdispatch,
				mcrPt->signal_set.size ) )
		{
			mcrPt->signal_isdispatch.used = mcrPt->signal_set.used ;
			mcrPt->signal_isdispatch.array
					[ mcrPt->signal_set.used - 1 ] = 1 ;
		}
		else
			dmsg ;
	}
	else
	{
		dmsg ;
		mcr_Signal_free ( & addSig ) ;
	}
	mtx_unlock ( & mcrPt->lock ) ;
}

void mcr_Macro_pop_signal ( mcr_Macro * mcrPt )
{
	dassert ( mcrPt ) ;
	mtx_lock ( & mcrPt->lock ) ;
	if ( mcrPt->signal_set.used )
	{
		mcr_Signal * sigPt = MCR_ARR_AT ( & mcrPt->signal_set,
				mcrPt->signal_set.used - 1 ) ;
		mcr_Signal_free ( sigPt ) ;
		-- mcrPt->signal_set.used ;
		-- mcrPt->signal_isdispatch.used ;
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
	dassert ( mcrPt ) ;
	mtx_lock ( & mcrPt->lock ) ;
	// Interrupt or disable, then do not start thread.
	if ( mcrPt->interruptor )
	{
		/* If interrupting one, then we just do not increment this
		 * time. We are already locked for it. */
		if ( mcrPt->interruptor == MCR_INTERRUPT )
			mcrPt->interruptor = MCR_CONTINUE ;
		mtx_unlock ( & mcrPt->lock ) ;
		return ;
	}
//	if ( ! mcrPt->interruptor )
//	{
//		/* If interrupt, then some other thread may be cleaning. */
//		finish_threads ( mcrPt ) ;
		if ( ! mcrPt->thread_max ||
				mcrPt->thread_max > MCR_THREAD_MAX )
		{
			dmsg ;
			mcrPt->thread_max = 1 ;
		}
		// Thread max not reached.
//		if ( mcrPt->thread_finish.set.used < mcrPt->thread_max &&
//				mcrPt->thread_finish.set.used < MCR_THREAD_MAX )
		if ( mcrPt->thread_count < mcrPt->thread_max &&
				mcrPt->thread_count < MCR_THREAD_MAX )
		{
			// TODO: Possible unused threads if only one queued item.
//			char done = 0 ;
//			mcr_MacroThread * mcrtrd = malloc (
//					sizeof ( mcr_MacroThread ) ) ;
//			if ( mcrtrd )
//			{
//				mcrtrd->mcr_pt = mcrPt ;
				thrd_t trd ;
				if ( thrd_create ( & trd, thread_macro,
						mcrPt ) == thrd_success )
				{
					++ mcrPt->queued ;
					++ mcrPt->thread_count ;
					if ( thrd_detach ( trd ) != thrd_success )
					{ dmsg ; }
//					mcr_Map_map ( & mcrPt->thread_finish, & mcrtrd->trd,
//							& done ) ;
				}
				else
				{ dmsg ; }
//			}
//			else
//				dmsg ;
//		}
	}
	mtx_unlock ( & mcrPt->lock ) ;
}

# define ceptloop( mcrPt ) \
	( ! ( mcrPt )->interruptor || ( mcrPt )->interruptor == \
			MCR_INTERRUPT )

static int thread_macro ( void * data )
{
	dassert ( data ) ;
//	mcr_MacroThread * mcrtrd = data ;
	mcr_Macro * mcrPt = data ;
	mcr_Signal * sigArr ;
	size_t index ;
	mtx_lock ( & mcrPt->lock ) ;
	// Loop only while interceptor states continue, or only one.
	// Loop only while triggers queued, or last thread is sticky.
	while ( ceptloop ( mcrPt ) && ( mcrPt->queued ||
			( mcrPt->sticky && mcrPt->thread_count == 1 ) ) )
	{
		if ( mcrPt->queued )
			-- mcrPt->queued ;
		mtx_unlock ( & mcrPt->lock ) ;
		// If sig set is changed, all threads should end.
		sigArr = ( mcr_Signal * ) mcrPt->signal_set.array ;
		index = 0 ;
		while ( ceptloop ( mcrPt ) && index < mcrPt->signal_set.used )
		{
			if ( mcrPt->interruptor == MCR_INTERRUPT &&
					mtx_trylock ( & mcrPt->lock ) == thrd_success )
			{
				/* Interrupt may change before locking. */
				if ( mcrPt->interruptor == MCR_INTERRUPT )
				{
					mcrPt->interruptor = MCR_CONTINUE ;
					mtx_unlock ( & mcrPt->lock ) ;
					break ;
				}
				mtx_unlock ( & mcrPt->lock ) ;
			}
			if ( sigArr [ index ].type )
			{
				if ( mcrPt->signal_isdispatch.array [ index ] )
				{
					if ( ! MCR_SEND ( sigArr + index ) )
					{ dmsg ; }
				}
				else if ( ! sigArr [ index ].type->send ( sigArr + index ) )
				{ dmsg ; }
			}
			++ index ;
		}
		mtx_lock ( & mcrPt->lock ) ;
	}
	// Is locked.
//	char done = 1 ;
//	// Signal this thread complete.
//	mcr_Map_map ( & mcrPt->thread_finish, & mcrtrd->trd, & done ) ;
	-- mcrPt->thread_count ;
	cnd_broadcast ( & mcrPt->cnd ) ;
	mtx_unlock ( & mcrPt->lock ) ;
	return thrd_success ;
}

static int thread_wait_reset ( void * data )
{
	dassert ( data ) ;
	mcr_Macro * mcrPt = data ;
	mtx_lock ( & mcrPt->lock ) ;
	// Interrupt, but break if interrupt changes.
	clear_threads ( mcrPt, MCR_INTERRUPT_ALL, 0 ) ;
	if ( mcrPt->interruptor == MCR_INTERRUPT_ALL )
	{
		mcrPt->interruptor = MCR_CONTINUE ;
	}
	mtx_unlock ( & mcrPt->lock ) ;
	return thrd_success ;
}

static void clear_threads ( mcr_Macro * mcrPt, mcr_Interrupt clearType,
		int stickyInterrupt )
{
	mcrPt->interruptor = clearType ;
	/* If timed out then we probably have a long noop or alarm running.
	 * As long as pthread_exit is not called, then the C runtime
	 * will also exit other threads. In this case valgrind will report
	 * thread is still using resources. */
	struct timespec cndTimeout = { MCR_MACRO_JOIN_TIMEOUT, 0 } ;
	int timedout = thrd_success ;
//	while ( mcrPt->thread_finish.set.used && timedout != thrd_timeout )
	while ( mcrPt->thread_count && timedout != thrd_timeout )
	{
		// Wait to be notified, or time out to destroy.
		timedout = cnd_timedwait ( & mcrPt->cnd, & mcrPt->lock,
				& cndTimeout ) ;
//		finish_threads ( mcrPt ) ;
		if ( stickyInterrupt )
			mcrPt->interruptor = clearType ;
		// Changed interrupt. Exit only if not sticking our interrupt.
		else if ( mcrPt->interruptor != clearType )
			return ;
	}
	// If timed out, we lose control over threads.
	if ( timedout )
	{
		mcrPt->thread_count = 0 ;
	}
//	finish_delete_threads ( mcrPt ) ;
}
/*
static void finish_threads ( mcr_Macro * mcrPt )
{
	thrd_t * trdPt ;
	char * finPt ;
	size_t i = 0 ;
	while ( i < mcrPt->thread_finish.set.used )
	{
		trdPt = MCR_ARR_AT ( & mcrPt->thread_finish.set, i ) ;
		finPt = ( ( char * ) ( trdPt ) ) + 1 ;
		// Remove finish, ignore not finished.
		if ( * finPt )
		{
			if ( thrd_detach ( * trdPt ) != thrd_success )
			{
				dmsg ;
			}
			mcr_Array_remove ( & mcrPt->thread_finish.set, i ) ;
		}
		else
			++ i ;
	}
	mcr_Map_sort ( & mcrPt->thread_finish ) ;
}

static void finish_delete_threads ( mcr_Macro * mcrPt )
{
	thrd_t * trdPt ;
	char * finPt ;
	for ( size_t i = 0 ; i < mcrPt->thread_finish.set.used ; i ++ )
	{
		trdPt = MCR_ARR_AT ( & mcrPt->thread_finish.set, i ) ;
		finPt = ( char * ) ( trdPt + 1 ) ;
		// For all threads either detach or destroy.
		if ( * finPt )
		{
			if ( thrd_detach ( * trdPt ) != thrd_success )
			{
				dmsg ;
			}
		}
		else
		{
			mcr_thrd_delete ( trdPt ) ;
		}
	}
	mcrPt->thread_finish.set.used = 0 ;
}
*/
