
extern "C" {
# include "hotkey/dispatch.h"
}

char fun_hotkey_init ( )
{
	size_t i = 0, sigCount = fun_ISignal_count ( ) ;
	fun_Dispatch disp ;
	while ( i++ < sigCount )
	{
		fun_Dispatch_init ( & disp ) ;
		fun_Dispatch_register ( & disp, i ) ;
	}
	fun_Dispatch * curDisp = fun_Dispatchers + fun_IAlarm.id ;
	curDisp->add_specific = NULL ;
	curDisp->dispatcher_pt = & fun_IAlarm.dispatch ;
	curDisp->dispatch_specific = NULL ;
	curDisp->free = NULL ;
	curDisp->release = NULL ;
	curDisp->remove_specific = NULL ;

	curDisp = fun_Dispatchers + fun_IHIDEcho.id ;
	curDisp->add_specific = NULL ;
	curDisp->dispatcher_pt = & fun_IHIDEcho.dispatch ;
	curDisp->dispatch_specific = NULL ;
	curDisp->free = NULL ;
	curDisp->release = NULL ;
	curDisp->remove_specific = NULL ;

	curDisp = fun_Dispatchers + fun_IKey.id ;
	curDisp->add_specific = NULL ;
	curDisp->dispatcher_pt = & fun_IKey.dispatch ;
	curDisp->dispatch_specific = NULL ;
	curDisp->free = NULL ;
	curDisp->release = NULL ;
	curDisp->remove_specific = NULL ;

	curDisp = fun_Dispatchers + fun_IMoveCursor.id ;
	curDisp->add_specific = NULL ;
	curDisp->dispatcher_pt = & fun_IMoveCursor.dispatch ;
	curDisp->dispatch_specific = NULL ;
	curDisp->free = NULL ;
	curDisp->release = NULL ;
	curDisp->remove_specific = NULL ;

	curDisp = fun_Dispatchers + fun_INoOp.id ;
	curDisp->add_specific = NULL ;
	curDisp->dispatcher_pt = & fun_INoOp.dispatch ;
	curDisp->dispatch_specific = NULL ;
	curDisp->free = NULL ;
	curDisp->release = NULL ;
	curDisp->remove_specific = NULL ;

	curDisp = fun_Dispatchers + fun_IScroll.id ;
	curDisp->add_specific = NULL ;
	curDisp->dispatcher_pt = & fun_IScroll.dispatch ;
	curDisp->dispatch_specific = NULL ;
	curDisp->free = NULL ;
	curDisp->release = NULL ;
	curDisp->remove_specific = NULL ;
}

char fun_HotkeyInit = fun_hotkey_init ( ) ;
