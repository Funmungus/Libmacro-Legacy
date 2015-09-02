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

# ifndef MCR_SAFESTRING_H
# define MCR_SAFESTRING_H

# include "extras/def.h"

# define MCR_AES_BLOCK_SIZE 128
# define MCR_AES_TAG_SIZE ( MCR_AES_BLOCK_SIZE / 8 )

typedef struct mcr_SafeString
{
	//
	// All internal
	//
	int cryptic ;
	unsigned char iv [ MCR_AES_BLOCK_SIZE + 1 ] ; /* randomized */
	/* Received from encryption. */
	unsigned char tag [ MCR_AES_TAG_SIZE + 1 ] ;
	mcr_Array text ;
} mcr_SafeString ;

MCR_API void mcr_SafeString_init ( mcr_SafeString * ssPt ) ;
MCR_API void mcr_SafeString_init_with ( mcr_SafeString * ssPt,
		const char * text, int cryptic ) ;
MCR_API int mcr_SafeString_compare ( const void * lhs,
		const void * rhs ) ;
MCR_API void mcr_SafeString_copy ( void * dstPt, void * srcPt ) ;
MCR_API void mcr_SafeString_free ( mcr_SafeString * ssPt ) ;
# define mcr_SafeString_free_foreach( ssPt, ... ) \
	mcr_SafeString_free ( ssPt )

MCR_API mcr_Array mcr_SafeString_get ( const mcr_SafeString * ssPt ) ;
MCR_API mcr_Array mcr_SafeString_from_password ( const mcr_SafeString * ssPt,
		const char * pass ) ;
MCR_API void mcr_SafeString_set ( mcr_SafeString * ssPt,
		const char * text, int cryptic ) ;
MCR_API void mcr_SafeString_set_cryptic ( mcr_SafeString * ssPt,
		int cryptic ) ;
MCR_API void mcr_SafeString_recrypt ( mcr_SafeString * ssPt,
		char * oldPass ) ;

MCR_API void mcr_String_to_encrypted ( mcr_Array * toEncryptPt,
		unsigned char * tag, unsigned char * pass, unsigned char * iv ) ;
MCR_API mcr_Array mcr_String_from_encrypted (
		const mcr_Array * encryptedPt,
		const unsigned char * tag, const unsigned char * pass,
		const unsigned char * iv ) ;
/*! Max 32 chars */
MCR_API void mcr_SafeString_set_password ( char * pass ) ;

MCR_API void mcr_safestring_initialize ( ) ;
MCR_API void mcr_safestring_cleanup ( void ) ;

//
// Implement in native directory
//
//! \brief Native, use encryption software where available.
MCR_API void mcr_randomize ( unsigned char * buffer,
		int bufferSize ) ;
/*! \brief Encrypt from original bytes
 *
 * \return String length of encrypted text
 * */
MCR_API int mcr_encrypt ( unsigned char * plaintext, int plaintext_len,
		unsigned char * aad, int aad_len, unsigned char * key,
		unsigned char * iv, unsigned char * ciphertext,
		unsigned char * tag ) ;
/*! \brief Decrypt from encrypted bytes
 *
 * \return String length of decrypted text
 * */
MCR_API int mcr_decrypt ( const unsigned char * ciphertext,
		const int ciphertext_len, const unsigned char * aad,
		const int aad_len, const unsigned char * tag,
		const unsigned char * key, const unsigned char * iv,
		unsigned char * plaintext ) ;

# endif // MCR_SAFESTRING_H
