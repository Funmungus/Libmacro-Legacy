/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

#include "mcr/standard/standard.h"
#include "mcr/standard/private.h"
#include MCR_STANDARD_NATIVE_INC
#include <errno.h>
#include <stdio.h>
#include <string.h>

MCR_API struct mcr_Array mcr_echoEvents;
MCR_API struct mcr_Map mcr_keyToEcho[2];
MCR_API mcr_SpacePosition mcr_cursor = { 0 };

static size_t _initialize_count = 0;

static int mcr_Mods_load_key_contract(struct mcr_context *ctx);
static int add_key_names(struct mcr_context *context);
static int add_echo_keys();
static int add_echo_names(struct mcr_context *context);

void mcr_cursor_position(mcr_SpacePosition buffer)
{
	for (int i = MCR_DIMENSION_CNT; i--;)
		buffer[i] = mcr_cursor[i];
}

int mcr_Echo_key(int echoCode)
{
	struct mcr_Key *keyPt =
		MCR_ARR_ELEMENT(mcr_echoEvents, (unsigned)echoCode);
	return keyPt ? MCR_KEY_KEY(*keyPt) : MCR_ECHO_ANY;
}

int mcr_Echo_set_key(int echoCode, struct mcr_Key *keyPt)
{
	int err, kVal;
	struct mcr_Key initial;
	struct mcr_Map *mapPt;
	dassert(keyPt);
	if (echoCode == MCR_ECHO_ANY) {
		mset_error(EINVAL)
			return -EINVAL;
	}
	mcr_Key_init(&initial);
	if ((err = mcr_Array_minfill(&mcr_echoEvents, echoCode + 1, &initial))
		|| (err = mcr_Array_set(&mcr_echoEvents, echoCode, keyPt)))
		return err;
	mapPt = mcr_keyToEcho + (MCR_KEY_UP_TYPE(*keyPt) ? 1 : 0);
	kVal = MCR_KEY_KEY(*keyPt);
	return mcr_Map_map(mapPt, &kVal, &echoCode);
}

void mcr_HidEcho_init(void *echoPt)
{
	dassert(echoPt);
	((struct mcr_HidEcho *)echoPt)->event = 0;
}

void mcr_Key_init(void *keyPt)
{
	struct mcr_Key *kPt = keyPt;
	dassert(keyPt);
	memset(kPt, 0, sizeof(struct mcr_Key));
	kPt->events[0].type = EV_MSC;
	kPt->events[0].code = MSC_SCAN;
	kPt->events[1].type = EV_KEY;
	kPt->events[2] = mcr_syncer;
	kPt->up_type = MCR_BOTH;
}

void mcr_MoveCursor_init(void *mcPt)
{
	struct mcr_MoveCursor *mPt = mcPt;
	dassert(mcPt);
	memset(mPt, 0, sizeof(struct mcr_MoveCursor));
	mPt->absvent[0].type = mPt->absvent[1].type =
		mPt->absvent[2].type = EV_ABS;
	mPt->relvent[0].type = mPt->relvent[1].type =
		mPt->relvent[2].type = EV_REL;
	mPt->absvent[MCR_X].code = ABS_X;
	mPt->absvent[MCR_Y].code = ABS_Y;
	mPt->absvent[MCR_Z].code = ABS_Z;
	mPt->relvent[MCR_X].code = REL_X;
	mPt->relvent[MCR_Y].code = REL_Y;
	mPt->relvent[MCR_Z].code = REL_Z;
	mPt->relvent[MCR_DIMENSION_CNT] =
		mPt->absvent[MCR_DIMENSION_CNT] = mcr_syncer;
	mPt->is_justify = true;
}

void mcr_Scroll_init(void *scrollPt)
{
	struct mcr_Scroll *sPt = scrollPt;
	dassert(scrollPt);
	memset(sPt, 0, sizeof(struct mcr_Scroll));
	sPt->events[0].type = sPt->events[1].type =
		sPt->events[2].type = EV_REL;
	sPt->events[MCR_X].code = REL_HWHEEL;
	sPt->events[MCR_Y].code = REL_WHEEL;
	sPt->events[MCR_Z].code = REL_DIAL;
	sPt->events[MCR_DIMENSION_CNT] = mcr_syncer;
}

int mcr_HidEcho_send_data(struct mcr_HidEcho *dataPt)
{
	ssize_t err;
	if ((unsigned)dataPt->event < mcr_echoEvents.used) {
		err = mcr_Key_send_data((struct mcr_Key *)
			MCR_ARR_ELEMENT(mcr_echoEvents,
				(unsigned)dataPt->event));
		if (err == -1) {
			mset_error(EINTR);
			return EINTR;
		}
	} else {
		mset_error(EFAULT);
		return -EFAULT;
	}
	return 0;
}

int mcr_Key_send_data(struct mcr_Key *dataPt)
{
	ssize_t err;
	if (dataPt->up_type != MCR_UP) {
		dataPt->events[1].value = 1;
		err = MCR_DEV_SEND(mcr_genDev, dataPt->events,
			sizeof(dataPt->events));
		if (err == -1) {
			mset_error(EINTR);
			return EINTR;
		}
	}
	if (dataPt->up_type != MCR_DOWN) {
		dataPt->events[1].value = 0;
		err = MCR_DEV_SEND(mcr_genDev, dataPt->events,
			sizeof(dataPt->events));
		if (err == -1) {
			mset_error(EINTR);
			return EINTR;
		}
	}
	return 0;
}

static inline void localJustify(struct mcr_MoveCursor *dataPt, int pos)
{
	mcr_cursor[pos] += dataPt->relvent[pos].value;
	if (mcr_cursor[pos] > mcr_abs_resolution)
		mcr_cursor[pos] = mcr_abs_resolution;
	else if (mcr_cursor[pos] < 0)
		mcr_cursor[pos] = 0;
}

int mcr_MoveCursor_send_data(struct mcr_MoveCursor *dataPt)
{
	ssize_t err;
	if (dataPt->is_justify) {
		err = MCR_DEV_SEND(mcr_genDev, dataPt->relvent,
			sizeof(dataPt->relvent));
		if (err == -1) {
			mset_error(EINTR);
			return EINTR;
		}
		localJustify(dataPt, MCR_X);
		localJustify(dataPt, MCR_Y);
		localJustify(dataPt, MCR_Z);
	} else {
		err = MCR_DEV_SEND(mcr_absDev, dataPt->absvent,
			sizeof(dataPt->absvent));
		if (err == -1) {
			mset_error(EINTR);
			return EINTR;
		}
		mcr_cursor[MCR_X] = dataPt->absvent[MCR_X].value;
		mcr_cursor[MCR_Y] = dataPt->absvent[MCR_Y].value;
		mcr_cursor[MCR_Z] = dataPt->absvent[MCR_Z].value;
	}
	return 0;
}

int mcr_Scroll_send_data(struct mcr_Scroll *dataPt)
{
	ssize_t err = MCR_DEV_SEND(mcr_genDev, dataPt->events,
		sizeof(dataPt->events));
	if (err == -1) {
		mset_error(EINTR);
		return EINTR;
	}
	return 0;
}

int mcr_Key_load_contract(struct mcr_context *context)
{
	return add_key_names(context);
}

int mcr_HidEcho_load_contract(struct mcr_context *context)
{
	int err = add_echo_keys();
	if (err)
		return err;
	return add_echo_names(context);
}

int mcr_standard_native_initialize(struct mcr_context *context)
{
	int ret;
	UNUSED(context);
	if (_initialize_count) {
		++_initialize_count;
		return 0;
	}
	mcr_Array_init(&mcr_echoEvents);
	mcr_echoEvents.element_size = sizeof(struct mcr_Key);
	mcr_Map_init(mcr_keyToEcho);
	mcr_Map_set_all(mcr_keyToEcho, sizeof(int), sizeof(int),
		mcr_int_compare, NULL, NULL);
	mcr_Map_init(mcr_keyToEcho + 1);
	mcr_Map_set_all(mcr_keyToEcho + 1, sizeof(int), sizeof(int),
		mcr_int_compare, NULL, NULL);
	if ((ret = mcr_Device_initialize(context)))
		return ret;
	return mcr_Mods_load_key_contract(context);
}

void mcr_standard_native_cleanup(struct mcr_context *context)
{
	UNUSED(context);
	/* Remove an initialized reference of internal modifiers */
	if (!_initialize_count || --_initialize_count)
		return;
	mcr_Device_cleanup(context);
	mcr_Array_free(&mcr_echoEvents);
	mcr_Map_free(mcr_keyToEcho);
	mcr_Map_free(mcr_keyToEcho + 1);
}

static int mcr_Mods_load_key_contract(struct mcr_context *ctx)
{
	/* Left defaults */
	const unsigned int types[] = {
		MCR_ALT, MCR_ALTGR, MCR_COMPOSE,
		MCR_CTRL, MCR_FN, MCR_FRONT,
		MCR_SHIFT, MCR_META
	};
	const int modKeys[] = {
		KEY_LEFTALT, KEY_RIGHTALT, KEY_COMPOSE,
		KEY_LEFTCTRL, KEY_FN, KEY_FRONT,
		KEY_LEFTSHIFT, KEY_LEFTMETA
	};
	/* Right extras */
	const unsigned int extraTypes[] = {
		MCR_OPTION, MCR_CTRL, MCR_SHIFT, MCR_META
	};
	const int extraModKeys[] = {
		KEY_OPTION, KEY_RIGHTCTRL, KEY_RIGHTSHIFT, KEY_RIGHTMETA
	};
	int err;
	size_t i = arrlen(types);
	while (i--) {
		if ((err = mcr_Key_mod_set_key(ctx, types[i], modKeys[i])))
			return err;
	}
	i = arrlen(extraTypes);
	while (i--) {
		if ((err = mcr_Key_mod_add(ctx, extraTypes[i],
					extraModKeys[i])))
			return err;
	}
	return 0;
}

static int add_key_names(struct mcr_context *context)
{
	const char *names[] = {
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
		"Katakana", "Hiragana", "Henkan", "KatakanaHiragana",
		"Muhenkan",
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
		"F20", "F21", "F22", "F23", "F24",	/* 194 */
		/* 195 - 199 */
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
		/* 249 - 254 */
		"", "", "", "", "", "",
		"AT_KBD_Driver", "BTN_0", "BTN_1", "BTN_2", "BTN_3",
		"BTN_4", "BTN_5", "BTN_6", "BTN_7", "BTN_8", "BTN_9",
		"BTN_Left", "BTN_Right", "BTN_Middle", "BTN_Side", "BTN_Extra",
		"BTN_Forward", "BTN_Back", "BTN_Task", "BTN_Trigger",
		"BTN_Thumb",
		"BTN_Thumb2", "BTN_Top", "BTN_Top2", "BTN_Pinkie", "BTN_Base",
		"BTN_Base2", "BTN_Base3", "BTN_Base4", "BTN_Base5", "BTN_Base6",
		/* 0x12c - 0x12e */
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
		/* 0x152 - 0x15f */
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
		"News", "VoiceMail", "AddressBook", "Messenger",
		"DisplayToggle",
		"SpellCheck", "LogOff", "Dollar", "Euro" "FrameBack",
		"FrameForward", "ContextMenu", "MediaRepeat", "10ChannelsUp",
		"10ChannelsDown",
		"Images",
		/* 0x1bb - 0x1bf */
		"", "", "", "", "",
		"DEL_EOL", "DEL_EOS", "INS_Line", "DEL_Line",
		/* 0x1c4 - 0x1cf */
		"", "", "", "", "", "", "", "", "", "", "", "",
		"FN", "FN_Esc", "FN_F1", "FN_F2", "FN_F3",
		"FN_F4", "FN_F5", "FN_F6", "FN_F7", "FN_F8",
		"FN_F9", "FN_F10", "FN_F11", "FN_F12", "FN_1",
		"FN_2", "FN_D", "FN_E", "FN_F", "FN_S",
		"FN_B",
		/* 0x1e5 - 0x1f0 */
		"", "", "", "", "", "", "", "", "", "", "",
		"",
		"BRL_Dot1", "BRL_Dot2", "BRL_Dot3", "BRL_Dot4", "BRL_Dot5",
		"BRL_Dot6", "BRL_Dot7", "BRL_Dot8", "BRL_Dot9", "BRL_Dot10",
		/* 0x1fb - 0x1ff */
		"", "", "", "", "",
		"Numeric_0", "Numeric_1", "Numeric_2", "Numeric_3", "Numeric_4",
		"Numeric_5", "Numeric_6", "Numeric_7", "Numeric_8", "Numeric_9",
		"Numeric_Star", "Numeric_Pound",
		/* 0x20c - 0x20f */
		"", "", "", "",
		"CameraFocus", "WPS_Button",

		"TouchpadToggle", "TouchpadOn", "TouchpadOff",

		"CameraZoomIn", "CameraZoomOut", "CameraUp",
		"CameraDown", "CameraLeft",
		"CameraRight", "AttendantOn", "AttendantOff",
		"AttendantToggle", "LightsToggle",
		"",
		"BTN_DPAD_Up", "BTN_DPAD_Down", "BTN_DPAD_Left",
		"BTN_DPAD_Right",
		/* 0x224 - 0x22f */
		"", "", "", "", "", "", "", "", "", "", "", "" "ALS_Toggle",
		/* 0x231 - 0x2bf */
		/* 0x231 - 0x23f */
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
		/* 0x240 - 0x24f */
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		/* 0x250 - 0x25f */
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		/* 0x260 - 0x26f */
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		/* 0x270 - 0x27f */
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		/* 0x280 - 0x28f */
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		/* 0x290 - 0x29f */
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		/* 0x2a0 - 0x2af */
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		/* 0x2b0 - 0x2bf */
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
	};
	const int extraKeys[] = {
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
	};
	const char *extraNames[][6] = {
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
		{"line feed", "line_feed"}, {"page up", "page_up"},
		{"page down",
			"page_down"}, {"volume down", "volume_down"},
		{"volume up", "volume_up"}, {"left meta", "left_meta", "meta"},
		{"right meta", "right_meta"}, {"scroll up", "scroll_up"},
		{"scroll down", "scroll_down"},

		{"Hanguel"}, {"Coffee"}, {"BrightnessAuto"}, {"WiMax"},
		{"BTN_Misc"}, {"BTN_Mouse"}, {"BTN_Joystick"},
		{"BTN_Gamepad"}, {"BTN_South"}, {"BTN_East"},
		{"BTN_North"}, {"BTN_West"}, {"BTN_Digi"},
		{"BTN_Wheel"}, {"TriggerHappy1"},
	};
	const size_t extraLens[] = {
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
	};
	size_t i = arrlen(names);
	int err = 0;
	while (i--) {
		if ((err = mcr_Key_set_name(context, i, names[i])))
			return err;
	}
	i = arrlen(extraKeys);
	while (i--) {
		if ((err = mcr_Key_add(context, extraKeys[i], extraNames[i],
					extraLens[i])))
			return err;
	}
	return err;
}

static int add_echo_keys()
{
	const int echokeys[] = {
		BTN_0, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7,
		BTN_8, BTN_9, BTN_LEFT, BTN_RIGHT, BTN_MIDDLE, BTN_SIDE,
		BTN_EXTRA, BTN_FORWARD, BTN_BACK, BTN_TASK, BTN_TRIGGER,
		BTN_THUMB, BTN_THUMB2, BTN_TOP, BTN_TOP2, BTN_PINKIE,
		BTN_BASE, BTN_BASE2, BTN_BASE3, BTN_BASE4, BTN_BASE5,
		BTN_BASE6, BTN_A /* BTN_SOUTH */ , BTN_B /* BTN_EAST */ ,
		BTN_C, BTN_X /* BTN_NORTH */ , BTN_Y /* BTN_WEST */ ,
		BTN_Z, BTN_TL, BTN_TR, BTN_TL2, BTN_TR2, BTN_SELECT,
		BTN_START, BTN_MODE, BTN_THUMBL, BTN_THUMBR
	};
	const int count = arrlen(echokeys);
	int i, ret, echoCode = 0;
	struct mcr_Key k;
	mcr_Key_init(&k);
	for (i = 0; i < count; i++) {
		MCR_KEY_SET_ALL(k, echokeys[i], echokeys[i], MCR_DOWN);
		if ((ret = mcr_Echo_set_key(echoCode++, &k)))
			return ret;
		MCR_KEY_SET_UP_TYPE(k, MCR_UP);
		if ((ret = mcr_Echo_set_key(echoCode++, &k)))
			return ret;
	}
	return 0;
}

static int add_echo_names(struct mcr_context *context)
{
	const char *names[] = {
		"0", "1", "2", "3", "4", "5", "6", "7",
		"8", "9", "Left", "Right", "Middle", "Side",
		"Extra", "Forward", "Back", "Task", "Trigger",
		"Thumb", "Thumb2", "Top", "Top2", "Pinkie",
		"Base", "Base2", "Base3", "Base4", "Base5",
		"Base6", "A" /* SOUTH */ , "B" /* EAST */ ,
		"C", "X" /* NORTH */ , "Y" /* WEST */ ,
		"Z", "TL", "TR", "TL2", "TR2", "Select",
		"Start", "Mode", "ThumbL", "ThumbR"
	};
	const char *addNames[] = {
		"Zero", "One", "Two", "Three", "Four",
		"Five", "Six", "Seven",
		"Eight", "Nine", NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL,
		NULL, "SOUTH", "EAST",
		NULL, "NORTH", "WEST",
		NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL
	};
	int err, echoCode = 0;
	size_t count = arrlen(names), i;
	char setname[64];
	for (i = 0; i < count; i++) {
		snprintf(setname, 63, "%s%s", names[i], "Down");
		if ((err = mcr_HidEcho_set_name(context, echoCode, setname)))
			return err;
		if (addNames[i]) {
			snprintf(setname, 63, "%s%s", addNames[i], "Down");
			if ((err = mcr_HidEcho_add(context, echoCode,
						(const char **)&setname, 1)))
				return err;
			snprintf(setname, 63, "%s %s", addNames[i], "Down");
			if ((err = mcr_HidEcho_add(context, echoCode,
						(const char **)&setname, 1)))
				return err;
		}
		snprintf(setname, 63, "%s %s", names[i], "Down");
		if ((err = mcr_HidEcho_add(context, echoCode++,
					(const char **)&setname, 1)))
			return err;

		snprintf(setname, 63, "%s%s", names[i], "Up");
		if ((err = mcr_HidEcho_set_name(context, echoCode, setname)))
			return err;
		if (addNames[i]) {
			snprintf(setname, 63, "%s%s", addNames[i], "Up");
			if ((err = mcr_HidEcho_add(context, echoCode,
						(const char **)&setname, 1)))
				return err;
			snprintf(setname, 63, "%s %s", addNames[i], "Up");
			if ((err = mcr_HidEcho_add(context, echoCode,
						(const char **)&setname, 1)))
				return err;
		}
		snprintf(setname, 63, "%s %s", names[i], "Up");
		if ((err = mcr_HidEcho_add(context, echoCode++,
					(const char **)&setname, 1)))
			return err;
	}
	return 0;
}
