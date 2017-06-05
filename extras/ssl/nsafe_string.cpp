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
#include <cstring>
#include <cstdio>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace mcr
{
static unsigned int _initCount = 0;
static inline void localOnErr(EVP_CIPHER_CTX *ctx = NULL,
			      unsigned char *deleteBuffer = NULL) throw(int)
{
	int err = errno;
	if (!err)
		err = EINTR;
	mset_error(err);
	ERR_print_errors_fp(stderr);
	if (ctx)
		EVP_CIPHER_CTX_free(ctx);
	if (deleteBuffer)
		delete []deleteBuffer;
	throw err;
}

int SafeString::encrypt(const string &plainText, const unsigned char key[],
			const unsigned char iv[], unsigned char tagOut[],
			unsigned char *bufferOut) throw(int)
{
	EVP_CIPHER_CTX *ctx;
	int len = 0;
	int cipherLen = 0;
	if (!plainText.size() || plainText[0] == '\0') {
		if (bufferOut)
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
				   plainText.size() + 1)) {
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

string SafeString::decrypt(const unsigned char *encrypted, int encryptedLength,
			   const unsigned char key[], const unsigned char iv[],
			   const unsigned char tag[]) throw(int)
{
	EVP_CIPHER_CTX *ctx;
	int len = 0;
	string ret;

	if (!encrypted || !encryptedLength)
		return "";

	unsigned char *buffer = new unsigned char[encryptedLength];

	if (!(ctx = EVP_CIPHER_CTX_new())) {
		localOnErr(NULL, buffer);
		return "";
	}
	if (tag) {
		if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL,
					    NULL)) {
			localOnErr(ctx, buffer);
			return "";
		}
		/* Set IV length 16 bytes */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
					     MCR_AES_IV_SIZE, NULL)) {
			localOnErr(ctx, buffer);
			return "";
		}
	} else {
		if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL,
					    NULL)) {
			localOnErr(ctx, buffer);
			return "";
		}
		/* A 128 bit IV, cannot be changed, 16 byte */
	}
	/* init key + iv */
	if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
		localOnErr(ctx, buffer);
		return "";
	}
	/* Decrypt */
	if (!EVP_DecryptUpdate(ctx, buffer, &len, encrypted,
				   encryptedLength)) {
		localOnErr(ctx, buffer);
		return "";
	}
	if (tag) {
		/* If using gcm, set the encryption-paired tag */
		if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG,
					     MCR_AES_TAG_SIZE, (void *)tag)) {
			localOnErr(ctx, buffer);
			return "";
		}
	}
	/* Finalise the decryption */
	if (!EVP_DecryptFinal_ex(ctx, buffer + len, &len)) {
		localOnErr(ctx, buffer);
		return "";
	}
	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	ret = (char *)buffer;
	delete []buffer;
	return ret;
}

void SafeString::randomize(unsigned char *bufferOut, int bufferSize) throw(int)
{
	if (!RAND_bytes(bufferOut, bufferSize)) {
		localOnErr();
		return;
	}
}

void SafeString::sha(const string &text, unsigned char bufferOut[]) throw(int)
{
	SHA256_CTX ctx;
	if (!SHA256_Init(&ctx)) {
		localOnErr();
		return;
	}
	if (!SHA256_Update(&ctx, bytes(text), text.size())) {
		localOnErr();
		return;
	}
	if (!SHA256_Final(bufferOut, &ctx)) {
		localOnErr();
	}
}

void SafeString::initialize() throw(int)
{
	if (!_initCount++) {
		OPENSSL_init();
		OPENSSL_no_config();
		ERR_load_CRYPTO_strings();
		OPENSSL_add_all_algorithms_noconf();
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

void SafeString::deinitialize() throw(int)
{
	if (!--_initCount) {
		ERR_free_strings();
		FIPS_mode_set(0);
		CONF_modules_unload(1);
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		ERR_free_strings();
	}
}
}
