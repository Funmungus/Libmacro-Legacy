#include <iostream>
# include "util/util.h"

using namespace std;
using namespace funlibrary ;

void charEvent ( void * sender, EventArgs & arg ) ;

int main()
{
	/*
	Event<EventArgs>::usage();

	Event < EventArgs > ev ;
	EventArgs nom ;
	nom.obj = (void *) 0 ;
	nom.what = 10 ;
	ev += charEvent ;
	ev.fire( (void *) 0,nom);
	ev -= charEvent ;
	ev.fire( (void *) 0,nom);
	ev += charEvent ;
	ev.clear ( ) ;
	ev.fire( (void *) 0,nom);
*/
	/*
	const char * container = "Funny business" ;
	bool nom = containsAt ( container, 0, "funny", true ) ;
	cout << "Ignore case is : " << nom << endl ;
	nom = containsAt ( container, 0, "funny", false ) ;
	cout << "Maintain case is : " << nom << endl ;
	*/
	return 0;
}

void charEvent (void * sender, EventArgs & arg )
{
	cout << "Chartest called: " << sender << ":" << arg.obj << ":" << arg.what << endl ;
}
