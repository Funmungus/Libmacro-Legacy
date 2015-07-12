/* hotkey/stage.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "hotkey/stage.h"

MCR_API mcr_isme_fnc mcr_Stage_ismeGeneric ;
MCR_API mcr_isme_fnc mcr_Stage_resembleGeneric ;

static mcr_Array _ismeSet ;
static mcr_Array _resembleSet ;

void mcr_Stage_init ( mcr_Stage * stagePt )
{
	if ( ! stagePt )
		return ;
	memset ( stagePt, 0, sizeof ( mcr_Stage ) ) ;
	stagePt->isme = mcr_Stage_ismeGeneric ;
	stagePt->resembles = mcr_Stage_resembleGeneric ;
}

void mcr_Stage_init_with ( mcr_Stage * stagePt, int blocking,
		mcr_Signal * interceptPt, unsigned int measurement_error,
		unsigned int modifiers )
{
	if ( ! stagePt )
		return ;
	memset ( stagePt, 0, sizeof ( mcr_Stage ) ) ;
	stagePt->blocking = blocking ;
	stagePt->measurement_error = measurement_error ;
	mcr_Stage_set ( stagePt, interceptPt, modifiers ) ;
}

void mcr_Stage_free ( mcr_Stage * stagePt )
{
	if ( ! stagePt )
		return ;
	mcr_Signal_free ( & stagePt->intercept ) ;
}

void mcr_Stage_set ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	dassert ( stagePt ) ;
	if ( interceptPt && interceptPt->type )
	{
		mcr_Signal_copy ( & stagePt->intercept, interceptPt ) ;
		stagePt->isme = mcr_Stage_isme_for (
			interceptPt->type->iface.id ) ;
		stagePt->resembles = mcr_Stage_resemble_for (
			interceptPt->type->iface.id ) ;
		stagePt->modifiers = mods ;
	}
	else
	{
		mcr_Stage_set_generic ( stagePt, interceptPt, mods ) ;
	}
}

void mcr_Stage_set_generic ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	dassert ( stagePt ) ;
	mcr_Signal_free ( & stagePt->intercept ) ;
	stagePt->intercept.type = NULL ;
	stagePt->intercept.data.data = interceptPt ;
	stagePt->isme = mcr_Stage_ismeGeneric ;
	stagePt->resembles = mcr_Stage_resembleGeneric ;
	stagePt->modifiers = mods ;
}

int mcr_Stage_compare ( const void * lhs, const void * rhs )
{
	dassert ( lhs ) ;
	dassert ( rhs ) ;
	const mcr_Stage * lPt = lhs, * rPt = rhs ;
	int ret = MCR_SIGNAL_CMP ( & lPt->intercept, & rPt->intercept ) ;
	if ( ret )
		return ret ;
	if ( lPt->modifiers != rPt->modifiers )
		return lPt->modifiers < rPt->modifiers ? -1 : 1 ;
	if ( lPt->blocking != rPt->blocking )
		return lPt->blocking < rPt->blocking ? -1 : 1 ;
	if ( lPt->measurement_error != rPt->measurement_error )
		return lPt->measurement_error < rPt->measurement_error ? -1 : 1 ;
	return 0 ;
}

mcr_isme_fnc mcr_Stage_isme_for ( size_t id )
{
	if ( id == ( size_t ) -1 )
		return mcr_Stage_ismeGeneric ;
	if ( id > _ismeSet.used )
		return NULL ;
	return * ( mcr_isme_fnc * )
			MCR_ARR_AT ( & _ismeSet, id ) ;
}

mcr_isme_fnc mcr_Stage_resemble_for ( size_t id )
{
	if ( id == ( size_t ) -1 )
		return mcr_Stage_resembleGeneric ;
	if ( id > _resembleSet.used )
		return NULL ;
	return * ( mcr_isme_fnc * )
			MCR_ARR_AT ( & _resembleSet, id ) ;
}

size_t mcr_Stage_count ( )
{
	return _ismeSet.used ;
}

void mcr_Stage_get_all ( mcr_isme_fnc * fncBuffer,
		mcr_isme_fnc * resembleBuffer, size_t bufferLength )
{
	if ( fncBuffer )
	{
		for ( size_t i = 0 ; i < _ismeSet.used && i < bufferLength ;
				i ++ )
		{
			fncBuffer [ i ] = mcr_Stage_isme_for ( i ) ;
		}
	}
	if ( resembleBuffer )
	{
		for ( size_t i = 0 ; i < _resembleSet.used && i < bufferLength ;
				i ++ )
		{
			resembleBuffer [ i ] = mcr_Stage_resemble_for ( i ) ;
		}
	}
}

# define CHKMOD( sPt, chkMods ) \
	if ( ( sPt )->modifiers != MCR_ANY_MOD && \
			( sPt )->modifiers != chkMods ) \
		return 0 ;

# define refcommons( stagePt, interceptPt ) \
	return ! ( stagePt )->intercept.data.data || \
			( stagePt )->intercept.data.data == ( interceptPt ) || \
			! mcr_Signal_compare ( ( stagePt )->intercept.data.data, \
			( interceptPt ) ) ;
int mcr_Stage_isref ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	dassert ( stagePt ) ;
	CHKMOD ( stagePt, mods ) ;
	// Case 1: Null intercept accepts all.
	// Case 2: Existing intercept does not accept NULL incomming.
	// Case 3: If both exist, the address must be the same,
	// or comparatively the same.
	refcommons ( stagePt, interceptPt ) ;
}

int mcr_Stage_resemble_ref ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	dassert ( stagePt ) ;
	UNUSED ( mods ) ;
	refcommons ( stagePt, interceptPt ) ;
}

# define CHKPT( stagePt, interceptPt ) \
	dassert ( stagePt ) ; \
	dassert ( ( interceptPt ) ? ( interceptPt )->type != NULL : 1 ) ; \
	dassert ( ( stagePt )->intercept.type != NULL ) ;

# define CHKID( interceptPt, chkId ) \
	if ( ! ( interceptPt ) || ( interceptPt )->type->iface.id \
			!= ( chkId ) ) \
		return 0 ;

# define ANYDATA( stagePt ) \
	if ( ! ( stagePt )->intercept.data.data ) \
		return 1 ;

# define BADDATA( interceptPt ) \
	if ( ! interceptPt->data.data ) \
		return 0 ;

/* Common to most isme checks is to...
 * 1 ) Stage reference cannot be null, and any intercept that is not
 * NULL __must__ have a mcr_ISignal type.
 * 2 ) Return false if no intercept or intercept id is not correct.
 * 3 ) Return false if both stage mods are not accept any, and not
 * equal to checking mods.
 * 4 ) Return true there is no intercept or data in stage reference.
 * 5 ) Return false if there is not data in the intercept reference.
 * */
# define ISCOMMONS( stagePt, interceptPt, chkMods, chkId ) \
	CHKPT ( stagePt, interceptPt ) ; \
	CHKID ( interceptPt, chkId ) ; \
	CHKMOD ( stagePt, chkMods ) ; \
	ANYDATA ( stagePt ) ; \
	BADDATA ( interceptPt ) ;

# define ISICOMMONS( stagePt, interceptPt, chkMods, chkId ) \
	CHKPT ( stagePt, interceptPt ) ; \
	CHKID ( interceptPt, chkId ) ; \
	UNUSED ( chkMods ) ; \
	ANYDATA ( stagePt ) ; \
	BADDATA ( interceptPt ) ;

int mcr_Stage_isalarm ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_iAlarm.iface.id ) ;
	time_t me = mktime ( stagePt->intercept.data.data ) ;
	time_t them = mktime ( interceptPt->data.data ) ;
	if ( me == -1 )
		return 1 ;
	if ( them == -1 )
		return 0 ;
	them = them > me ? them - me : me - them ;
	return ( size_t ) them <= stagePt->measurement_error ;
}

// ialarm default

# define echocommons( stagePt, interceptPt ) \
	int me = MCR_ECHO_GET ( ( mcr_HIDEcho * ) \
			( stagePt )->intercept.data.data ) ; \
	return me == ( int ) MCR_ANY_MOD || \
			me == MCR_ECHO_GET ( \
			( mcr_HIDEcho * ) ( interceptPt )->data.data ) ;
int mcr_Stage_isecho ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_iHIDEcho.iface.id ) ;
	echocommons ( stagePt, interceptPt ) ;
}

int mcr_Stage_resemble_echo ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISICOMMONS ( stagePt, interceptPt, mods, mcr_iHIDEcho.iface.id ) ;
	echocommons ( stagePt, interceptPt ) ;
}

# define keycommons( mePt, themPt ) \
	int me = MCR_KEY_GET ( ( mePt ) ) ; \
	int them = MCR_KEY_GET ( ( themPt ) ) ; \
	if ( me != MCR_ANY_KEY && me != them ) \
		return 0 ; \
	me = MCR_KEY_GET_SCAN ( ( mePt ) ) ; \
	them = MCR_KEY_GET_SCAN ( ( themPt ) ) ; \
	if ( me != MCR_ANY_KEY && me != them ) \
		return 0 ;
int mcr_Stage_iskey ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_iKey.iface.id ) ;
	mcr_Key * mePt = stagePt->intercept.data.data ;
	mcr_Key * themPt = interceptPt->data.data ;
	keycommons ( mePt, themPt ) ;
	me = MCR_KEY_GET_UP_TYPE ( mePt ) ;
	them = MCR_KEY_GET_UP_TYPE ( themPt ) ;
	return me == MCR_BOTH || me == them ;
}
// For key type do not check Key up or mods.
int mcr_Stage_resemble_key ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISICOMMONS ( stagePt, interceptPt, mods, mcr_iKey.iface.id ) ;
	mcr_Key * mePt = stagePt->intercept.data.data ;
	mcr_Key * themPt = interceptPt->data.data ;
	keycommons ( mePt, themPt ) ;
	return 0 ;
}

int mcr_Stage_ismovecursor ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods,
			mcr_iMoveCursor.iface.id ) ;
	return mcr_resembles ( stagePt->intercept.data.data,
			interceptPt->data.data, stagePt->measurement_error ) ;
}

int mcr_Stage_resemble_movecursor ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISICOMMONS ( stagePt, interceptPt, mods,
			mcr_iMoveCursor.iface.id ) ;
	return mcr_resembles ( stagePt->intercept.data.data,
			interceptPt->data.data, stagePt->measurement_error ) ;
}

int mcr_Stage_isnoop ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_iNoOp.iface.id ) ;
	time_t me = ( ( mcr_NoOp * )
			stagePt->intercept.data.data )->tv_sec,
			them = ( ( mcr_NoOp * ) interceptPt->data.data )->tv_sec ;
	them = them > me ? them - me : me - them ;
	return ( size_t ) them <= stagePt->measurement_error ;
}

// inoop default
# define scrollcommons( stagePt, interceptPt, mods ) \
	mcr_Dimensions lhs, rhs ; \
	MCR_SCROLL_GET ( ( mcr_Scroll * ) \
			( stagePt )->intercept.data.data, lhs ) ; \
	MCR_SCROLL_GET ( ( mcr_Scroll * ) \
			( interceptPt )->data.data, rhs ) ; \
	return mcr_resembles_justified ( lhs, rhs ) ;
int mcr_Stage_isscroll ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_iScroll.iface.id ) ;
	scrollcommons ( stagePt, interceptPt, mods ) ;
}

int mcr_Stage_resemble_scroll ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISICOMMONS ( stagePt, interceptPt, mods, mcr_iScroll.iface.id ) ;
	scrollcommons ( stagePt, interceptPt, mods ) ;
}

# define modcommons( mePt, themPt ) \
	if ( ( mePt )->modifiers != MCR_ANY_MOD && \
			( mePt )->modifiers != ( themPt )->modifiers ) \
		return 0 ;
int mcr_Stage_ismod ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_iMod.iface.id ) ;
	mcr_Mod * mePt = stagePt->intercept.data.data,
			 * themPt = interceptPt->data.data ;
	modcommons ( mePt, themPt ) ;
	return mePt->up_type == MCR_BOTH ||
			mePt->up_type == themPt->up_type ;
}

int mcr_Stage_resemble_mod ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISICOMMONS ( stagePt, interceptPt, mods, mcr_iMod.iface.id ) ;
	mcr_Mod * mePt = stagePt->intercept.data.data,
			 * themPt = interceptPt->data.data ;
	modcommons ( mePt, themPt ) ;
	return 0 ;
}

//
// Stage development
//
int mcr_Stage_set_isme ( mcr_isme_fnc setIsme,
		size_t signalId )
{
	if ( signalId == ( size_t ) -1 )
	{
		mcr_Stage_ismeGeneric = setIsme ;
		return 1 ;
	}
	if ( signalId > _ismeSet.used )
		return mcr_Array_insert_filled ( & _ismeSet, signalId,
				& setIsme, NULL ) ;
	return mcr_Array_set ( & _ismeSet, signalId, & setIsme ) ;
}

int mcr_Stage_set_resemble ( mcr_isme_fnc setResemble,
		size_t signalId )
{
	if ( signalId == ( size_t ) -1 )
	{
		mcr_Stage_resembleGeneric = setResemble ;
		return 1 ;
	}
	if ( signalId > _resembleSet.used )
		return mcr_Array_insert_filled ( & _resembleSet, signalId,
				& setResemble, NULL ) ;
	return mcr_Array_set ( & _resembleSet, signalId, & setResemble ) ;
}

void mcr_Stage_clear ( )
{
	mcr_Array_free ( & _ismeSet ) ;
	mcr_Array_free ( & _resembleSet ) ;
}

void mcr_stage_initialize ( )
{
	mcr_Array_init ( & _ismeSet, sizeof ( mcr_isme_fnc ) ) ;
	mcr_Array_init ( & _resembleSet, sizeof ( mcr_isme_fnc ) ) ;
	size_t ids [ ] = {
		-1, mcr_iAlarm.iface.id, mcr_iHIDEcho.iface.id,
		mcr_iKey.iface.id, mcr_iMoveCursor.iface.id,
		mcr_iNoOp.iface.id, mcr_iScroll.iface.id,
		mcr_iMod.iface.id
	} ;
	mcr_isme_fnc fncs [ ] = {
		mcr_Stage_isref, mcr_Stage_isalarm,
		mcr_Stage_isecho, mcr_Stage_iskey,
		mcr_Stage_ismovecursor, mcr_Stage_isnoop,
		mcr_Stage_isscroll, mcr_Stage_ismod
	} ;
	mcr_isme_fnc resFncs [ ] = {
		mcr_Stage_resemble_ref, NULL,
		mcr_Stage_resemble_echo, mcr_Stage_resemble_key,
		mcr_Stage_resemble_movecursor, NULL,
		mcr_Stage_resemble_scroll, mcr_Stage_resemble_mod
	} ;
	size_t len = sizeof ( ids ) / sizeof ( size_t ) ;
	for ( unsigned int i = 0 ; i < len ; i ++ )
	{
		mcr_Stage_set_isme ( fncs [ i ], ids [ i ] ) ;
		mcr_Stage_set_resemble ( resFncs [ i ], ids [ i ] ) ;
	}
}

void mcr_stage_cleanup ( )
{
	mcr_Stage_clear ( ) ;
}
