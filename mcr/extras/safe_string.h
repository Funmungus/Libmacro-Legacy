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

/*! \file
 *  \brief \ref SafeString - A string that can be encrypted in memory
 *  \ref IKeyProvider - Application-defined objects that store encryption
 *  keys to be used by SafeString.
 */

#ifndef MCR_EXTRAS_SAFE_STRING_H_
#define MCR_EXTRAS_SAFE_STRING_H_

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
/*! Get an encryption key for a specific string object */
class MCR_API IKeyProvider
{
public:
	IKeyProvider() = default;
	IKeyProvider(const IKeyProvider &) = default;
	virtual ~IKeyProvider() {}
	IKeyProvider &operator =(const IKeyProvider &) = default;
	/*! Get an encryption key for a specific string object
	 *
	 *  If the object is not yet registered the key should be generated
	 *  before given to the string object.
	 *  \param obj String object which is requesting a key
	 *  \param keyOut Return a reference to the key
	 */
	virtual void key(const void *obj,
					 const unsigned char *keyOut[MCR_AES_BLOCK_SIZE]) = 0;
	/*! Remove a string object.  If the key function is called again
	 *  the key should be regenerated.
	 *
	 *  \param obj String object to remove key for
	 */
	virtual void deregister(const void *obj) = 0;
	/*! \ref SafeString::generateKey
	 *  \param keyOut Buffer to write key to
	 */
	static inline void generate(unsigned char keyOut[MCR_AES_BLOCK_SIZE]);
	/*! \ref SafeString::sha
	 *  \param text Plain text to compute hash for
	 *  \param bufferOut Write hash to this buffer
	 */
	static inline void sha(const std::string &text,
						   unsigned char bufferOut[MCR_AES_BLOCK_SIZE]);
};

/*! A string that can be encrypted in memory */
class MCR_API SafeString
{
public:
	/*! \param keyProvider \ref opt Provider of encryption keys.  If null then
	 *  encryption is not possible and text will remain plain text.
	 *  \param str \ref opt Initial text
	 *  \param cryptic \ref opt Initial encryption state
	 */
	SafeString(IKeyProvider *keyProvider = nullptr,
			   const std::string &str = std::string(),
			   bool cryptic = true)
		: _cryptic(cryptic
				   && keyProvider), _encrypted(nullptr), _encryptedBufferSize(0),
		  _encryptedBytes(0),
		  _keyProvider(keyProvider), _lenMem(0), _plain(nullptr), _plainBufferSize(0),
		  _stateless(false)
	{
		std::memset(_iv, 0, sizeof(_iv));
		std::memset(_tag, 0, sizeof(_tag));
		if (cryptic)
			resetIv();
		setText(str);
	}
	SafeString(const SafeString &copytron);
	//! Will IKeyProvider::deregister if a key provider is available
	virtual ~SafeString();
	SafeString &operator =(const SafeString &copytron)
	{
		if (&copytron != this)
			setText(copytron.text());
		return *this;
	}
	inline SafeString &operator =(const std::string &str)
	{
		setText(str);
		return *this;
	}
	inline SafeString &operator =(const char *str)
	{
		if (!str)
			str = "";
		setText(str, strlen(str));
		return *this;
	}

	/*! Plain text => encrypted text
	 *
	 *  \param plainText String to be encrypted
	 *  \param key Encryption key, randomized bytes or hashed password
	 *  \param iv \ref opt Initializer that should not be shared between
	 *  texts encrypted with the same key.
	 *  \param tagOut \ref opt A paired value will write to this buffer,
	 *  required for encryption state and added security. Set to null for
	 *  stateless encryption.
	 *  \param bufferOut Buffer to hold encrypted bytes.  Encrypted text can
	 *  be greater than plain text length.  To be safe the byte length of
	 *  this buffer should be at least 1.5 times the plain text string
	 *  length.
	 *  \return Length of encrypted bytes written to bufferOut. Will be -1 for any errors.
	 */
	static inline int encrypt(const std::string &plainText,
							  const unsigned char key[MCR_AES_BLOCK_SIZE],
							  const unsigned char iv[MCR_AES_IV_SIZE],
							  unsigned char tagOut[MCR_AES_TAG_SIZE], unsigned char *bufferOut)
	{
		return encrypt(plainText.c_str(), plainText.size(), key, iv, tagOut, bufferOut);
	}
	/*! \ref mcr_is_platform Plain text => encrypted text
	 *
	 *  This is usually defined in the ssl directory.  Redefine if not
	 *  linking to a libcrypto- or libssl-compatible library.
	 *  \return Length of encrypted bytes written to bufferOut. Will be -1 for any errors.
	 */
	static int encrypt(const char *plain, size_t plainLen,
					   const unsigned char key[MCR_AES_BLOCK_SIZE],
					   const unsigned char iv[MCR_AES_IV_SIZE],
					   unsigned char tagOut[MCR_AES_TAG_SIZE], unsigned char *bufferOut);
	/*! Encrypted text => plain text
	 *
	 *  \param encrypted Encrypted bytes to decrypt as a string
	 *  \param encryptedLength Bytes to decrypt from encrypted string
	 *  \param key Encryption key, randomized bytes or hashed password
	 *  \param iv \ref opt Initializer that should not be shared between
	 *  texts encrypted with the same key.
	 *  \param tag \ref opt A paired value will write to this buffer,
	 *  required for encryption state and added security. Set to null for
	 *  stateless encryption.
	 *  \return Decrypted string
	 */
	static inline std::string decrypt(const unsigned char *encrypted,
									  int encryptedLength,
									  const unsigned char key[MCR_AES_BLOCK_SIZE],
									  const unsigned char iv[MCR_AES_IV_SIZE],
									  const unsigned char tag[])
	{
		std::string ret = "";
		size_t decLen = 0;
		if (!encrypted || !encryptedLength)
			return "";
		char *buffer = new char[static_cast<unsigned int>(encryptedLength) + 1];
		try {
			decLen = decrypt(encrypted, encryptedLength, key, iv, tag, buffer);
		} catch (int e) {
			delete []buffer;
			buffer = nullptr;
			throw e;
		}
		if (static_cast<int>(decLen) != -1)
			ret = buffer;
		delete []buffer;
		return ret;
	}
	/*! \ref mcr_is_platform Encrypted text => plain text
	 *
	 *  This is usually defined in the ssl directory.  Redefine if not
	 *  linking to a libcrypto- or libssl-compatible library.
	 *  \return Length of decrypted bytes written to bufferOut.
	 */
	static size_t decrypt(const unsigned char *encrypted, int encryptedLength,
							 const unsigned char key[MCR_AES_BLOCK_SIZE],
							 const unsigned char iv[MCR_AES_IV_SIZE],
							 const unsigned char tag[], char *bufferOut);
	/*! \ref mcr_is_platform Output pseudo randomized bytes
	 *
	 *  This is usually defined in the ssl directory.  Redefine if not
	 *  linking to a libcrypto- or libssl-compatible library.
	 *  \param bufferOut Write bytes to this buffer
	 *  \param bufferSize Number of bytes to write to output buffer
	 */
	static void randomize(unsigned char *bufferOut, int bufferSize);
	/*! Create a SHA hash suitable to use as an encryption key
	 *
	 *  \param text Plain text to compute hash for
	 *  \param bufferOut Write hash to this buffer
	 */
	static inline void sha(const std::string &text,
						   unsigned char bufferOut[MCR_AES_BLOCK_SIZE])
	{
		sha(text.c_str(), text.size(), bufferOut);
	}
	/*! \ref mcr_is_platform Create a SHA hash suitable to use as an encryption key
	 *
	 *  This is usually defined in the ssl directory.  Redefine if not
	 *  linking to a libcrypto- or libssl-compatible library.
	 */
	static void sha(const char *text, size_t textLen,
					unsigned char bufferOut[MCR_AES_BLOCK_SIZE]);
	/*! \ref mcr_is_platform This is called in library initialization
	 *
	 *  This is usually defined in the ssl directory.  Redefine if not
	 *  linking to a libcrypto- or libssl-compatible library.
	 */
	static void initialize();
	/*! \ref mcr_is_platform This is called in library cleanup
	 *
	 *  This is usually defined in the ssl directory.  Redefine if not
	 *  linking to a libcrypto- or libssl-compatible library.
	 */
	static void deinitialize();
	/*! Use randomize to generate a key
	 *  \param keyOut Byte array to use as an encryption key.
	 */
	static inline void generateKey(unsigned char keyOut[MCR_AES_BLOCK_SIZE])
	{
		if (keyOut)
			randomize(keyOut, MCR_AES_BLOCK_SIZE);
	}
	/*! Use randomize to generate an iv
	 *  \param ivOut Byte array to write iv to
	 */
	static inline void initializer(unsigned char ivOut[MCR_AES_IV_SIZE])
	{
		if (ivOut)
			randomize(ivOut, MCR_AES_IV_SIZE);
	}

	/*! Compare cryptic and \ref strcmp plain text */
	int compare(const SafeString &rhs) const;

	/*! True for encrypted, otherwise plain text */
	inline bool cryptic() const
	{
		return _cryptic;
	}
	/*! Set encryption state
	 *
	 *  If key provider is not available encryption state will always
	 *  be false.
	 *  Current text will be preserved
	 */
	void setCryptic(bool val);
	/*! Get encryption keys from this object */
	inline IKeyProvider *keyProvider() const
	{
		return _keyProvider;
	}
	/*! Set encryption key provider
	 *
	 *  Current text will be preserved
	 */
	void setKeyProvider(IKeyProvider *provider);
	/*! Initialization vector, added security for encryption */
	inline const unsigned char *iv() const
	{
		return _iv;
	}
	/*! Set initialization vector
	 *
	 *  Current text will be preserved
	 */
	void setIv(unsigned char *iv);
	/*! Randomize iv
	 *
	 *  Current text will be preserved
	 */
	void resetIv();
	/*! A stateless string can always be decrypted to the same
	 *  value, as long as the key and iv are the same.  Tag is not used,
	 *  or ignored.
	 */
	inline bool stateless() const
	{
		return _stateless;
	}
	/*! Set stateless ability of this string
	 *
	 *  Current text will be preserved
	 */
	void setStateless(bool val);

	/*! Byte length of plain text string, such as \ref strlen */
	inline size_t length() const
	{
		return _lenMem;
	}
	/*! Get string as plain text */
	inline std::string text() const
	{
		char *bufferOut;
		std::string ret = "";
		if (!_cryptic)
			return _plain ? _plain : "";
		if (!_lenMem)
			return "";
		bufferOut = new char[_lenMem + 1];
		try {
			text(bufferOut);
			ret = bufferOut;
		} catch (int e) {
			delete []bufferOut;
			bufferOut = nullptr;
			throw e;
		}
		delete []bufferOut;
		return ret;
	}
	/*! Get string as plain text
	 *  \param bufferOut Buffer to write plain text, must be at least \ref length + 1 in byte size
	 *  \return String length of plain text
	 */
	size_t text(char *bufferOut) const;
	/*! Set string plain text, and will encrypt if currently
	 *  encrypting */
	inline void setText(const std::string &str = std::string())
	{
		setText(str.c_str(), str.size());
	}
	/*! Set string plain text, and will encrypt if currently
	 *  encrypting
	 *  \param len \ref strlen of string, or -1 for entire string
	 */
	void setText(const char *str, size_t len);
	/*! Set string plain text
	 *
	 *  \param cryptic \ref setCryptic
	 */
	inline void setText(const std::string &str, bool cryptic)
	{
		setText(str.c_str(), str.size(), cryptic);
	}
	/*! Set string plain text
	 *
	 *  \param cryptic \ref setCryptic
	 */
	void setText(const char *str, size_t len, bool cryptic);

	/*! Clear all strings, allocated data, and tag.  iv will not
	 *  be changed.
	 */
	void clear();
private:
	bool _cryptic;
	unsigned char *_encrypted;
	int _encryptedBufferSize;
	int _encryptedBytes;
	IKeyProvider *_keyProvider;
	unsigned char _iv[MCR_AES_IV_SIZE];
	size_t _lenMem;
	char *_plain;
	size_t _plainBufferSize;
	bool _stateless;
	unsigned char _tag[MCR_AES_TAG_SIZE];
};

void IKeyProvider::generate(unsigned char keyOut[])
{
	SafeString::generateKey(keyOut);
}

void IKeyProvider::sha(const std::string &text, unsigned char bufferOut[])
{
	SafeString::sha(text, bufferOut);
}
}

#endif
