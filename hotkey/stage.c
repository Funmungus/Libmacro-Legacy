
# include "hotkey/stage.h"

MCR_API mcr_isme_fnc mcr_Stage_ismeGeneric ;

static mcr_Array _ismeSet ;

void mcr_Stage_init_with ( mcr_Stage * stagePt, int blocking,
		mcr_Signal * interceptPt, size_t measurement_error,
		unsigned int modifiers )
{
	dassert ( stagePt ) ;
	memset ( stagePt, 0, sizeof ( mcr_Stage ) ) ;
	mcr_isme_fnc isme = mcr_Stage_isme_for ( interceptPt &&
			interceptPt->type ? interceptPt->type->id :
			( size_t ) -1 ) ;
	stagePt->blocking = blocking ;
	stagePt->intercept_pt = interceptPt ;
	stagePt->measurement_error = measurement_error ;
	stagePt->modifiers = modifiers ;
	stagePt->isme = isme ;
}

void mcr_Stage_set ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	dassert ( stagePt ) ;
	mcr_isme_fnc isme = mcr_Stage_isme_for ( interceptPt &&
			interceptPt->type ? interceptPt->type->id :
			( size_t ) -1 ) ;
	stagePt->intercept_pt = interceptPt ;
	stagePt->modifiers = mods ;
	stagePt->isme = isme ;
}

mcr_isme_fnc mcr_Stage_isme_for ( size_t id )
{
	if ( id > _ismeSet.used )
		return mcr_Stage_ismeGeneric ;
	return * ( mcr_isme_fnc * )
			MCR_ARR_AT ( & _ismeSet, id ) ;
}

size_t mcr_Stage_count ( )
{
	return _ismeSet.used ;
}

void mcr_Stage_get_all ( mcr_isme_fnc * fncBuffer,
		size_t bufferLength )
{
	dassert ( fncBuffer ) ;
	for ( size_t i = 0 ; i < _ismeSet.used && i < bufferLength ;
			i ++ )
	{
		fncBuffer [ i ] = mcr_Stage_isme_for ( i ) ;
	}
}

# define CHKMOD( sPt, chkMods ) \
	if ( ( sPt )->modifiers != MCR_ANY_MOD && \
			( sPt )->modifiers != chkMods ) \
		return 0 ;

int mcr_Stage_isref ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	dassert ( stagePt ) ;
	CHKMOD ( stagePt, mods ) ;
	// Case 1: Null intercept accepts all.
	if ( ! stagePt->intercept_pt )
		return 1 ;
	// Case 2: Existing intercept does not accept NULL incomming.
	if ( interceptPt == NULL ) return 0 ;
	// Case 3: If both exist, the address must be the same.
	return stagePt->intercept_pt == interceptPt ;
}

# define CHKPT( stagePt, interceptPt ) \
	dassert ( stagePt ) ; \
	dassert ( ( interceptPt ) ? ( interceptPt )->type != NULL : 1 ) ; \
	dassert ( ( stagePt )->intercept_pt ? ( stagePt )->intercept_pt->type \
			!= NULL : 1 ) ;

# define CHKID( intercept, chkId ) \
	if ( ! ( interceptPt ) || ( interceptPt )->type->id != ( chkId ) ) \
		return 0 ;

# define ANYSTAGE( stagePt ) \
	if ( ! stagePt->intercept_pt || ! stagePt->intercept_pt->data ) \
		return 1 ;

# define BADDATA( interceptPt ) \
	if ( ! interceptPt->data ) \
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
	ANYSTAGE ( stagePt ) ; \
	BADDATA ( interceptPt ) ;

int mcr_Stage_isalarm ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_IAlarm.id ) ;
	time_t me = mktime ( stagePt->intercept_pt->data ) ;
	time_t them = mktime ( interceptPt->data ) ;
	if ( me == -1 )
		return 1 ;
	if ( them == -1 )
		return 0 ;
	them = me - them ;
	if ( them < 0 )
		them *= -1 ;
	return ( unsigned int ) them <= stagePt->measurement_error ;
}

int mcr_Stage_isecho ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_IHIDEcho.id ) ;
	int me = MCR_ECHO_GET ( ( mcr_HIDEcho * )
			stagePt->intercept_pt->data ) ;
	int them = MCR_ECHO_GET ( ( mcr_HIDEcho * ) interceptPt->data ) ;
	if ( me == ( int ) MCR_ANY_MOD )
		return 1 ;
	return me == them ;
}

int mcr_Stage_iskey ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_IKey.id ) ;
	mcr_Key * mePt = stagePt->intercept_pt->data ;
	mcr_Key * themPt = interceptPt->data ;
	int me = MCR_KEY_GET ( mePt ) ;
	int them = MCR_KEY_GET ( themPt ) ;
	if ( me != MCR_ANY_KEY && me != them )
		return 0 ;
	me = MCR_KEY_GET_SCAN ( mePt ) ;
	them = MCR_KEY_GET_SCAN ( themPt ) ;
	if ( me != MCR_ANY_KEY && me != them )
		return 0 ;
	me = MCR_KEY_GET_UP_TYPE ( mePt ) ;
	them = MCR_KEY_GET_UP_TYPE ( themPt ) ;
	return me == MCR_BOTH || me == them ;
}

int mcr_Stage_ismovecursor ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_IMoveCursor.id ) ;
	return mcr_resembles ( ( mcr_MoveCursor * )
			stagePt->intercept_pt->data, ( mcr_MoveCursor * )
			interceptPt->data, stagePt->measurement_error ) ;
}

int mcr_Stage_isnoop ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_INoOp.id ) ;
	time_t me = ( ( mcr_NoOp * ) stagePt->intercept_pt->data )->tv_sec,
			them = ( ( mcr_NoOp * ) interceptPt->data )->tv_sec ;
	them = me - them ;
	if ( them < 0 )
		them *= -1 ;
	return ( size_t ) them <= stagePt->measurement_error ;
}

int mcr_Stage_isscroll ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_IScroll.id ) ;
	mcr_Dimensions lhs, rhs ;
	MCR_SCROLL_GET ( ( mcr_Scroll * ) stagePt->intercept_pt->data, lhs ) ;
	MCR_SCROLL_GET ( ( mcr_Scroll * ) interceptPt->data, rhs ) ;
	return mcr_resembles_justified ( lhs, rhs ) ;
}

int mcr_Stage_ismod ( mcr_Stage * stagePt,
		mcr_Signal * interceptPt, unsigned int mods )
{
	ISCOMMONS ( stagePt, interceptPt, mods, mcr_IMod.id ) ;
	mcr_Mod * mePt = stagePt->intercept_pt->data,
			 * themPt = interceptPt->data ;
	if ( mePt->modifiers != MCR_ANY_MOD &&
			mePt->modifiers != themPt->modifiers )
		return 0 ;
	return mePt->up_type == MCR_BOTH ||
			mePt->up_type == themPt->up_type ;
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

void mcr_Stage_clear ( )
{
	mcr_Array_free ( & _ismeSet ) ;
}

void mcr_stage_initialize ( )
{
	mcr_Array_init ( & _ismeSet, sizeof ( mcr_isme_fnc ) ) ;
	size_t ids [ ] = {
		-1, mcr_IAlarm.id, mcr_IHIDEcho.id, mcr_IKey.id,
		mcr_IMoveCursor.id, mcr_INoOp.id, mcr_IScroll.id, mcr_IMod.id
	} ;
	mcr_isme_fnc fncs [ ] = {
		mcr_Stage_isref, mcr_Stage_isalarm,
		mcr_Stage_isecho, mcr_Stage_iskey,
		mcr_Stage_ismovecursor, mcr_Stage_isnoop,
		mcr_Stage_isscroll, mcr_Stage_ismod
	} ;
	size_t len = sizeof ( ids ) / sizeof ( size_t ) ;
	for ( unsigned int i = 0 ; i < len ; i ++ )
	{
		mcr_Stage_set_isme ( fncs [ i ], ids [ i ] ) ;
	}
}

void mcr_stage_cleanup ( )
{
	mcr_Stage_clear ( ) ;
}
