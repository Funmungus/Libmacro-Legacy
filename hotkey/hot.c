
# include "hotkey/dispatch.h"

void mcr_Hot_init ( mcr_Hot * hotPt )
{
	if ( ! hotPt ) return ;
	memset ( hotPt, 0, sizeof ( mcr_Hot ) ) ;
}
void mcr_Hot_init_with ( mcr_Hot * hotPt, int block,
		mcr_trigger_fnc trigger, void * data )
{
	if ( ! hotPt ) return ;
	hotPt->block = block ;
	hotPt->trigger = trigger ;
	hotPt->data = data ;
}

int mcr_Hot_trigger ( mcr_Hot * hotPt, mcr_Signal * signalPt,
		unsigned int mods )
{
	MCR_HOT_TRIGGER ( hotPt, signalPt, mods ) ;
	return hotPt->block ;
}
int mcr_Hot_trigger_array ( mcr_Hot ** hotArray, size_t count,
		mcr_Signal * signalPt, unsigned int mods )
{
	int block = 0 ;
	MCR_HOT_TRIGGER_ARRAY ( hotArray, count, signalPt, mods, block ) ;
	return block ;
}
