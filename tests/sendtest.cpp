

# include "signal/signal.h"
# include <ctime>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <sstream>

/*
# ifdef LNX
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
# endif
*/

using namespace std ;
using namespace funlibrary ;

# ifdef WIN
int _tmain ( int argc, _TCHAR* argv [ ] )
# endif
# ifdef LNX
int main ( )
# endif
{
	/*
	int                    fd;
	struct uinput_user_dev uidev;
	struct input_event     ev, sync, mver;

	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if ( fd == -1 )
		return -1 ;

	if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
	{ close ( fd ) ; return -1 ; }
	if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
	{ close ( fd ) ; return -1 ; }

	if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
	{ close ( fd ) ; return -1 ; }
	if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
	{ close ( fd ) ; return -1 ; }
	if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
	{ close ( fd ) ; return -1 ; }


	if(ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0)
	{ close ( fd ) ; return -1 ; }

	memset ( & uidev, 0, sizeof ( uidev ) ) ;
	memset ( & ev, 0, sizeof ( ev ) ) ;
	memset ( & sync, 0, sizeof ( sync ) ) ;
	memset ( & mver, 0, sizeof ( mver ) ) ;
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB ;
	uidev.id.vendor = 1 ;
	uidev.id.product = 1 ;
	uidev.id.version = 1 ;

	if(write(fd, &uidev, sizeof(uidev)) < 0)
	{ close ( fd ) ; return -1 ; }

	if(ioctl(fd, UI_DEV_CREATE) < 0)
	{ close ( fd ) ; return -1 ; }

	NoOp ( 10 ).send ( ) ;//system ( "sleep 1" ) ;

	ev.type = EV_KEY ;
	ev.code = BTN_LEFT ;
	ev.value = 1 ;

	sync.type = EV_SYN ;
	sync.code = SYN_REPORT ;
	sync.value = 0 ;

	mver.type = EV_REL ;
	mver.code = REL_X ;
	mver.value = 300 ;

	if(write(fd, &ev, sizeof ( ev ) ) < 0 )
		fprintf(stderr,"err ev");

	if(write(fd, &sync, sizeof ( sync ) ) < 0 )
		fprintf(stderr,"err sync");

	*//*
	if(write(fd, &mver, sizeof ( mver ) ) < 0 )
		fprintf(stderr,"err mver");

	if(write(fd, &sync, sizeof ( sync ) ) < 0 )
		fprintf(stderr,"err sync");

	*//*
	relDevice.send ( &mver ) ;
	relDevice.send ( &mver ) ;
	relDevice.send ( &mver ) ;
	relDevice.sync ( ) ;
	NoOp (10).send();
	ev.value = 0 ;
	if(write(fd, &ev, sizeof ( ev ) ) < 0 )
		fprintf(stderr,"err ev");

	if(write(fd, &sync, sizeof ( sync ) ) < 0 )
		fprintf(stderr,"err sync");

	ioctl(fd,UI_DEV_DESTROY) ;
	close(fd);
*/
//	KeyDevice keyDevice1 ;
	/*
	input_event ev, ev2 ;
	memset ( &ev, 0, sizeof ( ev ) ) ;
	memset ( &ev2, 0, sizeof ( ev2 ) ) ;
	ev.type=EV_KEY;
	ev.code=BTN_LEFT;
	ev.value=1;
	keyDevice.send(&ev);
	keyDevice.sync();
	ev2.type=EV_REL;
	ev2.code=REL_X;
	ev2.value=200;
	relDevice.send(&ev2);
	relDevice.sync();
	ev.value=0;
	//NoOp ( 2000 ).send ( ) ;
	keyDevice.send(&ev);
	keyDevice.sync();
	cout << "K" << keyDevice.isEnabled()<<endl;
	cout << "R" << relDevice.isEnabled()<<endl;
	cout << "A" << absDevice.isEnabled()<<endl;
	*/
/*
	KeySignal key;
	key.setKeyUp(KeyUpType::BOTH);
	/*
	for ( int i = KEY_1 ; i < KEY_SPACE ; i++ )
	{
		key.setKey(i);
		key.send();
	}
	*/
/*
	key.setKey ( BTN_LEFT ) ;
	key.setKeyUp ( KeyUpType::DOWN);
	key.send();
	*/

	SpacePosition p ;

	for ( int i = 0 ; i < 10 ; i ++ )
	{
		p = SpaceSignal::currentPosition();
		NoOp ( 1000 ).send ( ) ;
		cout << p.x << " " << p.y << " " << p.z << endl ;
	}

	NoOp ( 60000 ).send ( ) ;
/*
	SpaceSignal space, space1, space2;
	space.setEvent ( SpaceSignal::getEventInt("ldown"));

	space1.setEvent ( 0 ) ;
	space1.setX ( 200 ) ;
	space1.setY ( 200 ) ;

	space2.setEvent ( SpaceSignal::getEventInt("lup"));
	space1.setEnableCursorJustify(false);
	space.send();
	NoOp ( 100 ).send ( ) ;
	space1.send();
	NoOp ( 100 ).send ( ) ;
	space2.send();
*/
}

