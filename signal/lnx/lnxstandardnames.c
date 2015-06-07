/* signal/lnx/lnxstandardnames.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "signal/standard.h"
# include "signal/lnx/standard.h"

static void add_keynames ( ) ;
static void add_echokeys ( ) ;
static void add_echonames ( ) ;

void mcr_Key_load_contract ( )
{
	add_keynames ( ) ;
}

void mcr_Echo_load_contract ( )
{
	add_echokeys ( ) ;
	add_echonames ( ) ;
}

static void add_keynames ( )
{
	const char * names [ ] =
	{
		"None", "Esc", "1", "2", "3",
		"4", "5", "6", "7", "8",
		"9", "0", "Minus", "Equal", "Backspace",
		"Tab", "Q", "W", "E", "R",
		"T", "Y", "U", "I", "O",
		"P", "LeftBrace", "RightBrace", "Enter", "LeftCtrl",
		"A", "S", "D", "F", "G",
		"H", "J", "K", "L", "Semicolon",
		"Apostrophe", "Grave", "LeftShift", "Backslash", "Z",
		"X", "C", "V", "B", "N",
		"M", "Comma", "Dot", "Slash", "RightShift",
		"KPAsterisk", "LeftAlt", "Space", "CapsLock", "F1",
		"F2", "F3", "F4", "F5", "F6",
		"F7", "F8", "F9", "F10", "NumLock",
		"ScrollLock", "KP7", "KP8", "KP9", "KPMinus",
		"KP4", "KP5", "KP6", "KPPlus", "KP1",
		"KP2", "KP3", "KP0", "KPdot", "",
		"ZenkakuHankaku", "102ND", "F11", "F12", "RO",
		"Katakana", "Hiragana", "Henkan", "KatakanaHiragana", "Muhenkan",
		"KPJPComma", "KPEnter", "RightCtrl", "KPSlash", "SysRQ",
		"RightAlt", "LineFeed", "Home", "Up", "PageUp",
		"Left", "Right", "End", "Down", "PageDown",
		"Insert", "Delete", "Macro", "Mute", "VolumeDown",
		"VolumeUp", "Power", "KPEqual", "KPPlusMinus", "Pause",
		"Scale", "KPComma", "Hangeul", "Hanja", "Yen",
		"LeftMeta", "RightMeta", "Compose", "Stop", "Again",
		"Props", "Undo", "Front", "Copy", "Open",
		"Paste", "Find", "Cut", "Help", "Menu",
		"Calc", "Setup", "Sleep", "Wakeup", "File",
		"SendFile", "DeleteFile", "Xfer", "Prog1", "Prog2",
		"www", "MsDOS", "ScreenLock", "Direction", "CycleWindows",
		"Mail", "Bookmarks", "Computer", "Back", "Forward",
		"CloseCD", "EjectCD", "EjectCloseCD", "NextSong", "PlayPause",
		"PreviousSong", "StopCD", "Record", "Rewind", "Phone",
		"ISO", "Config", "HomePage", "Refresh", "Exit",
		"Move", "Edit", "ScrollUp", "ScrollDown", "KPLeftParen",
		"KPRightParen", "New", "Redo", "F13", "F14",
		"F15", "F16", "F17", "F18", "F19",
		"F20", "F21", "F22", "F23", "F24", // 194
		// 195 - 199
		"", "", "", "", "",
		"PlayCD", "PauseCD", "Prog3", "Prog4", "Dashboard",
		"Suspend", "Close", "Play", "FastForward", "BassBoost",
		"Print", "HP", "Camera", "Sound", "Question",
		"Email", "Chat", "Search", "Connect", "Finance",
		"Sport", "Shop", "AltErase", "Cancel", "BrightnessDown",
		"BrightnessUp", "Media", "SwitchVideoMode", "KBDIllumToggle",
		"KBDIllumDown",
		"KBDIllumUp", "Send", "Reply", "ForwardMail", "Save",
		"Documents", "Battery", "Bluetooth", "WLAN", "UWB",
		"",
		"VideoNext", "VideoPrev", "BrightnessCycle",
		"BrightnessZero",
		"DisplayOff", "WWAN", "RFKill", "MicMute",
		// 249 - 254
		"", "", "", "", "", "",
		"AT_KBD_Driver", "BTN_0", "BTN_1", "BTN_2", "BTN_3",
		"BTN_4", "BTN_5", "BTN_6", "BTN_7", "BTN_8", "BTN_9",
		"BTN_Left", "BTN_Right", "BTN_Middle", "BTN_Side", "BTN_Extra",
		"BTN_Forward", "BTN_Back", "BTN_Task", "BTN_Trigger", "BTN_Thumb",
		"BTN_Thumb2", "BTN_Top", "BTN_Top2", "BTN_Pinkie", "BTN_Base",
		"BTN_Base2", "BTN_Base3", "BTN_Base4", "BTN_Base5", "BTN_Base6",
		// 0x12c - 0x12e
		"", "", "",
		"BTN_Dead", "BTN_A", "BTN_B", "BTN_C", "BTN_X",
		"BTN_Y", "BTN_Z", "BTN_TL", "BTN_TR", "BTN_TL2",
		"BTN_TR2", "BTN_Select", "BTN_Start", "BTN_Mode", "BTN_ThumbL",
		"BTN_ThumbR",
		"",
		"BTN_Tool_Pen", "BTN_Tool_Rubber", "BTN_Tool_Brush",
		"BTN_Tool_Pencil", "BTN_Tool_Airbrush",
		"BTN_Tool_Finger", "BTN_Tool_Mouse", "BTN_Tool_Lens",
		"BTN_Tool_QuintTap",
		"",
		"BTN_Touch", "BTN_Stylus", "BTN_Stylus2", "BTN_Tool_DoubleTap",
		"BTN_Tool_TripleTap",
		"BTN_Tool_QuadTap", "BTN_Gear_Down", "BTN_Gear_Up",
		// 0x152 - 0x15f
		"", "", "", "", "", "", "", "", "", "", "", "", "", "",
		"OK", "Select", "Goto", "Clear", "Power2",
		"Option", "Info", "Time", "Vendor", "Archive",
		"Program", "Channel", "Favorites", "EPG", "PVR",
		"MHP", "Language", "Title", "Subtitle", "Angle",
		"Zoom", "Mode", "Keyboard", "Screen", "PC",
		"TV", "TV2", "VCR", "VCR2", "SAT",
		"SAT2", "CD", "Tape", "Radio", "Tuner",
		"Player", "Text", "DVD", "AUX", "MP3",
		"Audio", "Video", "Directory", "List", "Memo",
		"Calendar", "Red", "Green", "Yellow", "Blue",
		"ChannelUp", "ChannelDown", "First", "Last", "AB",
		"Next", "Restart", "Slow", "Shuffle", "Break",
		"Previous", "Digits", "TEEN", "TWEN", "VideoPhone",
		"Games", "ZoomIn", "ZoomOut", "ZoomReset", "WordProcessor",
		"Editor", "Spreadsheet", "GraphicsEditor", "Presentation",
		"Database",
		"News", "VoiceMail", "AddressBook", "Messenger", "DisplayToggle",
		"SpellCheck", "LogOff", "Dollar", "Euro" "FrameBack",
		"FrameForward", "ContextMenu", "MediaRepeat", "10ChannelsUp",
		"10ChannelsDown",
		"Images",
		// 0x1bb - 0x1bf
		"", "", "", "", "",
		"DEL_EOL", "DEL_EOS", "INS_Line", "DEL_Line",
		// 0x1c4 - 0x1cf
		"", "", "", "", "", "", "", "", "", "", "", "",
		"FN", "FN_Esc", "FN_F1", "FN_F2", "FN_F3",
		"FN_F4", "FN_F5", "FN_F6", "FN_F7", "FN_F8",
		"FN_F9", "FN_F10", "FN_F11", "FN_F12", "FN_1",
		"FN_2", "FN_D", "FN_E", "FN_F", "FN_S",
		"FN_B",
		// 0x1e5 - 0x1f0
		"", "", "", "", "", "", "", "", "", "", "",
		"",
		"BRL_Dot1", "BRL_Dot2", "BRL_Dot3", "BRL_Dot4", "BRL_Dot5",
		"BRL_Dot6", "BRL_Dot7", "BRL_Dot8", "BRL_Dot9", "BRL_Dot10",
		// 0x1fb - 0x1ff
		"", "", "", "", "",
		"Numeric_0", "Numeric_1", "Numeric_2", "Numeric_3", "Numeric_4",
		"Numeric_5", "Numeric_6", "Numeric_7", "Numeric_8", "Numeric_9",
		"Numeric_Star", "Numeric_Pound",
		// 0x20c - 0x20f
		"", "", "", "",
		"CameraFocus", "WPS_Button",

		"TouchpadToggle", "TouchpadOn", "TouchpadOff",

		"CameraZoomIn", "CameraZoomOut", "CameraUp",
		"CameraDown", "CameraLeft",
		"CameraRight", "AttendantOn", "AttendantOff",
		"AttendantToggle", "LightsToggle",
		"",
		"BTN_DPAD_Up", "BTN_DPAD_Down", "BTN_DPAD_Left", "BTN_DPAD_Right",
		// 0x224 - 0x22f
		"", "", "", "", "", "", "", "", "", "", "", ""
		"ALS_Toggle",
		// 0x231 - 0x2bf
		// 0x231 - 0x23f
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		// 0x240 - 0x24f
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		// 0x250 - 0x25f
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		// 0x260 - 0x26f
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		// 0x270 - 0x27f
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		// 0x280 - 0x28f
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		// 0x290 - 0x29f
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		// 0x2a0 - 0x2af
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		// 0x2b0 - 0x2bf
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"TriggerHappy", "TriggerHappy2", "TriggerHappy3",
		"TriggerHappy4", "TriggerHappy5", "TriggerHappy6",
		"TriggerHappy7", "TriggerHappy8", "TriggerHappy9",
		"TriggerHappy10", "TriggerHappy11", "TriggerHappy12",
		"TriggerHappy13", "TriggerHappy14", "TriggerHappy15",
		"TriggerHappy16", "TriggerHappy17", "TriggerHappy18",
		"TriggerHappy19", "TriggerHappy20", "TriggerHappy21",
		"TriggerHappy22", "TriggerHappy23", "TriggerHappy24",
		"TriggerHappy25", "TriggerHappy26", "TriggerHappy27",
		"TriggerHappy28", "TriggerHappy29", "TriggerHappy30",
		"TriggerHappy31", "TriggerHappy32", "TriggerHappy33",
		"TriggerHappy34", "TriggerHappy35", "TriggerHappy36",
		"TriggerHappy37", "TriggerHappy38", "TriggerHappy39",
		"TriggerHappy40"
	} ;
	const int extraKeys [ ] =
	{
		KEY_ESC, KEY_1, KEY_2, KEY_3, KEY_4,
		KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
		KEY_0, KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_LEFTCTRL,
		KEY_LEFTSHIFT, KEY_RIGHTSHIFT, KEY_LEFTALT, KEY_CAPSLOCK,
		KEY_NUMLOCK, KEY_SCROLLLOCK, KEY_ZENKAKUHANKAKU,
		KEY_KATAKANAHIRAGANA, KEY_RIGHTCTRL, KEY_RIGHTALT,
		KEY_LINEFEED, KEY_PAGEUP, KEY_PAGEDOWN, KEY_VOLUMEDOWN,
		KEY_VOLUMEUP, KEY_LEFTMETA, KEY_RIGHTMETA, KEY_SCROLLUP,
		KEY_SCROLLDOWN,

		KEY_HANGUEL, KEY_COFFEE, KEY_BRIGHTNESS_ZERO, KEY_WIMAX,
		BTN_MISC, BTN_MOUSE, BTN_JOYSTICK,
		BTN_GAMEPAD, BTN_SOUTH, BTN_EAST,
		BTN_NORTH, BTN_WEST, BTN_DIGI,
		BTN_WHEEL, BTN_TRIGGER_HAPPY1
	} ;
	const char * extraNames [ ] [ 6 ] =
	{
		{"Escape"}, {"One"}, {"Two"}, {"Three"}, {"Four"},
		{"Five"}, {"Six"}, {"Seven"}, {"Eight"}, {"Nine"},
		{"Zero"}, {"left brace", "left_brace"},
		{"right brace", "right_brace"}, {"left ctrl", "left control",
				"left_ctrl", "left_control", "ctrl", "control"},
		{"left shift", "left_shift", "shift"}, {"right shift",
				"right_shift"}, {"left alt", "left_alt", "alt"},
				{"caps lock", "caps_lock"},
		{"num lock", "num_lock"}, {"scroll lock", "scroll_lock"},
				{"zenkaku hankaku", "zenkaku_hankaku"},
		{"katagana hiragana", "katagana_hiragana"}, {"right ctrl",
				"right_ctrl", "right control", "right_control"},
				{"right alt", "right_alt", "AltGr", "alt gr", "alt_gr"},
		{"line feed", "line_feed"}, {"page up", "page_up"}, {"page down",
				"page_down"}, {"volume down", "volume_down"},
		{"volume up", "volume_up"}, {"left meta", "left_meta", "meta"},
				{"right meta", "right_meta"}, {"scroll up", "scroll_up"},
		{"scroll down", "scroll_down"},

		{"Hanguel"}, {"Coffee"}, {"BrightnessAuto"}, {"WiMax"},
		{"BTN_Misc"}, {"BTN_Mouse"}, {"BTN_Joystick"},
		{"BTN_Gamepad"}, {"BTN_South"}, {"BTN_East"},
		{"BTN_North"}, {"BTN_West"}, {"BTN_Digi"},
		{"BTN_Wheel"}, {"TriggerHappy1"},
	} ;
	const size_t extraLens [ ] =
	{
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 2,
		2, 6,
		3, 2, 3, 2,
		2, 2, 2,
		2, 4, 5,
		2, 2, 2, 2,
		2, 3, 2, 2,
		2,

		1, 1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1
	} ;
	int count = sizeof ( names ) / sizeof ( char * ) ;
	int i ;
	for ( i = 0 ; i < count ; i ++ )
	{
		mcr_Key_set_name ( i, names [ i ] ) ;
	}
	count = sizeof ( extraKeys ) / sizeof ( int ) ;
	for ( i = 0 ; i < count ; i ++ )
	{
		mcr_Key_add_names ( extraKeys [ i ], extraNames [ i ],
				extraLens [ i ] ) ;
	}
}

# define keysetall( keyPt, keyscan, uptype ) \
	MCR_KEY_SET ( keyPt, keyscan ) ; \
	MCR_KEY_SET_SCAN ( keyPt, keyscan ) ; \
	MCR_KEY_SET_UP_TYPE ( keyPt, uptype ) ;

static void add_echokeys ( )
{
	const int echokeys [ ] =
	{
		BTN_0, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7,
		BTN_8, BTN_9, BTN_LEFT, BTN_RIGHT, BTN_MIDDLE, BTN_SIDE,
		BTN_EXTRA, BTN_FORWARD, BTN_BACK, BTN_TASK, BTN_TRIGGER,
		BTN_THUMB, BTN_THUMB2, BTN_TOP, BTN_TOP2, BTN_PINKIE,
		BTN_BASE, BTN_BASE2, BTN_BASE3, BTN_BASE4, BTN_BASE5,
		BTN_BASE6, BTN_A /*BTN_SOUTH*/, BTN_B /*BTN_EAST*/,
		BTN_C, BTN_X /*BTN_NORTH*/, BTN_Y /*BTN_WEST*/,
		BTN_Z, BTN_TL, BTN_TR, BTN_TL2, BTN_TR2, BTN_SELECT,
		BTN_START, BTN_MODE, BTN_THUMBL, BTN_THUMBR
	} ;
	const int count = sizeof ( echokeys ) / sizeof ( int ) ;
	mcr_Key k ;
	mcr_Key_init_with ( & k, 0, 0, MCR_DOWN ) ;
	int echoCode = 0 ;
	for ( int i = 0 ; i < count ; i ++ )
	{
		keysetall ( & k, echokeys [ i ], MCR_DOWN ) ;
		mcr_Echo_set_key ( echoCode ++, & k ) ;
		MCR_KEY_SET_UP_TYPE ( & k, MCR_UP ) ;
		mcr_Echo_set_key ( echoCode ++, & k ) ;
	}
}

static void add_echonames ( )
{
	const char * names [ ] =
	{
		"0", "1", "2", "3", "4", "5", "6", "7",
		"8", "9", "Left", "Right", "Middle", "Side",
		"Extra", "Forward", "Back", "Task", "Trigger",
		"Thumb", "Thumb2", "Top", "Top2", "Pinkie",
		"Base", "Base2", "Base3", "Base4", "Base5",
		"Base6", "A" /*SOUTH*/, "B" /*EAST*/,
		"C", "X" /*NORTH*/, "Y" /*WEST*/,
		"Z", "TL", "TR", "TL2", "TR2", "Select",
		"Start", "Mode", "ThumbL", "ThumbR"
	} ;
	const char * addNames [ ] =
	{
		"Zero", "One", "Two", "Three", "Four",
		"Five", "Six", "Seven",
		"Eight", "Nine", "", "", "", "",
		"", "", "", "", "",
		"", "", "", "", "",
		"", "", "", "", "",
		"", "SOUTH", "EAST",
		"", "NORTH", "WEST",
		"", "", "", "", "", "",
		"", "", "", ""
	} ;
	int count = sizeof ( names ) / sizeof ( char * ) ;
	char setname [ 64 ] ;
	int i = 0, echoCode = 0 ;
	for ( ; i < count ; i ++ )
	{
		snprintf ( setname, 63, "%s%s", names [ i ], "Down" ) ;
		mcr_Echo_set_name ( echoCode, setname ) ;
		if ( addNames [ i ] [ 0 ] != '\0' )
		{
			snprintf ( setname, 63, "%s%s", addNames [ i ], "Down" ) ;
			mcr_Echo_add_name ( echoCode, setname ) ;
			snprintf ( setname, 63, "%s %s", addNames [ i ], "down" ) ;
			mcr_Echo_add_name ( echoCode, setname ) ;
		}
		snprintf ( setname, 63, "%s %s", names [ i ], "down" ) ;
		mcr_Echo_add_name ( echoCode ++, setname ) ;

		snprintf ( setname, 63, "%s%s", names [ i ], "Up" ) ;
		mcr_Echo_set_name ( echoCode, setname ) ;
		if ( addNames [ i ] [ 0 ] != '\0' )
		{
			snprintf ( setname, 63, "%s%s", addNames [ i ], "Up" ) ;
			mcr_Echo_add_name ( echoCode, setname ) ;
			snprintf ( setname, 63, "%s %s", addNames [ i ], "up" ) ;
			mcr_Echo_add_name ( echoCode, setname ) ;
		}
		snprintf ( setname, 63, "%s %s", names [ i ], "up" ) ;
		mcr_Echo_add_name ( echoCode ++, setname ) ;
	}
}
