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

# include "hotkey/hotkey.h"

MCR_API const unsigned int MCR_ANY_MOD = -1 ;
MCR_API const unsigned int MCR_ANY_ECHO = -1 ;
MCR_API const int MCR_ANY_KEY = 0 ;
MCR_API unsigned int mcr_internalMods = 0 ;
MCR_API mtx_t mcr_modLock ;
MCR_API struct timespec mcr_modLockTimeout = { 0, 250000000 } ;
/* 1/4 second internal modifiers lock timeout.
 * 250000000 nano = 250000 micro = 250 milli */

void mcr_hotkey_initialize ( )
{
	mtx_init ( & mcr_modLock, mtx_timed ) ;
	mcr_mods_initialize ( ) ;
	mcr_hot_initialize ( ) ;
	mcr_stage_initialize ( ) ;
	mcr_hotstandard_initialize ( ) ;
	mcr_dispatch_initialize ( ) ;
	mcr_dispatchstandard_initialize ( ) ;
}

void mcr_hotkey_cleanup ( )
{
	mcr_dispatchstandard_cleanup ( ) ;
	mcr_dispatch_cleanup ( ) ;
	mcr_hotstandard_cleanup ( ) ;
	mcr_stage_cleanup ( ) ;
	mcr_hot_cleanup ( ) ;
	mcr_mods_cleanup ( ) ;
	mtx_destroy ( & mcr_modLock ) ;
}
