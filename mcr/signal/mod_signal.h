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

/*! \file
 * \brief \ref mcr_mod_signal
 */

#ifndef MCR_MOD_SIGNAL_H
#define MCR_MOD_SIGNAL_H

#include "mcr/util/util.h"

struct mcr_ISignal;
struct mcr_Dispatcher;
struct mcr_mod_signal {
	struct mcr_IRegistry isignals;
	struct mcr_Array dispatchers;
	bool dispatcher_generic_enabled;
	struct mcr_Dispatcher *dispatcher_generic_pt;
	mcr_NameBiMap mod_names;
};

#endif
