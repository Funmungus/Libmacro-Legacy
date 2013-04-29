
# include "signal/signal.h"
# include "hotkey/dispatcher.h"

namespace macro
{
	KeyCallback Dispatcher::_keyback = NULL ;
	SpaceCallback Dispatcher::_spaceback = NULL ;
}
# ifdef LNX
# include "lnxdispatcher.hxx"
# endif
# ifdef WIN
# include "windispatcher.hxx"
# endif
macro::Dispatcher macro::Dispatcher::_instance ; 	// Only needed for destruction.
