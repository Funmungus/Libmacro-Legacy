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

#include "mcr/signal/signal.h"

/* Init mcr_Array of mcr_DispatchPair */
static void mcr_Array_DispatchPair_init(void *dataPt);
static const struct mcr_Interface _MCR_ARR_DISPATCHPAIR_IFACE = {
	.id = ~0,
	/* 0 compare, copy */
	.data_size = sizeof(struct mcr_Array),
	.init = mcr_Array_DispatchPair_init,
	.free = mcr_Array_free
};

const struct mcr_Interface *mcr_Array_DispatchPair_iface()
{
	return &_MCR_ARR_DISPATCHPAIR_IFACE;
}

static void mcr_Array_DispatchPair_init(void *dataPt)
{
	mcr_Array_init(dataPt);
	mcr_Array_set_all(dataPt, mcr_DispatchPair_compare,
		sizeof(struct mcr_DispatchPair));
}
