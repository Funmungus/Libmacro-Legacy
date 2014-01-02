
# include "signal/signal.h"

MCR_API mcr_signal_fnc mcr_AllDispatch = NULL ;
static mcr_Array _iSignalSet ;
static mcr_Map _nameMap ;

void mcr_Signal_init ( mcr_Signal * sigPt, mcr_ISignal * type )
{
	sigPt->type = type ;
}
void mcr_Signal_init_with ( mcr_Signal * sigPt, mcr_ISignal * type,
		void * data )
{
	sigPt->type = type ;
	sigPt->data = data ;
}

int mcr_send ( mcr_Signal * sigPt )
{
	int success = 1 ;
	MCR_SEND ( sigPt, success ) ;
	return success ;
}
void mcr_ISignal_init ( mcr_ISignal * newType, const char * name,
		mcr_signal_fnc sender, size_t dataSize )
{
	MCR_ISIGNAL_INIT ( newType, name, sender, dataSize ) ;
}

//
// Signal development: Add signal type to end, replace signal type,
//
size_t mcr_ISignal_register ( mcr_ISignal * newType )
{
	size_t id = _iSignalSet.used ;
	if ( ! mcr_Array_push ( & _iSignalSet, & newType ) )
	{
		DMSG ( "%s\n", "register" ) ;
		return -1 ;
	}
	// If successful, we can set the id.
	newType->id = id ;
	if ( ! mcr_Map_map ( & _nameMap, ( void * ) & newType->name,
			& newType ) )
	{
		mcr_Array_pop ( & _iSignalSet ) ; // Remove unmapped signal.
		DMSG ( "%s\n", "register" ) ;
		return -1 ;
	}
	return id ;
}

mcr_ISignal * mcr_ISignal_get ( size_t typeId )
{
	mcr_ISignal ** ret = MCR_ARR_AT ( & _iSignalSet, typeId ) ;
	return ret ? * ret : NULL ;
}

mcr_ISignal * mcr_ISignal_from_name ( const char * typeName )
{
	mcr_ISignal ** found = MCR_MAP_GET ( & _nameMap, & typeName ) ;
	if ( ! found )
		return NULL ;
	found = MCR_MAP_VALUE ( & _nameMap, found ) ;
	return * found ;
}

size_t mcr_ISignal_get_id ( const char * typeName )
{
	mcr_ISignal * sig = mcr_ISignal_from_name ( typeName ) ;
	if ( ! sig )
		return -1 ;
	return sig->id ;
}

size_t mcr_ISignal_count ( )
{
	return _iSignalSet.used ;
}

void mcr_ISignal_get_all ( mcr_ISignal ** buffer, size_t bufferLength )
{
	mcr_ISignal ** arr = ( mcr_ISignal ** ) _iSignalSet.array ;
	for ( size_t i = 0 ; i < bufferLength && i < _iSignalSet.used ; i ++ )
	{
		buffer [ i ] = arr [ i ] ;
	}
}

int mcr_name_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : lhs > rhs ;
	}
	return strcasecmp ( * ( const char * const * ) lhs,
						 * ( const char * const * ) rhs ) ;
}

int mcr_int_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : lhs > rhs ;
	}
	return * ( const int * ) lhs < * ( const int * ) rhs ? -1 :
			 * ( const int * ) lhs > * ( const int * ) rhs ;
}

int mcr_unsigned_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : lhs > rhs ;
	}
	return * ( const unsigned int * ) lhs <
			 * ( const unsigned int * ) rhs ? -1 :
			 * ( const unsigned int * ) lhs >
			 * ( const unsigned int * ) rhs ;
}

int mcr_size_t_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : lhs > rhs ;
	}
	return * ( const size_t * ) lhs < * ( const size_t * ) rhs ? -1 :
			 * ( const size_t * ) lhs > * ( const size_t * ) rhs ;
}

int mcr_ref_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : lhs > rhs ;
	}
	return * ( void * const * ) lhs < * ( void * const * ) rhs ? -1 :
			 * ( void * const * ) lhs > * ( void * const * ) rhs ;
}

void mcr_signal_cleanup ( void )
{
	mcr_standard_native_cleanup ( ) ;
	mcr_standard_cleanup ( ) ;
	mcr_Array_free ( & _iSignalSet ) ;
	mcr_Map_free ( & _nameMap ) ;
}

void mcr_signal_initialize ( )
{
	mcr_Array_init ( & _iSignalSet, sizeof ( mcr_ISignal * ) ) ;
	mcr_Map_init ( & _nameMap, sizeof ( const char * ),
			sizeof ( mcr_ISignal * ) ) ;
	_nameMap.compare = mcr_name_compare ;
	mcr_standard_initialize ( ) ;
	mcr_standard_native_initialize ( ) ;
}
