/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

#ifndef MCR_DISPATCHUTILS_H
#define MCR_DISPATCHUTILS_H

#include "mcr/signal/signal.h"
#include <stdlib.h>

typedef const void *cpointer_t;

/* Using macros instead of mcr_Array_for_each because
 * sending and dispatching are special
 */

/* 1 ) Get array for keyPt
 * 2 ) Array is mcr_Trigger **
 * 3 ) Trigger array with signalPt, mods, and block.
 */
#define MCR_MAP_ARRAY_TRIGGER( map, keyPt, signalPt, mods, \
		block ) \
{ \
	mcr_Array *local_arrPt = MCR_MAP_VALUE ( map, keyPt ); \
	if ( local_arrPt && local_arrPt->used ) \
	{ \
		mcr_Trigger **local_trigsPt = MCR_ARR_FIRST ( *local_arrPt ); \
		MCR_TRIGGER_REF_ARRAY ( local_trigsPt, ( local_arrPt )->used, \
				signalPt, mods, block ); \
	} \
}

/* Trigger only one map out of given map */
#define MCR_MAP_TRIGGER( map, keyPt, mappedKeyPt, signalPt, \
		mods, block ) \
{ \
	mcr_Map *local_mapPt = MCR_MAP_VALUE ( map, keyPt ); \
	if ( local_mapPt ) \
	{ \
		MCR_MAP_ARRAY_TRIGGER ( *local_mapPt, mappedKeyPt, signalPt, mods, \
				block ); \
	} \
}

/* 1 ) Get mapped map from mapKeyPt. ( value of MCR_MAP_GET )
 * 2 ) If mapped map's compare differs for specificKeyPt and genericKeyPt,
 * then MCR_MAP_TRIGGER_ARRAY for mapped map and specificKeyPt.
 * 3 ) Then MCR_MAP_TRIGGER_ARRAY for mapped map and genericKeyPt.
 */
#define MCR_MAP_MAP_TRIGGER( map, mapKeyPt, specificKeyPt, \
		genericKeyPt, signalPt, mods, block ) \
{ \
	mcr_Map *local_mapPt = MCR_MAP_VALUE ( map, mapKeyPt ); \
	if ( local_mapPt ) \
	{ \
		if ( local_mapPt->set.compare ? local_mapPt->set.compare \
				( specificKeyPt, genericKeyPt ) : \
				memcmp ( specificKeyPt, genericKeyPt, \
				local_mapPt->set.data_size )) \
		{ \
			MCR_MAP_ARRAY_TRIGGER ( *local_mapPt, specificKeyPt, \
					signalPt, mods, block ); \
		} \
		MCR_MAP_ARRAY_TRIGGER ( *local_mapPt, genericKeyPt, \
				signalPt, mods, block ); \
	} \
}

#endif
