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

# ifndef MCR_DISPATCHUTILS_H
# define MCR_DISPATCHUTILS_H

# include "hotkey/dispatch.h"

typedef const void * cpointer_t ;

/* Using macros instead of mcr_Array_for_each because
 * sending and dispatching are special.
 * */

/* 1 ) Get array for keyPt.
 * 2 ) Array is mcr_Hot **
 * 3 ) Trigger array with signalPt, mods, and block.
 * */
# define TRIGGER_MAPPED_ARRAY( map, keyPt, signalPt, mods, \
		block ) \
{ \
	mcr_Array * _arrPt_ = MCR_MAP_GET_VALUE ( map, keyPt ) ; \
	if ( _arrPt_ && _arrPt_->used ) \
	{ \
		mcr_Hot ** _hotsPt_ = MCR_ARR_AT ( * _arrPt_, 0 ) ; \
		MCR_HOT_TRIGGER_REF_ARRAY ( _hotsPt_, ( _arrPt_ )->used, \
				signalPt, mods, block ) ; \
	} \
}

/* 1 ) Get mapped map from mapKeyPt. ( value of MCR_MAP_GET )
 * 2 ) If mapped map's compare differs for specificKeyPt and genericKeyPt,
 * then MCR_MAP_TRIGGER_ARRAY for mapped map and specificKeyPt.
 * 3 ) Then MCR_MAP_TRIGGER_ARRAY for mapped map and genericKeyPt.
 * */
# define TRIGGER_MAPPED_MAPS( map, mapKeyPt, specificKeyPt, \
		genericKeyPt, signalPt, mods, block ) \
{ \
	mcr_Map * _mapPt_ = MCR_MAP_GET_VALUE ( map, mapKeyPt ) ; \
	if ( _mapPt_ ) \
	{ \
		if ( _mapPt_->compare && _mapPt_->compare ( specificKeyPt, \
				genericKeyPt ) ) \
		{ \
			TRIGGER_MAPPED_ARRAY ( * _mapPt_, specificKeyPt, \
				signalPt, mods, block ) ; \
		} \
		TRIGGER_MAPPED_ARRAY ( * _mapPt_, genericKeyPt, \
				signalPt, mods, block ) ; \
	} \
}

// Trigger only one map out of given map
# define TRIGGER_MAPPED_MAP( map, keyPt, mappedKeyPt, signalPt, \
		mods, block ) \
{ \
	mcr_Map * _found_ = MCR_MAP_GET_VALUE ( map, keyPt ) ; \
	if ( _found_ ) \
	{ \
		TRIGGER_MAPPED_ARRAY ( * _found_, mappedKeyPt, signalPt, mods, \
				block ) ; \
	} \
}

/*
 * After removing deletion items from arrays, also remove
 * empty mapped arrays.
 * */
static void clear_arr_empties ( mcr_Map * mapPt )
{
	mcr_dassert ( mapPt ) ;
	for ( size_t i = 0 ; i < mapPt->set.used ; )
	{
		mcr_Array * arr = MCR_ARR_AT ( mapPt->set, i ) ;
		arr = MCR_MAP_VALUE ( * mapPt, arr ) ;
		if ( arr->used )
		{
			++ i ;
		}
		else
		{
			mcr_Array_free ( arr ) ;
			mcr_Array_remove ( & mapPt->set, i ) ;
		}
	}
	mcr_Map_trim ( mapPt ) ;
	mcr_Map_sort ( mapPt ) ;
}

/*
 * After removing deletion items and empty arrays, also remove
 * empty mapped maps.
 * */
static void clear_map_empties ( mcr_Map * mapPt )
{
	for ( size_t i = 0 ; i < mapPt->set.used ; )
	{
		mcr_Map * map = MCR_ARR_AT ( mapPt->set, i ) ;
		map = MCR_MAP_VALUE ( * mapPt, map ) ;
		if ( map->set.used )
		{
			++ i ;
		}
		else
		{
			mcr_Map_free ( map ) ;
			mcr_Array_remove ( & mapPt->set, i ) ;
		}
	}
	mcr_Map_trim ( mapPt ) ;
	mcr_Map_sort ( mapPt ) ;
}

/* mcr_Array_remove_all and mcr_Array_trim
 * */
static void arr_remove_sorted ( mcr_Array * arrPt, const void * deletion,
		mcr_compare_fnc cmp )
{
	mcr_Array_remove_sorted ( arrPt, deletion, cmp ) ;
	if ( arrPt->used < ( arrPt->size >> 1 ) )
		mcr_Array_trim ( arrPt ) ;
}

/* MCR_MAP_FOR_EACH_VALUE, arr_remove_all for all array entries.
 * clear_arr_empties.
 * */
static void map_arr_remove_sorted ( mcr_Map * mapPt, const void * deletion,
		mcr_compare_fnc cmp )
{
	MCR_MAP_FOR_EACH_VALUE ( * mapPt, arr_remove_sorted, deletion, cmp ) ;
	// Deletion is removed from all arrays. Remove empties.
	clear_arr_empties ( mapPt ) ;
}

/* MCR_MAP_FOR_EACH_VALUE map_arr_remove_all for all values.
 * clear_map_empties.
 * */
static void map_map_remove_sorted ( mcr_Map * mapPt, const void * deletion,
		mcr_compare_fnc cmp )
{
	MCR_MAP_FOR_EACH_VALUE ( * mapPt, map_arr_remove_sorted, deletion,
			cmp ) ;
	clear_map_empties ( mapPt ) ;
}

/* map->array. mcr_Array_free all mapped arrays.
 * mcr_Map_free the parameter after all arrays are freed.
 * */
static void map_arr_free_foreach ( mcr_Map * mapPt, ... )
{
	MCR_MAP_FOR_EACH_VALUE ( * mapPt,
			MCR_EXP ( mcr_Array_free_foreach ),) ;
	mcr_Map_free ( mapPt ) ;
}

// map->map
static void map_map_free ( mcr_Map * mapPt, ... )
{
	MCR_MAP_FOR_EACH_VALUE ( * mapPt, map_arr_free_foreach, 0 ) ;
	mcr_Map_free ( mapPt ) ;
}

// map->array. Push_unique into the mapped array.
static void add_mapped_array_sorted ( mcr_Map * mapPt, const void * keyPt,
		const void * pushValue, const mcr_Array * arrEnsured,
		mcr_compare_fnc cmp )
{
	mcr_Array * found = mcr_Map_get_ensured ( mapPt, keyPt, arrEnsured ) ;
	if ( found )
	{
		found = MCR_MAP_VALUE ( * mapPt, found ) ;
		mcr_Array_add_sorted ( found, pushValue, cmp ) ;
	}
	// Else allocation error.
	else
		dmsg ;
}

// map->map->array. Push_unique into the mapped array.
static void add_mapped_map_sorted ( mcr_Map * mapPt, const void * keyPt,
		const void * mappedKeyPt, const void * pushValue,
		const mcr_Map * mapEnsured, const mcr_Array * arrEnsured,
		mcr_compare_fnc cmp )
{
	mcr_Map * found = mcr_Map_get_ensured ( mapPt, keyPt, mapEnsured ) ;
	if ( found )
	{
		found = MCR_MAP_VALUE ( * mapPt, found ) ;
		add_mapped_array_sorted ( found, mappedKeyPt, pushValue,
				arrEnsured, cmp ) ;
	}
	// Else allocation error.
	else
		dmsg ;
}

# endif // MCR_DISPATCHUTILS_H
