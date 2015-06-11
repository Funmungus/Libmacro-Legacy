/* include/hotkey/hotstandard.h - Specific hotkey type that is triggered in stages.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file hotkey/hotstaged.h
 * \brief \ref mcr_HotStaged Is a hotkey triggered after the activation
 * of all contained stages.
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

extern mcr_IHot mcr_iHot ;
# define mcr_iHotkey mcr_iHot
extern mcr_IHot mcr_iHotStaged ;

MCR_API void mcr_Hot_load_contract ( ) ;

//
// Stage manipulation
//
MCR_API void mcr_HotStaged_set_style ( mcr_HotStaged * hotPt,
		mcr_BlockStyle style ) ;
MCR_API int mcr_HotStaged_is_blocking ( mcr_HotStaged * hotPt ) ;
MCR_API void mcr_HotStaged_enable_blocking ( mcr_HotStaged * hotPt,
		int blocking ) ;
MCR_API int mcr_HotStaged_push ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers ) ;
MCR_API int mcr_HotStaged_push_with ( mcr_HotStaged * hotPt, int blocking,
		mcr_Signal * interceptPt, size_t measurement_error,
		unsigned int modifiers ) ;
// Insert always insert_filled with a "activate for any" stage filler.
MCR_API int mcr_HotStaged_insert ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers, size_t index ) ;
MCR_API int mcr_HotStaged_insert_with ( mcr_HotStaged * hotPt,
		int blocking, mcr_Signal * interceptPt, size_t measurement_error,
		unsigned int modifiers, size_t index ) ;
MCR_API int mcr_HotStaged_set ( mcr_HotStaged * hotPt,
		mcr_Signal * interceptPt, unsigned int modifiers, size_t index ) ;
MCR_API int mcr_HotStaged_set_with ( mcr_HotStaged * hotPt,
		int blocking, mcr_Signal * interceptPt, size_t measurement_error,
		unsigned int modifiers, size_t index ) ;
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
/*! \brief \ref mcr_trigger_fnc for staged hotkey.
 *
 * If all stages activate, all will be deactivated and
 * \ref mcr_HotStaged#on_complete will be called.
 * */
MCR_API void mcr_HotStaged_trigger ( mcr_Hot * hotPt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

# define MCR_HOTSTAGED_ONCOMPLETE( hotPt, sigPt, mods ) \
	if ( ( hotPt )->trigger ) \
		( hotPt )->trigger ( hotPt, sigPt, mods ) ;

MCR_API void mcr_hotstandard_initialize ( ) ;
MCR_API void mcr_hotstandard_cleanup ( ) ;

# endif // MCR_HOTSTAGED_H
