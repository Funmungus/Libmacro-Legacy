/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
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

# ifndef _GNU_SOURCE
# define _GNU_SOURCE
# endif

# include "extras/lnx/def.h"
# include "extras/extras.h"

void mcr_extras_native_initialize ( )
{
	ERR_load_CRYPTO_strings ( ) ;
	OpenSSL_add_all_ciphers ( ) ;
	OPENSSL_config ( NULL ) ;

	RAND_poll ( ) ;
}

void mcr_extras_native_cleanup ( void )
{
	EVP_cleanup ( ) ;
	CRYPTO_cleanup_all_ex_data ( ) ;
	ERR_free_strings ( ) ;

	RAND_cleanup ( ) ;
}
