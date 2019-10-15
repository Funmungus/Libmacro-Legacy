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

namespace mcr
{
SafeString::SafeString(const SafeString &copytron)
	: _cryptic(copytron.cryptic()), _encrypted(nullptr), _encryptedBufferSize(0),
	  _encryptedBytes(0),
	  _keyProvider(copytron.keyProvider()), _lenMem(0), _plain(nullptr),
	  _plainBufferSize(0),
	  _stateless(copytron.stateless())
{
	std::memset(_iv, 0, sizeof(_iv));
	std::memset(_tag, 0, sizeof(_tag));
	if (cryptic())
		resetIv();
	setText(copytron.text());
}

SafeString::~SafeString()
{
	if (_keyProvider)
		_keyProvider->deregister(this);
	if (_encrypted)
		delete []_encrypted;
	if (_plain)
		delete []_plain;
}

int SafeString::compare(const SafeString &rhs) const
{
	int cmp;
	std::string lMem, rMem;
	if (&rhs == this)
		return 0;
	if ((cmp = MCR_CMP(cryptic(), rhs.cryptic())))
		return cmp;
	lMem = text();
	rMem = rhs.text();
	return lMem.compare(rMem);
}

void SafeString::setCryptic(bool val)
{
	/* If changing from plain to encrypted, be sure there is a key
	 * provider */
	if (val != cryptic() && _keyProvider)
		setText(text(), val);
}

void SafeString::setKeyProvider(IKeyProvider *provider)
{
	std::string mem;
	if (provider != _keyProvider) {
		mem = text();
		clear();
		if (!provider)
			setCryptic(false);
		_keyProvider = provider;
		resetIv();
		setText(mem);
	}
}

void SafeString::setIv(unsigned char *iv)
{
	std::string mem;
	if (cryptic()) {
		mem = text();
		std::memcpy(_iv, iv, sizeof(_iv));
		setText(mem);
	} else {
		std::memcpy(_iv, iv, sizeof(_iv));
	}
}

void SafeString::resetIv()
{
	std::string mem;
	if (cryptic()) {
		mem = text();
		initializer(_iv);
		setText(mem);
	} else {
		std::memset(_iv, 0, sizeof(_iv));
	}
}

void SafeString::setStateless(bool val)
{
	std::string mem;
	if (val != stateless()) {
		if (cryptic()) {
			mem = text();
			clear();
			_stateless = val;
			setText(mem);
		} else {
			_stateless = val;
		}
	}
}

size_t SafeString::text(char *bufferOut) const
{
	const unsigned char *key;
	if (!bufferOut)
		return 0;
	if (!cryptic()) {
		if (_plain) {
			memcpy(bufferOut, _plain, _lenMem + 1);
		} else {
			bufferOut[0] = '\0';
		}
		return _lenMem;
	}
	if (!_encrypted || !_encryptedBytes) {
		bufferOut[0] = '\0';
		return 0;
	}
	/* Encrypted, but no key available, so something is wrong */
	if (!_keyProvider)
		throw EFAULT;
	_keyProvider->key(this, &key);
	if (!key)
		throw EFAULT;
	return decrypt(_encrypted, _encryptedBytes, key, _iv,
				   stateless() ? nullptr : _tag, bufferOut);
}

void SafeString::setText(const char *str, size_t len)
{
	int buffLen;
	const unsigned char *key;
	/* Empty string */
	if (!len || !str || str[0] == '\0') {
		clear();
		return;
	}
	if (cryptic()) {
		buffLen = static_cast<int>(len * 1.5);
		/* First check valid key provider */
		if (!_keyProvider)
			throw EFAULT;
		_keyProvider->key(this, &key);
		if (!key)
			throw EFAULT;
		/* Create encryption buffer with enough bytes for this string */
		if (_encryptedBufferSize < buffLen) {
			if (_encrypted)
				delete []_encrypted;
			_encryptedBufferSize = buffLen;
			_encrypted = new unsigned char[static_cast<unsigned int>(_encryptedBufferSize)];
		}
		_encryptedBytes = encrypt(str, len, key, _iv,
								  stateless() ? nullptr : _tag, _encrypted);
		/* Error, but encrypt function will throw an error so this will
		 * not be used */
		if (_encryptedBytes == -1)
			_encryptedBytes = 0;
	} else {
		if (_plainBufferSize < len) {
			if (_plain)
				delete []_plain;
			_plainBufferSize = len + 1;
			_plain = new char[_plainBufferSize];
		}
		memcpy(_plain, str, len + 1);
	}
	_lenMem = len;
}

void SafeString::setText(const char *str, size_t len, bool cryptic)
{
	/* If changing from plain to encrypted, be sure there is a key
	 * provider */
	if (cryptic != _cryptic && _keyProvider) {
		clear();
		_cryptic = cryptic;
		resetIv();
	}
	setText(str, len);
}

void SafeString::clear()
{
	_encryptedBufferSize = _encryptedBytes = 0;
	if (_encrypted) {
		delete []_encrypted;
		_encrypted = nullptr;
	}
	_plainBufferSize = _lenMem = 0;
	if (_plain) {
		delete []_plain;
		_plain = nullptr;
	}
	if (cryptic())
		std::memset(_tag, 0, sizeof(_tag));
}
}
