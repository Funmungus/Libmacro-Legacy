/* include/hotkey/def.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file hotkey/def.h
 * */

# ifndef MCR_HOTKEY_DEFINES_H
# define MCR_HOTKEY_DEFINES_H

# include "signal/signal.h"

//! \brief Value to trigger for any modifier or echo code.
extern const unsigned int MCR_ANY_MOD ;
//! \brief Value to trigger for any scan or key code.
extern const int MCR_ANY_KEY ;
//! \brief All modifiers known by Macro library to be set.
extern unsigned int mcr_internalMods ;
//! \brief Locking modifiers will block dispatch and changing modifiers.
extern mtx_t mcr_modLock ;
//! \brief Maximum time to wait for locking modifiers. Default 1/4 second.
extern struct timespec mcr_modLockTimeout ;

# endif
