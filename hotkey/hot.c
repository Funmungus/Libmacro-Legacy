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

# include "hotkey/dispatch.h"

static mcr_IRegistry _ihotReg ;

void mcr_Hot_init ( mcr_Hot * hotPt, mcr_IHot * ihotPt )
{
	if ( ! hotPt ) return ;
	memset ( hotPt, 0, sizeof ( mcr_Hot ) ) ;
	hotPt->type = ihotPt ;
}

void mcr_Hot_init_with ( mcr_Hot * hotPt, mcr_IHot * ihotPt,
		void * data, int isHeap, int block, mcr_trigger_fnc trigger,
		void * triggerData )
{
	if ( ! hotPt ) return ;
	memset ( hotPt, 0, sizeof ( mcr_Hot ) ) ;
	MCR_HOT_INIT ( * hotPt, ihotPt, data, isHeap, block, trigger,
			triggerData ) ;
}

void mcr_Hot_free ( mcr_Hot * hotPt )
{
	if ( ! hotPt ) return ;
	MCR_HOT_FREE ( * hotPt ) ;
}

void * mcr_IHot_mkdata_data ( mcr_IHot * ihotPt )
{
	dassert ( ihotPt ) ;
	mcr_Data d = { 0 } ;
	MCR_IHOT_MKDATA ( * ihotPt, d ) ;
	return d.data ;
}

mcr_Data mcr_IHot_mkdata ( mcr_IHot * ihotPt )
{
	dassert ( ihotPt ) ;
	mcr_Data d = { 0 } ;
	MCR_IHOT_MKDATA ( * ihotPt, d ) ;
	return d ;
}

void mcr_Hot_copy ( mcr_Hot * dstPt, mcr_Hot * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	MCR_HOT_COPY ( * dstPt, * srcPt ) ;
}

int mcr_Hot_compare ( const void * lhsHotPt,
		const void * rhsHotPt )
{
	dassert ( lhsHotPt ) ;
	dassert ( rhsHotPt ) ;
	const mcr_Hot * lHotPt = lhsHotPt, * rHotPt = rhsHotPt ;
	return MCR_HOT_CMP ( * lHotPt, * rHotPt ) ;
}


int mcr_Hot_trigger ( mcr_Hot * hotPt, mcr_Signal * signalPt,
		unsigned int mods )
{
	MCR_HOT_TRIGGER ( * hotPt, signalPt, mods ) ;
	return hotPt->block ;
}

int mcr_Hot_trigger_array ( mcr_Hot ** hotArray, size_t count,
		mcr_Signal * signalPt, unsigned int mods )
{
	int block = 0 ;
	MCR_HOT_TRIGGER_REF_ARRAY ( hotArray, count, signalPt, mods, block ) ;
	return block ;
}

void mcr_IHot_init ( mcr_IHot * ihotPt, size_t dataSize )
{
	dassert ( ihotPt ) ;
	memset ( ihotPt, 0, sizeof ( mcr_IHot ) ) ;
	ihotPt->iface.id = -1 ;
	ihotPt->iface.data_size = dataSize ;
}

void mcr_IHot_init_with ( mcr_IHot * ihotPt, mcr_compare_fnc compare,
		void ( * copy ) ( void *, void * ), size_t dataSize,
		void ( * init ) ( void * ), void ( * free ) ( void * ),
		mcr_trigger_fnc trigger )
{
	dassert ( ihotPt ) ;
	memset ( ihotPt, 0, sizeof ( mcr_IHot ) ) ;
	MCR_IHOT_INIT ( * ihotPt, compare, copy, dataSize, init,
			free, trigger ) ;
}

size_t mcr_IHot_register ( mcr_IHot * newType )
{
	dassert ( newType ) ;
	return mcr_iregister ( & _ihotReg, ( mcr_Interface * ) newType ) ;
}

mcr_IHot * mcr_IHot_get ( size_t typeId )
{
	return ( mcr_IHot * ) mcr_iget ( & _ihotReg, typeId ) ;
}

mcr_IHot * mcr_IHot_from_name ( const char * typeName )
{
	return ( mcr_IHot * ) mcr_ifrom_name ( & _ihotReg, typeName ) ;
}

size_t mcr_IHot_get_id ( const char * typeName )
{
	return mcr_iget_id ( & _ihotReg, typeName ) ;
}

const char * mcr_IHot_get_name ( size_t id )
{
	return mcr_iget_name ( & _ihotReg, id ) ;
}

int mcr_IHot_set_name ( mcr_IHot * ihotPt,
		const char * name )
{
	return mcr_iset_name ( & _ihotReg, ( mcr_Interface * ) ihotPt, name ) ;
}

int mcr_IHot_add_name ( mcr_IHot * ihotPt,
		const char * name )
{
	return mcr_iadd_name ( & _ihotReg, ( mcr_Interface * ) ihotPt, name ) ;
}

int mcr_IHot_add_names ( mcr_IHot * ihotPt,
		const char ** names, size_t bufferLen )
{
	dassert ( ihotPt ) ;
	dassert ( names ) ;
	int ret = 1 ;
	for ( size_t i = 0 ; i < bufferLen ; i ++ )
	{
		if ( ! mcr_IHot_add_name ( ihotPt, names [ i ] ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	return ret ;
}

int mcr_IHot_set_names ( mcr_IHot * ihotPt,
		const char * name,
		const char ** addNames, size_t bufferLen )
{
	int ret = 1 ;
	if ( ! mcr_IHot_set_name ( ihotPt, name ) )
	{
		dmsg ;
		ret = 0 ;
	}
	if ( ! mcr_IHot_add_names ( ihotPt, addNames, bufferLen ) )
	{
		dmsg ;
		ret = 0 ;
	}
	return ret ;
}

int mcr_IHot_rename ( const char * oldName,
		const char * newName )
{
	return mcr_irename ( & _ihotReg, oldName, newName ) ;
}

size_t mcr_IHot_count ( )
{
	return _ihotReg.set.used ;
}

void mcr_IHot_get_all ( mcr_IHot ** buffer,
		size_t bufferLength )
{
	mcr_iget_all ( & _ihotReg, ( mcr_Interface ** ) buffer, bufferLength ) ;
}

void mcr_IHot_clear ( )
{
	mcr_iclear ( & _ihotReg ) ;
}

void mcr_hot_initialize ( )
{
	mcr_iinit_maps ( & _ihotReg ) ;
}

void mcr_hot_cleanup ( void )
{
	mcr_IHot_clear ( ) ;
}
