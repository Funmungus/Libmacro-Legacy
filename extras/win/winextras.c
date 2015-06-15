/* extras/win/winextras.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "extras/win/def.h"
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
