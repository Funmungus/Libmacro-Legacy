/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

# include "signal/standard.h"
# include "signal/win/standard.h"

static void add_keynames ( ) ;
static void add_echoflags ( ) ;
static void add_echonames ( ) ;

void mcr_Key_load_contract ( )
{
	add_keynames ( ) ;
}

void mcr_Echo_load_contract ( )
{
	add_echoflags ( ) ;
	add_echonames ( ) ;
}

static void add_keynames ( )
{
	const char * names [ ] =
	{
		"None", //0x00
		"LButton", //0x01
		"RButton", //0x02
		"Cancel", //0x03
		"MButton", //0x04
		"XButton1", //0x05
		"XButton2", //0x06
		"", //0x07
		"Back", //0x08
		"Tab", //0x09
		"", //0x0A
		"", //0x0B
		"Clear", //0x0C
		"Return", //0x0D
		"", //0x0E
		"", //0x0F
		"Shift", //0x10
		"Control", //0x11
		"Menu",// ( ALT ) ", //0x12
		"PAUSE", //0x13
		"Capital", //0x14
		"Kana",// ( HANGUL ) ", //0x15
		"", //0x16
		"Junja", //0x17
		"Final", //0x18
		"Hanja",// ( KANJI ) ", //0x19
		"", //0x1A
		"Escape", //0x1B
		"Convert", //0x1C
		"NonConvert", //0x1D
		"Accept", //0x1E
		"ModeChange", //0x1F
		"Space", //0x20
		"Prior", //0x21
		"Next", //0x22
		"End", //0x23
		"Home", //0x24
		"Left", //0x25
		"Up", //0x26
		"Right", //0x27
		"Down", //0x28
		"Select", //0x29
		"Print", //0x2A
		"Execute", //0x2B
		"Snapshot", //0x2C
		"Insert", //0x2D
		"Delete", //0x2E
		"Help", //0x2F
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
		"", //0x3A
		"", //0x3B
		"", //0x3C
		"", //0x3D
		"", //0x3E
		"", //0x3F
		"", //0x40
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
		"LWin", //0x5B
		"RWin", //0x5C
		"Apps", //0x5D
		"", //0x5E
		"SLeep", //0x5F
		"Numpad0", //0x60
		"Numpad1", //0x61
		"Numpad2", //0x62
		"Numpad3", //0x63
		"Numpad4", //0x64
		"Numpad5", //0x65
		"Numpad6", //0x66
		"Numpad7", //0x67
		"Numpad8", //0x68
		"Numpad9", //0x69
		"Multiply", //0x6A
		"Add", //0x6B
		"Seperator", //0x6C
		"Subtract", //0x6D
		"Decimal", //0x6E
		"Divide", //0x6F
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
		"", //0x88
		"", //0x89
		"", //0x8A
		"", //0x8B
		"", //0x8C
		"", //0x8D
		"", //0x8E
		"", //0x8F
		"Numlock", //0x90
		"Scroll", //0x91
		"", //0x92-96
		"", //0x93
		"", //0x94
		"", //0x95
		"", //0x96
		"", //0x97-9F
		"", //0x98
		"", //0x99
		"", //0x9A
		"", //0x9B
		"", //0x9C
		"", //0x9D
		"", //0x9E
		"", //0x9F
		"LShift", //0xA0
		"RShift", //0xA1
		"LControl", //0xA2
		"RControl", //0xA3
		"LMenu", //0xA4
		"RMenu", //0xA5
		"BrowserBack", //0xA6
		"BrowserForward", //0xA7
		"BrowserRefresh", //0xA8
		"BrowserStop", //0xA9
		"BrowserSearch", //0xAA
		"BrowserFavorites", //0xAB
		"BrowserHome", //0xAC
		"VolumeMute", //0xAD
		"VolumeDown", //0xAE
		"VolumeUp", //0xAF
		"MediaNextTrack", //0xB0
		"MediaPrevTrack", //0xB1
		"MediaStop", //0xB2
		"MediaPlayPause", //0xB3
		"LaunchMail", //0xB4
		"LaunchMediaSelect", //0xB5
		"LaunchApp1", //0xB6
		"LaunchApp2", //0xB7
		"", //0xB8
		"", //0xB9
		"OEM_1 ( US ' ; :' ) ", //0xBA US ' ; :'
		"+", //0xBB '+'
		",", //0xBC ', '
		"-", //0xBD '-'
		".", //0xBE '.'
		"OEM_2 ( US '/?' ) ", //0xBF US '/?'
		"OEM_3 ( US '`‾' ) ", //0xC0 US '`‾'
		"", //0xC1
		"", //0xC2
		"", //0xC3
		"", //0xC4
		"", //0xC5
		"", //0xC6
		"", //0xC7
		"", //0xC8
		"", //0xC9
		"", //0xCA
		"", //0xCB
		"", //0xCC
		"", //0xCD
		"", //0xCE
		"", //0xCF
		"", //0xD0
		"", //0xD1
		"", //0xD2
		"", //0xD3
		"", //0xD4
		"", //0xD5
		"", //0xD6
		"", //0xD7
		"", //0xD8
		"", //0xD9
		"", //0xDA
		"OEM_4 ( US ' [ {' ) ", //0xDB US ' [ {'
		"OEM_5 ( US '¥¥|' ) ", //0xDC US '¥|'
		"OEM_6 ( US ' ] }' ) ", //0xDD US ' ] }'
		"OEM_7 ( US '/¥'' ) ", //0xDE US 'single-quote/double-quote'
		"OEM_8", //0xDF
		"", //0xE0
		"", //0xE1
		"OEM_102 ( '¥¥|' ) ", //0xE2 ¥|
		"", //0xE3
		"", //0xE4
		"Process", //0xE5
		"", //0xE6
		"Packet", //0xE7
		"", //0xE8
		"", //0xE9-F5
		"", //0xEA
		"", //0xEB
		"", //0xEC
		"", //0xED
		"", //0xEE
		"", //0xEF
		"", //0xF0
		"", //0xF1
		"", //0xF2
		"", //0xF3
		"", //0xF4
		"", //0xF5
		"Attn", //0xF6
		"Crsel", //0xF7
		"Exsel", //0xF8
		"Ereof", //0xF9
		"Play", //0xFA
		"Zoom", //0xFB
		"Noname", //0xFC
		"Pa1", //0xFD
		"OEM_CLEAR", //0xFE
		//"None", //0xFF
	} ;
	int count = sizeof ( names ) / sizeof ( char * ) ;
	int i ;
	for ( i = 0 ; i < count ; i ++ )
	{
		mcr_Key_set_name ( i, names [ i ] ) ;
	}
}

static void add_echoflags ( )
{
	const int echoflags [ ] =
	{
		MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP,
		MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP,
		MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP,
		MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP
	} ;
	const int count = sizeof ( echoflags ) / sizeof ( int ) ;
	for ( int i = 0 ; i < count ; i ++ )
	{
		mcr_Echo_set_mouseflag ( i, echoflags [ i ] ) ;
	}
}

static void add_echonames ( )
{
	const char * names [ ] =
	{
		"Left",
		"Middle",
		"Right",
		"X"
	} ;
	int count = sizeof ( names ) / sizeof ( char * ) ;
	char setname [ 64 ] ;
	int i = 0, echoCode = 0 ;
	while ( i < count )
	{
		snprintf ( setname, 63, "%s%s", names [ i ], "Down" ) ;
		mcr_Echo_set_name ( echoCode, setname ) ;
		snprintf ( setname, 63, "%s %s", names [ i ], "down" ) ;
		mcr_Echo_add_name ( echoCode, setname ) ;
		snprintf ( setname, 63, "%s_%s", names [ i ], "down" ) ;
		mcr_Echo_add_name ( echoCode ++, setname ) ;

		snprintf ( setname, 63, "%s%s", names [ i ], "Up" ) ;
		mcr_Echo_set_name ( echoCode, setname ) ;
		snprintf ( setname, 63, "%s %s", names [ i ], "up" ) ;
		mcr_Echo_add_name ( echoCode, setname ) ;
		snprintf ( setname, 63, "%s_%s", names [ i ], "up" ) ;
		mcr_Echo_add_name ( echoCode ++, setname ) ;
		++ i ;
	}
}
