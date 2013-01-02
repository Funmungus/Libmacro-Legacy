
# include "signal/keysignal.h"

namespace funlibrary
{
	const std::string KeySignal::_keyNames [ ] = {
		"RESERVED",		//0
		"ESC",		//	1
		"1",		//	2
		"2",		//	3
		"3",		//	4
		"4",		//	5
		"5",		//	6
		"6",		//	7
		"7",		//	8
		"8",		//	9
		"9",		//	10
		"0",		//	11
		"MINUS",		//12
		"EQUAL",		//13
		"BACKSPACE",		//14
		"TAB",		//	15
		"Q",		//	16
		"W",		//	17
		"E",		//	18
		"R",		//	19
		"T",		//	20
		"Y",		//	21
		"U",		//	22
		"I",		//	23
		"O",		//	24
		"P",		//	25
		"LEFTBRACE",		//26
		"RIGHTBRACE",		//27
		"ENTER",		//28
		"LEFTCTRL",		//29
		"A",		//	30
		"S",		//	31
		"D",		//	32
		"F",		//	33
		"G",		//	34
		"H",		//	35
		"J",		//	36
		"K",		//	37
		"L",		//	38
		"SEMICOLON",		//39
		"APOSTROPHE",		//40
		"GRAVE",		//41
		"LEFTSHIFT",		//42
		"BACKSLASH",		//43
		"Z",		//	44
		"X",		//	45
		"C",		//	46
		"V",		//	47
		"B",		//	48
		"N",		//	49
		"M",		//	50
		"COMMA",		//51
		"DOT",		//	52
		"SLASH",		//53
		"RIGHTSHIFT",		//54
		"KPASTERISK",		//55
		"LEFTALT",		//56
		"SPACE",		//57
		"CAPSLOCK",		//58
		"F1",		//	59
		"F2",		//	60
		"F3",		//	61
		"F4",		//	62
		"F5",		//	63
		"F6",		//	64
		"F7",		//	65
		"F8",		//	66
		"F9",		//	67
		"F10",		//	68
		"NUMLOCK",		//69
		"SCROLLLOCK",		//70
		"KP7",		//	71
		"KP8",		//	72
		"KP9",		//	73
		"KPMINUS",		//74
		"KP4",		//	75
		"KP5",		//	76
		"KP6",		//	77
		"KPPLUS",		//78
		"KP1",		//	79
		"KP2",		//	80
		"KP3",		//	81
		"KP0",		//	82
		"KPDOT",		//83
		"UNKNOWN",		//84
		"ZENKAKUHANKAKU",	//85
		"102ND",		//86
		"F11",		//	87
		"F12",		//	88
		"RO",		//	89
		"KATAKANA",		//90
		"HIRAGANA",		//91
		"HENKAN",		//92
		"KATAKANAHIRAGANA",	//93
		"MUHENKAN",		//94
		"KPJPCOMMA",		//95
		"KPENTER",		//96
		"RIGHTCTRL",		//97
		"KPSLASH",		//98
		"SYSRQ",		//99
		"RIGHTALT",		//100
		"LINEFEED",		//101
		"HOME",		//102
		"UP",		//	103
		"PAGEUP",		//104
		"LEFT",		//105
		"RIGHT",		//106
		"END",		//	107
		"DOWN",		//108
		"PAGEDOWN",		//109
		"INSERT",		//110
		"DELETE",		//111
		"MACRO",		//112
		"MUTE",		//113
		"VOLUMEDOWN",		//114
		"VOLUMEUP",		//115
		"POWER",		//116	/* SC System Power Down */
		"KPEQUAL",		//117
		"KPPLUSMINUS",		//118
		"PAUSE",		//119
		"SCALE",		//120	/* AL Compiz Scale (Expose) */

		"KPCOMMA",		//121
		"HANGEUL",		//122
		"HANJA",		//123
		"YEN",		//	124
		"LEFTMETA",		//125
		"RIGHTMETA",		//126
		"COMPOSE",		//127

		"STOP",		//128	/* AC Stop */
		"AGAIN",		//129
		"PROPS",		//130	/* AC Properties */
		"UNDO",		//131	/* AC Undo */
		"FRONT",		//132
		"COPY",		//133	/* AC Copy */
		"OPEN",		//134	/* AC Open */
		"PASTE",		//135	/* AC Paste */
		"FIND",		//136	/* AC Search */
		"CUT",		//	137	/* AC Cut */
		"HELP",		//138	/* AL Integrated Help Center */
		"MENU",		//139	/* Menu (show menu) */
		"CALC",		//140	/* AL Calculator */
		"SETUP",		//141
		"SLEEP",		//142	/* SC System Sleep */
		"WAKEUP",		//143	/* System Wake Up */
		"FILE",		//144	/* AL Local Machine Browser */
		"SENDFILE",		//145
		"DELETEFILE",		//146
		"XFER",		//147
		"PROG1",		//148
		"PROG2",		//149
		"WWW",		//	150	/* AL Internet Browser */
		"MSDOS",		//151
		"SCREENLOCK",		//152	/* AL Terminal Lock/Screensaver */
		"DIRECTION",		//153
		"CYCLEWINDOWS",	//154
		"MAIL",		//155
		"BOOKMARKS",		//156	/* AC Bookmarks */
		"COMPUTER",		//157
		"BACK",		//158	/* AC Back */
		"FORWARD",		//159	/* AC Forward */
		"CLOSECD",		//160
		"EJECTCD",		//161
		"EJECTCLOSECD",	//162
		"NEXTSONG",		//163
		"PLAYPAUSE",		//164
		"PREVIOUSSONG",	//165
		"STOPCD",		//166
		"RECORD",		//167
		"REWIND",		//168
		"PHONE",		//169	/* Media Select Telephone */
		"ISO",		//	170
		"CONFIG",		//171	/* AL Consumer Control Configuration */
		"HOMEPAGE",		//172	/* AC Home */
		"REFRESH",		//173	/* AC Refresh */
		"EXIT",		//174	/* AC Exit */
		"MOVE",		//175
		"EDIT",		//176
		"SCROLLUP",		//177
		"SCROLLDOWN",		//178
		"KPLEFTPAREN",		//179
		"KPRIGHTPAREN",	//180
		"NEW",		//	181	/* AC New */
		"REDO",		//182	/* AC Redo/Repeat */

		"F13",		//	183
		"F14",		//	184
		"F15",		//	185
		"F16",		//	186
		"F17",		//	187
		"F18",		//	188
		"F19",		//	189
		"F20",		//	190
		"F21",		//	191
		"F22",		//	192
		"F23",		//	193
		"F24",		//	194
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",	// 195-199
		"PLAYCD",		//200
		"PAUSECD",		//201
		"PROG3",		//202
		"PROG4",		//203
		"DASHBOARD",		//204	/* AL Dashboard */
		"SUSPEND",		//205
		"CLOSE",		//206	/* AC Close */
		"PLAY",		//207
		"FASTFORWARD",		//208
		"BASSBOOST",		//209
		"PRINT",		//210	/* AC Print */
		"HP",		//	211
		"CAMERA",		//212
		"SOUND",		//213
		"QUESTION",		//214
		"EMAIL",		//215
		"CHAT",		//216
		"SEARCH",		//217
		"CONNECT",		//218
		"FINANCE",		//219	/* AL Checkbook/Finance */
		"SPORT",		//220
		"SHOP",		//221
		"ALTERASE",		//222
		"CANCEL",		//223	/* AC Cancel */
		"BRIGHTNESSDOWN",	//224
		"BRIGHTNESSUP",	//225
		"MEDIA",		//226

		"SWITCHVIDEOMODE",	//227	/* Cycle between available video
							/* outputs (Monitor/LCD/TV-out/etc) */
		"KBDILLUMTOGGLE",	//228
		"KBDILLUMDOWN",	//229
		"KBDILLUMUP",		//230

		"SEND",		//231	/* AC Send */
		"REPLY",		//232	/* AC Reply */
		"FORWARDMAIL",		//233	/* AC Forward Msg */
		"SAVE",		//234	/* AC Save */
		"DOCUMENTS",		//235

		"BATTERY",		//236

		"BLUETOOTH",		//237
		"WLAN",		//238
		"UWB",		//	239

		"UNKNOWN",		//240

		"VIDEO_NEXT",		//241	/* drive next video source */
		"VIDEO_PREV",		//242	/* drive previous video source */
		"BRIGHTNESS_CYCLE",	//243	/* brightness up, after max is min */
		"BRIGHTNESS_ZERO",	//244	/* brightness off, use ambient */
		"DISPLAY_OFF",		//245	/* display device to off state */

		"WWAN",		//246	/* Wireless WAN (LTE, UMTS, GSM, etc.) */
		"RFKILL",		//247	/* Key that controls all radios */

		"MICMUTE",		//248	/* Mute / unmute the microphone */
		//249-0x0FE==254
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		/* Code 255 is reserved for special needs of AT keyboard driver */
		"AT_KBD_DRIVER",

		"BTN_0",		//	0x100
		"BTN_1",		//	0x101
		"BTN_2",		//	0x102
		"BTN_3",		//	0x103
		"BTN_4",		//	0x104
		"BTN_5",		//	0x105
		"BTN_6",		//	0x106
		"BTN_7",		//	0x107
		"BTN_8",		//	0x108
		"BTN_9",		//	0x109

		// 0x10A-0x10F
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"BTN_LEFT",		//0x110
		"BTN_RIGHT",		//0x111
		"BTN_MIDDLE",		//0x112
		"BTN_SIDE",		//0x113
		"BTN_EXTRA",		//0x114
		"BTN_FORWARD",		//0x115
		"BTN_BACK",		//0x116
		"BTN_TASK",		//0x117

		// 0x118-11F
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"BTN_TRIGGER",		//0x120
		"BTN_THUMB",		//0x121
		"BTN_THUMB2",		//0x122
		"BTN_TOP",		//	0x123
		"BTN_TOP2",		//0x124
		"BTN_PINKIE",		//0x125
		"BTN_BASE",		//0x126
		"BTN_BASE2",		//0x127
		"BTN_BASE3",		//0x128
		"BTN_BASE4",		//0x129
		"BTN_BASE5",		//0x12a
		"BTN_BASE6",		//0x12b
		"UNKNOWN", "UNKNOWN", "UNKNOWN", 		//0x12c-0x12e
		"BTN_DEAD",		//0x12f

		"BTN_A",		//	//0x130 BTN_SOUTH
		"BTN_B",		////0x131	BTN_EAST
		"BTN_C",		//	0x132
		"BTN_X",		////0x133	BTN_NORTH
		"BTN_Y",		////0x134	BTN_WEST
		"BTN_Z",		//	0x135
		"BTN_TL",		//	0x136
		"BTN_TR",		//	0x137
		"BTN_TL2",		//	0x138
		"BTN_TR2",		//	0x139
		"BTN_SELECT",		//0x13a
		"BTN_START",		//0x13b
		"BTN_MODE",		//0x13c
		"BTN_THUMBL",		//0x13d
		"BTN_THUMBR",		//0x13e
		"UNKNOWN", 		//0x13f
		//"BTN_DIGI",		//0x140
		"BTN_TOOL_PEN",		//0x140
		"BTN_TOOL_RUBBER",		//0x141
		"BTN_TOOL_BRUSH",		//0x142
		"BTN_TOOL_PENCIL",		//0x143
		"BTN_TOOL_AIRBRUSH",	//0x144
		"BTN_TOOL_FINGER",		//0x145
		"BTN_TOOL_MOUSE",		//0x146
		"BTN_TOOL_LENS",		//0x147
		"BTN_TOOL_QUINTTAP",	//0x148	/* Five fingers on trackpad */
		"UNKNOWN", 		//0x149
		"BTN_TOUCH",		//0x14a
		"BTN_STYLUS",		//0x14b
		"BTN_STYLUS2",		//0x14c
		"BTN_TOOL_DOUBLETAP",	//0x14d
		"BTN_TOOL_TRIPLETAP",	//0x14e
		"BTN_TOOL_QUADTAP",	//0x14f	/* Four fingers on trackpad */

		"BTN_WHEEL",		//0x150
		"BTN_GEAR_UP",		//0x151

		// 0x152-0x15f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"OK",		//	0x160
		"SELECT",		//0x161
		"GOTO",		//0x162
		"CLEAR",		//0x163
		"POWER2",		//0x164
		"OPTION",		//0x165
		"INFO",		//0x166	/* AL OEM Features/Tips/Tutorial */
		"TIME",		//0x167
		"VENDOR",		//0x168
		"ARCHIVE",		//0x169
		"PROGRAM",		//0x16a	/* Media Select Program Guide */
		"CHANNEL",		//0x16b
		"FAVORITES",		//0x16c
		"EPG",		//	0x16d
		"PVR",		//	0x16e	/* Media Select Home */
		"MHP",		//	0x16f
		"LANGUAGE",		//0x170
		"TITLE",		//0x171
		"SUBTITLE",		//0x172
		"ANGLE",		//0x173
		"ZOOM",		//0x174
		"MODE",		//0x175
		"KEYBOARD",		//0x176
		"SCREEN",		//0x177
		"PC",		//	0x178	/* Media Select Computer */
		"TV",		//	0x179	/* Media Select TV */
		"TV2",		//	0x17a	/* Media Select Cable */
		"VCR",		//	0x17b	/* Media Select VCR */
		"VCR2",		//0x17c	/* VCR Plus */
		"SAT",		//	0x17d	/* Media Select Satellite */
		"SAT2",		//0x17e
		"CD",		//	0x17f	/* Media Select CD */
		"TAPE",		//0x180	/* Media Select Tape */
		"RADIO",		//0x181
		"TUNER",		//0x182	/* Media Select Tuner */
		"PLAYER",		//0x183
		"TEXT",		//0x184
		"DVD",		//	0x185	/* Media Select DVD */
		"AUX",		//	0x186
		"MP3",		//	0x187
		"AUDIO",		//0x188	/* AL Audio Browser */
		"VIDEO",		//0x189	/* AL Movie Browser */
		"DIRECTORY",		//0x18a
		"LIST",		//0x18b
		"MEMO",		//0x18c	/* Media Select Messages */
		"CALENDAR",		//0x18d
		"RED",		//	0x18e
		"GREEN",		//0x18f
		"YELLOW",		//0x190
		"BLUE",		//0x191
		"CHANNELUP",		//0x192	/* Channel Increment */
		"CHANNELDOWN",		//0x193	/* Channel Decrement */
		"FIRST",		//0x194
		"LAST",		//0x195	/* Recall Last */
		"AB",		//	0x196
		"NEXT",		//0x197
		"RESTART",		//0x198
		"SLOW",		//0x199
		"SHUFFLE",		//0x19a
		"BREAK",		//0x19b
		"PREVIOUS",		//0x19c
		"DIGITS",		//0x19d
		"TEEN",		//0x19e
		"TWEN",		//0x19f
		"VIDEOPHONE",		//0x1a0	/* Media Select Video Phone */
		"GAMES",		//0x1a1	/* Media Select Games */
		"ZOOMIN",		//0x1a2	/* AC Zoom In */
		"ZOOMOUT",		//0x1a3	/* AC Zoom Out */
		"ZOOMRESET",		//0x1a4	/* AC Zoom */
		"WORDPROCESSOR",	//0x1a5	/* AL Word Processor */
		"EDITOR",		//0x1a6	/* AL Text Editor */
		"SPREADSHEET",		//0x1a7	/* AL Spreadsheet */
		"GRAPHICSEDITOR",	//0x1a8	/* AL Graphics Editor */
		"PRESENTATION",	//0x1a9	/* AL Presentation App */
		"DATABASE",		//0x1aa	/* AL Database App */
		"NEWS",		//0x1ab	/* AL Newsreader */
		"VOICEMAIL",		//0x1ac	/* AL Voicemail */
		"ADDRESSBOOK",		//0x1ad	/* AL Contacts/Address Book */
		"MESSENGER",		//0x1ae	/* AL Instant Messaging */
		"DISPLAYTOGGLE",	//0x1af	/* Turn display (LCD) on and off */
		"SPELLCHECK",		//0x1b0   /* AL Spell Check */
		"LOGOFF",		//0x1b1   /* AL Logoff */

		"DOLLAR",		//0x1b2
		"EURO",		//0x1b3

		"FRAMEBACK",		//0x1b4	/* Consumer - transport controls */
		"FRAMEFORWARD",	//0x1b5
		"CONTEXT_MENU",	//0x1b6	/* GenDesc - system context menu */
		"MEDIA_REPEAT",	//0x1b7	/* Consumer - transport control */
		"10CHANNELSUP",	//0x1b8	/* 10 channels up (10+) */
		"10CHANNELSDOWN",	//0x1b9	/* 10 channels down (10-) */
		"IMAGES",		//0x1ba	/* AL Image Browser */

		// 0x1bb-0x1bf
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"DEL_EOL",		//0x1c0
		"DEL_EOS",		//0x1c1
		"INS_LINE",		//0x1c2
		"DEL_LINE",		//0x1c3

		//0x1c4-0x1cf
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"FN",		//	0x1d0
		"FN_ESC",		//0x1d1
		"FN_F1",		//0x1d2
		"FN_F2",		//0x1d3
		"FN_F3",		//0x1d4
		"FN_F4",		//0x1d5
		"FN_F5",		//0x1d6
		"FN_F6",		//0x1d7
		"FN_F7",		//0x1d8
		"FN_F8",		//0x1d9
		"FN_F9",		//0x1da
		"FN_F10",		//0x1db
		"FN_F11",		//0x1dc
		"FN_F12",		//0x1dd
		"FN_1",		//0x1de
		"FN_2",		//0x1df
		"FN_D",		//0x1e0
		"FN_E",		//0x1e1
		"FN_F",		//0x1e2
		"FN_S",		//0x1e3
		"FN_B",		//0x1e4

		// 0x1e5-0x1f0
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"BRL_DOT1",		//0x1f1
		"BRL_DOT2",		//0x1f2
		"BRL_DOT3",		//0x1f3
		"BRL_DOT4",		//0x1f4
		"BRL_DOT5",		//0x1f5
		"BRL_DOT6",		//0x1f6
		"BRL_DOT7",		//0x1f7
		"BRL_DOT8",		//0x1f8
		"BRL_DOT9",		//0x1f9
		"BRL_DOT10",		//0x1fa

		//0x1fb-0x1ff
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"NUMERIC_0",		//0x200	/* used by phones, remote controls, */
		"NUMERIC_1",		//0x201	/* and other keypads */
		"NUMERIC_2",		//0x202
		"NUMERIC_3",		//0x203
		"NUMERIC_4",		//0x204
		"NUMERIC_5",		//0x205
		"NUMERIC_6",		//0x206
		"NUMERIC_7",		//0x207
		"NUMERIC_8",		//0x208
		"NUMERIC_9",		//0x209
		"NUMERIC_STAR",	//0x20a
		"NUMERIC_POUND",	//0x20b

		// 0x20c-0x20f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"CAMERA_FOCUS",	//0x210
		"WPS_BUTTON",		//0x211	/* WiFi Protected Setup key */

		"TOUCHPAD_TOGGLE",	//0x212	/* Request switch touchpad on or off */
		"TOUCHPAD_ON",		//0x213
		"TOUCHPAD_OFF",	//0x214

		"CAMERA_ZOOMIN",	//0x215
		"CAMERA_ZOOMOUT",	//0x216
		"CAMERA_UP",		//0x217
		"CAMERA_DOWN",		//0x218
		"CAMERA_LEFT",		//0x219
		"CAMERA_RIGHT",	//0x21a

		"ATTENDANT_ON",	//0x21b
		"ATTENDANT_OFF",	//0x21c
		"ATTENDANT_TOGGLE",	//0x21d	/* Attendant call on or off */
		"LIGHTS_TOGGLE",	//0x21e	/* Reading light on or off */

		"UNKNOWN", 		// 0x21f

		"BTN_DPAD_UP",		//0x220
		"BTN_DPAD_DOWN",		//0x221
		"BTN_DPAD_LEFT",		//0x222
		"BTN_DPAD_RIGHT",		//0x223

		// 0x224-0x22f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"ALS_TOGGLE",		//0x230	/* Ambient light sensor */

		// 0x231-0x2bf
		// 0x231-0x23f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		// 0x240-0x24f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		// 0x250-0x25f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		// 0x260-0x26f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		// 0x270-0x27f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		// 0x280-0x28f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		// 0x290-0x29f
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		// 0x2a0-0x2af
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		// 0x2b0-0x2bf
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",
		"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN",

		"BTN_TRIGGER_HAPPY",		//0x2c0
		"BTN_TRIGGER_HAPPY2",		//0x2c1
		"BTN_TRIGGER_HAPPY3",		//0x2c2
		"BTN_TRIGGER_HAPPY4",		//0x2c3
		"BTN_TRIGGER_HAPPY5",		//0x2c4
		"BTN_TRIGGER_HAPPY6",		//0x2c5
		"BTN_TRIGGER_HAPPY7",		//0x2c6
		"BTN_TRIGGER_HAPPY8",		//0x2c7
		"BTN_TRIGGER_HAPPY9",		//0x2c8
		"BTN_TRIGGER_HAPPY10",		//0x2c9
		"BTN_TRIGGER_HAPPY11",		//0x2ca
		"BTN_TRIGGER_HAPPY12",		//0x2cb
		"BTN_TRIGGER_HAPPY13",		//0x2cc
		"BTN_TRIGGER_HAPPY14",		//0x2cd
		"BTN_TRIGGER_HAPPY15",		//0x2ce
		"BTN_TRIGGER_HAPPY16",		//0x2cf
		"BTN_TRIGGER_HAPPY17",		//0x2d0
		"BTN_TRIGGER_HAPPY18",		//0x2d1
		"BTN_TRIGGER_HAPPY19",		//0x2d2
		"BTN_TRIGGER_HAPPY20",		//0x2d3
		"BTN_TRIGGER_HAPPY21",		//0x2d4
		"BTN_TRIGGER_HAPPY22",		//0x2d5
		"BTN_TRIGGER_HAPPY23",		//0x2d6
		"BTN_TRIGGER_HAPPY24",		//0x2d7
		"BTN_TRIGGER_HAPPY25",		//0x2d8
		"BTN_TRIGGER_HAPPY26",		//0x2d9
		"BTN_TRIGGER_HAPPY27",		//0x2da
		"BTN_TRIGGER_HAPPY28",		//0x2db
		"BTN_TRIGGER_HAPPY29",		//0x2dc
		"BTN_TRIGGER_HAPPY30",		//0x2dd
		"BTN_TRIGGER_HAPPY31",		//0x2de
		"BTN_TRIGGER_HAPPY32",		//0x2df
		"BTN_TRIGGER_HAPPY33",		//0x2e0
		"BTN_TRIGGER_HAPPY34",		//0x2e1
		"BTN_TRIGGER_HAPPY35",		//0x2e2
		"BTN_TRIGGER_HAPPY36",		//0x2e3
		"BTN_TRIGGER_HAPPY37",		//0x2e4
		"BTN_TRIGGER_HAPPY38",		//0x2e5
		"BTN_TRIGGER_HAPPY39",		//0x2e6
		"BTN_TRIGGER_HAPPY40",		//0x2e7
	} ;

	void KeySignal::initVent ( )
	{
		memset ( &_keyvent, 0, sizeof ( _keyvent ) ) ;
		_keyvent [ 0 ].type = EV_MSC ;
		_keyvent [ 0 ].code = MSC_SCAN ;
		_keyvent [ 0 ].value = getScan ( ) ;
		_keyvent [ 1 ].type = EV_KEY ;
		_keyvent [ 1 ].code = getKey ( ) ;
		_keyvent [ 2 ].type = EV_SYN ;
		_keyvent [ 2 ].code = SYN_REPORT ;
	}

	bool KeySignal::send ( )
	{
		const static unsigned int size = sizeof ( _keyvent ) ;
		// should already be done in setKey
	//	_keyvent.code = getKey ( ) ;
		// Key down for all not equal to up only.
		if ( getKeyUp ( ) != KeyUpType::UP )
		{
			_keyvent [ 1 ].value = 1 ;
			if ( ! KeyDev.send ( _keyvent, size ) )
			{
				return false ;
			}
		}

		// Key up for all not equal to down only.
		if ( getKeyUp ( ) != KeyUpType::DOWN )
		{
			_keyvent [ 1 ].value = 0 ;
			if ( ! KeyDev.send ( _keyvent, size ) )
			{
				return false ;
			}
		}

		return true ;
	}
}

