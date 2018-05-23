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

#include "mcr/extras/extras.h"

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace mcr
{
static unsigned int _initCount = 0;
static inline void localOnErr(EVP_CIPHER_CTX *ctx = NULL,
							  unsigned char *deleteBuffer = NULL)
{
	int err = errno;
	if (!err)
		err = EINTR;
	mset_error(err);
	/* Windows may crash with "no OPENSSL_Applink" */
#ifndef WIN32
	ERR_print_errors_fp(stderr);
#endif
	if (ctx)
		EVP_CIPHER_CTX_free(ctx);
	if (deleteBuffer)
		delete []deleteBuffer;
	throw err;
}

int SafeString::encrypt(const char *plainText, size_type pLen,
						const unsigned char key[],
						const unsigned char iv[], unsigned char tagOut[],
						unsigned char *bufferOut)
{
	EVP_CIPHER_CTX *ctx;
	int len = 0;
	int cipherLen = 0;
	if (!bufferOut)
		return 0;
	if (!pLen || !plainText || plainText[0] == '\0') {
		bufferOut[0] = '\0';
		return 0;
	}

	if (!(ctx = EVP_CIPHER_CTX_new())) {
		localOnErr();
		return -1;
	}
	if (tagOut) {
		if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL,
								NULL)) {
			localOnErr(ctx);
			return -1;
		}
		/* Set IV length 16 bytes */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
								 MCR_AES_IV_SIZE, NULL)) {
			localOnErr(ctx);
			return -1;
		}
	} else {
		if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL,
								NULL)) {
			localOnErr(ctx);
			return -1;
		}
		/* A 128 bit IV, cannot be changed, 16 byte */
	}
	/* init key + iv */
	if (!EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)) {
		localOnErr(ctx);
		return -1;
	}
	/* Encrypt */
	if (!EVP_EncryptUpdate(ctx, bufferOut, &len,
						   (const unsigned char *)bytes(plainText),
						   ((int)pLen) + 1)) {
		localOnErr(ctx);
		return -1;
	}
	cipherLen = len;
	/* Finalise the encryption. */
	if (!EVP_EncryptFinal_ex(ctx, bufferOut + len, &len)) {
		localOnErr(ctx);
		return -1;
	}
	if (len)
		cipherLen += len;

	if (tagOut) {
		/* If using gcm, get the tag */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG,
								 MCR_AES_TAG_SIZE, tagOut)) {
			localOnErr(ctx);
			return -1;
		}
	}
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return cipherLen;
}

size_type SafeString::decrypt(const unsigned char *encrypted,
							  int encryptedLength,
							  const unsigned char key[], const unsigned char iv[],
							  const unsigned char tag[], char *bufferOut)
{
	EVP_CIPHER_CTX *ctx;
	int len = 0;

	if (!bufferOut)
		return 0;
	if (!encrypted || !encryptedLength) {
		bufferOut[0] = '\0';
		return 0;
	}

	if (!(ctx = EVP_CIPHER_CTX_new())) {
		localOnErr();
		bufferOut[0] = '\0';
		return 0;
	}
	if (tag) {
		if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL,
								NULL)) {
			localOnErr(ctx);
			bufferOut[0] = '\0';
			return 0;
		}
		/* Set IV length 16 bytes */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
								 MCR_AES_IV_SIZE, NULL)) {
			localOnErr(ctx);
			bufferOut[0] = '\0';
			return 0;
		}
	} else {
		if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL,
								NULL)) {
			localOnErr(ctx);
			bufferOut[0] = '\0';
			return 0;
		}
		/* A 128 bit IV, cannot be changed, 16 byte */
	}
	/* init key + iv */
	if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
		localOnErr(ctx);
		bufferOut[0] = '\0';
		return 0;
	}
	/* Decrypt */
	if (!EVP_DecryptUpdate(ctx, (unsigned char *)bufferOut, &len, encrypted,
						   encryptedLength)) {
		localOnErr(ctx);
		bufferOut[0] = '\0';
		return 0;
	}
	if (tag) {
		/* If using gcm, set the encryption-paired tag */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG,
								 MCR_AES_TAG_SIZE, (void *)tag)) {
			localOnErr(ctx);
			bufferOut[0] = '\0';
			return 0;
		}
	}
	/* Finalise the decryption */
	if (!EVP_DecryptFinal_ex(ctx, (unsigned char *)(bufferOut + len), &len)) {
		localOnErr(ctx);
		bufferOut[0] = '\0';
		return 0;
	}
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return len;
}

void SafeString::randomize(unsigned char *bufferOut,
						   int bufferSize)
{
	if (!RAND_bytes(bufferOut, bufferSize)) {
		localOnErr();
		return;
	}
}

void SafeString::sha(const char *text, size_type length,
					 unsigned char bufferOut[])
{
	SHA256_CTX ctx;
	if (!text) {
		text = "";
		length = 0;
	}
	if (!SHA256_Init(&ctx)) {
		localOnErr();
		return;
	}
	if (!SHA256_Update(&ctx, text, length)) {
		localOnErr();
		return;
	}
	if (!SHA256_Final(bufferOut, &ctx)) {
		localOnErr();
	}
}

void SafeString::initialize()
{
	if (!_initCount++) {
		OPENSSL_no_config();
		ERR_load_CRYPTO_strings();
//		OPENSSL_add_all_algorithms_noconf();
		if (!EVP_add_cipher(EVP_aes_256_cbc()) ||
			!EVP_add_cipher(EVP_aes_256_gcm())) {
			localOnErr();
			return;
		}
		if (!EVP_add_digest(EVP_sha256())) {
			localOnErr();
			return;
		}

		if (!RAND_poll()) {
			localOnErr();
			return;
		}
	}
}

void SafeString::deinitialize()
{
	if (!--_initCount) {
//		ERR_free_strings();
//		FIPS_mode_set(0);
//		CONF_modules_unload(1);
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		ERR_free_strings();
	}
}
}
