
# ifndef MCR_DISPATCHUTILS_H
# define MCR_DISPATCHUTILS_H

# include "hotkey/dispatch.h"

typedef const void * cpointer_t ;

/* Using macros instead of MCR_ARR_FOR_EACH because
 * sending and dispatching are special.
 * */

/* 1 ) Get array for keyPt.
 * 2 ) Array is mcr_Hot **
 * 3 ) Trigger array with signalPt, modsPt, and block.
 * */
# define TRIGGER_MAPPED_ARRAY( mcr_MapPt, keyPt, signalPt, modsPt, \
		block ) \
{ \
	mcr_Array * _arrPt_ = MCR_MAP_GET ( mcr_MapPt, keyPt ) ; \
	if ( _arrPt_ ) \
	{ \
		_arrPt_ = MCR_MAP_VALUE ( mcr_MapPt, _arrPt_ ) ; \
		if ( _arrPt_->used ) \
		{ \
			mcr_Hot ** _hotsPt_ = MCR_ARR_AT ( _arrPt_, 0 ) ; \
			MCR_HOT_TRIGGER_ARRAY ( _hotsPt_, ( _arrPt_ )->used, \
					signalPt, modsPt, block ) ; \
		} \
	} \
}

/* 1 ) Get mapped map from mapKeyPt. ( value of MCR_MAP_GET )
 * 2 ) If mapped map's compare differs for specificKeyPt and genericKeyPt,
 * then MCR_MAP_TRIGGER_ARRAY for mapped map and specificKeyPt.
 * 3 ) Then MCR_MAP_TRIGGER_ARRAY for mapped map and genericKeyPt.
 * */
# define TRIGGER_MAPPED_MAPS( mcr_MapPt, mapKeyPt, specificKeyPt, \
		genericKeyPt, signalPt, modsPt, block ) \
{ \
	mcr_Map * _mapPt_ = MCR_MAP_GET ( mcr_MapPt, mapKeyPt ) ; \
	if ( _mapPt_ ) \
	{ \
		_mapPt_ = MCR_MAP_VALUE ( mcr_MapPt, _mapPt_ ) ; \
		if ( _mapPt_->compare && _mapPt_->compare ( specificKeyPt, \
				genericKeyPt ) ) \
		{ \
			TRIGGER_MAPPED_ARRAY ( _mapPt_, specificKeyPt, \
				signalPt, modsPt, block ) ; \
		} \
		TRIGGER_MAPPED_ARRAY ( _mapPt_, genericKeyPt, \
				signalPt, modsPt, block ) ; \
	} \
}

// Trigger only one map out of given mapPt
# define TRIGGER_MAPPED_MAP( mapPt, keyPt, mappedKeyPt, signalPt, \
		modsPt, block ) \
{ \
	mcr_Map * _found_ = MCR_MAP_GET ( mapPt, keyPt ) ; \
	if ( _found_ ) \
	{ \
		_found_ = MCR_MAP_VALUE ( mapPt, _found_ ) ; \
		TRIGGER_MAPPED_ARRAY ( _found_, mappedKeyPt, signalPt, modsPt, \
				block ) ; \
	} \
}

/*
 * After removing deletion items from arrays, also remove
 * empty mapped arrays.
 * */
static void clear_arr_empties ( mcr_Map * mapPt )
{
	for ( size_t i = 0 ; i < mapPt->set.used ; )
	{
		mcr_Array * arr = MCR_ARR_AT ( & mapPt->set, i ) ;
		arr = MCR_MAP_VALUE ( mapPt, arr ) ;
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
		mcr_Map * map = MCR_ARR_AT ( & mapPt->set, i ) ;
		map = MCR_MAP_VALUE ( mapPt, map ) ;
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
static void arr_remove_all ( mcr_Array * arrPt, const void * deletion )
{
	mcr_Array_remove_all ( arrPt, deletion ) ;
	mcr_Array_trim ( arrPt ) ;
}

/* MCR_MAP_FOR_EACH_VALUE, arr_remove_all for all array entries.
 * clear_arr_empties.
 * */
static void map_arr_remove_all ( mcr_Map * mapPt, const void * deletion )
{
	MCR_MAP_FOR_EACH_VALUE ( mapPt, arr_remove_all, deletion ) ;
	// Deletion is removed from all arrays. Remove empties.
	clear_arr_empties ( mapPt ) ;
}

/* MCR_MAP_FOR_EACH_VALUE map_arr_remove_all for all values.
 * clear_map_empties.
 * */
static void map_map_remove_all ( mcr_Map * mapPt, const void * deletion )
{
	MCR_MAP_FOR_EACH_VALUE ( mapPt, map_arr_remove_all, deletion ) ;
	clear_map_empties ( mapPt ) ;
}

// mcr_Array_free mcr_Array * param
static void arr_free_redirect ( mcr_Array * arrPt,... )
{
	mcr_Array_free ( arrPt ) ;
}

/* map->array. mcr_Array_free all mapped arrays.
 * mcr_Map_free the parameter after all arrays are freed.
 * */
static void map_arr_free_redirect ( mcr_Map * mapPt,... )
{
	MCR_MAP_FOR_EACH_VALUE ( mapPt, arr_free_redirect, 0 ) ;
	mcr_Map_free ( mapPt ) ;
}

// map->map
static void map_map_free ( mcr_Map * mapPt,... )
{
	MCR_MAP_FOR_EACH_VALUE ( mapPt, map_arr_free_redirect, 0 ) ;
	mcr_Map_free ( mapPt ) ;
}

// map->array. Push_unique into the mapped array.
static void add_mapped_array ( mcr_Map * mapPt, const void * keyPt,
		const void * pushValue, const mcr_Array * arrEnsured )
{
	mcr_Array * found = mcr_Map_get_ensured ( mapPt, keyPt, arrEnsured ) ;
	if ( found )
	{
		found = MCR_MAP_VALUE ( mapPt, found ) ;
		mcr_Array_push_unique ( found, pushValue ) ;
	}
	// Else allocation error.
}

// map->map->array. Push_unique into the mapped array.
static void add_mapped_map ( mcr_Map * mapPt, const void * keyPt,
		const void * mappedKeyPt, const void * pushValue,
		const mcr_Map * mapEnsured, const mcr_Array * arrEnsured )
{
	mcr_Map * found = mcr_Map_get_ensured ( mapPt, keyPt, mapEnsured ) ;
	if ( found )
	{
		found = MCR_MAP_VALUE ( mapPt, found ) ;
		add_mapped_array ( found, mappedKeyPt, pushValue, arrEnsured ) ;
	}
	// Else allocation error.
}

# endif // MCR_DISPATCHUTILS_H
