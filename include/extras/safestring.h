/* include/extras/safestring.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

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
MCR_API void mcr_SafeString_free_foreach ( mcr_SafeString * ssPt, ... ) ;

MCR_API mcr_Array mcr_SafeString_get ( const mcr_SafeString * ssPt ) ;
MCR_API mcr_Array mcr_SafeString_from_password ( const mcr_SafeString * ssPt,
		const char * pass ) ;
MCR_API void mcr_SafeString_set ( mcr_SafeString * ssPt,
		const char * text, int cryptic ) ;
MCR_API void mcr_SafeString_recrypt ( mcr_SafeString * ssPt,
		char * oldPass ) ;

MCR_API void mcr_String_to_encrypted ( mcr_Array * toEncryptPt,
		unsigned char * tag, unsigned char * pass, unsigned char * iv ) ;
MCR_API mcr_Array mcr_String_from_encrypted (
		const mcr_Array * encryptedPt,
		const unsigned char * tag, const unsigned char * pass,
		const unsigned char * iv ) ;
/* Max 32 chars */
MCR_API void mcr_SafeString_set_password ( char * pass ) ;

MCR_API void mcr_safestring_initialize ( ) ;
MCR_API void mcr_safestring_cleanup ( void ) ;

//
// Implement in native directory
//
//! \brief Native, use encryption software where available.
MCR_API void mcr_randomize ( unsigned char * buffer,
		int bufferSize ) ;
MCR_API int mcr_encrypt ( unsigned char * plaintext, int plaintext_len,
		unsigned char * aad, int aad_len, unsigned char * key,
		unsigned char * iv, unsigned char * ciphertext,
		unsigned char * tag ) ;
MCR_API int mcr_decrypt ( const unsigned char * ciphertext,
		const int ciphertext_len, const unsigned char * aad,
		const int aad_len, const unsigned char * tag,
		const unsigned char * key, const unsigned char * iv,
		unsigned char * plaintext ) ;

# endif // MCR_SAFESTRING_H
