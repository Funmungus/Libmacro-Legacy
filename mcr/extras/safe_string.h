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

/*!
 * \file
 * \brief \ref SafeString - A string that can be encrypted in memory
 * \ref IKeyProvider - Application-defined objects that store encryption
 * keys to be used by SafeString.
 */

#ifndef __cplusplus
#pragma message "C++ support is required for extras module"
#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_SAFE_STRING_H
#define MCR_EXTRAS_SAFE_STRING_H

#include "mcr/extras/def.h"

/* 256 / 8, 256 bits => 32 bytes */
/*! AES block bytes */
#define MCR_AES_BLOCK_SIZE 32
/*! AES iv bytes */
#define MCR_AES_IV_SIZE 16
/*! AES tag bytes */
#define MCR_AES_TAG_SIZE 16

namespace mcr
{
class SafeString;
/*! \brief Get an encryption key for a specific string object */
class MCR_API IKeyProvider
{
public:
	virtual ~IKeyProvider() {}
	/*!
	 * \brief Get an encryption key for a specific string object
	 *
	 * If the object is not yet registered the key should be generated
	 * before given to the string object.
	 * \param obj String object which is requesting a key
	 * \param keyOut Return a reference to the key
	 */
	virtual void key(const SafeString *obj,
			 const unsigned char *keyOut[MCR_AES_BLOCK_SIZE]) throw(int) = 0;
	/*!
	 * \brief Remove a string object.  If the key function is called again
	 * the key should be regenerated.
	 *
	 * \param obj String object to remove key for
	 */
	virtual void deregister(const SafeString *obj) = 0;
	/*!
	 * \brief \ref SafeString::generateKey
	 * \param keyOut Buffer to write key to
	 */
	static inline void generate(unsigned char keyOut[MCR_AES_BLOCK_SIZE]) throw(
		int);
	/*!
	 * \brief \ref SafeString::sha
	 * \param text Plain text to compute hash for
	 * \param bufferOut Write hash to this buffer
	 */
	static inline void sha(const string &text,
			       unsigned char bufferOut[MCR_AES_BLOCK_SIZE]) throw(int);
};

/*! \brief A string that can be encrypted in memory */
class MCR_API SafeString
{
public:
	/*!
	 * \param keyProvider \ref opt Provider of encryption keys.  If null then
	 * encryption is not possible and text will remain plain text.
	 * \param str \ref opt Initial text
	 * \param cryptic \ref opt Initial encryption state
	 */
	SafeString(IKeyProvider *keyProvider = NULL, const string &str = string(),
		   bool cryptic = true) throw(int)
		: _cryptic(cryptic
			   && keyProvider), _encrypted(NULL), _encryptedBufferSize(0), _encryptedBytes(0),
		  _keyProvider(keyProvider), _iv{0}, _lenMem(0), _plain(""),
		  _stateless(false), _tag{0}
	{
		if (cryptic)
			resetIv();
		setText(str);
	}
	//! Will IKeyProvider::deregister if a key provider is available
	virtual ~SafeString();
	SafeString(const SafeString &copytron) throw(int);
	SafeString &operator =(const SafeString &copytron) throw(int)
	{
		if (&copytron != this) {
			setText(copytron.text());
		}
		return *this;
	}

	/*!
	 * \brief \ref mcr_is_platform Plain text => encrypted text
	 *
	 * This is usually defined in the ssl directory.  Redefine if not
	 * linking to a libcrypto- or libssl-compatible library.
	 * \param plainText String to be encrypted
	 * \param key Encryption key, randomized bytes or hashed password
	 * \param iv \ref opt Initializer that should not be shared between
	 * texts encrypted with the same key.
	 * \param tagOut \ref opt A paired value will write to this buffer,
	 * required for encryption state and added security. Set to null for
	 * stateless encryption.
	 * \param bufferOut Buffer to hold encrypted bytes.  Encrypted text can
	 * be greater than plain text length.  To be safe the byte length of
	 * this buffer should be at least 1.5 times the plain text string
	 * length.
	 * \return Length of encrypted bytes written to bufferOut
	 */
	static int encrypt(const string &plainText,
			   const unsigned char key[MCR_AES_BLOCK_SIZE],
			   const unsigned char iv[MCR_AES_IV_SIZE],
			   unsigned char tagOut[MCR_AES_TAG_SIZE], unsigned char *bufferOut) throw(int);
	/*!
	 * \brief \ref mcr_is_platform Encrypted text => plain text
	 *
	 * This is usually defined in the ssl directory.  Redefine if not
	 * linking to a libcrypto- or libssl-compatible library.
	 * \param encrypted Encrypted bytes to decrypt as a string
	 * \param encryptedLength Bytes to decrypt from encrypted string
	 * \param key Encryption key, randomized bytes or hashed password
	 * \param iv \ref opt Initializer that should not be shared between
	 * texts encrypted with the same key.
	 * \param tag \ref opt A paired value will write to this buffer,
	 * required for encryption state and added security. Set to null for
	 * stateless encryption.
	 * \return Decrypted string
	 */
	static string decrypt(const unsigned char *encrypted, int encryptedLength,
			      const unsigned char key[MCR_AES_BLOCK_SIZE],
			      const unsigned char iv[MCR_AES_IV_SIZE],
			      const unsigned char tag[]) throw(int);
	/*!
	 * \brief \ref mcr_is_platform Output pseudo randomized bytes
	 *
	 * This is usually defined in the ssl directory.  Redefine if not
	 * linking to a libcrypto- or libssl-compatible library.
	 * \param bufferOut Write bytes to this buffer
	 * \param bufferSize Number of bytes to write to output buffer
	 */
	static void randomize(unsigned char *bufferOut, int bufferSize) throw(int);
	/*!
	 * \brief \ref mcr_is_platform Create a SHA hash suitable to use as an encryption key
	 *
	 * This is usually defined in the ssl directory.  Redefine if not
	 * linking to a libcrypto- or libssl-compatible library.
	 * \param text Plain text to compute hash for
	 * \param bufferOut Write hash to this buffer
	 */
	static void sha(const string &text,
			unsigned char bufferOut[MCR_AES_BLOCK_SIZE]) throw(int);
	/*! \brief \ref mcr_is_platform This is called in library initialization
	 *
	 * This is usually defined in the ssl directory.  Redefine if not
	 * linking to a libcrypto- or libssl-compatible library.
	 */
	static void initialize() throw(int);
	/*! \brief \ref mcr_is_platform This is called in library cleanup
	 *
	 * This is usually defined in the ssl directory.  Redefine if not
	 * linking to a libcrypto- or libssl-compatible library.
	 */
	static void deinitialize() throw(int);
	/*!
	 * \brief Use randomize to generate a key
	 * \param keyOut Byte array to use as an encryption key.
	 */
	static inline void generateKey(unsigned char keyOut[MCR_AES_BLOCK_SIZE]) throw(
		int)
	{
		if (keyOut)
			randomize(keyOut, MCR_AES_BLOCK_SIZE);
	}
	/*!
	 * \brief Use randomize to generate an iv
	 * \param ivOut Byte array to write iv to
	 */
	static inline void initializer(unsigned char ivOut[MCR_AES_IV_SIZE]) throw(int)
	{
		if (ivOut)
			randomize(ivOut, MCR_AES_IV_SIZE);
	}

	/*! \brief Compare cryptic and \ref strcmp plain text */
	int compare(const SafeString &rhs) const throw(int);

	/*! \brief True for encrypted, otherwise plain text */
	inline bool cryptic() const
	{
		return _cryptic;
	}
	/*!
	 * \brief Set encryption state
	 *
	 * If key provider is not available encryption state will always
	 * be false.
	 * Current text will be preserved
	 */
	void setCryptic(bool val) throw(int);
	/*! \brief Get encryption keys from this object */
	inline IKeyProvider *keyProvider() const
	{
		return _keyProvider;
	}
	/*! \brief Set encryption key provider
	 *
	 * Current text will be preserved
	 */
	void setKeyProvider(IKeyProvider *provider) throw(int);
	/*! \brief Initialization vector, added security for encryption */
	inline const unsigned char *iv() const
	{
		return _iv;
	}
	/*!
	 * \brief Set initialization vector
	 *
	 * Current text will be preserved
	 */
	void setIv(unsigned char *iv) throw(int);
	/*! \brief Randomize iv
	 *
	 * Current text will be preserved
	 */
	void resetIv() throw(int);
	/*! \brief A stateless string can always be decrypted to the same
	 * value, as long as the key and iv are the same.  Tag is not used,
	 * or ignored.
	 */
	inline bool stateless() const
	{
		return _stateless;
	}
	/*!
	 * \brief Set stateless ability of this string
	 *
	 * Current text will be preserved
	 */
	void setStateless(bool val) throw(int);

	/*! \brief Byte length of plain text string, such as \ref strlen */
	inline size_type length() const
	{
		return _lenMem;
	}
	/*! \brief Get string as plain text */
	string text() const throw(int);
	/*! \brief Set string plain text, and will encrypt if currently
	 * encrypting */
	void setText(const string &str = string()) throw(int);
	/*! \brief Set string plain text
	 *
	 * \param cryptic \ref setCryptic
	 */
	void setText(const string &str, bool cryptic) throw(int);

	/*!
	 * \brief Clear all strings, allocated data, and tag.  iv will not
	 * be changed.
	 */
	void clear();
private:
	bool _cryptic;
	unsigned char *_encrypted;
	int _encryptedBufferSize;
	int _encryptedBytes;
	IKeyProvider *_keyProvider;
	unsigned char _iv[MCR_AES_IV_SIZE];
	size_type _lenMem;
	string _plain;
	bool _stateless;
	unsigned char _tag[MCR_AES_TAG_SIZE];
};

void IKeyProvider::generate(unsigned char keyOut[]) throw(int)
{
	SafeString::generateKey(keyOut);
}

void IKeyProvider::sha(const string &text, unsigned char bufferOut[]) throw(int)
{
	SafeString::sha(text, bufferOut);
}
}

#endif
