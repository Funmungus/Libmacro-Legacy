/* include/hotkey/stage.h - Stages of activation for hotkeys.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file hotkey/stage.h
 * \brief Stages of activation for \ref mcr_HotStaged
 * */

# ifndef MCR_STAGE_H
# define MCR_STAGE_H

# include "hotkey/mods.h"

typedef struct mcr_Stage mcr_Stage ;

typedef int ( * mcr_isme_fnc )
		( mcr_Stage *, mcr_Signal *, unsigned int ) ;

typedef struct mcr_Stage
{
	int activated ;
	int blocking ;
	mcr_Signal * intercept_pt ;
	size_t measurement_error ;
	unsigned int modifiers ;
	// Interface for stage activating.
	mcr_isme_fnc isme ;
} mcr_Stage ;

extern mcr_isme_fnc mcr_Stage_ismeGeneric ;

MCR_API void mcr_Stage_init_with ( mcr_Stage * stagePt,
		int blocking, mcr_Signal * interceptPt,
		size_t measurement_error, unsigned int modifiers ) ;
MCR_API void mcr_Stage_set ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

MCR_API mcr_isme_fnc mcr_Stage_isme_for ( size_t id ) ;
MCR_API size_t mcr_Stage_count ( ) ;
MCR_API void mcr_Stage_get_all ( mcr_isme_fnc * fncBuffer,
		size_t bufferLength ) ;

MCR_API int mcr_Stage_isref ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_isalarm ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_isecho ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_iskey ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_ismovecursor ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_isnoop ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_isscroll ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_ismod ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

//
// Stage development
//
MCR_API int mcr_Stage_set_isme ( mcr_isme_fnc setIsme,
		size_t signalId ) ;
MCR_API void mcr_Stage_clear ( ) ;

MCR_API void mcr_stage_initialize ( ) ;
MCR_API void mcr_stage_cleanup ( ) ;

# define MCR_STAGE_ISTYPE( sPt, sigPt ) \
	( ! ( sPt )->intercept_pt || ( sPt )->intercept_pt->type->id == \
			( sigPt )->type->id )

# endif
