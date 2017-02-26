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

/*! \file
 * \brief \ref mcr_SafeString - A \ref mcr_String that is able to become
 * encrypted in memory.
 */

#ifndef MCR_SAFESTRING_H
#define MCR_SAFESTRING_H

#include "mcr/extras/def.h"

/*! AES block bytes */
#define MCR_AES_BLOCK_SIZE (256 >> 3)
/*! AES iv bytes */
#define MCR_AES_IV_SIZE (16)
/*! AES tag bytes */
#define MCR_AES_TAG_SIZE (16)

/*! \brief \ref A \ref mcr_String that is able to become
 * encrypted in memory. */
struct mcr_SafeString {
	/* All internal */
	/*! False means the text is decrypted, otherwise it is encrypted. */
	bool cryptic;
	/*! Randomized encryption iv */
	unsigned char iv[MCR_AES_IV_SIZE];
	/*! Identification tag received from encryption */
	unsigned char tag[MCR_AES_TAG_SIZE];
	/*! Either a C-String or encrypted text */
	mcr_String text;
};

/*! \brief Data interface of safe string structures
 *
 * \return Interface to manage safe string objects */
MCR_API const struct mcr_Interface *mcr_SafeString_interface();

/*! \brief \ref mcr_SafeString ctor
 *
 * \param ssPt \ref opt \ref mcr_SafeString *
 * \return 0
 */
MCR_API int mcr_SafeString_init(void *ssPt);
/*! \brief Construct an empty safe string */
MCR_API struct mcr_SafeString mcr_SafeString_new();
/*! \brief \ref mcr_SafeString dtor
 *
 * \param ssPt \ref opt \ref mcr_SafeString *
 * \return 0
 */
MCR_API int mcr_SafeString_deinit(void *ssPt);
/*! \brief Compare two safe strings
 *
 * \param lhs \ref opt mcr_SafeString *
 * \param rhs \ref opt mcr_SafeString *
 * \return \ref retcmp
 */
MCR_API int mcr_SafeString_compare(const void *lhs, const void *rhs);
/*! \brief Copy a safe string
 *
 * \param dstPt \ref mcr_SafeString * Destination to copy to
 * \param srcPt \ref opt \ref mcr_SafeString * Source to copy from
 * \return \ref reterr
 */
MCR_API int mcr_SafeString_copy(void *dstPt, void *srcPt);

/*! \brief Copy decrypted text from a safe string
 *
 * \param ssPt \ref opt
 * \param strOut String output
 * \return \ref reterr
 */
MCR_API int mcr_SafeString_text(const struct mcr_SafeString *ssPt,
	mcr_String * strOut);
/*! \brief Set text, and set encryption
 *
 * \param text \ref opt C String to set
 * \param cryptic True to encrypt
 * \return \ref reterr
 */
MCR_API int mcr_SafeString_set_text(struct mcr_SafeString *ssPt,
	const char *text, bool cryptic);
/*! \brief Copy decrypted text from a safe string, with a specific key
 *
 * \param ssPt \ref opt
 * \param pass Specific encryption key to use
 * \param strOut String output
 * \return \ref reterr
 */
MCR_API int mcr_SafeString_text_pass(const struct mcr_SafeString *ssPt,
	const char *pass, mcr_String * strOut);
/*! \brief Set text, and set encryption, with a specific key
 *
 * \param pass Specific encryption key to use
 * \param text \ref opt C String to set
 * \param cryptic True to encrypt
 * \return \ref reterr
 */
MCR_API int mcr_SafeString_set_text_pass(struct mcr_SafeString *ssPt,
	const char *pass, const char *text, bool cryptic);
/*! \brief Get the current encryption state
 *
 * \param ssPt \ref opt
 * \return False for not encrypted, otherwise encrypted
 */
MCR_API bool mcr_SafeString_cryptic(const struct mcr_SafeString *ssPt);
/*! \brief Set the current encryption state
 *
 * \return \ref reterr
 */
MCR_API int mcr_SafeString_set_cryptic(struct mcr_SafeString *ssPt,
	bool cryptic);
/*! \brief Decrypt with old key, and re-encrypt with the new one
 *
 * \return \ref reterr
 */
MCR_API int mcr_SafeString_recrypt(struct mcr_SafeString *ssPt, char *oldPass);

/*! \brief Encrypt or decrypt a string
 *
 * \param strOriginalPt \ref opt Reference to the original string to encrypt
 * \param pass Encryption key to use
 * \param iv Output encryption iv of byte size \ref MCR_AES_IV_SIZE
 * \param tag Output encryption tag of byte size \ref MCR_AES_TAG_SIZE
 * \param strPtOut String output
 * \param toEncryptedFlag False to decrypt, true to encrypt
 * \return \ref reterr
 */
MCR_API int mcr_String_encrypt(const mcr_String * strOriginalPt,
	const unsigned char *pass, const unsigned char *iv,
	unsigned char *tag, mcr_String * strPtOut, bool toEncryptedFlag);
/*! \brief Set a new current encryption key
 *
 * \param pass MCR_AES_BLOCK_SIZE randomized chars, or fewer characters with
 * null-termination. */
MCR_API void mcr_SafeString_set_password(char *pass);

/* Implement in native directory */
/*! \brief Randomize bytes
 *
 * Native, use encryption software where available
 * \param buffer Bytes output
 * \param bufferSize Buffer byte count
 * \return \ref reterr
 */
MCR_API int mcr_randomize(unsigned char *buffer, int bufferSize);
/*! \brief Encrypt from original bytes
 *
 * \param plaintext Text to be encrypted
 * \param plaintext_len Byte count of text
 * \param aad \ref opt Additional data not to be encrypted
 * \param aad_len Byte count of additional data
 * \param key Encryption key
 * \param iv Encryption iv output
 * \param tag \ref opt Encryption state tag output, null for stateless
 * encryption.
 * \param ciphertextOut Encrypted string output
 * \return String length of encrypted text, -1 on error
 */
MCR_API int mcr_encrypt(const unsigned char *plaintext, int plaintext_len,
	const unsigned char *aad, int aad_len, const unsigned char *key,
	const unsigned char *iv, unsigned char *tag,
	unsigned char *ciphertextOut);
/*! \brief Decrypt from encrypted bytes
 *
 * \param ciphertext Text to be decrypted
 * \param ciphertext_len Byte count of text
 * \param aad \ref opt Additional data output
 * \param aad_len Byte count of additional data buffer
 * \param key Encryption key
 * \param iv Encryption iv
 * \param tag \ref opt Encryption state tag, null for stateless encryption.
 * \param plaintextOut Decrypted string output
 * \return String length of decrypted text, -1 on error
 */
MCR_API int mcr_decrypt(const unsigned char *ciphertext,
	const int ciphertext_len, const unsigned char *aad,
	const int aad_len, const unsigned char *key,
	const unsigned char *iv, const unsigned char *tag,
	unsigned char *plaintextOut);

#endif
