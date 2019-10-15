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

/*! \file
 *  \brief \ref mcr_GenericDispatcher - Dispatcher of any signal type
 */

#ifndef MCR_SIGNAL_GENERIC_DISPATCHER_H_
#define MCR_SIGNAL_GENERIC_DISPATCHER_H_

#include "mcr/signal/dispatcher.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Dispatcher of any signal type
 *
 *  In cases of extreme complexity please break glass.
 */
struct mcr_GenericDispatcher {
	struct mcr_Dispatcher dispatcher;
	struct mcr_Array receivers;
	struct mcr_Map signal_receivers;
};

/*!
 */
MCR_API int mcr_GenericDispatcher_init(void *genericDispatcherPt);
/*!
 */
MCR_API int mcr_GenericDispatcher_deinit(void *genericDispatcherPt);
/*!
 */
MCR_API int mcr_GenericDispatcher_trim(void *genericDispatcherPt);

#ifdef __cplusplus
}
#endif

#endif
