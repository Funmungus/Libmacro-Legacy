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

#ifndef MCR_LNX_STANDARD_H
#define MCR_LNX_STANDARD_H

#include "mcr/standard/lnx/ndevice.h"

/* In order to inline we cannot privatize this. */
extern MCR_API struct mcr_Array mcr_echoEvents;
extern MCR_API struct mcr_Map mcr_keyToEcho[2];
extern MCR_API mcr_SpacePosition mcr_cursor;

struct mcr_HidEcho {
	int event;
};

struct mcr_Key {
	/* scan, key, sync */
	struct input_event events[3];
	enum mcr_KeyUpType up_type;
};

struct mcr_MoveCursor {
	/* x, y, z, sync */
	/* Relative with coordinates. */
	struct input_event relvent[MCR_DIMENSION_CNT + 1];
	/* Absolute with coordinates. */
	struct input_event absvent[MCR_DIMENSION_CNT + 1];
	bool is_justify;
};

struct mcr_Scroll {
	/* x, y, z, sync */
	struct input_event events[MCR_DIMENSION_CNT + 1];
};

MCR_API int mcr_Echo_key(int echoCode);
MCR_API int mcr_Echo_set_key(int echoCode, struct mcr_Key *keyPt);

/* mcr_HidEcho */
#undef MCR_HIDECHO_ECHO
#undef MCR_HIDECHO_SET_ECHO
#define MCR_HIDECHO_ECHO(echo) \
(echo).event
#define MCR_HIDECHO_SET_ECHO(echo, code) \
(echo).event = (code)

/* mcr_Key */
#undef MCR_KEY_KEY
#undef MCR_KEY_SET_KEY
#undef MCR_KEY_SCAN
#undef MCR_KEY_SET_SCAN
#undef MCR_KEY_UP_TYPE
#undef MCR_KEY_SET_UP_TYPE
#define MCR_KEY_KEY(key) \
(key).events[1].code
#define MCR_KEY_SET_KEY(key, keyCode) \
(key).events[1].code = (keyCode)
#define MCR_KEY_SCAN(key) \
(key).events[0].value
#define MCR_KEY_SET_SCAN(key, scan) \
(key).events[0].value = (scan)
#define MCR_KEY_UP_TYPE(key) \
(key).up_type
#define MCR_KEY_SET_UP_TYPE(key, keyUp) \
(key).up_type = (keyUp)

/* mcr_MoveCursor */
#undef MCR_MOVECURSOR_POSITION
#undef MCR_MOVECURSOR_SET_POSITION
#undef MCR_MOVECURSOR_COORDINATE
#undef MCR_MOVECURSOR_SET_COORDINATE
#undef MCR_MOVECURSOR_IS_JUSTIFY
#undef MCR_MOVECURSOR_SET_JUSTIFY
#define MCR_MOVECURSOR_POSITION(mc, buffer) \
(buffer)[MCR_X] = (mc).relvent[MCR_X].value; \
(buffer)[MCR_Y] = (mc).relvent[MCR_Y].value; \
(buffer)[MCR_Z] = (mc).relvent[MCR_Z].value;
#define MCR_MOVECURSOR_SET_POSITION(mc, buffer) \
(mc).relvent[MCR_X].value = \
(mc).absvent[MCR_X].value = (buffer)[MCR_X]; \
(mc).relvent[MCR_Y].value = \
(mc).absvent[MCR_Y].value = (buffer)[MCR_Y]; \
(mc).relvent[MCR_Z].value = \
(mc).absvent[MCR_Z].value = (buffer)[MCR_Z];
#define MCR_MOVECURSOR_COORDINATE(mc, posType) \
(mc).relvent[posType].value
#define MCR_MOVECURSOR_SET_COORDINATE(mc, posType, val) \
(mc).relvent[posType].value = \
(mc).absvent[posType].value = val;
#define MCR_MOVECURSOR_IS_JUSTIFY(mc) \
(mc).is_justify
#define MCR_MOVECURSOR_SET_JUSTIFY(mc, enable) \
(mc).is_justify = (enable)

/* mcr_Scroll */
#undef MCR_SCROLL_DIMENSIONS
#undef MCR_SCROLL_SET_DIMENSIONS
#undef MCR_SCROLL_COORDINATE
#undef MCR_SCROLL_SET_COORDINATE
#define MCR_SCROLL_DIMENSIONS(scroll, buffer) \
(buffer)[MCR_X] = (scroll).events[MCR_X].value; \
(buffer)[MCR_Y] = (scroll).events[MCR_Y].value; \
(buffer)[MCR_Z] = (scroll).events[MCR_Z].value;
#define MCR_SCROLL_SET_DIMENSIONS(scroll, buffer) \
(scroll).events[MCR_X].value = (buffer)[MCR_X]; \
(scroll).events[MCR_Y].value = (buffer)[MCR_Y]; \
(scroll).events[MCR_Z].value = (buffer)[MCR_Z];
#define MCR_SCROLL_COORDINATE(scroll, pos) \
(scroll).events[pos].value
#define MCR_SCROLL_SET_COORDINATE(scroll, pos, val) \
(scroll).events[pos].value = val

#endif
