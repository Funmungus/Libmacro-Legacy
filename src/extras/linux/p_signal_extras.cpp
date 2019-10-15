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

#include "mcr/extras/linux/p_extras.h"
#include "mcr/extras/extras.h"

extern "C" {
#include <sys/wait.h>
}

namespace mcr
{
void Command::send()
{
	pid_t child, child2;
	std::string f = file();
	std::vector<std::string> args = this->args();
	/* file + all args + NULL ending */
	size_t i, argsLen = args.size() + 2;
	int err;
	char **strArgs;
	if (!_file.length()) {
		mset_error(ENOENT);
		throw ENOENT;
	}
	mcr_err = 0;
	strArgs = new char *[argsLen];
	strArgs[0] = &f.front();
	for (i = 0; i < args.size(); i++) {
		strArgs[i + 1] = &args[i].front();
	}
	strArgs[argsLen - 1] = nullptr;
	child = fork();
	if (child == -1) {
		mcr_errno(EINTR);
		throw mcr_err;
	} else if (child) {
		if (waitpid(child, NULL, 0) == -1) {
			mcr_errno(EINTR);
			throw mcr_err;
		}
	} else {
		/* child pid = 0, this is the child */
		if ((err = mcr_privilege_deactivate()))
			_exit(err);
		if ((child2 = fork()) == -1) {
			mcr_errno(EINTR);
			_exit(err);
		} else if (child2) {
			_exit(EXIT_SUCCESS);
		} else {
			/* Execution child
			 * Do not exit when parent does */
			signal(SIGHUP, SIG_IGN);
			execvp(strArgs[0], strArgs);
			mcr_errno(EINTR);
			/* Continued execution is strange */
		}
		/* Ensured exit for children, we do not want hanging processes */
		_exit(err);
	}
	delete []strArgs;
}
}
