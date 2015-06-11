/* include/hotkey/dispatchstandard.h - Dispatch for standard types.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file hotkey/dispatchstandard.h
 * \ref mcr_Dispatch and \ref mcr_Hot definitions for standard signals.
 * */

# ifndef MCR_DISPATCHSTANDARD_H
# define MCR_DISPATCHSTANDARD_H

# include "hotkey/dispatch.h"

// Initialize and free for specific dispatchers
MCR_API void mcr_DispatchAlarm_init ( mcr_Dispatch * dispPt ) ;
MCR_API void mcr_DispatchAlarm_free ( ) ;
MCR_API void mcr_DispatchHIDEcho_init ( mcr_Dispatch * dispPt ) ;
MCR_API void mcr_DispatchHIDEcho_free ( ) ;
MCR_API void mcr_DispatchKey_init ( mcr_Dispatch * dispPt ) ;
MCR_API void mcr_DispatchKey_free ( ) ;
MCR_API void mcr_DispatchMoveCursor_init ( mcr_Dispatch * dispPt ) ;
MCR_API void mcr_DispatchMoveCursor_free ( ) ;
MCR_API void mcr_DispatchNoOp_init ( mcr_Dispatch * dispPt ) ;
MCR_API void mcr_DispatchNoOp_free ( ) ;
MCR_API void mcr_DispatchScroll_init ( mcr_Dispatch * dispPt ) ;
MCR_API void mcr_DispatchScroll_free ( ) ;
MCR_API void mcr_DispatchMod_init ( mcr_Dispatch * dispPt ) ;
MCR_API void mcr_DispatchMod_free ( ) ;


// Interface function implementation for specific dispatchers.
// This does not include modifiers
MCR_API void mcr_DispatchAlarm_add_specific (
		mcr_Signal * signalPt, unsigned int mods, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchAlarm_dispatch_specific (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchAlarm_remove_specific (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchAlarm_clear ( ) ;

MCR_API void mcr_DispatchHIDEcho_add_specific (
		mcr_Signal * signalPt, unsigned int mods, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchHIDEcho_dispatch_specific (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchHIDEcho_remove_specific (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchHIDEcho_clear ( ) ;

MCR_API void mcr_DispatchKey_add_specific (
		mcr_Signal * signalPt, unsigned int mods, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchKey_dispatch_specific (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchKey_remove_specific (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchKey_clear ( ) ;

MCR_API void mcr_DispatchMoveCursor_add_specific (
		mcr_Signal * signalPt, unsigned int mods, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchMoveCursor_dispatch_specific (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchMoveCursor_remove_specific (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchMoveCursor_clear ( ) ;

MCR_API void mcr_DispatchNoOp_add_specific (
		mcr_Signal * signalPt, unsigned int mods, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchNoOp_dispatch_specific (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchNoOp_remove_specific (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchNoOp_clear ( ) ;

MCR_API void mcr_DispatchScroll_add_specific (
		mcr_Signal * signalPt, unsigned int mods, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchScroll_dispatch_specific (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchScroll_remove_specific (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchScroll_clear ( ) ;

MCR_API void mcr_DispatchMod_add_specific (
		mcr_Signal * signalPt, unsigned int mods, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchMod_dispatch_specific (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchMod_remove_specific (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchMod_clear ( ) ;

MCR_API void mcr_dispatchstandard_initialize ( ) ;

MCR_API void mcr_dispatchstandard_cleanup ( void ) ;

# endif // MCR_DISPATCHSTANDARD_H
