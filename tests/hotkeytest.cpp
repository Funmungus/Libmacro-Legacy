#include <iostream>
#include "hotkey/hotkey.h"

using namespace std;
using namespace funlibrary;

//bool changeKey ( KeySignal * modified, Mods * mod ) ;
//bool changeSpace ( SpaceSignal * modified, Mods * mod ) ;
class MCall : public ICall < void >
{
	virtual void call ( )
	{
		cout << " I have a callback" << endl ;
	}
} ;

int main()
{
	NoOp ( 100 ).send ( ) ;

	cout << "About to enable" << endl ;
	string nom ;
	MCall mCall ;
	KeySignal one1 ( KEY_1 );//, one1 ( KEY_1, 0, UP ),
//			two ( KEY_2 ), two1 ( KEY_2, 0, UP ),
//			three ( KEY_3 ), three1 ( KEY_3, 0, UP ) ;
	SpaceSignal one ( 0,0, true, SpacePosition(0,0,0)) ;
	Mods mod ;
	vector < HotkeyStage > stages = {
		HotkeyStage ( false, false, &one, mod ),
		HotkeyStage ( false, false, &one1, mod )
//		HotkeyStage ( false, false, &two, mod ),
//		HotkeyStage ( false, false, &two1, mod ),
//		HotkeyStage ( false, true, &three, mod ),
//		HotkeyStage ( false, true, &three1, mod )
	} ;
	StagedHotkey hotty ( stages, BLOCK_FINAL, false, &mCall ) ; // ( true, false, &mCall ) ;
	hotty.enable ( true ) ;

	cin >> nom ;

	cout << "Re-enable" << endl ;
	cin >> nom ;

	return 0;
}
