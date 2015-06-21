/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "extras/extras.h"
# include "extras/lnx/def.h"

void cleanup ( )
{
	mcr_extras_cleanup ( ) ;
	mcr_intercept_cleanup ( ) ;
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}
void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( cleanup ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
	mcr_intercept_initialize ( ) ;
	mcr_extras_initialize ( ) ;
}

//
// Symetric encryption example found here
// http://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
// 2015
int main ( )
{
	/* Set up the key and iv. Do I need to say to not hard code these in a
	 * real application? :- )
	 */
	/* A 256 bit key */
	unsigned char * key =
			( unsigned char * ) "0123456789012345678901234567890" ;
	/* aad doesn't seem to actually do anything with encrypt/decrypt */
	unsigned char * aad =
			( unsigned char * ) "039485ow3t8y " ; //"01012345678901234567890123456789" ;

	/* A 128 bit IV */
	unsigned char * iv =
			( unsigned char * ) "0123456789abcdef" ;

	/* Message to be encrypted */
	unsigned char * plaintext = ( unsigned char * )
			"The quick brown fox jumps over the lazy dog" ;

	/* Buffer for ciphertext. Ensure the buffer is long enough for the
	 * ciphertext which may be longer than the plaintext, dependant on the
	 * algorithm and mode
	 */
	unsigned char ciphertext [ 128 ] ;

	/* Buffer for the decrypted text */
	unsigned char decryptedtext [ 128 ] ;

	/* GCM tag */
	unsigned char tag [ 16 ] ;

	int decryptedtext_len, ciphertext_len ;

	// library initialized in extras */

	/* Encrypt the plaintext */
	ciphertext_len = mcr_encrypt ( plaintext,
			strlen ( ( const char * ) plaintext ), aad,
			strlen ( ( const char * ) aad ), key, iv, ciphertext, tag ) ;

	/* Do something useful with the ciphertext here */
	fprintf ( mcr_stdout, "Ciphertext is:\n" ) ;
	BIO_dump_fp ( stdout, ( const char * ) ciphertext, ciphertext_len ) ;

	/* Decrypt the ciphertext */
	decryptedtext_len = mcr_decrypt ( ciphertext, ciphertext_len, aad,
			strlen ( ( const char * ) aad ), tag, key, iv,
			decryptedtext ) ;

	/* Add a NULL terminator. We are expecting printable text */
	decryptedtext [ decryptedtext_len ] = '\0' ;

	/* Show the decrypted text */
	fprintf ( mcr_stdout, "Decrypted text is:\n" ) ;
	fprintf ( mcr_stdout, "%s\n", decryptedtext ) ;

	/* Library clean up in extras */
	return 0 ;
}
