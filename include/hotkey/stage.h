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
	unsigned int measurement_error ;
	unsigned int modifiers ;
	mcr_TriggerFor trigger_for ;
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

MCR_API extern mcr_isme_fnc mcr_Stage_ismeGeneric ;
MCR_API extern mcr_isme_fnc mcr_Stage_resembleGeneric ;

MCR_API void mcr_Stage_init ( mcr_Stage * stagePt ) ;
MCR_API void mcr_Stage_init_with ( mcr_Stage * stagePt,
		int blocking, mcr_Signal * interceptPt,
		unsigned int measurement_error, unsigned int modifiers,
		mcr_TriggerFor trigFor ) ;
MCR_API void mcr_Stage_free ( mcr_Stage * stagePt ) ;
# define mcr_Stage_free_foreach( sPt, ... ) mcr_Stage_free ( sPt )
MCR_API void mcr_Stage_set ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods,
		mcr_TriggerFor trigFor ) ;
MCR_API void mcr_Stage_set_generic ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods,
		mcr_TriggerFor trigFor ) ;

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

# define MCR_STAGE_ISME( stage, sigPt, mods ) \
	( ( stage ).isme ? \
		( stage ).isme ( & ( stage ), sigPt, mods ) : \
	( stage ).modifiers != MCR_ANY_MOD && \
			( stage ).modifiers != mods ? \
		0 : \
	! ( stage ).intercept.type || \
			! ( stage ).intercept.data.data ? \
		1 : \
	! ( MCR_SIGNAL_CMP ( ( stage ).intercept, * sigPt ) ) )

# define MCR_STAGE_RESEMBLES( stage, sigPt, mods ) \
	( ( stage ).resembles ? ( stage ).resembles ( & ( stage ), sigPt, mods ) : \
			! ( stage ).intercept.type || \
			( stage ).intercept.type == ( sigPt )->type )

# endif
