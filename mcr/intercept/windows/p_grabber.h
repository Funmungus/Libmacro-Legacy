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
 *  \brief Hooks are for intercepting events in Windows.
 */

#ifndef MCR_INTERCEPT_WIN_NGRABBER_H_
#define MCR_INTERCEPT_WIN_NGRABBER_H_

#include "mcr/intercept/windows/p_def.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_Grabber {
	DWORD dwThread;
	HMODULE hModule;
	HANDLE hMutex;
	HANDLE hThread;
	HHOOK id;
	HOOKPROC proc;
	int type;
};

MCR_API int mcr_Grabber_init(void *grabDataPt);
MCR_API int mcr_Grabber_deinit(void *grabDataPt);
MCR_API int mcr_Grabber_set_all(struct mcr_Grabber *grabPt, int type,
								HOOKPROC proc);
MCR_API bool mcr_Grabber_is_enabled(struct mcr_Grabber *grabPt);
MCR_API int mcr_Grabber_set_enabled(struct mcr_Grabber *grabPt, bool enable);

#define MCR_GRABBER_IS_ENABLED(grab) \
((grab).id != mcr_null)

#ifdef __cplusplus
}
#endif

#endif
