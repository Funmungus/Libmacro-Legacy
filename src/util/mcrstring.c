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

#include "mcr/util/util.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const struct mcr_Interface _MCR_ISTRING = {
	.id = (size_t)-1,
	.compare = mcr_String_compare,
	.copy = mcr_String_copy,
	.data_size = sizeof(mcr_String),
	.init = mcr_String_init,
	.deinit = mcr_String_deinit
};

const struct mcr_Interface *mcr_String_interface()
{
	return &_MCR_ISTRING;
}

int mcr_atob(const char *value, bool * retVal)
{
	dassert(retVal);
	if (value == NULL) {
		*retVal = false;
		mset_error_return(EINVAL);
	}
	while (isspace(*value))
		++value;
	if (*value == '\0') {
		*retVal = false;
		return EINVAL;
	}

	if (isdigit(*value)) {
		if (*value == '0') {
			*retVal = false;
		} else {
			*retVal = true;
		}
		return 0;
	}

	if (value[1] == '\0') {
		if (*value == 't' || *value == 'T' ||
			*value == 'y' || *value == 'Y') {
			*retVal = true;
		} else if (*value == 'f' || *value == 'F' ||
				   *value == 'n' || *value == 'N') {
			*retVal = false;
		}
	} else {
		if (!mcr_casecmp(value, "true") || !mcr_casecmp(value, "yes")) {
			*retVal = true;
		} else if (!mcr_casecmp(value, "false") ||
				   !mcr_casecmp(value, "no")) {
			*retVal = false;
		}
	}
	return 0;
}

/// \todo Allow wchar_t
int mcr_String_init(void *strPt)
{
	if (strPt) {
		mcr_Array_init(strPt);
		mcr_Array_set_all(strPt, NULL, sizeof(char));
	}
	return 0;
}

mcr_String mcr_String_new()
{
	mcr_String ret = { 0 };
	ret.element_size = sizeof(char);
	return ret;
}

int mcr_String_copy(void *dstPt, const void *srcPt)
{
	dassert(dstPt);
	return mcr_String_replace(dstPt, srcPt ? *(const char * const*)srcPt : NULL);
}

void mcr_String_clear(mcr_String * strPt)
{
	dassert(strPt);
	MCR_STR_CLEAR(*strPt);
}

int mcr_String_insert(mcr_String * strPt, size_t index,
					  const char *str, size_t len)
{
	dassert(strPt);
	dassert(strPt->element_size == 1);
	if (!str || !len || !(len = strnlen(str, len)))
		return 0;
	if (mcr_String_smartlen(strPt, len))
		return mcr_err;
	if (!strPt->used) {
		MCR_STR_CLEAR(*strPt);
	}
	/* out of bounds, cannot insert after null terminator index */
	if (index >= strPt->used)
		mset_error_return(EFAULT);
	mcr_err = 0;
	/* memmove will also copy null char */
	memmove((char *)strPt->array + index + len, (char *)strPt->array + index,
			strPt->used - index);
	memcpy((char *)strPt->array + index, str, len);
	strPt->used += len;
	return 0;
}

void mcr_String_remove_index(mcr_String * strPt, size_t index,
							 size_t count)
{
	dassert(strPt);
	dassert(strPt->element_size == 1);
	/* Index out of bounds.  Cannot remove null char, last element */
	if (!strPt->used || index >= strPt->used - 1)
		return;
	/* Nothing to move back, just null-terminate. */
	if (index + count >= strPt->used - 1) {
		((char *)strPt->array)[index] = '\0';
		strPt->used = index + 1;
	} else {
		/* Move into index, position at index + count, size is */
		/* array size - source position. */
		memmove((char *)strPt->array + index, (char *)strPt->array + index + count,
				strPt->used - index - count);
		strPt->used -= count;
	}
}

int mcr_String_append(mcr_String * strPt, const char *str, size_t len)
{
	dassert(strPt);
	return mcr_String_insert(strPt, strPt->used ? strPt->used - 1 : 0,
							 str, len);
}

int mcr_String_push(mcr_String * strPt, const char c)
{
	dassert(strPt);
	dassert(strPt->element_size == 1);
	if (mcr_String_smartlen(strPt, 1))
		return mcr_err;
	if (!strPt->used)
		++strPt->used;
	((char *)strPt->array)[strPt->used - 1] = c;
	((char *)strPt->array)[strPt->used++] = '\0';
	return 0;
}

char mcr_String_pop(mcr_String * strPt)
{
	char ret;
	dassert(strPt);
	dassert(strPt->element_size == 1);
	if (MCR_STR_IS_EMPTY(*strPt))
		return '\0';
	ret = ((char *)strPt->array)[strPt->used - 2];
	((char *)strPt->array)[--strPt->used - 1] = '\0';
	return ret;
}

int mcr_String_nreplace(mcr_String * strPt, const char *str, size_t len)
{
	dassert(strPt);
	dassert(strPt->element_size == 1);
	if (!str) {
		mcr_String_deinit(strPt);
		return 0;
	}
	len = strnlen(str, len);
	if (mcr_Array_resize(strPt, len + 1))
		return mcr_err;
	memcpy(strPt->array, str, len);
	((char *)strPt->array)[len] = '\0';
	strPt->used = len + 1;
	return 0;
}

int mcr_String_copy_str(mcr_String * strPt, size_t index,
						const char *str, size_t len)
{
	dassert(strPt);
	dassert(strPt->element_size == 1);
	if (!str || !len || !(len = strnlen(str, len)))
		return 0;
	if (!strPt->used) {
		index = 0;
	} else if (index >= strPt->used) {
		index = strPt->used - 1;
	}
	if (mcr_Array_minused(strPt, index + len + 1))
		return mcr_err;
	memcpy((char *)strPt->array + index, str, len);
	((char *)strPt->array)[len] = '\0';
	return 0;
}

int mcr_String_set(mcr_String * strPt, size_t index, char c)
{
	dassert(strPt);
	dassert(strPt->element_size == 1);
	if (!strPt->used || index >= strPt->used - 1)
		return mcr_String_push(strPt, c);
	dassert(index < strPt->used - 1);
	((char *)strPt->array)[index] = c;
	return 0;
}

int mcr_String_fill(mcr_String * strPt, size_t index, size_t count,
					char c)
{
	dassert(strPt);
	dassert(strPt->element_size == 1);
	if (!strPt->used) {
		index = 0;
	} else if (index >= strPt->used) {
		index = strPt->used - 1;
	}
	if (mcr_Array_minused(strPt, index + count + 1))
		return mcr_err;
	memset((char *)strPt->array + index, c, count);
	((char *)strPt->array)[strPt->used - 1] = '\0';
	return 0;
}

int mcr_String_move(mcr_String * dstPt, size_t dstPos,
					mcr_String * srcPt, size_t srcPos, size_t count)
{
	size_t srcLastPos, dstLastPos;
	dassert(dstPt);
	dassert(srcPt);
	dassert(dstPt->element_size == 1);
	dassert(srcPt->element_size == 1);
	/* Limit to copying/moving existing items. */
	srcLastPos = srcPt->used - 1;
	dstLastPos = dstPt->used - 1;
	if (!count || MCR_STR_IS_EMPTY(*srcPt) || srcPos >= srcLastPos)
		return 0;
	if (count > srcLastPos || srcPos + count > srcLastPos)
		count = srcLastPos - srcPos;
	if (dstPos > dstLastPos)
		dstPos = dstLastPos;
	if (mcr_Array_minused(dstPt, dstPos + count + 1))
		return mcr_err;
	void *copyDst = (char *)dstPt->array + dstPos,
		  *copySrc = (char *)srcPt->array + srcPos;
	if (dstPt->array == srcPt->array) {
		memmove(copyDst, copySrc, count);
	} else {
		memcpy(copyDst, copySrc, count);
	}
	((char *)dstPt->array)[dstPt->used - 1] = '\0';
	return 0;
}
