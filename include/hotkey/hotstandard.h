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

/*! \file hotkey/hotstandard.h
 * \brief Standard hotkey types
 * */

# ifndef MCR_HOTSTAGED_H
# define MCR_HOTSTAGED_H

# include "hotkey/stage.h"
# include "hotkey/hot.h"

typedef enum mcr_BlockStyle
{
	MCR_UNMANAGED = 0, /* Do not change blocking for any stages. */
	MCR_NOBLOCK,
	MCR_BLOCK_FINAL,
	MCR_BLOCK_ALL
} mcr_BlockStyle ;

typedef struct mcr_HotStaged
{
	mcr_Array stages ;
	mcr_BlockStyle style ;
} mcr_HotStaged ;

MCR_API extern mcr_IHot mcr_iHot ;
# define mcr_iHotkey mcr_iHot
MCR_API extern mcr_IHot mcr_iHotStaged ;

MCR_API void mcr_Hot_load_contract ( ) ;

//
// Stage manipulation
//
MCR_API void mcr_HotStaged_set_style ( mcr_HotStaged * hotPt,
		mcr_BlockStyle style ) ;
MCR_API int mcr_HotStaged_blocking ( mcr_HotStaged * hotPt ) ;
MCR_API void mcr_HotStaged_set_blocking ( mcr_HotStaged * hotPt,
		int blocking ) ;
MCR_API int mcr_HotStaged_push (mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers , mcr_TriggerFor trigFor) ;
MCR_API int mcr_HotStaged_push_with (mcr_HotStaged * hotPt, int blocking,
		mcr_Signal * interceptPt, unsigned int measurement_error,
		unsigned int modifiers , mcr_TriggerFor trigFor) ;
// Insert always insert_filled with a "activate for any" stage filler.
MCR_API int mcr_HotStaged_insert (mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers, mcr_TriggerFor trigFor, size_t index ) ;
MCR_API int mcr_HotStaged_insert_with (mcr_HotStaged * hotPt,
		int blocking, mcr_Signal * interceptPt, unsigned int measurement_error,
		unsigned int modifiers, mcr_TriggerFor trigFor, size_t index ) ;
MCR_API int mcr_HotStaged_set (mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers, mcr_TriggerFor trigFor, size_t index ) ;
MCR_API int mcr_HotStaged_set_with (mcr_HotStaged * hotPt,
		int blocking, mcr_Signal * interceptPt, unsigned int measurement_error,
		unsigned int modifiers, mcr_TriggerFor trigFor, size_t index ) ;
MCR_API void mcr_deactivate_stages ( mcr_Array * stagesPt ) ;

//
// Internal, interface.
//
MCR_API int mcr_HotStaged_compare ( const void * lhs, const void * rhs ) ;
MCR_API void mcr_HotStaged_copy ( void * dstPt, void * srcPt ) ;
//! \brief Nullify and set up the stage array.
MCR_API void mcr_HotStaged_init ( void * hotPt ) ;
//! \brief Initialize hotkey with values.
MCR_API void mcr_HotStaged_init_with ( mcr_HotStaged * hotPt,
		mcr_Stage * stages, size_t stageCount, mcr_BlockStyle style ) ;
MCR_API void mcr_HotStaged_free ( void * hotPt ) ;
# define mcr_HotStaged_free_foreach( hotPt, ... ) \
	mcr_HotStaged_free ( hotPt )
/*! \brief \ref mcr_trigger_fnc for staged hotkey.
 *
 * If all stages activate, all will be deactivated and
 * \ref mcr_HotStaged#on_complete will be called.
 * */
MCR_API void mcr_HotStaged_trigger ( mcr_Hot * hotPt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

# define MCR_HOTSTAGED_ONCOMPLETE( hot, sigPt, mods ) \
	if ( ( hot ).trigger ) \
		( hot ).trigger ( & hot, sigPt, mods ) ;
# define mcr_HotStaged_on_complete( hotPt, sigPt, mods ) \
	if ( ( hotPt )->trigger ) \
		( hotPt )->trigger ( hotPt, sigPt, mods ) ;

MCR_API void mcr_hotstandard_initialize ( ) ;
MCR_API void mcr_hotstandard_cleanup ( ) ;

# endif // MCR_HOTSTAGED_H
