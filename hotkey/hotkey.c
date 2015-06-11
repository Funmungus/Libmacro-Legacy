/* hotkey/hotkey.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "hotkey/hotkey.h"

MCR_API const unsigned int MCR_ANY_MOD = -1 ;
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
