
# include "signal/win/key.h"
# include "signal/key.h"

namespace funlibrary
{
/*
	/// <summary>Names of keyCodes on Windows. Contains values 0-0xFE, but 0 is an invalid value.</summary>
	/// <summary>
	/// String representation for VirtualKey. Please use this instead of
	/// enum value names. VirtualKey has gaps of unused or unnamed items,
	/// which are filled in by other values in this array.
	/// This also consolidates multiple enums of the same value.
	/// </summary>
	const std::string KeySignal::_keyNames [ ] =
	{
		"NONE", //0x00
		"LBUTTON", //0x01
		"RBUTTON", //0x02
		"CANCEL", //0x03
		"MBUTTON", //0x04
		"XBUTTON1", //0x05
		"XBUTTON2", //0x06
		"UNDEFINED", //0x07
		"BACK", //0x08
		"TAB", //0x09
		"RESERVED", //0x0A
		"RESERVED", //0x0B
		"CLEAR", //0x0C
		"RETURN", //0x0D
		"UNDEFINED", //0x0E
		"UNDEFINED", //0x0F
		"SHIFT", //0x10
		"CONTROL", //0x11
		"MENU ( ALT ) ", //0x12
		"PAUSE", //0x13
		"CAPITAL", //0x14
		"KANA ( HANGUL ) ", //0x15
		"UNDEFINED", //0x16
		"JUNJA", //0x17
		"FINAL", //0x18
		"HANJA ( KANJI ) ", //0x19
		"UNDEFINED", //0x1A
		"ESCAPE", //0x1B
		"CONVERT", //0x1C
		"NONCONVERT", //0x1D
		"ACCEPT", //0x1E
		"MODECHANGE", //0x1F
		"SPACE", //0x20
		"PRIOR", //0x21
		"NEXT", //0x22
		"END", //0x23
		"HOME", //0x24
		"LEFT", //0x25
		"UP", //0x26
		"RIGHT", //0x27
		"DOWN", //0x28
		"SELECT", //0x29
		"PRINT", //0x2A
		"EXECUTE", //0x2B
		"SNAPSHOT", //0x2C
		"INSERT", //0x2D
		"DELETE", //0x2E
		"HELP", //0x2F
		"0", //0x30
		"1", //0x31
		"2", //0x32
		"3", //0x33
		"4", //0x34
		"5", //0x35
		"6", //0x36
		"7", //0x37
		"8", //0x38
		"9", //0x39
		"UNDEFINED", //0x3A
		"UNDEFINED", //0x3B
		"UNDEFINED", //0x3C
		"UNDEFINED", //0x3D
		"UNDEFINED", //0x3E
		"UNDEFINED", //0x3F
		"UNDEFINED", //0x40
		"A", //0x41
		"B", //0x42
		"C", //0x43
		"D", //0x44
		"E", //0x45
		"F", //0x46
		"G", //0x47
		"H", //0x48
		"I", //0x49
		"J", //0x4A
		"K", //0x4B
		"L", //0x4C
		"M", //0x4D
		"N", //0x4E
		"O", //0x4F
		"P", //0x50
		"Q", //0x51
		"R", //0x52
		"S", //0x53
		"T", //0x54
		"U", //0x55
		"V", //0x56
		"W", //0x57
		"X", //0x58
		"Y", //0x59
		"Z", //0x5A
		"LWIN", //0x5B
		"RWIN", //0x5C
		"APPS", //0x5D
		"RESERVED", //0x5E
		"SLEEP", //0x5F
		"NUMPAD0", //0x60
		"NUMPAD1", //0x61
		"NUMPAD2", //0x62
		"NUMPAD3", //0x63
		"NUMPAD4", //0x64
		"NUMPAD5", //0x65
		"NUMPAD6", //0x66
		"NUMPAD7", //0x67
		"NUMPAD8", //0x68
		"NUMPAD9", //0x69
		"MULTIPLY", //0x6A
		"ADD", //0x6B
		"SEPARATO", //0x6C
		"SUBTRACT", //0x6D
		"DECIMAL", //0x6E
		"DIVIDE", //0x6F
		"F1", //0x70
		"F2", //0x71
		"F3", //0x72
		"F4", //0x73
		"F5", //0x74
		"F6", //0x75
		"F7", //0x76
		"F8", //0x77
		"F9", //0x78
		"F10", //0x79
		"F11", //0x7A
		"F12", //0x7B
		"F13", //0x7C
		"F14", //0x7D
		"F15", //0x7E
		"F16", //0x7F
		"F17", //0x80
		"F18", //0x81
		"F19", //0x82
		"F20", //0x83
		"F21", //0x84
		"F22", //0x85
		"F23", //0x86
		"F24", //0x87
		"UNASSIGNED", //0x88
		"UNASSIGNED", //0x89
		"UNASSIGNED", //0x8A
		"UNASSIGNED", //0x8B
		"UNASSIGNED", //0x8C
		"UNASSIGNED", //0x8D
		"UNASSIGNED", //0x8E
		"UNASSIGNED", //0x8F
		"NUMLOCK", //0x90
		"SCROLL", //0x91
		"OEM SPECIFIC", //0x92-96
		"OEM SPECIFIC", //0x93
		"OEM SPECIFIC", //0x94
		"OEM SPECIFIC", //0x95
		"OEM SPECIFIC", //0x96
		"UNASSIGNED", //0x97-9F
		"UNASSIGNED", //0x98
		"UNASSIGNED", //0x99
		"UNASSIGNED", //0x9A
		"UNASSIGNED", //0x9B
		"UNASSIGNED", //0x9C
		"UNASSIGNED", //0x9D
		"UNASSIGNED", //0x9E
		"UNASSIGNED", //0x9F
		"LSHIFT", //0xA0
		"RSHIFT", //0xA1
		"LCONTROL", //0xA2
		"RCONTROL", //0xA3
		"LMENU", //0xA4
		"RMENU", //0xA5
		"BROWSER_BACK", //0xA6
		"BROWSER_FORWARD", //0xA7
		"BROWSER_REFRESH", //0xA8
		"BROWSER_STOP", //0xA9
		"BROWSER_SEARCH", //0xAA
		"BROWSER_FAVORITES", //0xAB
		"BROWSER_HOME", //0xAC
		"VOLUME_MUTE", //0xAD
		"VOLUME_DOWN", //0xAE
		"VOLUME_UP", //0xAF
		"MEDIA_NEXT_TRACK", //0xB0
		"MEDIA_PREV_TRACK", //0xB1
		"MEDIA_STOP", //0xB2
		"MEDIA_PLAY_PAUSE", //0xB3
		"LAUNCH_MAIL", //0xB4
		"LAUNCH_MEDIA_SELECT", //0xB5
		"LAUNCH_APP1", //0xB6
		"LAUNCH_APP2", //0xB7
		"RESERVED", //0xB8
		"RESERVED", //0xB9
		"OEM_1 ( US ' ; :' ) ", //0xBA US ' ; :'
		"OEM_PLUS", //0xBB '+'
		"OEM_COMMA", //0xBC ', '
		"OEM_MINUS", //0xBD '-'
		"OEM_PERIOD", //0xBE '.'
		"OEM_2 ( US '/?' ) ", //0xBF US '/?'
		"OEM_3 ( US '`‾' ) ", //0xC0 US '`‾'
		"RESERVED", //0xC1
		"RESERVED", //0xC2
		"RESERVED", //0xC3
		"RESERVED", //0xC4
		"RESERVED", //0xC5
		"RESERVED", //0xC6
		"RESERVED", //0xC7
		"RESERVED", //0xC8
		"RESERVED", //0xC9
		"RESERVED", //0xCA
		"RESERVED", //0xCB
		"RESERVED", //0xCC
		"RESERVED", //0xCD
		"RESERVED", //0xCE
		"RESERVED", //0xCF
		"RESERVED", //0xD0
		"RESERVED", //0xD1
		"RESERVED", //0xD2
		"RESERVED", //0xD3
		"RESERVED", //0xD4
		"RESERVED", //0xD5
		"RESERVED", //0xD6
		"RESERVED", //0xD7
		"UNASSIGNED", //0xD8
		"UNASSIGNED", //0xD9
		"UNASSIGNED", //0xDA
		"OEM_4 ( US ' [ {' ) ", //0xDB US ' [ {'
		"OEM_5 ( US '¥¥|' ) ", //0xDC US '¥|'
		"OEM_6 ( US ' ] }' ) ", //0xDD US ' ] }'
		"OEM_7 ( US '/¥'' ) ", //0xDE US 'single-quote/double-quote'
		"OEM_8", //0xDF
		"RESERVED", //0xE0
		"OEM SPECIFIC", //0xE1
		"OEM_102 ( '¥¥|' ) ", //0xE2 ¥|
		"OEM SPECIFIC", //0xE3
		"OEM SPECIFIC", //0xE4
		"PROCESSKEY", //0xE5
		"OEM SPECIFIC", //0xE6
		"PACKET", //0xE7
		"UNASSIGNED", //0xE8
		"OEM SPECIFIC", //0xE9-F5
		"OEM SPECIFIC", //0xEA
		"OEM SPECIFIC", //0xEB
		"OEM SPECIFIC", //0xEC
		"OEM SPECIFIC", //0xED
		"OEM SPECIFIC", //0xEE
		"OEM SPECIFIC", //0xEF
		"OEM SPECIFIC", //0xF0
		"OEM SPECIFIC", //0xF1
		"OEM SPECIFIC", //0xF2
		"OEM SPECIFIC", //0xF3
		"OEM SPECIFIC", //0xF4
		"OEM SPECIFIC", //0xF5
		"ATTN", //0xF6
		"CRSEL", //0xF7
		"EXSEL", //0xF8
		"EREOF", //0xF9
		"PLAY", //0xFA
		"ZOOM", //0xFB
		"NONAME", //0xFC
		"PA1", //0xFD
		"OEM_CLEAR", //0xFE
		//"None", //0xFF
	} ;
	*/
}
