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

# ifndef MCR_SIGNALOPTION_H
# define MCR_SIGNALOPTION_H

# include "extras/script.h"
# include "extras/signalextras.h"

typedef int ( * mcr_signal_modify_fnc ) ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
typedef int ( * mcr_signal_print_fnc ) ( int argc, char ** argv,
		int index, mcr_Array * flattenString ) ;

MCR_API int mcr_signal_option ( int argc, char ** argv, int index ) ;
MCR_API void mcr_signal_option_set ( char * key,
		mcr_option_fnc optFnc ) ;
MCR_API int mcr_signal_send_option ( int argc, char ** argv, int index ) ;
MCR_API int mcr_signal_add_name ( int argc, char ** argv, int index ) ;
MCR_API int mcr_signal_add_names ( int argc, char ** argv, int index ) ;

MCR_API int mcr_signal_modify ( int argc, char ** argv, int index,
		mcr_Signal * sigPtOut ) ;
MCR_API void mcr_signal_modify_set ( size_t sigId,
		mcr_signal_modify_fnc setModifier ) ;
MCR_API int mcr_alarm_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_echo_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_key_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_movecursor_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_noop_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_scroll_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_mods_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_command_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_stringkey_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;
MCR_API int mcr_script_modify ( int argc, char ** argv,
		int index, mcr_Signal * sigPtOut ) ;

MCR_API void mcr_signaloption_initialize ( ) ;
MCR_API void mcr_signaloption_cleanup ( ) ;

# endif
