/* signal/signal.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "signal/signal.h"

void mcr_Signal_init ( mcr_Signal * sigPt, mcr_ISignal * type )
{
	if ( ! sigPt )
	{
		dmsg ;
		return ;
	}
	memset ( sigPt, 0, sizeof ( mcr_Signal ) ) ;
	sigPt->type = type ;
}

void mcr_Signal_init_with ( mcr_Signal * sigPt, mcr_ISignal * type,
		void * data, int isHeap )
{
	if ( ! sigPt )
	{
		dmsg ;
		return ;
	}
	memset ( sigPt, 0, sizeof ( mcr_Signal ) ) ;
	MCR_SIGNAL_INIT ( sigPt, type, data, isHeap ) ;
}

void mcr_Signal_free ( mcr_Signal * sigPt )
{
	if ( ! sigPt )
	{
		dmsg ;
		return ;
	}
	MCR_SIGNAL_FREE ( sigPt ) ;
}

void mcr_Signal_free_foreach ( mcr_Signal * sigPt, ... )
{
	if ( ! sigPt ) return ;
	MCR_SIGNAL_FREE ( sigPt ) ;
}

int mcr_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	if ( ! MCR_SEND ( sigPt ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

void * mcr_ISignal_mkdata_data ( mcr_ISignal * isigPt )
{
	dassert ( isigPt ) ;
	mcr_Data d = { 0 } ;
	MCR_ISIGNAL_MKDATA ( isigPt, & d ) ;
	return d.data ;
}

mcr_Data mcr_ISignal_mkdata ( mcr_ISignal * isigPt )
{
	dassert ( isigPt ) ;
	mcr_Data d = { 0 } ;
	MCR_ISIGNAL_MKDATA ( isigPt, & d ) ;
	return d ;
}

void mcr_Signal_copy ( mcr_Signal * dstPt, mcr_Signal * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	if(srcPt->type)
	{
		MCR_SIGNAL_COPY ( dstPt, srcPt ) ;
	}
	else
	{
		MCR_SIGNAL_FREE ( dstPt ) ;
	}
}

int mcr_Signal_compare ( const void * lhsSigPt,
		const void * rhsSigPt )
{
	dassert ( lhsSigPt ) ;
	dassert ( rhsSigPt ) ;
	const mcr_Signal * lSigPt = lhsSigPt, * rSigPt = rhsSigPt ;
	return MCR_SIGNAL_CMP ( lSigPt, rSigPt ) ;
}

void mcr_ISignal_init ( mcr_ISignal * newType,
		mcr_signal_fnc sender, size_t dataSize )
{
	dassert ( newType ) ;
	memset ( newType, 0, sizeof ( mcr_ISignal ) ) ;
	newType->iface.id = -1 ;
	newType->iface.data_size = dataSize ;
	newType->send = sender ;
}

void mcr_ISignal_init_with ( mcr_ISignal * newType,
		mcr_compare_fnc compare,
		void ( * copy ) ( void *, void * ),
		size_t dataSize, void ( * init ) ( void * ),
		void ( * free ) ( void * ),
		mcr_signal_fnc dispatch, mcr_signal_fnc sender )
{
	dassert ( newType ) ;
	memset ( newType, 0, sizeof ( mcr_ISignal ) ) ;
	MCR_ISIGNAL_INIT ( newType, compare, copy, dataSize,
			init, free, sender ) ;
	newType->dispatch = dispatch ;
}

void mcr_signal_initialize ( )
{
	mcr_signalreg_initialize ( ) ;
	mcr_standard_initialize ( ) ;
	mcr_standardnames_initialize ( ) ;
	mcr_standard_native_initialize ( ) ;
}

void mcr_signal_cleanup ( void )
{
	mcr_standard_native_cleanup ( ) ;
	mcr_standardnames_cleanup ( ) ;
	mcr_standard_cleanup ( ) ;
	mcr_signalreg_cleanup ( ) ;
}
