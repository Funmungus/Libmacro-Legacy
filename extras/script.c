/* extras/option.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "extras/script.h"

MCR_API const char * mcr_helpOptionName = "help" ;
MCR_API mcr_option_fnc mcr_defaultOption = NULL ;
MCR_API const char * mcr_descNotAvailable =
		"Description is not available." ;
MCR_API const char * mcr_parseError =
		"A parsing error has occurred." ;

static mcr_Map _optionMap ;
static mcr_Map _optDescMap ;

static void script_copy_foreach ( mcr_SafeString * ssPt,
		mcr_Script * scrPt ) ;

void mcr_Script_init ( void * scriptPt )
{
	if ( ! scriptPt )
	{
		dmsg ;
		return ;
	}
	mcr_Array_init ( & ( ( mcr_Script * ) scriptPt )->scripts,
			sizeof ( mcr_SafeString ) ) ;
}

void mcr_Script_free ( void * scriptPt )
{
	if ( ! scriptPt )
	{
		dmsg ;
		return ;
	}
	MCR_ARR_FOR_EACH ( & ( ( mcr_Script * ) scriptPt )->scripts,
			mcr_SafeString_free_foreach, 0 ) ;
	mcr_Array_free ( & ( ( mcr_Script * ) scriptPt )->scripts ) ;
}

void mcr_Script_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	mcr_Script * dPt = dstPt, * sPt = srcPt ;
	mcr_Script_free ( dPt ) ;
	if ( ! mcr_Array_resize ( & dPt->scripts, sPt->scripts.used ) )
	{
		dmsg ;
		return ;
	}
	MCR_ARR_FOR_EACH ( & sPt->scripts, script_copy_foreach, dPt ) ;
}

int mcr_Script_compare ( const void * lhs, const void * rhs )
{
	dassert ( lhs ) ;
	dassert ( rhs ) ;
	const mcr_Script * lPt = lhs, * rPt = rhs ;
	if ( lPt->scripts.used != rPt->scripts.used )
		return lPt->scripts.used < rPt->scripts.used ? -1 : 1 ;
	int ret = 0 ;
	mcr_SafeString * lss = MCR_ARR_AT ( & lPt->scripts, 0 ) ;
	mcr_SafeString * rss = MCR_ARR_AT ( & rPt->scripts, 0 ) ;
	for ( size_t i = 0 ; ! ret && i < lPt->scripts.used ; i ++ )
	{
		ret = mcr_SafeString_compare ( lss + i, rss + i ) ;
	}
	return ret ;
}

int mcr_Script_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	mcr_Script * scrPt = sigPt->data.data ;
	if ( ! scrPt )
		return 0 ;
	mcr_Array scripts ;
	mcr_Array_init ( & scripts, sizeof ( mcr_Array ) ) ;
	char ** args = malloc ( scrPt->scripts.used * sizeof ( void * ) ) ;
	if ( args && mcr_Array_resize ( & scripts, scrPt->scripts.used ) )
	{
		scripts.used = scripts.size ;
		mcr_Array * arrArr = ( mcr_Array * ) scripts.array ;
		for ( size_t i = 0 ; i < scripts.used ; i ++ )
		{
			arrArr [ i ] = mcr_SafeString_get ( MCR_ARR_AT (
					& scrPt->scripts, i ) ) ;
			args [ i ] = arrArr [ i ].array ;
		}
		mcr_Script_option ( ( int ) scripts.used, args ) ;
	}
	else
		dmsg ;
	MCR_ARR_FOR_EACH ( & scripts, mcr_Array_free_foreach, 0 ) ;
	mcr_Array_free ( & scripts ) ;
	free ( args ) ;
	return 1 ;
}

void mcr_Script_option ( int argc, char ** argv )
{
	int i = 0, next = 0 ;
	while ( i < argc )
	{
		next = mcr_Script_do_option ( argc, argv, i ) ;
		if ( next <= i )
		{
			dmsg ;
			mcr_Script_parse_error ( ) ;
			++ i ;
		}
		else
			i = next ;
	}
}

int mcr_Script_do_option ( int argc, char ** argv, int index )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;
	int next = index ;
	mcr_option_fnc * found = MCR_STRINGMAP_GET_VALUE ( & _optionMap,
			argv [ index ] ) ;
	if ( found )
	{
		dassert ( * found ) ;
		next = ( * found ) ( argc, argv, index ) ;
	}
	if ( mcr_defaultOption && next == index )
	{
		next = mcr_defaultOption ( argc, argv, index ) ;
	}
	return next ;
}

void mcr_Script_set_option ( const char * key, mcr_option_fnc optFnc )
{
	dassert ( key ) ;
	if ( ! optFnc )
	{
		mcr_StringMap_unmap ( & _optionMap, key ) ;
	}
	else if ( ! mcr_StringMap_map ( & _optionMap, key, & optFnc ) )
		dmsg ;
}

int mcr_Script_short_help ( int argc, char ** argv, int index )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;

	if ( index + 1 < argc )
	{
		char * helpArgs [ 2 ] =
				{ argv [ index + 1 ], ( char * ) mcr_helpOptionName } ;
		int handled = mcr_Script_do_option ( 2, helpArgs, 0 ) ;
		/* If both the option and "help" are not handled, assume
		 * help was not available for given option. */
		if ( handled > 1 )
			return index + 2 ;
		else
		{
			fprintf ( mcr_stdout, "%s\n", mcr_descNotAvailable ) ;
			return index + 1 ;
		}
	}
	for ( size_t i = 0 ; i < _optionMap.set.used ; i ++ )
	{
		mcr_Array * optName, * optHelp ;
		optName = MCR_ARR_AT ( & _optionMap.set, i ) ;
		dassert ( optName ) ;
		optHelp = MCR_STRINGMAP_GET_VALUE ( & _optDescMap,
				optName->array ) ;
		fprintf ( mcr_stdout, "%-32s - %-s\n", optName->array,
				optHelp ? optHelp->array : mcr_descNotAvailable ) ;
	}
	/* Handled help without argument specified. */
	return index + 1 ;
}

void mcr_Script_parse_error ( )
{
	time_t t ;
	struct tm * tInfo ;
	time ( & t ) ;
	tInfo = localtime ( & t ) ;
	dassert ( tInfo ) ;
	fprintf ( mcr_stderr, "%s:%s", asctime ( tInfo ),
			mcr_parseError ) ;
}

void mcr_script_initialize ( )
{
	mcr_StringMap_init ( & _optionMap, sizeof ( mcr_option_fnc ) ) ;
	mcr_StringMap_init ( & _optDescMap, sizeof ( mcr_Array ) ) ;
	mcr_Script_set_option ( mcr_helpOptionName, mcr_Script_short_help ) ;
	// TODO script isignal
}

void mcr_script_cleanup ( )
{
	MCR_MAP_FOR_EACH ( & _optionMap, mcr_Array_free_foreach, 0 ) ;
	MCR_MAP_FOR_EACH_VALUE ( & _optDescMap, mcr_Array_free_foreach,
			0 ) ;
	MCR_MAP_FOR_EACH ( & _optDescMap, mcr_Array_free_foreach, 0 ) ;
	mcr_Map_free ( & _optionMap ) ;
	mcr_Map_free ( & _optDescMap ) ;
}

static void script_copy_foreach ( mcr_SafeString * ssPt,
		mcr_Script * scrPt )
{
	mcr_SafeString ss ;
	mcr_SafeString_init ( & ss ) ;
	mcr_SafeString_copy ( & ss, ssPt ) ;

	if ( ! mcr_Array_push ( & scrPt->scripts, & ss ) )
		mcr_SafeString_free ( & ss ) ;
}
