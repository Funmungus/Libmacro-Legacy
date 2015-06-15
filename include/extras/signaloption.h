/* include/extras/signaloption.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

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
