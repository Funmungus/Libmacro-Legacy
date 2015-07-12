
# ifndef MCR_USEX_H
# define MCR_USEX_H

# include "signal/signal.h"
# include "signal/lnx/def.h"
# include "X11/Xlib.h"
# include "X11/keysym.h"
# include "X11/keysymdef.h"

/* Mods known by XLib */
extern unsigned int mcr_xMods ;

// Install senders to replace standard ones.
MCR_API void mcr_usex_enable ( int enable ) ;
MCR_API int mcr_HIDEcho_usex_send ( mcr_Signal * sigPt ) ;
MCR_API int mcr_Key_usex_send ( mcr_Signal * sigPt ) ;
MCR_API int mcr_MoveCursor_usex_send ( mcr_Signal * sigPt ) ;
MCR_API int mcr_Scroll_usex_send ( mcr_Signal * sigPt ) ;

MCR_API void mcr_usex_initialize ( ) ;
MCR_API void mcr_usex_cleanup ( void ) ;

# endif // MCR_USEX_H
