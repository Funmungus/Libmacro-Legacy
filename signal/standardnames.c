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

# include "signal/standard.h"

//
// Code memory
//
// Echo
// echo name-> echo index
static mcr_Map _echoMap ;
// echo index-> echo name
static mcr_Array _echoNames ;
int mcr_Echo_code ( const char * eventName )
{
	if ( ! eventName )
	{
		dmsg ;
		return -1 ;
	}
	int * codePt = MCR_STRINGMAP_GET_VALUE ( _echoMap, eventName ) ;
	return codePt ? * codePt : -1 ;
}

const char * mcr_Echo_name ( int eventCode )
{
	mcr_Array * namePt = MCR_ARR_AT ( _echoNames,
			( size_t ) eventCode ) ;
	return namePt ? namePt->array : NULL ;
}

size_t mcr_Echo_count ( )
{
	return _echoNames.used ;
}

// Key
static mcr_Array _keyNames ;
static mcr_Map _keyMap ;
int mcr_Key_code ( const char * keyName )
{
	if ( ! keyName )
	{
		dmsg ;
		return 0 ;
	}
	int * keyPt = MCR_STRINGMAP_GET_VALUE ( _keyMap, keyName ) ;
	return keyPt ? * keyPt : 0 ;
}

const char * mcr_Key_name ( int keyCode )
{
	mcr_Array * namePt = MCR_ARR_AT ( _keyNames,
			( size_t ) keyCode ) ;
	return namePt ? namePt->array : NULL ;
}

size_t mcr_Key_count ( )
{
	return _keyNames.used ;
}


//
// Code development
//
// Echo
int mcr_Echo_set_name ( int eventCode, const char * eventName )
{
	if ( eventCode != -1 )
	{
		if ( ( unsigned int ) eventCode >= _echoNames.used )
		{
			mcr_Array name, initial ;
			mcr_String_init ( & name ) ;
			mcr_String_init ( & initial ) ;
			if ( ! mcr_String_from_string ( & name, eventName ) ||
					! mcr_Array_insert_filled ( & _echoNames, eventCode,
					& name, & initial ) )
			{
				dmsg ;
				mcr_Array_free ( & name ) ;
				return 0 ;
			}
		}
		else
		{
			if ( ! mcr_String_from_string ( MCR_ARR_AT (
					_echoNames, ( unsigned int ) eventCode ),
					eventName ) )
			{
				dmsg ;
				return 0 ;
			}
		}
		if ( ! mcr_Echo_add_name ( eventCode, eventName ) )
		{
			dmsg ;
			return 0 ;
		}
	}
	return 1 ;
}

int mcr_Echo_add_name ( int eventCode, const char * addName )
{
	if ( ! mcr_StringMap_map ( & _echoMap, addName, & eventCode ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

int mcr_Echo_add_names ( int eventCode,
		const char ** addNames, size_t bufferLen )
{
	dassert ( addNames ) ;
	int ret = 1 ;
	for ( size_t i = 0 ; i < bufferLen ; i ++ )
	{
		if ( ! mcr_Echo_add_name ( eventCode, addNames [ i ] ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	return ret ;
}

int mcr_Echo_set_names ( int eventCode,
		const char * eventName,
		const char ** addNames, size_t bufferLen )
{
	int ret = 1 ;
	if ( ! mcr_Echo_set_name ( eventCode, eventName ) )
	{
		dmsg ;
		ret = 0 ;
	}
	if ( ! mcr_Echo_add_names ( eventCode, addNames, bufferLen ) )
	{
		dmsg ;
		ret = 0 ;
	}
	return ret ;
}

int mcr_Echo_rename ( int eventCode,
		const char * newName )
{
	mcr_Array * found = mcr_Array_at ( & _echoNames, eventCode ) ;
	if ( found )
	{
		mcr_StringMap_unmap ( & _echoMap, found->array ) ;
	}
	return mcr_Echo_set_name ( eventCode, newName ) ;
}

int mcr_Echo_rename_from_name ( const char * oldName,
		const char * newName )
{
	dassert ( oldName ) ;
	int * found = MCR_STRINGMAP_GET_VALUE ( _echoMap, oldName ) ;
	if ( found )
		return mcr_Echo_rename ( * found, newName ) ;
	return 0 ;
}

void mcr_Echo_clear ( )
{
	MCR_MAP_FOR_EACH ( _echoMap, mcr_Array_free_foreach,) ;
	mcr_Map_free ( & _echoMap ) ;
	MCR_ARR_FOR_EACH ( _echoNames, mcr_Array_free_foreach,) ;
	mcr_Array_free ( & _echoNames ) ;
}

// Key
int mcr_Key_set_name ( int keyCode, const char * newName )
{
	if ( ! newName )
	{
		dmsg ;
		return 0 ;
	}
	if ( keyCode != -1 )
	{
		if ( ( unsigned int ) keyCode >= _keyNames.used )
		{
			mcr_Array name, initial ;
			mcr_String_init ( & name ) ;
			mcr_String_init ( & initial ) ;
			if ( ! mcr_String_from_string ( & name, newName ) ||
					! mcr_Array_insert_filled ( & _keyNames, keyCode,
					& name, & initial ) )
			{
				dmsg ;
				mcr_Array_free ( & name ) ;
				return 0 ;
			}
		}
		else
		{
			if ( ! mcr_String_from_string ( MCR_ARR_AT (
					_keyNames, ( unsigned int ) keyCode ),
					newName ) )
			{
				dmsg ;
				return 0 ;
			}
		}
		if ( ! mcr_Key_add_name ( keyCode, newName ) )
		{
			dmsg ;
			return 0 ;
		}
	}
	return 1 ;
}

int mcr_Key_add_name ( int keyCode, const char * addName )
{
	if ( ! mcr_StringMap_map ( & _keyMap, addName, & keyCode ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

int mcr_Key_add_names ( int keyCode,
		const char ** addNames, size_t bufferLen )
{
	dassert ( addNames ) ;
	int ret = 1 ;
	for ( size_t i = 0 ; i < bufferLen ; i ++ )
	{
		if ( ! mcr_Key_add_name ( keyCode, addNames [ i ] ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	return ret ;
}

int mcr_Key_set_names ( int keyCode,
		const char * newName,
		const char ** addNames, size_t bufferLen )
{
	int ret = 1 ;
	if ( ! mcr_Key_set_name ( keyCode, newName ) )
	{
		dmsg ;
		ret = 0 ;
	}
	if ( ! mcr_Key_add_names ( keyCode, addNames, bufferLen ) )
	{
		dmsg ;
		ret = 0 ;
	}
	return ret ;
}

int mcr_Key_rename ( int keyCode, const char * newName )
{
	mcr_Array * found = mcr_Array_at ( & _keyNames, keyCode ) ;
	if ( found )
	{
		mcr_StringMap_unmap ( & _keyMap, found->array ) ;
	}
	return mcr_Key_set_name ( keyCode, newName ) ;
}

int mcr_Key_rename_from_name ( const char * oldName,
		const char * newName )
{
	dassert ( oldName ) ;
	int * found = MCR_STRINGMAP_GET_VALUE ( _keyMap, oldName ) ;
	if ( found )
		return mcr_Key_rename ( * found, newName ) ;
	return 0 ;
}

void mcr_Key_clear ( )
{
	MCR_MAP_FOR_EACH ( _keyMap, mcr_Array_free_foreach,) ;
	mcr_Map_free ( & _keyMap ) ;
	MCR_ARR_FOR_EACH ( _keyNames, mcr_Array_free_foreach,) ;
	mcr_Array_free ( & _keyNames ) ;
}

void mcr_standardnames_initialize ( )
{
	mcr_Array_init ( & _echoNames, sizeof ( mcr_Array ) ) ;
	mcr_StringMap_init ( & _echoMap, sizeof ( int ) ) ;
	mcr_Array_init ( & _keyNames, sizeof ( mcr_Array ) ) ;
	mcr_StringMap_init ( & _keyMap, sizeof ( int ) ) ;
}

void mcr_standardnames_cleanup ( void )
{
	mcr_Echo_clear ( ) ;
	mcr_Key_clear ( ) ;
}
