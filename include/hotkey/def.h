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

/*! \file hotkey/def.h
 * Definitions used in hotkey module
 * */

# ifndef MCR_HOTKEY_DEFINES_H
# define MCR_HOTKEY_DEFINES_H

# include "signal/signal.h"

/*! \brief Define behaviour of how modifiers may or
 * may not trigger a callback on dispatch. */
typedef enum mcr_TriggerFor
{
	//! \brief Do not trigger if any of these modifiers are set.
	MCR_NONE = 0,
	//! \brief Trigger if any of these modifiers are set.
	MCR_ANY,
	//! \brief Trigger only if all of these modifiers are set.
	MCR_ALL
} mcr_TriggerFor ;

//! \brief Value to trigger for any modifier.
MCR_API extern const unsigned int MCR_ANY_MOD ;
//! \brief Value to trigger for any echo code.
MCR_API extern const unsigned int MCR_ANY_ECHO ;
//! \brief Value to trigger for any scan or key code.
MCR_API extern const int MCR_ANY_KEY ;
//! \brief All modifiers known by Macrolibrary to be set.
MCR_API extern unsigned int mcr_internalMods ;
//! \brief Locking modifiers will block dispatch and changing modifiers.
MCR_API extern mtx_t mcr_modLock ;
//! \brief Maximum time to wait for locking modifiers. Default 1/4 second.
MCR_API extern struct timespec mcr_modLockTimeout ;

# endif
