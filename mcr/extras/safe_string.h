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

#ifndef MCR_SAFESTRING_H
#define MCR_SAFESTRING_H

#include "mcr/extras/def.h"

#define MCR_AES_BLOCK_SIZE 128
#define MCR_AES_TAG_SIZE (MCR_AES_BLOCK_SIZE / 8)

struct mcr_SafeString {
	/* */
	/* All internal */
	/* */
	bool cryptic;
	/* randomized */
	unsigned char iv[MCR_AES_BLOCK_SIZE + 1];
	/* Received from encryption. */
	unsigned char tag[MCR_AES_TAG_SIZE + 1];
	mcr_String text;
};

MCR_API void mcr_SafeString_init(void *ssDataPt);
MCR_API void mcr_SafeString_free(void *ssDataPt);
#define mcr_SafeString_free_foreach(ssPt, ignore) \
mcr_SafeString_free(ssPt)
MCR_API int mcr_SafeString_compare(const void *lhs, const void *rhs);
MCR_API int mcr_SafeString_copy(void *dstPt, void *srcPt);

MCR_API int mcr_SafeString_text(const struct mcr_SafeString *ssPt,
	mcr_String * strOut);
MCR_API int mcr_SafeString_set_text(struct mcr_SafeString *ssPt,
	const char *text, bool cryptic);
MCR_API int mcr_SafeString_text_pass(const struct mcr_SafeString *ssPt,
	const char *pass, mcr_String * strOut);
MCR_API int mcr_SafeString_set_text_pass(struct mcr_SafeString *ssPt,
	const char *pass, const char *text, bool cryptic);
MCR_API bool mcr_SafeString_cryptic(struct mcr_SafeString *ssPt);
MCR_API int mcr_SafeString_set_cryptic(struct mcr_SafeString *ssPt,
	bool cryptic);
MCR_API int mcr_SafeString_recrypt(struct mcr_SafeString *ssPt, char *oldPass);

MCR_API int mcr_String_encrypt(const mcr_String * strOriginalPt,
	const unsigned char *pass, const unsigned char *iv,
	unsigned char *tag, mcr_String * strPtOut, bool toEncryptedFlag);
/*! \brief Max MCR_AES_BLOCK_SIZE chars */
MCR_API void mcr_SafeString_set_password(char *pass);

/* */
/* Implement in native directory */
/* */
/*! \brief Native, use encryption software where available. */
MCR_API int mcr_randomize(unsigned char *buffer, int bufferSize);
/*!
 * \brief Encrypt from original bytes
 *
 * \return String length of encrypted text, -1 on error.
 */
MCR_API int mcr_encrypt(const unsigned char *plaintext, int plaintext_len,
	const unsigned char *aad, int aad_len, const unsigned char *key,
	const unsigned char *iv, unsigned char *tag,
	unsigned char *ciphertextOut);
/*!
 * \brief Decrypt from encrypted bytes
 *
 * \return String length of decrypted text, -1 on error.
 */
MCR_API int mcr_decrypt(const unsigned char *ciphertext,
	const int ciphertext_len, const unsigned char *aad,
	const int aad_len, const unsigned char *key,
	const unsigned char *iv, const unsigned char *tag,
	unsigned char *plaintextOut);

#endif
