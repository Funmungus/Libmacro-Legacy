/*! \file hotkey/dispatchstandard.h
 * mcr_Dispatch and mcr_Hot definitions for standard signals.
 * */

# ifndef MCR_DISPATCHSTANDARD_H
# define MCR_DISPATCHSTANDARD_H

# include "hotkey/dispatch.h"

MCR_API void mcr_DispatchAlarm_free ( ) ;
MCR_API int mcr_DispatchAlarm_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int * modsPt ) ;
MCR_API void mcr_DispatchAlarm_add_specific ( mcr_Dispatch * container,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchAlarm_remove_specific ( mcr_Dispatch * container,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchAlarm_release ( mcr_Dispatch * dispPt ) ;

MCR_API void mcr_DispatchHIDEcho_free ( ) ;
MCR_API int mcr_DispatchHIDEcho_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int * modsPt ) ;
MCR_API void mcr_DispatchHIDEcho_add_specific ( mcr_Dispatch * container,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchHIDEcho_remove_specific ( mcr_Dispatch * container,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchHIDEcho_release ( mcr_Dispatch * dispPt ) ;

MCR_API void mcr_DispatchKey_free ( ) ;
MCR_API int mcr_DispatchKey_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int * modsPt ) ;
MCR_API void mcr_DispatchKey_add_specific ( mcr_Dispatch * container,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchKey_remove_specific ( mcr_Dispatch * container,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchKey_release ( mcr_Dispatch * dispPt ) ;

MCR_API void mcr_DispatchMoveCursor_free ( ) ;
MCR_API int mcr_DispatchMoveCursor_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int * modsPt ) ;
MCR_API void mcr_DispatchMoveCursor_add_specific ( mcr_Dispatch * container,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchMoveCursor_remove_specific ( mcr_Dispatch * container,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchMoveCursor_release ( mcr_Dispatch * dispPt ) ;

MCR_API void mcr_DispatchNoOp_free ( ) ;
MCR_API int mcr_DispatchNoOp_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int * modsPt ) ;
MCR_API void mcr_DispatchNoOp_add_specific ( mcr_Dispatch * container,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchNoOp_remove_specific ( mcr_Dispatch * container,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchNoOp_release ( mcr_Dispatch * dispPt ) ;

MCR_API void mcr_DispatchScroll_free ( ) ;
MCR_API int mcr_DispatchScroll_dispatch_specific ( mcr_Dispatch * dispPt,
		mcr_Signal * signalPt, unsigned int * modsPt ) ;
MCR_API void mcr_DispatchScroll_add_specific ( mcr_Dispatch * container,
		mcr_Hot * newHotkey, mcr_Signal * signalPt, unsigned int mods ) ;
MCR_API void mcr_DispatchScroll_remove_specific ( mcr_Dispatch * container,
		mcr_Hot * delHotkey ) ;
MCR_API void mcr_DispatchScroll_release ( mcr_Dispatch * dispPt ) ;

MCR_API void mcr_dispatchstandard_initialize ( ) ;

MCR_API void mcr_dispatchstandard_cleanup ( void ) ;

# endif // MCR_DISPATCHSTANDARD_H
