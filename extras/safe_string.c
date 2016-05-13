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

#include "mcr/extras/extras.h"
#include "mcr/extras/private.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* All data here will be dynamically allocated. */
static char _pass[MCR_AES_BLOCK_SIZE + 1] = { 0 };

void mcr_SafeString_init(void *ssDataPt)
{
	struct mcr_SafeString *ssPt = ssDataPt;
	if (ssPt) {
		memset(ssPt, 0, sizeof(struct mcr_SafeString));
		mcr_String_init(&ssPt->text);
	}
}

void mcr_SafeString_free(void *ssDataPt)
{
	struct mcr_SafeString *ssPt = ssDataPt;
	if (ssPt) {
		mcr_String_free(&ssPt->text);
		if (ssPt->cryptic) {
			memset(ssPt->iv, 0, sizeof(MCR_AES_BLOCK_SIZE + 1));
			memset(ssPt->tag, 0, sizeof(MCR_AES_TAG_SIZE + 1));
		}
		ssPt->cryptic = false;
	}
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
							MCR_AES_BLOCK_SIZE)))
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
		mcr_SafeString_free(dPt);
		return 0;
	}
	if (sPt->cryptic) {
		mcr_String_init(&txt);
		err = mcr_SafeString_text(sPt, &txt);
		if (!err)
			err = mcr_SafeString_set_text(dPt, txt.array, true);
		mcr_String_free(&txt);
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
		if ((err = mcr_randomize(ssPt->iv, MCR_AES_BLOCK_SIZE)))
			return err;
		ssPt->iv[MCR_AES_BLOCK_SIZE] = '\0';

		mcr_String_init(&strMem);
		err = mcr_String_replace(&strMem, text);
		if (!err)
			err = mcr_String_encrypt(&strMem,
				(const unsigned char *)pass, ssPt->iv,
				ssPt->tag, &ssPt->text, 1);
		mcr_String_free(&strMem);
		ssPt->cryptic = 1;
	} else {
		/* Case, not encryptable */
		if ((err = mcr_String_replace(&ssPt->text, text)))
			return err;
		if (ssPt->cryptic) {
			memset(ssPt->iv, 0, sizeof(MCR_AES_BLOCK_SIZE + 1));
			memset(ssPt->tag, 0, sizeof(MCR_AES_TAG_SIZE + 1));
			ssPt->cryptic = false;
		}
	}
	return err;
}

bool mcr_SafeString_cryptic(struct mcr_SafeString * ssPt)
{
	return ssPt ? ssPt->cryptic : false;
}

int mcr_SafeString_set_cryptic(struct mcr_SafeString *ssPt, bool cryptic)
{
	mcr_String strMem;
	int err = 0;
	dassert(ssPt);
	if ((! !cryptic) == (! !ssPt->cryptic)
		|| MCR_STR_IS_EMPTY(ssPt->text))
		return 0;
	mcr_String_init(&strMem);
	if (cryptic) {
		if ((err = mcr_randomize(ssPt->iv, MCR_AES_BLOCK_SIZE)))
			return err;
		ssPt->iv[MCR_AES_BLOCK_SIZE] = '\0';

		if ((err = mcr_String_encrypt(&ssPt->text, ssPt->tag,
					(const unsigned char *)_pass, ssPt->iv,
					&strMem, true))) {
			mcr_String_free(&strMem);
			return err;
		}
		mcr_String_free(&ssPt->text);
		ssPt->text = strMem;
		ssPt->cryptic = true;
	} else {
		if ((err = mcr_String_encrypt(&ssPt->text,
					(const unsigned char *)_pass, ssPt->iv,
					ssPt->tag, &strMem, false))) {
			mcr_String_free(&strMem);
			return err;
		}
		/* Switch current from encrypted to decrypted version */
		mcr_String_free(&ssPt->text);
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
	mcr_String_free(&strMem);
	return err;
}

int mcr_String_encrypt(const mcr_String * strOriginalPt,
	const unsigned char *pass, const unsigned char *iv,
	unsigned char *tag, mcr_String * strPtOut, bool toEncryptedFlag)
{
	size_t len = MCR_STR_LEN(*strOriginalPt);
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
				(toEncryptedFlag ? len >> 1 : 0))))
		return err;
	if (toEncryptedFlag)
		eLen = mcr_encrypt((const unsigned char *)
			strOriginalPt->array, (unsigned int)(len), NULL,
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
	strPtOut->used = eLen;
	fixme;
	dprint("Original len = %lu, modified len = %lu, "
		"encFlag = %d\n", (long unsigned)strOriginalPt->used,
		(long unsigned)strPtOut->used, toEncryptedFlag);
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

int mcr_safe_string_initialize(struct mcr_context *ctx)
{
	int ret = mcr_randomize((unsigned char *)_pass,
		MCR_AES_BLOCK_SIZE);
	_pass[MCR_AES_BLOCK_SIZE] = '\0';
	UNUSED(ctx);
	return ret;
}

void mcr_safe_string_cleanup(struct mcr_context *ctx)
{
	UNUSED(ctx);
}
