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

# ifndef MCR_SIGNALEXTRAS_H
# define MCR_SIGNALEXTRAS_H

# include "extras/safestring.h"

/*! \brief
 *
 * All members are arrays of safe strings.
 * */
typedef struct mcr_Command
{
	mcr_SafeString file ;
	//
	// Internal
	//
	mcr_Array argv ;
	mcr_Array env ;
} mcr_Command ;

// TODO: unicode support
typedef struct mcr_StringKey
{
	mcr_NoOp interval ;
	mcr_SafeString string ;
} mcr_StringKey ;

MCR_API extern mcr_ISignal mcr_iCommand ;
MCR_API extern mcr_ISignal mcr_iStringKey ;

MCR_API void mcr_Command_init ( void * cmdPt ) ;
MCR_API void mcr_Command_init_with ( mcr_Command * cmdPt,
		const char * file, char * const argv [ ],
		char * const envp [ ], int cryptic ) ;
MCR_API int mcr_Command_compare ( const void * lhs, const void * rhs ) ;
MCR_API void mcr_Command_copy ( void * dstPt, void * srcPt ) ;
MCR_API void mcr_Command_free ( void * cmdPt ) ;

MCR_API void mcr_StringKey_init ( void * skPt ) ;
MCR_API void mcr_StringKey_init_with ( mcr_StringKey * skPt,
		mcr_NoOp interval, const char * text, int cryptic ) ;
MCR_API int mcr_StringKey_compare ( const void * lhs, const void * rhs ) ;
MCR_API void mcr_StringKey_copy ( void * dstPt, void * srcPt ) ;
MCR_API void mcr_StringKey_free ( void * skPt ) ;

MCR_API void mcr_Command_set_file ( mcr_Command * cmdPt,
		const char * file, int cryptic ) ;
MCR_API void mcr_Command_set_args ( mcr_Command * cmdPt,
		char * const argv [ ], int cryptic ) ;
MCR_API void mcr_Command_add_arg ( mcr_Command * cmdPt,
		char * const arg, int cryptic ) ;
MCR_API void mcr_Command_clear_args ( mcr_Command * cmdPt ) ;
MCR_API void mcr_Command_set_env ( mcr_Command * cmdPt,
		char * const envp [ ], int cryptic ) ;
MCR_API void mcr_Command_add_env ( mcr_Command * cmdPt,
		char * const env, int cryptic ) ;
MCR_API void mcr_Command_clear_env ( mcr_Command * cmdPt ) ;


MCR_API int mcr_StringKey_send ( mcr_Signal * sigPt ) ;
MCR_API int mcr_StringKey_send_keys ( mcr_StringKey * skPt ) ;

MCR_API int mcr_Command_send ( mcr_Signal * sigPt ) ;

//
// String key development
//
MCR_API mcr_Array * mcr_StringKey_char ( int character ) ;
MCR_API void mcr_StringKey_set_char ( int character,
		mcr_Signal * charSignalArr, size_t arrLen ) ;
MCR_API void mcr_StringKey_remove_char ( int character ) ;
MCR_API void mcr_StringKey_set_shifted ( int character,
		int key, long nsec ) ;
MCR_API void mcr_StringKey_set_nonshifted ( int character,
		int key, long nsec ) ;
MCR_API void mcr_StringKey_set_delays ( mcr_NoOp delay ) ;
MCR_API size_t mcr_StringKey_char_count ( ) ;
MCR_API void mcr_StringKey_char_clear ( ) ;

//
// Contract strings are in English.
// TODO: load language files.
//
MCR_API void mcr_load_contracts ( ) ;
/* Native key definitions. */
MCR_API void mcr_StringKey_load_contract ( ) ;

MCR_API void mcr_signalextras_initialize ( ) ;
MCR_API void mcr_signalextras_cleanup ( void ) ;

//
// Implemented in native directory
//
//! \brief Same as execvpe, except a new process is created to not kill this one.
MCR_API int mcr_execvpe ( const char * file, char * const * args,
		char * const * env ) ;

# endif // MCR_SIGNALEXTRAS_H
