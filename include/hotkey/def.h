/*! \file hotkey/def.h
 * \brief Definitions for hotkeys.
 */

# ifndef MCR_HOTKEY_DEFINES_H
# define MCR_HOTKEY_DEFINES_H

# include "signal/signal.h"

//! \brief Value to trigger for any modifier or echo code.
extern const unsigned int MCR_ANY_MOD ;
//! \brief Value to trigger for any scan or key code.
extern const int MCR_ANY_KEY ;
//! \brief All modifiers known by Macro library to be set.
extern unsigned int mcr_InternalMods ;
//! \brief Locking modifiers will block dispatch and changing modifiers.
extern mtx_t mcr_ModLock ;
//! \brief Maximum time to wait for locking modifiers. Default 1 second.
extern struct timespec mcr_ModLockTimeout ;

# endif
