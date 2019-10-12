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

#include "mcr/extras/windows/p_extras.h"
#include "mcr/extras/extras.h"

#include <cstring>

#define winerr dprint("Windows Error: %ld\n", GetLastError());
#define zero(obj) std::memset(&(obj), 0, sizeof(obj))

namespace mcr
{
void Command::send()
{
	std::string cmdline, mem;
	STARTUPINFOA sInfo;
	PROCESS_INFORMATION pInfo;
	if (!_file.length()) {
		mset_error(ENOENT);
		throw ENOENT;
	}
	mcr_err = 0;
	zero(sInfo);
	zero(pInfo);
	cmdline = "\"" + _file.text() + "\"";
	for (auto &i: argsRef()) {
		mem = i.text();
		if (mem.length())
			cmdline.append(" \"" + mem + '"');
	}
	GetStartupInfoA(&sInfo);
	/* Replace with CreateThread + CreateProcess if CreateProcess does not work */
	/* Create as user, lower securty rights */
	/// \bug Executable with lower access rights, or create process as user
	if (!CreateProcessA(nullptr, &cmdline.front(), nullptr,
						nullptr, FALSE,
						PROFILE_USER | DETACHED_PROCESS | NORMAL_PRIORITY_CLASS,
						nullptr, nullptr, &sInfo, &pInfo)) {
		winerr;
		mcr_errno(EINTR);
	} else if (WaitForSingleObject(pInfo.hProcess,
								   MCR_INTERCEPT_WAIT_MILLIS)/* != WAIT_OBJECT_0*/) {
		winerr;
		mcr_errno(EINTR);
	}
	if (mcr_err) {
		if (!CloseHandle(pInfo.hThread) || !CloseHandle(pInfo.hProcess)) {
			winerr;
		}
		throw mcr_err;
	}
}
}
