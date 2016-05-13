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

#include "mcr/extras/win/nextras.h"
#include "mcr/extras/extras.h"
#include <errno.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

int mcr_extras_native_initialize(struct mcr_context *ctx)
{
	int err = 0;
	ERR_load_CRYPTO_strings();
	OpenSSL_add_all_ciphers();
	OPENSSL_config(NULL);

	if (RAND_poll() != 1) {
		ddo(ERR_print_errors_fp(stderr));
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
		return err;
	}
	return 0;
}

void mcr_extras_native_cleanup(struct mcr_context *ctx)
{
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();

	RAND_cleanup();
}
