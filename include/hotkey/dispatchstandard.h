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

/*! \file hotkey/dispatchstandard.h
 * \ref mcr_Dispatch and \ref mcr_Hot definitions for standard signals
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
MCR_API void mcr_DispatchAlarm_add (
		mcr_Signal * signalPt, unsigned int mods,
		mcr_TriggerFor triggerFor, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchAlarm_dispatch (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchAlarm_remove (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchAlarm_clear ( ) ;

MCR_API void mcr_DispatchHIDEcho_add (
		mcr_Signal * signalPt, unsigned int mods,
		mcr_TriggerFor triggerFor, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchHIDEcho_dispatch (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchHIDEcho_remove (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchHIDEcho_clear ( ) ;

MCR_API void mcr_DispatchKey_add (
		mcr_Signal * signalPt, unsigned int mods,
		mcr_TriggerFor triggerFor, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchKey_dispatch (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchKey_remove (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchKey_clear ( ) ;

MCR_API void mcr_DispatchMoveCursor_add (
		mcr_Signal * signalPt, unsigned int mods,
		mcr_TriggerFor triggerFor, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchMoveCursor_dispatch (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchMoveCursor_remove (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchMoveCursor_clear ( ) ;

MCR_API void mcr_DispatchNoOp_add (
		mcr_Signal * signalPt, unsigned int mods,
		mcr_TriggerFor triggerFor, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchNoOp_dispatch (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchNoOp_remove (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchNoOp_clear ( ) ;

MCR_API void mcr_DispatchScroll_add (
		mcr_Signal * signalPt, unsigned int mods,
		mcr_TriggerFor triggerFor, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchScroll_dispatch (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchScroll_remove (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchScroll_clear ( ) ;

MCR_API void mcr_DispatchMod_add (
		mcr_Signal * signalPt, unsigned int mods,
		mcr_TriggerFor triggerFor, mcr_Hot * newHotkey ) ;
MCR_API int mcr_DispatchMod_dispatch (
		mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchMod_remove (
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchMod_clear ( ) ;

MCR_API void mcr_dispatchstandard_initialize ( ) ;

MCR_API void mcr_dispatchstandard_cleanup ( void ) ;

# endif // MCR_DISPATCHSTANDARD_H
