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

# include "util/interface.h"

void * mcr_mkdata_data ( mcr_Interface * iPt )
{
	dassert ( iPt ) ;
	mcr_Data d = { 0 } ;
	MCR_IMKDATA ( * iPt, d ) ;
	return d.data ;
}

mcr_Data mcr_mkdata ( mcr_Interface * iPt )
{
	dassert ( iPt ) ;
	mcr_Data d = { 0 } ;
	MCR_IMKDATA ( * iPt, d ) ;
	return d ;
}

void mcr_icpy ( mcr_Interface * iPt, mcr_Data * dstPt,
		mcr_Data * srcPt )
{
	dassert ( iPt ) ;
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	MCR_ICPY ( * iPt, * dstPt, * srcPt ) ;
}

int mcr_icmp ( mcr_Interface * iPt, mcr_Data * lhs,
		mcr_Data * rhs )
{
	dassert ( iPt ) ;
	dassert ( lhs ) ;
	dassert ( rhs ) ;
	return MCR_ICMP ( * iPt, * lhs, * rhs ) ;
}

void mcr_ifree ( mcr_Interface * iPt, mcr_Data * dataPt )
{
	dassert ( iPt ) ;
	dassert ( dataPt ) ;
	MCR_IFREE ( * iPt, * dataPt ) ;
}

void mcr_iinit ( mcr_Interface * newType,
		size_t dataSize )
{
	if ( ! newType )
	{
		dmsg ;
		return ;
	}
	memset ( newType, 0, sizeof ( mcr_Interface ) ) ;
	newType->id = -1 ;
	newType->data_size = dataSize ;
}

void mcr_iinit_with ( mcr_Interface * newType,
		mcr_compare_fnc compare,
		void ( * copy ) ( void *, void * ),
		size_t dataSize, void ( * init ) ( void * ),
		void ( * free ) ( void * ) )
{
	if ( ! newType )
	{
		dmsg ;
		return ;
	}
	memset ( newType, 0, sizeof ( mcr_Interface ) ) ;
	MCR_IINIT ( * newType, compare, copy, dataSize, init, free ) ;
}


void mcr_iinit_maps ( mcr_IRegistry * iRegPt )
{
	dassert ( iRegPt ) ;
	mcr_Array_init ( & iRegPt->set, sizeof ( mcr_Interface * ) ) ;
	mcr_Array_init ( & iRegPt->names, sizeof ( mcr_Array ) ) ;
	mcr_StringMap_init ( & iRegPt->name_map, sizeof ( mcr_Interface * ) ) ;
}

size_t mcr_iregister ( mcr_IRegistry * iRegPt, mcr_Interface * newType )
{
	dassert ( iRegPt ) ;
	dassert ( newType ) ;
	size_t id = iRegPt->set.used ;
	if ( ! mcr_Array_push ( & iRegPt->set, & newType ) )
	{
		dmsg ;
		return -1 ;
	}
	// If successful, we can set the id.
	newType->id = id ;
	return id ;
}

mcr_Interface * mcr_iget ( mcr_IRegistry * iRegPt, size_t typeId )
{
	if ( typeId < iRegPt->set.used )
	{
		mcr_Interface ** ifacePtPt = MCR_ARR_AT ( iRegPt->set, typeId ) ;
		return ifacePtPt ? * ifacePtPt : NULL ;
	}
	return NULL ;
}

mcr_Interface * mcr_ifrom_name (
		mcr_IRegistry * iRegPt, const char * typeName )
{
	if (typeName)
	{
		mcr_Interface ** retPt = MCR_MAP_GET_VALUE (
				( iRegPt )->name_map, & ( typeName ) ) ;
		return retPt ? * retPt : NULL ;
	}
	return NULL ;
}
size_t mcr_iget_id ( mcr_IRegistry * iRegPt, const char * typeName )
{
	mcr_Interface * iPt = mcr_ifrom_name ( iRegPt, typeName ) ;
	return iPt ? iPt->id : ( size_t ) -1 ;
}

int mcr_iset_name ( mcr_IRegistry * iRegPt,
		mcr_Interface * iPt, const char * name )
{
	dassert ( iRegPt ) ;
	dassert ( iPt ) ;
	dassert ( iPt->id != ( size_t ) -1 ) ;
	dassert ( name ) ;
	if ( iPt->id >= iRegPt->names.used )
	{
		mcr_Array setName = { 0 }, initial = { 0 } ;
		setName.element_size = initial.element_size = sizeof ( char ) ;
		if ( ! mcr_String_from_string ( & setName, name ) ||
				! mcr_Array_insert_filled ( & iRegPt->names, iPt->id,
				& setName, & initial ) )
		{
			dmsg ;
			mcr_Array_free ( & setName ) ;
			return 0 ;
		}
	}
	else
	{
		mcr_Array * namePt = MCR_ARR_AT ( iRegPt->names, iPt->id ) ;
		if ( ! mcr_String_from_string ( namePt, name ) )
		{
			dmsg ;
			return 0 ;
		}
	}
	return mcr_iadd_name ( iRegPt, iPt, name ) ;
}

int mcr_iadd_name ( mcr_IRegistry * iRegPt, mcr_Interface * iPt,
		const char * name )
{
	dassert ( iRegPt ) ;
	dassert ( iPt ) ;
	dassert ( name ) ;
	if ( ! mcr_StringMap_map ( & iRegPt->name_map, name, & iPt ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

int mcr_irename ( mcr_IRegistry * iRegPt, const char * oldName,
		const char * newName )
{
	dassert ( iRegPt ) ;
	mcr_Interface * iPt = mcr_ifrom_name ( iRegPt, oldName ) ;
	if ( iPt )
	{
		// Remove old name, set new.
		mcr_StringMap_unmap ( & iRegPt->name_map, oldName ) ;
		return mcr_iset_name ( iRegPt, iPt, newName ) ;
	}
	dmsg ;
	return 0 ;
}

void mcr_iget_all ( mcr_IRegistry * iRegPt, mcr_Interface ** buffer,
		size_t bufferLength )
{
	dassert ( iRegPt ) ;
	dassert ( buffer ) ;
	mcr_Interface ** arr = ( mcr_Interface ** ) iRegPt->set.array ;
	for ( size_t i = 0 ; i < bufferLength && i < iRegPt->set.used ; i ++ )
	{
		buffer [ i ] = arr [ i ] ;
	}
}

void mcr_iclear ( mcr_IRegistry * iRegPt )
{
	dassert ( iRegPt ) ;
	mcr_Array_free ( & iRegPt->set ) ;
	MCR_ARR_FOR_EACH ( iRegPt->names,
			MCR_EXP ( mcr_Array_free_foreach ),) ;
	mcr_Array_free ( & iRegPt->names ) ;
	MCR_MAP_FOR_EACH ( iRegPt->name_map,
			MCR_EXP (  mcr_Array_free_foreach ),) ;
	mcr_Map_free ( & iRegPt->name_map ) ;
}