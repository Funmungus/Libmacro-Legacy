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

#include "mcr/extras/lnx/nextras.h"
#include "mcr/extras/extras.h"
#include <errno.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>

int mcr_randomize(unsigned char *buffer, int bufferSize)
{
	int err = 0;
	if (RAND_bytes(buffer, bufferSize) != 1) {
		ddo(ERR_print_errors_fp(stderr));
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
		return err;
	}
	return 0;
}

#define handleErrors(ctx) { \
	ddo(ERR_print_errors_fp(stderr)); \
	EVP_CIPHER_CTX_free(ctx); \
	return -1; \
}
/*
 * The following is from example evp encrypt/decrypt found at
 * http://wiki.openssl.org/index.php/CCM
 * 2015
 */
int mcr_encrypt(const unsigned char *plaintext, int plaintext_len,
		const unsigned char *aad, int aad_len, const unsigned char *key,
		const unsigned char *iv, unsigned char *tag,
		unsigned char *ciphertextOut)
{
	EVP_CIPHER_CTX *ctx;
	int len;
	int ciphertext_len;

	if (!(ctx = EVP_CIPHER_CTX_new()))
		handleErrors(ctx);
	if (tag) {
		if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL,
					    NULL))
			handleErrors(ctx);
		/* Set IV length 16 bytes */
		if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
					     MCR_AES_IV_SIZE, NULL))
			handleErrors(ctx);
	} else {
		if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL,
					    NULL))
			handleErrors(ctx);
		/* A 128 bit IV, cannot be changed, 16 byte */
	}
	/* init key + iv */
	if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
		handleErrors(ctx);
	/* plain data aad */
	if (aad && aad_len) {
		if (1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
			handleErrors(ctx);
	}
	/* Encrypt */
	if (1 != EVP_EncryptUpdate(ctx, ciphertextOut, &len, plaintext,
				   plaintext_len))
		handleErrors(ctx);
	ciphertext_len = len;
	/* Finalise the encryption. */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertextOut + len, &len))
		handleErrors(ctx);
	ciphertext_len += len;

	if (tag) {
		/* If using gcm, get the tag */
		if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG,
					     MCR_AES_TAG_SIZE, tag))
			handleErrors(ctx);
	}
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return ciphertext_len;
}

int mcr_decrypt(const unsigned char *ciphertext,
		const int ciphertext_len, const unsigned char *aad,
		const int aad_len, const unsigned char *key,
		const unsigned char *iv, const unsigned char *tag,
		unsigned char *plaintextOut)
{
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;
	int err;

	if (!(ctx = EVP_CIPHER_CTX_new()))
		handleErrors(ctx);
	if (tag) {
		if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL,
					    NULL))
			handleErrors(ctx);
		/* Set IV length 16 bytes */
		if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
					     MCR_AES_IV_SIZE, NULL))
			handleErrors(ctx);
	} else {
		if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL,
					    NULL))
			handleErrors(ctx);
		/* A 128 bit IV, cannot be changed, 16 byte */
	}
	/* init key + iv */
	if (1 != EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
		handleErrors(ctx);
	/* plain data aad */
	if (aad && aad_len) {
		if (1 != EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len))
			handleErrors(ctx);
	}
	/* Decrypt */
	if (!EVP_DecryptUpdate(ctx, plaintextOut, &len, ciphertext,
			       ciphertext_len))
		handleErrors(ctx);
	plaintext_len = len;
	if (tag) {
		/* If using gcm, set the encryption-paired tag */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG,
					 MCR_AES_TAG_SIZE, (void *)tag))
			handleErrors(ctx);
	}
	/* Finalise the decryption */
	err = EVP_DecryptFinal_ex(ctx, plaintextOut + len, &len);
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	if (err > 0) {
		/* Success */
		plaintext_len += len;
		return plaintext_len;
	} else {
		/* Verify failed */
		ddo(ERR_print_errors_fp(stderr));
		return -1;
	}
}
