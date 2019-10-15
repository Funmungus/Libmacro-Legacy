/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

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

#include "mcr/signal/signal.h"

/* Init mcr_Array of mcr_DispatchPair */
static int mcr_Array_DispatchPair_init(void *arrPt);
static const struct mcr_Interface _MCR_ARR_DISPATCHPAIR_IFACE = {
	.id = (size_t)-1,
	/* 0 compare, copy */
	.data_size = sizeof(struct mcr_Array),
	.init = mcr_Array_DispatchPair_init,
	.deinit = mcr_Array_deinit
};

struct mcr_DispatchPair mcr_DispatchPair_new(void *receiver,
		mcr_Dispatcher_receive_fnc dispatch)
{
	struct mcr_DispatchPair ret = {
		.receiver = receiver,
		.dispatch = dispatch
	};
	return ret;
}

const struct mcr_Interface *mcr_Array_DispatchPair_interface()
{
	return &_MCR_ARR_DISPATCHPAIR_IFACE;
}

static int mcr_Array_DispatchPair_init(void *arrPt)
{
	mcr_Array_init(arrPt);
	mcr_Array_set_all(arrPt, mcr_DispatchPair_compare,
					  sizeof(struct mcr_DispatchPair));
	return 0;
}
