
# include "macro.h"

void mcr_macro_initialize ( )
{
	mcr_reg_cleanup_filed ( mcr_macro_cleanup, __FILE__ ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
}

void mcr_macro_cleanup ( void )
{
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}
