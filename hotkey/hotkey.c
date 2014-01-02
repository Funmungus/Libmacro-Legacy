
# include "hotkey/hotkey.h"

MCR_API const unsigned int MCR_ANY_MOD = -1 ;
MCR_API const int MCR_ANY_KEY = 0 ;
MCR_API unsigned int mcr_InternalMods = 0 ;
MCR_API mtx_t mcr_ModLock ;
MCR_API struct timespec mcr_ModLockTimeout = { 1, 0 } ;

void mcr_hotkey_initialize ( )
{
	mtx_init ( & mcr_ModLock, mtx_timed ) ;
	mcr_dispatch_initialize ( ) ;
	mcr_dispatchstandard_initialize ( ) ;
}

void mcr_hotkey_cleanup ( )
{
	mcr_dispatchstandard_cleanup ( ) ;
	mcr_dispatch_cleanup ( ) ;
	mtx_destroy ( & mcr_ModLock ) ;
}
