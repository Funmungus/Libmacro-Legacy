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
	int blocking ;
	mcr_Signal intercept ;
	size_t measurement_error ;
	unsigned int modifiers ;
	//
	// Internal
	//
	int activated ;
	//
	// Interface for stage activating.
	//
	mcr_isme_fnc isme ;
	mcr_isme_fnc resembles ;
} mcr_Stage ;

extern mcr_isme_fnc mcr_Stage_ismeGeneric ;
extern mcr_isme_fnc mcr_Stage_resembleGeneric ;

MCR_API void mcr_Stage_init ( mcr_Stage * stagePt ) ;
MCR_API void mcr_Stage_init_with ( mcr_Stage * stagePt,
		int blocking, mcr_Signal * interceptPt,
		size_t measurement_error, unsigned int modifiers ) ;
MCR_API void mcr_Stage_free ( mcr_Stage * stagePt ) ;
MCR_API void mcr_Stage_set ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API void mcr_Stage_set_generic ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

MCR_API int mcr_Stage_compare ( const void * lhs, const void * rhs ) ;

//
// Interface development.
//
MCR_API mcr_isme_fnc mcr_Stage_isme_for ( size_t id ) ;
MCR_API mcr_isme_fnc mcr_Stage_resemble_for ( size_t id ) ;
MCR_API size_t mcr_Stage_count ( ) ;
MCR_API void mcr_Stage_get_all ( mcr_isme_fnc * fncBuffer,
		mcr_isme_fnc * resembleBuffer,
		size_t bufferLength ) ;

MCR_API int mcr_Stage_isref ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_resemble_ref ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

MCR_API int mcr_Stage_isalarm ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
// ialarm default

MCR_API int mcr_Stage_isecho ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_resemble_echo ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

MCR_API int mcr_Stage_iskey ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_resemble_key ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

MCR_API int mcr_Stage_ismovecursor ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_resemble_movecursor ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

MCR_API int mcr_Stage_isnoop ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
// inoop default

MCR_API int mcr_Stage_isscroll ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_resemble_scroll ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

MCR_API int mcr_Stage_ismod ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;
MCR_API int mcr_Stage_resemble_mod ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods ) ;

//
// Stage development
//
MCR_API int mcr_Stage_set_isme ( mcr_isme_fnc setIsme,
		size_t signalId ) ;
MCR_API int mcr_Stage_set_resemble ( mcr_isme_fnc setResemble,
		size_t signalId ) ;
MCR_API void mcr_Stage_clear ( ) ;

MCR_API void mcr_stage_initialize ( ) ;
MCR_API void mcr_stage_cleanup ( ) ;

# define MCR_STAGE_ISME( sPt, sigPt, mods ) \
	( ( sPt )->isme ? \
		( sPt )->isme ( sPt, sigPt, mods ) : \
	( sPt )->modifiers != MCR_ANY_MOD && \
			( sPt )->modifiers != mods ? \
		0 : \
	! ( sPt )->intercept.type || \
			! ( sPt )->intercept.data.data ? \
		1 : \
	! ( MCR_SIGNAL_CMP ( & ( sPt )->intercept, sigPt ) ) )

# define MCR_STAGE_RESEMBLES( sPt, sigPt, mods ) \
	( ( sPt )->resembles ? ( sPt )->resembles ( sPt, sigPt, mods ) : \
			! ( sPt )->intercept.type || \
			( sPt )->intercept.type == ( sigPt )->type )

# endif
