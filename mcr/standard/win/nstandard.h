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

#ifndef MCR_WIN_STANDARD_H
#define MCR_WIN_STANDARD_H

#include "mcr/standard/win/ndef.h"

struct mcr_HidEcho {
	int event;
};

struct mcr_Key {
	int key;
	int scan;
	enum mcr_KeyUpType up_type;
};

struct mcr_MoveCursor {
	mcr_SpacePosition pos;
	int is_justify;
};

struct mcr_Scroll {
	mcr_Dimensions dm;
};

MCR_API int mcr_Echo_mouse_flag(int echoCode);
MCR_API int mcr_Echo_set_mouse_flag(int echoCode, int mouseEventFlags);

#undef MCR_HIDECHO_ECHO
#undef MCR_HIDECHO_SET_ECHO
#define MCR_HIDECHO_ECHO(echo) \
(echo).event
#define MCR_HIDECHO_SET_ECHO(echo, code) \
(echo).event = (code)

#undef MCR_KEY_KEY
#undef MCR_KEY_SET_KEY
#undef MCR_KEY_SCAN
#undef MCR_KEY_SET_SCAN
#undef MCR_KEY_UP_TYPE
#undef MCR_KEY_SET_UP_TYPE
#define MCR_KEY_KEY(keyObj) \
(keyObj).key
#define MCR_KEY_SET_KEY(keyObj, keyCode) \
(keyObj).key = (keyCode)
#define MCR_KEY_SCAN(keyObj) \
(keyObj).scan
#define MCR_KEY_SET_SCAN(keyObj, scanCode) \
(keyObj).scan = (scanCode)
#define MCR_KEY_UP_TYPE(keyObj) \
(keyObj).up_type
#define MCR_KEY_SET_UP_TYPE(keyObj, upType) \
(keyObj).up_type = (upType)

#undef MCR_MOVECURSOR_POSITION
#undef MCR_MOVECURSOR_SET_POSITION
#undef MCR_MOVECURSOR_COORDINATE
#undef MCR_MOVECURSOR_SET_COORDINATE
#undef MCR_MOVECURSOR_JUSTIFY
#undef MCR_MOVECURSOR_SET_JUSTIFY
#define MCR_MOVECURSOR_POSITION(mc, buffer) \
(buffer)[MCR_X] = (mc).pos[MCR_X]; \
(buffer)[MCR_Y] = (mc).pos[MCR_Y]; \
(buffer)[MCR_Z] = (mc).pos[MCR_Z];
#define MCR_MOVECURSOR_SET_POSITION(mc, buffer) \
(mc).pos[MCR_X] = (buffer)[MCR_X]; \
(mc).pos[MCR_Y] = (buffer)[MCR_Y]; \
(mc).pos[MCR_Z] = (buffer)[MCR_Z];
#define MCR_MOVECURSOR_COORDINATE(mc, _pos_) \
(mc).pos[_pos_]
#define MCR_MOVECURSOR_SET_COORDINATE(mc, _pos_, value) \
(mc).pos[_pos_] = value;
#define MCR_MOVECURSOR_IS_JUSTIFY(mc) \
(mc).is_justify
#define MCR_MOVECURSOR_SET_JUSTIFY(mc, enable) \
(mc).is_justify = enable

#undef MCR_SCROLL_DIMENSIONS
#undef MCR_SCROLL_SET_DIMENSIONS
#undef MCR_SCROLL_COORDINATE
#undef MCR_SCROLL_SET_COORDINATE
#define MCR_SCROLL_DIMENSIONS(scroll, buffer) \
(buffer)[MCR_X] = (scroll).dm[MCR_X]; \
(buffer)[MCR_Y] = (scroll).dm[MCR_Y]; \
(buffer)[MCR_Z] = (scroll).dm[MCR_Z];
#define MCR_SCROLL_SET_DIMENSIONS(scroll, buffer) \
(scroll).dm[MCR_X] = (buffer)[MCR_X]; \
(scroll).dm[MCR_Y] = (buffer)[MCR_Y]; \
(scroll).dm[MCR_Z] = (buffer)[MCR_Z];
#define MCR_SCROLL_COORDINATE(scroll, pos) \
(scroll).dm[pos]
#define MCR_SCROLL_SET_COORDINATE(scroll, pos, val) \
(scroll).dm[pos] = val

#endif
