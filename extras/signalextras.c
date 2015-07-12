/* extras/signalextras.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "extras/signalextras.h"

MCR_API mcr_ISignal mcr_iCommand ;
MCR_API mcr_ISignal mcr_iStringKey ;

// All data here will be dynamically allocated.
static mcr_Array _keyChars ;
static mcr_Array _charInitial ;
static mcr_Array _ssArrInitial ;

static int compare_args ( const mcr_Command * lhs,
		const mcr_Command * rhs ) ;
static int compare_env ( const mcr_Command * lhs,
		const mcr_Command * rhs ) ;
static void copy_args ( mcr_Command * dstPt, mcr_Command * srcPt ) ;
static void copy_env ( mcr_Command * dstPt, mcr_Command * srcPt ) ;

static void arr_set_delay_redirect ( mcr_Array * arrPt, mcr_NoOp delay ) ;
static void char_free_redirect ( mcr_Array * arrIt, ... ) ;

void mcr_Command_init ( void * cmdPt )
{
	if ( ! cmdPt )
	{
		dmsg ;
		return ;
	}
	mcr_Command * cPt = cmdPt ;
	mcr_SafeString_init ( & cPt->file ) ;
	mcr_Array_init ( & cPt->argv, sizeof ( mcr_SafeString ) ) ;
	mcr_Array_init ( & cPt->env, sizeof ( mcr_SafeString ) ) ;
}

void mcr_Command_init_with ( mcr_Command * cmdPt,
		const char * file, char * const argv [ ],
		char * const envp [ ], int cryptic )
{
	if ( ! cmdPt )
	{
		dmsg ;
		return ;
	}
	mcr_Command_init ( cmdPt ) ;
	mcr_SafeString_set ( & cmdPt->file, file,
			cryptic ) ;
	mcr_Command_set_args ( cmdPt, argv, cryptic ) ;
	mcr_Command_set_env ( cmdPt, envp, cryptic ) ;
}

int mcr_Command_compare ( const void * lhs, const void * rhs )
{
	dassert ( lhs ) ;
	dassert ( rhs ) ;
	const mcr_Command * lPt = lhs, * rPt = rhs ;
	if ( lPt->file.cryptic != rPt->file.cryptic )
		return lPt->file.cryptic < rPt->file.cryptic ? -1 : 1 ;
	int ret = mcr_SafeString_compare ( & lPt->file, & rPt->file ) ;
	if ( ret )
		return ret ;
	ret = compare_args ( lPt, rPt ) ;
	if ( ret )
		return ret ;
	return compare_env ( lPt, rPt ) ;
}

void mcr_Command_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	mcr_Command * dPt = dstPt, * sPt = srcPt ;
	mcr_Command_free ( dPt ) ;
	mcr_SafeString_copy ( & dPt->file, & sPt->file ) ;
	copy_args ( dPt, sPt ) ;
	copy_env ( dPt, sPt ) ;
}

void mcr_Command_free ( void * cmdPt )
{
	if ( ! cmdPt )
	{
		dmsg ;
		return ;
	}
	mcr_Command * cPt = cmdPt ;
	mcr_SafeString_free ( & cPt->file ) ;
	MCR_ARR_FOR_EACH ( & cPt->argv, mcr_SafeString_free_foreach, 0 ) ;
	MCR_ARR_FOR_EACH ( & cPt->env, mcr_SafeString_free_foreach, 0 ) ;
	mcr_Array_free ( & cPt->argv ) ;
	mcr_Array_free ( & cPt->env ) ;
}

void mcr_StringKey_init ( void * skPt )
{
	if ( ! skPt )
	{
		dmsg ;
		return ;
	}
	mcr_StringKey * sPt = skPt ;
	memset ( sPt, 0, sizeof ( mcr_NoOp ) ) ;
	mcr_SafeString_init ( & sPt->string ) ;
}

void mcr_StringKey_init_with ( mcr_StringKey * skPt,
		mcr_NoOp interval, const char * text, int cryptic )
{
	if ( ! skPt )
	{
		dmsg ;
		return ;
	}
	memcpy ( & skPt->interval, & interval, sizeof ( mcr_NoOp ) ) ;
	mcr_SafeString_init_with ( & skPt->string, text, cryptic ) ;
}

int mcr_StringKey_compare ( const void * lhs, const void * rhs )
{
	dassert ( lhs ) ;
	dassert ( rhs ) ;
	const mcr_StringKey * lPt = lhs, * rPt = rhs ;
	int ret = memcmp ( & lPt->interval, & rPt->interval,
			sizeof ( mcr_NoOp ) ) ;
	if ( ret )
		return ret ;
	return mcr_SafeString_compare ( & lPt->string, & rPt->string ) ;
}

void mcr_StringKey_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	dassert ( srcPt ) ;
	mcr_StringKey * dPt = dstPt, * sPt = srcPt ;
	dPt->interval = sPt->interval ;
	mcr_SafeString_copy ( & dPt->string, & sPt->string ) ;
}

void mcr_StringKey_free ( void * skPt )
{
	if ( ! skPt )
	{
		dmsg ;
		return ;
	}
	mcr_StringKey * sPt = skPt ;
	mcr_SafeString_free ( & sPt->string ) ;
}


void mcr_Command_set ( mcr_Command * cmdPt,
		const char * file, int cryptic )
{
	dassert ( cmdPt ) ;
	mcr_SafeString_set ( & cmdPt->file, file,
			cryptic ) ;
}

void mcr_Command_set_args ( mcr_Command * cmdPt,
		char * const argv [ ], int cryptic )
{
	dassert ( cmdPt ) ;
	MCR_ARR_FOR_EACH ( & cmdPt->argv, mcr_SafeString_free_foreach, 0 ) ;
	cmdPt->argv.used = 0 ;
	for ( size_t i = 0 ; argv [ i ] != NULL ; i ++ )
	{
		mcr_Command_add_arg ( cmdPt, argv [ i ], cryptic ) ;
	}
	mcr_Array_trim ( & cmdPt->argv ) ;
}

void mcr_Command_add_arg ( mcr_Command * cmdPt,
		char * const arg, int cryptic )
{
	dassert ( cmdPt ) ;
	if ( arg )
	{
		mcr_SafeString addString ;
		mcr_SafeString_init_with ( & addString,
				arg, cryptic ) ;
		if ( ! mcr_Array_push ( & cmdPt->argv, & addString ) )
		{
			dmsg ;
			mcr_SafeString_free ( & addString ) ;
		}
	}
}

void mcr_Command_clear_args ( mcr_Command * cmdPt )
{
	dassert ( cmdPt ) ;
	MCR_ARR_FOR_EACH ( & cmdPt->argv, mcr_SafeString_free_foreach, 0 ) ;
	mcr_Array_free ( & cmdPt->argv ) ;
}

void mcr_Command_set_env ( mcr_Command * cmdPt,
		char * const envp [ ], int cryptic )
{
	dassert ( cmdPt ) ;
	MCR_ARR_FOR_EACH ( & cmdPt->env, mcr_SafeString_free_foreach, 0 ) ;
	cmdPt->env.used = 0 ;
	for ( size_t i = 0 ; envp [ i ] != NULL ; i ++ )
	{
		mcr_Command_add_env ( cmdPt, envp [ i ], cryptic ) ;
	}
	mcr_Array_trim ( & cmdPt->env ) ;
}

void mcr_Command_add_env ( mcr_Command * cmdPt,
		char * const env, int cryptic )
{
	dassert ( cmdPt ) ;
	if ( env )
	{
		mcr_SafeString addString ;
		mcr_SafeString_init_with ( & addString,
				( char * const ) env, cryptic ) ;
		if ( ! mcr_Array_push ( & cmdPt->env, & addString ) )
		{
			dmsg ;
			mcr_SafeString_free ( & addString ) ;
		}
	}
}

void mcr_Command_clear_env ( mcr_Command * cmdPt )
{
	dassert ( cmdPt ) ;
	MCR_ARR_FOR_EACH ( & cmdPt->env, mcr_SafeString_free_foreach, 0 ) ;
	mcr_Array_free ( & cmdPt->env ) ;
}

int mcr_StringKey_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	if ( sigPt->data.data )
		return mcr_StringKey_send_keys ( sigPt->data.data ) ;
	return 0 ;
}

// TODO: Unicode support.
int mcr_StringKey_send_keys ( mcr_StringKey * skPt )
{
	dassert ( skPt ) ;
	int success = 1 ;
	mcr_Array * found ;
	mcr_Signal * sigPt, * end ;
	mcr_Array string ;
	mcr_Array * strPt = & string ;
	if ( skPt->string.cryptic )
		string = mcr_SafeString_get ( & skPt->string ) ;
	else
		strPt = & skPt->string.text ;
	for ( size_t i = 0 ; i < strPt->used && strPt->array [ i ] != '\0' ;
			i ++ )
	{
		found = MCR_ARR_AT ( & _keyChars,
				( unsigned char ) strPt->array [ i ] ) ;
		if ( found && found->used )
		{
			end = MCR_ARR_END ( found ) ;
			for ( sigPt = MCR_ARR_AT ( found, 0 ) ;
					sigPt < end ; sigPt ++ )
			{
				if ( ! MCR_SEND ( sigPt ) )
				{
					dmsg ;
					success = 0 ;
				}
			}
		}
	}
	if ( skPt->string.cryptic )
		mcr_Array_free ( & string ) ;
	return success ;
}

int mcr_Command_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	if ( sigPt->data.data )
		return mcr_Command_execvpe ( sigPt->data.data ) ;
	return 0 ;
}

//
// String key development
//
mcr_Array * mcr_StringKey_get_char ( int character )
{
	return MCR_ARR_AT ( & _keyChars, ( unsigned int ) character ) ;
}

void mcr_StringKey_set_char ( int character,
		mcr_Signal * charSignalArr, size_t arrLen )
{
	if ( character == -1 ) return ;
	if ( _keyChars.used <= ( unsigned int ) character )
	{
		if ( ! mcr_Array_insert_filled ( & _keyChars, character,
				& _charInitial, & _charInitial ) )
		{
			dmsg ;
			return ;
		}
	}
	mcr_Array * arrPt = MCR_ARR_AT ( & _keyChars,
			( unsigned int ) character ) ;
	if ( ! arrPt )
	{
		dmsg ;
		return ;
	}
	if ( charSignalArr && arrLen )
	{
		MCR_ARR_FOR_EACH ( arrPt, mcr_Signal_free_foreach, 0 ) ;
		if ( mcr_Array_resize ( arrPt, arrLen ) )
		{
			arrPt->used = arrLen ;
			mcr_Signal * sigs = ( mcr_Signal * ) arrPt->array ;
			for ( size_t i = 0 ; i < arrLen ; i ++ )
			{
				mcr_Signal_copy ( sigs + i, charSignalArr + i ) ;
			}
		}
	}
	else
	{
		dmsg ;
		mcr_StringKey_remove_char ( character ) ;
	}
}

void mcr_StringKey_remove_char ( int character )
{
	mcr_Array * arrPt = mcr_Array_at ( & _keyChars,
			( unsigned int ) character ) ;
	if ( arrPt )
	{
		MCR_ARR_FOR_EACH ( arrPt, mcr_Signal_free_foreach, 0 ) ;
		mcr_Array_free ( arrPt ) ;
	}
}

void mcr_StringKey_set_shifted ( int character,
		int key, long nsec )
{
	// shifted set is shift, key press, key release, and unshift
	// ( with delays is 7 in all )
	mcr_NoOp delay = { 0 } ;
	delay.tv_nsec = nsec ;
	mcr_Key keys [ 4 ] ;
	int shift = mcr_Mod_key_get_key ( MCR_SHIFT ) ;
	mcr_Key_init_with ( keys, shift, shift, MCR_DOWN ) ;
	mcr_Key_init_with ( keys + 1, key, key, MCR_DOWN ) ;
	mcr_Key_init_with ( keys + 2, key, key, MCR_UP ) ;
	mcr_Key_init_with ( keys + 3, shift, shift, MCR_UP ) ;
	mcr_Signal sigs [ 7 ] = {
	{ & mcr_iKey, { keys, 0 } }, { & mcr_iNoOp, { & delay, 0 } },
		{ & mcr_iKey, { keys + 1, 0 } }, { & mcr_iNoOp, { & delay, 0 } },
		{ & mcr_iKey, { keys + 2, 0 } }, { & mcr_iNoOp, { & delay, 0 } },
		{ & mcr_iKey, { keys + 3, 0 } }
	} ;
	mcr_StringKey_set_char ( character, sigs, 7 ) ;
}

void mcr_StringKey_set_nonshifted ( int character,
		int key, long nsec )
{
	// non-shifted set is key press, and key release
	// ( with delays is 3 in all )
	mcr_NoOp delay = { 0 } ;
	delay.tv_nsec = nsec ;
	mcr_Key keys [ 2 ] ;
	mcr_Key_init_with ( keys + 0, key, key, MCR_DOWN ) ;
	mcr_Key_init_with ( keys + 1, key, key, MCR_UP ) ;
	mcr_Signal sigs [ 3 ] = {
	{ & mcr_iKey, { keys, 0 } }, { & mcr_iNoOp, { & delay, 0 } },
		{ & mcr_iKey, { keys + 1, 0 } }
	} ;
	mcr_StringKey_set_char ( character, sigs, 3 ) ;
}

void mcr_StringKey_set_delays ( mcr_NoOp delay )
{
	MCR_ARR_FOR_EACH ( & _keyChars, arr_set_delay_redirect, delay ) ;
}

size_t mcr_StringKey_char_count ( )
{
	return _keyChars.used ;
}

void mcr_StringKey_char_clear ( )
{
	MCR_ARR_FOR_EACH ( & _keyChars, char_free_redirect, 0 ) ;
	mcr_Array_free ( & _keyChars ) ;
}

void mcr_load_contracts ( )
{
	mcr_Signal_load_contract ( ) ;
	mcr_Key_load_contract ( ) ;
	mcr_Echo_load_contract ( ) ;
	mcr_Hot_load_contract ( ) ;
	mcr_Mods_load_contract ( ) ;
	mcr_StringKey_load_contract ( ) ;
	mcr_ISignal_set_name ( & mcr_iCommand, "Command" ) ;
}

static int compare_args ( const mcr_Command * lhs,
		const mcr_Command * rhs )
{
	if ( lhs->argv.used != rhs->argv.used )
		return lhs->argv.used < rhs->argv.used ? -1 : 1 ;
	mcr_SafeString * lss = MCR_ARR_AT ( & lhs->argv, 0 ),
			 * rss = MCR_ARR_AT ( & rhs->argv, 0 ) ;
	size_t i = 0 ;
	int ret = 0 ;
	while ( ! ret && i < lhs->argv.used )
	{
		ret = mcr_SafeString_compare ( lss + i, rss + i ) ;
		++ i ;
	}
	return ret ;
}

static int compare_env ( const mcr_Command * lhs,
		const mcr_Command * rhs )
{
	if ( lhs->env.used != rhs->env.used )
		return lhs->env.used < rhs->env.used ? -1 : 1 ;
	mcr_SafeString * lss = MCR_ARR_AT ( & lhs->env, 0 ),
			 * rss = MCR_ARR_AT ( & rhs->env, 0 ) ;
	size_t i = 0 ;
	int ret = 0 ;
	while ( ! ret && i < lhs->env.used )
	{
		ret = mcr_SafeString_compare ( lss + i, rss + i ) ;
		++ i ;
	}
	return ret ;
}

static void copy_args ( mcr_Command * dstPt, mcr_Command * srcPt )
{
	mcr_Array arr ;
	mcr_SafeString * ssPt = ( mcr_SafeString * ) srcPt->argv.array ;
	for ( size_t i = 0 ; i < srcPt->argv.used ; i ++ )
	{
		if ( ssPt [ i ].cryptic )
		{
			arr = mcr_SafeString_get ( ssPt + i ) ;
			mcr_Command_add_arg ( dstPt, arr.array, 1 ) ;
			mcr_Array_free ( & arr ) ;
		}
		else
			mcr_Command_add_arg ( dstPt, ssPt [ i ].text.array, 0 ) ;
	}
}

static void copy_env ( mcr_Command * dstPt, mcr_Command * srcPt )
{
	mcr_Array arr ;
	mcr_SafeString * ssPt = ( mcr_SafeString * ) srcPt->env.array ;
	for ( size_t i = 0 ; i < srcPt->env.used ; i ++ )
	{
		if ( ssPt [ i ].cryptic )
		{
			arr = mcr_SafeString_get ( ssPt + i ) ;
			mcr_Command_add_env ( dstPt, arr.array, 1 ) ;
			mcr_Array_free ( & arr ) ;
		}
		else
			mcr_Command_add_env ( dstPt, ssPt [ i ].text.array, 0 ) ;
	}
}

static void set_delay_redirect ( mcr_Signal * sigPt, mcr_NoOp delay )
{
	if ( sigPt->type == & mcr_iNoOp )
	{
		if ( sigPt->data.data )
		{
			memcpy ( sigPt->data.data, & delay, sizeof ( mcr_NoOp ) ) ;
		}
	}
}

static void arr_set_delay_redirect ( mcr_Array * arrPt, mcr_NoOp delay )
{
	MCR_ARR_FOR_EACH ( arrPt, set_delay_redirect, delay ) ;
}

static void char_free_redirect ( mcr_Array * arrIt, ... )
{
	MCR_ARR_FOR_EACH ( arrIt, mcr_Signal_free_foreach, 0 ) ;
	mcr_Array_free ( arrIt ) ;
}

void mcr_signalextras_initialize ( )
{
	mcr_ISignal_init_with ( & mcr_iStringKey, mcr_StringKey_compare,
			mcr_StringKey_copy, sizeof ( mcr_StringKey ),
			mcr_StringKey_init, mcr_StringKey_free, NULL,
			mcr_StringKey_send ) ;
	mcr_ISignal_init_with ( & mcr_iCommand, mcr_Command_compare,
			mcr_Command_copy, sizeof ( mcr_Command ),
			mcr_Command_init, mcr_Command_free, NULL,
			mcr_Command_send ) ;
	mcr_Array_init ( & _keyChars, sizeof ( mcr_Array ) ) ;
	mcr_Array_init ( & _charInitial, sizeof ( mcr_Signal ) ) ;
	mcr_Array_init ( & _ssArrInitial, sizeof ( mcr_SafeString ) ) ;

	if ( mcr_ISignal_register ( & mcr_iStringKey ) == ( size_t ) -1 )
	{
		dmsg ;
	}
	if ( mcr_ISignal_register ( & mcr_iCommand ) == ( size_t ) -1 )
	{
		dmsg ;
	}
}

void mcr_signalextras_cleanup ( void )
{
	mcr_StringKey_char_clear ( ) ;
	mcr_Array_free ( & _keyChars ) ;
	mcr_Array_free ( & _charInitial ) ;
	mcr_Array_free ( & _ssArrInitial ) ;
}
