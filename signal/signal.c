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
	sigPt->is_dispatch = 1 ;
}

void mcr_Signal_init_with ( mcr_Signal * sigPt, mcr_ISignal * type,
		void * data, int isHeap , int isDispatch )
{
	if ( ! sigPt )
	{
		dmsg ;
		return ;
	}
	memset ( sigPt, 0, sizeof ( mcr_Signal ) ) ;
	MCR_SIGNAL_INIT ( * sigPt, type, data, isHeap, isDispatch ) ;
}

void mcr_Signal_free ( mcr_Signal * sigPt )
{
	if ( ! sigPt )
	{
		dmsg ;
		return ;
	}
	MCR_SIGNAL_FREE ( * sigPt ) ;
}

int mcr_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	if ( ! mcr_Send_impl ( sigPt ) )
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
	MCR_ISIGNAL_MKDATA ( * isigPt, d ) ;
	return d.data ;
}

mcr_Data mcr_ISignal_mkdata ( mcr_ISignal * isigPt )
{
	dassert ( isigPt ) ;
	mcr_Data d = { 0 } ;
	MCR_ISIGNAL_MKDATA ( * isigPt, d ) ;
	return d ;
}

void mcr_Signal_copy ( mcr_Signal * dstPt, mcr_Signal * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	if(srcPt->type)
	{
		MCR_SIGNAL_COPY ( * dstPt, * srcPt ) ;
	}
	else
	{
		MCR_SIGNAL_FREE ( * dstPt ) ;
	}
}

int mcr_Signal_compare ( const void * lhsSigPt,
		const void * rhsSigPt )
{
	dassert ( lhsSigPt ) ;
	dassert ( rhsSigPt ) ;
	const mcr_Signal * lSigPt = lhsSigPt, * rSigPt = rhsSigPt ;
	return MCR_SIGNAL_CMP ( * lSigPt, * rSigPt ) ;
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
	MCR_ISIGNAL_INIT ( * newType, compare, copy, dataSize,
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
