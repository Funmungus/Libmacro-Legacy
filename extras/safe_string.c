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
#include "mcr/modules.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Initialized to random bytes, can be set manually */
static char _pass[MCR_AES_BLOCK_SIZE + 1] = { 0 };

static const struct mcr_Interface _MCR_SAFE_STRING_IFACE = {
	.id = ~0,
	.copy = mcr_SafeString_copy,
	.data_size = sizeof(struct mcr_SafeString),
	.init = mcr_SafeString_init,
	.deinit = mcr_SafeString_deinit
};

const struct mcr_Interface *mcr_SafeString_interface()
{
	return &_MCR_SAFE_STRING_IFACE;
}

int mcr_SafeString_init(void *ssPt)
{
	struct mcr_SafeString *localPt = ssPt;
	if (localPt) {
		memset(localPt, 0, sizeof(struct mcr_SafeString));
		mcr_String_init(&localPt->text);
	}
	return 0;
}

struct mcr_SafeString mcr_SafeString_new()
{
	struct mcr_SafeString ret;
	mcr_SafeString_init(&ret);
	return ret;
}

int mcr_SafeString_deinit(void *ssPt)
{
	struct mcr_SafeString *localPt = ssPt;
	if (localPt) {
		mcr_String_deinit(&localPt->text);
		if (localPt->cryptic) {
			memset(localPt->iv, 0, MCR_AES_IV_SIZE);
			memset(localPt->tag, 0, MCR_AES_TAG_SIZE);
		}
		localPt->cryptic = false;
	}
	return 0;
}

int mcr_SafeString_compare(const void *lhs, const void *rhs)
{
	const struct mcr_SafeString *lPt = lhs, *rPt = rhs;
	int ret;
	if (rhs) {
		if (lhs) {
			if (lhs == rhs)
				return 0;
			if ((ret = MCR_CMP_INTEGRAL(lPt->cryptic,
						rPt->cryptic)))
				return ret;
			if (lPt->cryptic) {
				if ((ret = MCR_CMP_INTEGRAL(lPt->text.used,
							rPt->text.used)))
					return ret;
				if (!lPt->text.used)
					return 0;
				if ((ret = memcmp(lPt->iv, rPt->iv,
							MCR_AES_IV_SIZE)))
					return ret;
				if ((ret = memcmp(lPt->tag, rPt->tag,
							MCR_AES_TAG_SIZE)))
					return ret;
				return memcmp(lPt->text.array, rPt->text.array,
					lPt->text.used);
			}
			return mcr_String_compare(lPt, rPt);
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_SafeString_copy(void *dstPt, void *srcPt)
{
	dassert(dstPt);
	struct mcr_SafeString *dPt = dstPt, *sPt = srcPt;
	mcr_String txt;
	int err = 0;
	if (!sPt || MCR_STR_IS_EMPTY(sPt->text)) {
		mcr_SafeString_deinit(dPt);
		return 0;
	}
	if (sPt->cryptic) {
		mcr_String_init(&txt);
		err = mcr_SafeString_text(sPt, &txt);
		if (!err)
			err = mcr_SafeString_set_text(dPt, txt.array, true);
		mcr_String_deinit(&txt);
	} else {
		err = mcr_SafeString_set_text(dPt, sPt->text.array, false);
	}
	return err;
}

int mcr_SafeString_text(const struct mcr_SafeString *ssPt, mcr_String * strOut)
{
	dassert(ssPt);
	dassert(strOut);
	return mcr_SafeString_text_pass(ssPt, _pass, strOut);
}

int mcr_SafeString_set_text(struct mcr_SafeString *ssPt,
	const char *text, bool cryptic)
{
	dassert(ssPt);
	return mcr_SafeString_set_text_pass(ssPt, _pass, text, cryptic);
}

int mcr_SafeString_text_pass(const struct mcr_SafeString *ssPt,
	const char *pass, mcr_String * strOut)
{
	dassert(ssPt);
	dassert(strOut);
	if (ssPt->cryptic && !MCR_STR_IS_EMPTY(ssPt->text)) {
		return mcr_String_encrypt(&ssPt->text,
			(const unsigned char *)pass, ssPt->iv,
			(unsigned char *)ssPt->tag, strOut, false);
	}
	return mcr_String_copy(strOut, (void *)&ssPt->text);
}

int mcr_SafeString_set_text_pass(struct mcr_SafeString *ssPt,
	const char *pass, const char *text, bool cryptic)
{
	mcr_String strMem;
	int err = 0;
	dassert(ssPt);
	dassert(cryptic ? ! !pass : 1);
	/* Null text, still set ssPt encrypting */
	fixme;
	if (cryptic && text && pass[0] != '\0') {
		if ((err = mcr_randomize(ssPt->iv, MCR_AES_IV_SIZE)))
			return err;

		mcr_String_init(&strMem);
		err = mcr_String_replace(&strMem, text);
		if (!err)
			err = mcr_String_encrypt(&strMem,
				(const unsigned char *)pass, ssPt->iv,
				ssPt->tag, &ssPt->text, 1);
		mcr_String_deinit(&strMem);
		ssPt->cryptic = 1;
	} else {
		/* Case, not encryptable */
		if ((err = mcr_String_replace(&ssPt->text, text)))
			return err;
		if (ssPt->cryptic) {
			memset(ssPt->iv, 0, MCR_AES_IV_SIZE);
			memset(ssPt->tag, 0, MCR_AES_TAG_SIZE);
			ssPt->cryptic = false;
		}
	}
	return err;
}

bool mcr_SafeString_cryptic(const struct mcr_SafeString * ssPt)
{
	return ssPt ? ssPt->cryptic : false;
}

int mcr_SafeString_set_cryptic(struct mcr_SafeString *ssPt, bool cryptic)
{
	mcr_String strMem;
	int err = 0;
	dassert(ssPt);
	if ((! !cryptic) == (! !ssPt->cryptic))
		return 0;
	if (MCR_STR_IS_EMPTY(ssPt->text)) {
		ssPt->cryptic = cryptic;
		return 0;
	}
	mcr_String_init(&strMem);
	if (cryptic) {
		if ((err = mcr_randomize(ssPt->iv, MCR_AES_IV_SIZE)))
			return err;

		if ((err = mcr_String_encrypt(&ssPt->text,
					(const unsigned char *)_pass,
					ssPt->iv, ssPt->tag, &strMem, true))) {
			mcr_String_deinit(&strMem);
			return err;
		}
		mcr_String_deinit(&ssPt->text);
		ssPt->text = strMem;
		ssPt->cryptic = true;
	} else {
		if ((err = mcr_String_encrypt(&ssPt->text,
					(const unsigned char *)_pass, ssPt->iv,
					ssPt->tag, &strMem, false))) {
			mcr_String_deinit(&strMem);
			return err;
		}
		/* Switch current from encrypted to decrypted version */
		mcr_String_deinit(&ssPt->text);
		ssPt->text = strMem;
		memset(ssPt->iv, 0, sizeof(ssPt->iv));
		memset(ssPt->tag, 0, sizeof(ssPt->tag));
		ssPt->cryptic = false;
	}
	return err;
}

int mcr_SafeString_recrypt(struct mcr_SafeString *ssPt, char *oldPass)
{
	mcr_String strMem;
	int err;
	dassert(ssPt);
	if (!ssPt->cryptic || MCR_STR_IS_EMPTY(ssPt->text))
		return 0;
	mcr_String_init(&strMem);
	err = mcr_SafeString_text_pass(ssPt, oldPass, &strMem);
	if (!err)
		err = mcr_SafeString_set_text_pass(ssPt, _pass, strMem.array,
			ssPt->cryptic);
	mcr_String_deinit(&strMem);
	return err;
}

int mcr_String_encrypt(const mcr_String * strOriginalPt,
	const unsigned char *pass, const unsigned char *iv,
	unsigned char *tag, mcr_String * strPtOut, bool toEncryptedFlag)
{
	size_t len = strOriginalPt->used;
	int err = 0, eLen = 0;
	dassert(strOriginalPt);
	dassert(tag);
	dassert(pass);
	dassert(iv);
	dassert(strPtOut);
	dassert(strOriginalPt != strPtOut);
	if (MCR_STR_IS_EMPTY(*strOriginalPt)) {
		mcr_String_clear(strPtOut);
		return 0;
	}
	/* Encrypted text may be larger than original. */
	if ((err = mcr_String_resize(strPtOut, len +
				(toEncryptedFlag ? len >> 1 : 1))))
		return err;
	/* To encrypted we have to remove null char, from encrypted no
	 * null char is included, but we have to add it after decrypting */
	if (toEncryptedFlag)
		eLen = mcr_encrypt((const unsigned char *)
			strOriginalPt->array, (unsigned int)(len - 1), NULL,
			0, pass, iv, tag, (unsigned char *)strPtOut->array);
	else
		eLen = mcr_decrypt((const unsigned char *)
			strOriginalPt->array, (unsigned int)(len), NULL,
			0, pass, iv, tag, (unsigned char *)strPtOut->array);
	if (eLen == -1) {
		err = mcr_error();
		if (!err)
			err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
		return err;
	}
	/* Add null char to decryption */
	if (toEncryptedFlag) {
		strPtOut->used = eLen;
	} else {
		strPtOut->used = eLen + 1;
		strPtOut->array[eLen] = '\0';
	}
	fixme;
	return err;
}

void mcr_SafeString_set_password(char *pass)
{
	if (pass) {
		strncpy(_pass, pass, MCR_AES_BLOCK_SIZE);
	} else {
		memset(_pass, 0, MCR_AES_BLOCK_SIZE);
	}
	_pass[MCR_AES_BLOCK_SIZE] = '\0';
}
