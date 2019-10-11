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

#include "mcr/extras/extras.h"

#include <cerrno>
#include <cstdio>

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace mcr
{
static unsigned int _initCount = 0;
[[noreturn]] static inline void localOnErr(EVP_CIPHER_CTX *ctx = nullptr,
		unsigned char *deleteBuffer = nullptr)
{
	mcr_errno(EINTR);
	/* Windows may crash with "no OPENSSL_Applink" */
#ifndef WIN32
	ERR_print_errors_fp(stderr);
#endif
	if (ctx)
		EVP_CIPHER_CTX_free(ctx);
	if (deleteBuffer)
		delete []deleteBuffer;
	throw mcr_err;
}

int SafeString::encrypt(const char *plainText, size_t pLen,
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
	}
	if (tagOut) {
		if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr,
								nullptr)) {
			localOnErr(ctx);
		}
		/* Set IV length 16 bytes */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
								 MCR_AES_IV_SIZE, nullptr)) {
			localOnErr(ctx);
		}
	} else {
		if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, nullptr,
								nullptr)) {
			localOnErr(ctx);
		}
		/* A 128 bit IV, cannot be changed, 16 byte */
	}
	/* init key + iv */
	if (!EVP_EncryptInit_ex(ctx, nullptr, nullptr, key, iv)) {
		localOnErr(ctx);
	}
	/* Encrypt */
	if (!EVP_EncryptUpdate(ctx, bufferOut, &len,
						   reinterpret_cast<const unsigned char *>(plainText),
						   static_cast<int>(pLen) + 1)) {
		localOnErr(ctx);
	}
	cipherLen = len;
	/* Finalise the encryption. */
	if (!EVP_EncryptFinal_ex(ctx, bufferOut + len, &len)) {
		localOnErr(ctx);
	}
	if (len)
		cipherLen += len;

	if (tagOut) {
		/* If using gcm, get the tag */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG,
								 MCR_AES_TAG_SIZE, tagOut)) {
			localOnErr(ctx);
		}
	}
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return cipherLen;
}

size_t SafeString::decrypt(const unsigned char *encrypted,
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
		bufferOut[0] = '\0';
		localOnErr();
	}
	if (tag) {
		if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr,
								nullptr)) {
			bufferOut[0] = '\0';
			localOnErr(ctx);
		}
		/* Set IV length 16 bytes */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
								 MCR_AES_IV_SIZE, nullptr)) {
			bufferOut[0] = '\0';
			localOnErr(ctx);
		}
	} else {
		if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, nullptr,
								nullptr)) {
			bufferOut[0] = '\0';
			localOnErr(ctx);
		}
		/* A 128 bit IV, cannot be changed, 16 byte */
	}
	/* init key + iv */
	if (!EVP_DecryptInit_ex(ctx, nullptr, nullptr, key, iv)) {
		bufferOut[0] = '\0';
		localOnErr(ctx);
	}
	/* Decrypt */
	if (!EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(bufferOut), &len,
						   encrypted,
						   encryptedLength)) {
		bufferOut[0] = '\0';
		localOnErr(ctx);
	}
	if (tag) {
		/* If using gcm, set the encryption-paired tag */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG,
								 MCR_AES_TAG_SIZE, const_cast<void *>(static_cast<const void *>(tag)))) {
			bufferOut[0] = '\0';
			localOnErr(ctx);
		}
	}
	/* Finalise the decryption */
	if (!EVP_DecryptFinal_ex(ctx,
							 reinterpret_cast<unsigned char *>(bufferOut + len), &len)) {
		bufferOut[0] = '\0';
		localOnErr(ctx);
	}
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return static_cast<size_t>(len);
}

void SafeString::randomize(unsigned char *bufferOut,
						   int bufferSize)
{
	if (!RAND_bytes(bufferOut, bufferSize)) {
		localOnErr();
	}
}

void SafeString::sha(const char *text, size_t length,
					 unsigned char bufferOut[])
{
	SHA256_CTX ctx;
	if (!text) {
		text = "";
		length = 0;
	}
	if (!SHA256_Init(&ctx)) {
		localOnErr();
	}
	if (!SHA256_Update(&ctx, text, length)) {
		localOnErr();
	}
	if (!SHA256_Final(bufferOut, &ctx))
		localOnErr();
}

void SafeString::initialize()
{
	if (!_initCount++) {
		OPENSSL_init_crypto(OPENSSL_INIT_NO_ADD_ALL_CIPHERS | OPENSSL_INIT_NO_ADD_ALL_DIGESTS, nullptr);
//		OPENSSL_config(nullptr);
		ERR_load_CRYPTO_strings();
//		OPENSSL_add_all_algorithms_noconf();
		if (!EVP_add_cipher(EVP_aes_256_cbc()) ||
			!EVP_add_cipher(EVP_aes_256_gcm())) {
			localOnErr();
		}
		if (!EVP_add_digest(EVP_sha256())) {
			localOnErr();
		}

		if (!RAND_poll()) {
			localOnErr();
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
