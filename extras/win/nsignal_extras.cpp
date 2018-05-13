/* Libmacro - A multi-platform, extendable macro and hotkey C library
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

#include <cstring>
#include "mcr/extras/win/nextras.h"
#include "mcr/extras/mod_extras.h"

#define winerr dprint("Windows Error: %ld\n", GetLastError());

namespace mcr
{
void Command::send()
{
	int err = 0;
	size_type i;
	string cmdline, mem;
	STARTUPINFOA sInfo;
	PROCESS_INFORMATION pInfo;
	if (!file.length()) {
		mcr_err = ENOENT;
		throw(ENOENT);
	}
	mcr_err = 0;
	std::memset(&sInfo, 0, sizeof(sInfo));
	std::memset(&pInfo, 0, sizeof(pInfo));
	cmdline = "\"" + file.text() + "\"";
	for (i = 0; i < argsCount(); i++) {
		mem = _args[i].text();
		if (!mem.empty())
			cmdline.append(" \"" + mem + '"');
	}
	GetStartupInfoA(&sInfo);
	/* Replace with CreateThread + CreateProcess if CreateProcess does not work */
	/* Create as user, lower securty rights */
	fixme;
	if (!CreateProcessA(NULL, bytes(cmdline), NULL, NULL, FALSE,
						PROFILE_USER | DETACHED_PROCESS | NORMAL_PRIORITY_CLASS,
						NULL, NULL, &sInfo, &pInfo)) {
		winerr;
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
	} else if (WaitForSingleObject(pInfo.hProcess,
								   MCR_INTERCEPT_WAIT_MILLIS) != WAIT_OBJECT_0) {
		winerr;
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
	}
	if (err) {
		if (!CloseHandle(pInfo.hThread) || !CloseHandle(pInfo.hProcess)) {
			winerr;
		}
		throw err;
	}
}
}
