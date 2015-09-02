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

# include "extras/lnx/def.h"
# include "extras/safestring.h"

void mcr_randomize ( unsigned char * buffer,
		int bufferSize )
{
	if ( RAND_bytes ( buffer, bufferSize ) != 1 )
	{
		dmsg ;
	}
}

# define handleErrors( ctx ) \
{ \
	dmsg ; \
	EVP_CIPHER_CTX_free ( ctx ) ; \
	return -1 ; \
}
//
// The following is from example evp encrypt/decrypt found at
// http://wiki.openssl.org/index.php/CCM
// 2015
int mcr_encrypt ( unsigned char * plaintext, int plaintext_len,
		unsigned char * aad, int aad_len, unsigned char * key,
		unsigned char * iv, unsigned char * ciphertext,
		unsigned char * tag )
{
	EVP_CIPHER_CTX * ctx ;
	int len ;
	int ciphertext_len ;

	if ( ! ( ctx = EVP_CIPHER_CTX_new ( ) ) )
	{
		return 0 ;
	}

	if ( 1 != EVP_EncryptInit_ex ( ctx, EVP_aes_256_gcm ( ), NULL, NULL,
			NULL ) )
		handleErrors ( ctx ) ;

	/* Set IV length if default 12 bytes ( 96 bits ) is not appropriate */
	if ( 1 != EVP_CIPHER_CTX_ctrl ( ctx, EVP_CTRL_GCM_SET_IVLEN, 16,
			NULL ) )
		handleErrors ( ctx ) ;

	/* Initialise key and IV */
	if ( 1 != EVP_EncryptInit_ex ( ctx, NULL, NULL, key, iv ) )
		handleErrors ( ctx ) ;

	if ( aad && aad_len )
	{
		/* Provide any AAD data. This can be called zero or more times as
		 * required
		 */
		if ( 1 != EVP_EncryptUpdate ( ctx, NULL, & len, aad, aad_len ) )
			handleErrors ( ctx ) ;
	}

	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if ( 1 != EVP_EncryptUpdate ( ctx, ciphertext, & len, plaintext,
			plaintext_len ) )
		handleErrors ( ctx ) ;
	ciphertext_len = len ;

	/* Finalise the encryption. Normally ciphertext bytes may be written at
	 * this stage, but this does not occur in GCM mode
	 */
	if ( 1 != EVP_EncryptFinal_ex ( ctx, ciphertext + len, & len ) )
		handleErrors ( ctx ) ;
	ciphertext_len += len ;

	/* Get the tag */
	if ( 1 != EVP_CIPHER_CTX_ctrl ( ctx, EVP_CTRL_GCM_GET_TAG, 16, tag ) )
		handleErrors ( ctx ) ;

	/* Clean up */
	EVP_CIPHER_CTX_free ( ctx ) ;

	return ciphertext_len ;
}

int mcr_decrypt ( const unsigned char * ciphertext,
		const int ciphertext_len, const unsigned char * aad,
		const int aad_len, const unsigned char * tag,
		const unsigned char * key, const unsigned char * iv,
		unsigned char * plaintext )
{
	EVP_CIPHER_CTX * ctx ;
	int len ;
	int plaintext_len ;
	int ret ;

	/* Create and initialise the context */
	if ( ! ( ctx = EVP_CIPHER_CTX_new ( ) ) ) handleErrors ( ctx ) ;

	/* Initialise the decryption operation. */
	if ( ! EVP_DecryptInit_ex ( ctx, EVP_aes_256_gcm ( ), NULL,
			NULL, NULL ) )
		handleErrors ( ctx ) ;

	/* Set IV length. Not necessary if this is 12 bytes ( 96 bits ) */
	if ( ! EVP_CIPHER_CTX_ctrl ( ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL ) )
		handleErrors ( ctx ) ;

	/* Initialise key and IV */
	if ( ! EVP_DecryptInit_ex ( ctx, NULL, NULL, key, iv ) )
		handleErrors ( ctx ) ;

	if ( aad && aad_len )
	{
		/* Provide any AAD data. This can be called zero or more times as
		 * required
		 */
		if ( ! EVP_DecryptUpdate ( ctx, NULL, & len, aad, aad_len ) )
			handleErrors ( ctx ) ;
	}

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if ( ! EVP_DecryptUpdate ( ctx, plaintext, & len, ciphertext,
			ciphertext_len ) )
		handleErrors ( ctx ) ;
	plaintext_len = len ;

	/* Set expected tag value. Works in OpenSSL 1.0.1d and later */
	if ( ! EVP_CIPHER_CTX_ctrl ( ctx, EVP_CTRL_GCM_SET_TAG, 16,
			( void * ) tag ) )
		handleErrors ( ctx ) ;

	/* Finalise the decryption. A positive return value indicates success,
	 * anything else is a failure - the plaintext is not trustworthy.
	 */
	ret = EVP_DecryptFinal_ex ( ctx, plaintext + len, & len ) ;

	/* Clean up */
	EVP_CIPHER_CTX_free ( ctx ) ;

	if ( ret > 0 )
	{
		/* Success */
		plaintext_len += len ;
		return plaintext_len ;
	}
	else
	{
		/* Verify failed */
		return -1 ;
	}
}
