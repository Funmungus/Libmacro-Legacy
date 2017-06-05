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

#include <cstring>
#include "mcr/extras/extras.h"

namespace mcr
{
SafeString::~SafeString()
{
	if (_keyProvider)
		_keyProvider->deregister(this);
	if (_encrypted)
		delete _encrypted;
}

SafeString::SafeString(const SafeString &copytron) throw(int)
	: _cryptic(copytron.cryptic()), _encrypted(NULL), _encryptedBufferSize(0),
	  _encryptedBytes(0),
	  _keyProvider(copytron.keyProvider()), _iv{0}, _lenMem(0), _plain(""),
	  _stateless(copytron.stateless()), _tag{0}
{
	if (cryptic())
		resetIv();
	setText(copytron.text());
}

int SafeString::compare(const SafeString &rhs) const throw(int)
{
	int cmp;
	mcr::string lMem, rMem;
	if (&rhs == this)
		return 0;
	if ((cmp = MCR_CMP_INTEGRAL(cryptic(), rhs.cryptic())))
		return cmp;
	lMem = text();
	rMem = rhs.text();
	if (rMem.empty())
		return !lMem.empty();
	return lMem.empty() ? -1 : std::strcmp(bytes(lMem), bytes(rMem));
}

void SafeString::setCryptic(bool val) throw (int)
{
	/* If changing from plain to encrypted, be sure there is a key
	 * provider */
	if (val != cryptic() && _keyProvider) {
		setText(text(), val);
	}
}

void SafeString::setKeyProvider(IKeyProvider *provider) throw(int)
{
	string mem;
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

void SafeString::setIv(unsigned char *iv) throw(int)
{
	string mem;
	if (cryptic()) {
		mem = text();
		std::memcpy(_iv, iv, sizeof(_iv));
		setText(mem);
	} else {
		std::memcpy(_iv, iv, sizeof(_iv));
	}
}

void SafeString::resetIv() throw(int)
{
	string mem;
	if (cryptic()) {
		mem = text();
		initializer(_iv);
		setText(mem);
	} else {
		std::memset(_iv, 0, sizeof(_iv));
	}
}

void SafeString::setStateless(bool val) throw(int)
{
	string mem;
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

string SafeString::text() const throw(int)
{
	const unsigned char *key;
	if (!cryptic())
		return _plain;
	if (!_encrypted || !_encryptedBytes)
		return "";
	if (!_keyProvider)
		throw EFAULT;
	_keyProvider->key(this, &key);
	if (!key)
		throw EFAULT;
	return decrypt(_encrypted, _encryptedBytes, key, _iv,
		       stateless() ? NULL : _tag);
}

void SafeString::setText(const string &str) throw(int)
{
	int buffLen;
	const unsigned char *key;
	/* Empty string */
	if (!str.size() || str[0] == '\0') {
		clear();
		return;
	}
	if (cryptic()) {
		buffLen = str.size() * 1.5f;
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
			_encrypted = new unsigned char[buffLen];
		}
		_encryptedBytes = encrypt(str, key, _iv,
					  stateless() ? NULL : _tag, _encrypted);
		/* Error, but encrypt function will throw an error so this will
		 * not be used */
		if (_encryptedBytes == -1)
			_encryptedBytes = 0;
	} else {
		_plain = str;
	}
	_lenMem = str.size();
}

void SafeString::setText(const string &str, bool cryptic) throw(int)
{
	/* If changing from plain to encrypted, be sure there is a key
	 * provider */
	if (cryptic != _cryptic && _keyProvider) {
		clear();
		_cryptic = cryptic;
		resetIv();
	}
	setText(str);
}

void SafeString::clear()
{
	_encryptedBufferSize = _encryptedBytes = 0;
	if (_encrypted)
		delete []_encrypted;
	_lenMem = 0;
	_plain = "";
	if (cryptic())
		std::memset(_tag, 0, sizeof(_tag));
}
}
