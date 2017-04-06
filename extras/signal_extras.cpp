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

/* All data here will be dynamically allocated. */
static int compare_args(const struct mcr_Command *lhs,
			const struct mcr_Command *rhs);
static int copy_args(struct mcr_Command *dstPt, struct mcr_Command *srcPt);

static int set_signal_delays(struct mcr_Array *sigArrPt,
			     struct mcr_ISignal *inoop, struct mcr_NoOp delay);
static int ssget_foreach(struct mcr_SafeString *ssPt,
			 struct mcr_Array *argArrPt, char **argsArr, size_t * argsItPt);

struct mcr_ISignal *mcr_iCommand(struct mcr_context *ctx)
{
	return &ctx->extras.icommand;
}

struct mcr_ISignal *mcr_iStringKey(struct mcr_context *ctx)
{
	return &ctx->extras.istring_key.isignal;
}

int mcr_Command_init(void *cmdPt)
{
	struct mcr_Command *localPt = cmdPt;
	if (localPt) {
		memset(cmdPt, 0, sizeof(struct mcr_Command));
		mcr_SafeString_init(&localPt->file);
		localPt->argv =
			mcr_Array_new(NULL, sizeof(struct mcr_SafeString));
	}
	return 0;
}

struct mcr_Command mcr_Command_new()
{
	struct mcr_Command ret;
	mcr_Command_init(&ret);
	return ret;
}

int mcr_Command_deinit(void *cmdPt)
{
	struct mcr_Command *localPt = cmdPt;
	if (localPt) {
		mcr_SafeString_deinit(&localPt->file);
		MCR_ARR_FOR_EACH(localPt->argv, mcr_SafeString_deinit);
		mcr_Array_deinit(&localPt->argv);
	}
	return 0;
}

int mcr_Command_compare(const void *lhs, const void *rhs)
{
	const struct mcr_Command *lPt = lhs, *rPt = rhs;
	int ret;
	if (rhs) {
		if (lhs) {
			if (lhs == rhs)
				return 0;
			if ((ret = mcr_SafeString_compare(&lPt->file,
							  &rPt->file)))
				return ret;
			return compare_args(lPt, rPt);
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_Command_copy(void *dstPt, void *srcPt)
{
	int err;
	struct mcr_Command *dPt = dstPt, *sPt = srcPt;
	dassert(dstPt);
	if (sPt) {
		if ((err = mcr_SafeString_copy(&dPt->file, &sPt->file)))
			return err;
		return copy_args(dPt, sPt);
	}
	mcr_Command_deinit(dPt);
	mcr_Command_init(dPt);
	return 0;
}

int mcr_StringKey_init(void *skPt)
{
	struct mcr_StringKey *localPt = skPt;
	if (localPt) {
		memset(skPt, 0, sizeof(struct mcr_StringKey));
		localPt->interval.msec = 100;
		mcr_SafeString_init(&localPt->string);
	}
	return 0;
}

struct mcr_StringKey mcr_StringKey_new()
{
	struct mcr_StringKey ret;
	mcr_StringKey_init(&ret);
	return ret;
}

int mcr_StringKey_deinit(void *skPt)
{
	struct mcr_StringKey *localPt = skPt;
	if (localPt) {
		mcr_SafeString_deinit(&localPt->string);
	}
	return 0;
}

int mcr_StringKey_compare(const void *lhs, const void *rhs)
{
	const struct mcr_StringKey *lPt = lhs, *rPt = rhs;
	int ret;
	if (rhs) {
		if (lhs) {
			if (lhs == rhs)
				return 0;
			if ((ret = MCR_CMP_INTEGRAL(lPt->interval.sec,
						    rPt->interval.sec)))
				return ret;
			if ((ret = MCR_CMP_INTEGRAL(lPt->interval.msec,
						    rPt->interval.msec)))
				return ret;
			return mcr_SafeString_compare(&lPt->string,
						      &rPt->string);
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_StringKey_copy(void *dstPt, void *srcPt)
{
	struct mcr_StringKey *dPt = dstPt, *sPt = srcPt;
	dassert(dstPt);
	if (sPt) {
		dPt->interval = sPt->interval;
		return mcr_SafeString_copy(&dPt->string, &sPt->string);
	}
	mcr_StringKey_deinit(dPt);
	mcr_StringKey_init(dPt);
	return 0;
}

int mcr_Command_set_file(struct mcr_Command *cmdPt, const char *file,
			 bool cryptic)
{
	dassert(cmdPt);
	return mcr_SafeString_set_text(&cmdPt->file, file, cryptic);
}

int mcr_Command_set_args(struct mcr_Command *cmdPt, char *const argv[],
			 bool cryptic)
{
	size_t i = 0;
	int err = 0;
	dassert(cmdPt);
	MCR_ARR_FOR_EACH(cmdPt->argv, mcr_SafeString_deinit);
	cmdPt->argv.used = 0;
	for (i = 0; argv[i] != NULL; i++) {
		if ((err = mcr_Command_add_arg(cmdPt, argv[i], cryptic)))
			return err;
	}
	mcr_Array_trim(&cmdPt->argv);
	return err;
}

int mcr_Command_add_arg(struct mcr_Command *cmdPt, char *const arg,
			bool cryptic)
{
	struct mcr_SafeString addString;
	int err = 0;
	dassert(cmdPt);
	if (arg) {
		mcr_SafeString_init(&addString);
		if ((err = mcr_SafeString_set_text(&addString, arg, cryptic)) ||
		    (err = mcr_Array_push(&cmdPt->argv, &addString))) {
			mcr_SafeString_deinit(&addString);
			return err;
		}
	}
	return err;
}

void mcr_Command_clear_args(struct mcr_Command *cmdPt)
{
	dassert(cmdPt);
	MCR_ARR_FOR_EACH(cmdPt->argv, mcr_SafeString_deinit);
	mcr_Array_clear(&cmdPt->argv);
}

/* TODO: Unicode support. */
fixme;
int mcr_StringKey_send(struct mcr_Signal *sigPt)
{
	struct mcr_StringKey *skPt = mcr_SK_data(sigPt);
	struct mcr_context *ctx =
		((struct mcr_CtxISignal *)sigPt->isignal)->ctx;
	dassert(sigPt);
	if (!skPt)
		return 0;
	return mcr_StringKey_send_data(ctx, skPt);
}

int mcr_StringKey_send_data(struct mcr_context *ctx, struct mcr_StringKey *skPt)
{
	int err = 0, lastErr = 0;
	struct mcr_Array *found;
	struct mcr_Signal *sigPt, *end;
	mcr_String string;
	mcr_String *strPt = &string;
	unsigned char *i;
	dassert(skPt);
	dassert(ctx);
	mcr_String_init(&string);
	if (skPt->string.cryptic) {
		if ((err = mcr_SafeString_text(&skPt->string, strPt))) {
			mcr_String_deinit(&string);
			return err;
		}
	} else {
		strPt = &skPt->string.text;
	}
	if (strPt->used) {
		dassert(strPt->array[strPt->used - 1] == '\0');
		for (i = (unsigned char *)strPt->array; *i; i++) {
			found = MCR_ARR_ELEMENT(ctx->extras.key_chars, *i);
			if (found && found->used) {
				end = mcr_Array_end(found);
				for (sigPt = mcr_Array_first(found);
				     sigPt < end; sigPt++) {
					if ((err = mcr_send(ctx, sigPt)))
						lastErr = err;
				}
				mcr_NoOp_send_data(&skPt->interval);
			}
		}
	}
	mcr_String_deinit(&string);
	return lastErr;
}

int mcr_Command_send(struct mcr_Signal *sigPt)
{
	struct mcr_Command *cmdPt = mcr_Cmd_data(sigPt);
	dassert(sigPt);
	return cmdPt ? mcr_Command_send_data(cmdPt) : 0;
}

int mcr_Command_send_data(struct mcr_Command *cmdPt)
{
	int err = 0;
	size_t i, argsUsed = cmdPt->argv.used;
	char **args = NULL;
	mcr_String file;
	struct mcr_Array argsArr;
	mcr_String_init(&file);
	mcr_Array_init(&argsArr);
	mcr_Array_set_all(&argsArr, NULL, sizeof(mcr_String));
	if ((err = mcr_SafeString_text(&cmdPt->file, &file))) {
		goto cleanup;
	}
	if (MCR_STR_IS_EMPTY(file)) {
		goto cleanup;
	}
	/* +1, extra space for NULL termination
	 * args + 1, file is required first */
	args = malloc(sizeof(char *) * (argsUsed + 2));
	if (!args) {
		err = ENOMEM;
		mset_error(ENOMEM);
		goto cleanup;
	}
	if ((err = mcr_Array_resize(&argsArr, argsUsed))) {
		goto cleanup;
	}
	/* For each argument, push argument and set into arg array */
	/* Also null terminate. */
	args[0] = file.array;
	i = 1;
#define localExp(itPt) \
ssget_foreach(itPt, &argsArr, args, &i);
	MCR_ARR_FOR_EACH(cmdPt->argv, localExp);
#undef localExp
	args[argsUsed + 1] = NULL;

	err = mcr_execvp(file.array, args);
cleanup:
	if (args)
		free(args);
	MCR_ARR_FOR_EACH(argsArr, mcr_String_deinit);
	mcr_Array_deinit(&argsArr);
	mcr_String_deinit(&file);

	return err;
}

/* */
/* String key development */
/* */
struct mcr_Array *mcr_StringKey_char(struct mcr_context *ctx, int character)
{
	return MCR_ARR_ELEMENT(ctx->extras.key_chars, (unsigned)character);
}

int mcr_StringKey_set_char(struct mcr_context *ctx, int character,
			   struct mcr_Signal *charSignalArr, size_t arrLen)
{
	struct mcr_Array initial, *arrPt;
	int err = 0;
	struct mcr_Signal emptySig, cpy;
	size_t i;
	mcr_Array_init(&initial);
	mcr_Array_set_all(&initial, NULL, sizeof(struct mcr_Signal));
	if ((err = mcr_Array_minfill(&ctx->extras.key_chars,
				     (unsigned)character + 1, &initial)))
		return err;
	arrPt = MCR_ARR_ELEMENT(ctx->extras.key_chars, (unsigned)character);
	dassert(arrPt);
	MCR_ARR_FOR_EACH(*arrPt, mcr_Signal_deinit);
	if (charSignalArr && arrLen) {
		mcr_Array_clear(arrPt);
		if ((err = mcr_Array_resize(arrPt, arrLen)))
			return err;
		mcr_Signal_init(&emptySig);
		mcr_Signal_init(&cpy);
		for (i = 0; i < arrLen; i++) {
			if ((err = mcr_Signal_copy(&cpy, charSignalArr + i)) ||
			    (err = mcr_Array_push(arrPt, &cpy))) {
				mcr_Signal_deinit(&cpy);
				return err;
			}
			cpy = emptySig;
		}
	} else {
		mcr_Array_deinit(arrPt);
	}
	return err;
}

void mcr_StringKey_remove_char(struct mcr_context *ctx, int character)
{
	struct mcr_Array *arrPt = mcr_Array_element(&ctx->extras.key_chars,
				  (unsigned)character);
	if (arrPt) {
		MCR_ARR_FOR_EACH(*arrPt, mcr_Signal_deinit);
		if ((unsigned)character ==
		    MCR_ARR_LAST_INDEX(ctx->extras.key_chars)) {
			mcr_Array_deinit(arrPt);
			mcr_Array_pop(&ctx->extras.key_chars);
		} else
			mcr_Array_clear(arrPt);
	}
}

int mcr_StringKey_set_char_keys(struct mcr_context *ctx, int character,
				int key, long msec, bool shiftFlag)
{
	int i, ret;
	int shift = mcr_Key_mod_key(ctx, MCR_SHIFT);
	struct mcr_NoOp delay = { 0 };
	struct mcr_Signal sigs[7];
	struct mcr_ISignal *ikey = mcr_iKey(ctx), *inoop = mcr_iNoOp(ctx);
	if (shift == -1) {
		mset_error(EINVAL);
		return EINVAL;
	}
	delay.msec = msec;
	memset(sigs, 0, sizeof(sigs));
	// Evens are keys
	for (i = 0; i < 7; i += 2) {
		sigs[i].isignal = ikey;
		mcr_Instance_reset(&sigs[i].instance);
		if (!mcr_Instance_is_heap(&sigs[i].instance)) {
			mset_error(ENOMEM);
			ret = ENOMEM;
			goto cleanout;
		}
	}
	// Odds are delays
	for (i = 1; i < 7; i += 2) {
		sigs[i].isignal = inoop;
		sigs[i].instance.data.data = &delay;
	}
	mcr_Key_set_all(MCR_KEY_DATA(sigs[0]), shift, shift, MCR_DOWN);
	mcr_Key_set_all(MCR_KEY_DATA(sigs[2]), key, key, MCR_DOWN);
	mcr_Key_set_all(MCR_KEY_DATA(sigs[4]), key, key, MCR_UP);
	mcr_Key_set_all(MCR_KEY_DATA(sigs[6]), shift, shift, MCR_UP);
	/* Nonshift skips first key + delay, only have 2 keys and 1 delays */
	ret = shiftFlag ? mcr_StringKey_set_char(ctx, character, sigs, 7) :
	      mcr_StringKey_set_char(ctx, character, sigs + 2, 3);
cleanout:
	for (i = 7; i--;) {
		mcr_Signal_deinit(sigs + i);
	}
	return ret;
}

int mcr_StringKey_set_delays(struct mcr_context *ctx, struct mcr_NoOp delay)
{
	int err = 0;
	char *itPt, *end;
	size_t bytes;
	struct mcr_ISignal *inoop = mcr_iNoOp(ctx);
	mcr_Array_iter(&ctx->extras.key_chars, &itPt, &end, &bytes);
	while (itPt < end) {
		if ((err = set_signal_delays((struct mcr_Array *)itPt, inoop,
					     delay)))
			return err;
		itPt += bytes;
	}
	return err;
}

size_t mcr_StringKey_char_count(struct mcr_context * ctx)
{
	return ctx->extras.key_chars.used;
}

void mcr_StringKey_char_clear(struct mcr_context *ctx)
{
	struct mcr_Array *arrPt;
	char *itPt, *end;
	size_t bytes;
	mcr_Array_iter(&ctx->extras.key_chars, &itPt, &end, &bytes);
	while (itPt < end) {
		arrPt = (struct mcr_Array *)itPt;
		MCR_ARR_FOR_EACH(*arrPt, mcr_Signal_deinit);
		mcr_Array_deinit(arrPt);
		itPt += bytes;
	}
	mcr_Array_clear(&ctx->extras.key_chars);
}

void mcr_StringKey_char_trim(struct mcr_context *ctx)
{
	struct mcr_Array *arrPt;
	char *first, *rIt;
	size_t bytes;
	mcr_Array_iter(&ctx->extras.key_chars, &first, &rIt, &bytes);
	if (first) {
		rIt -= bytes;
		while (rIt >= first) {
			arrPt = (struct mcr_Array *)rIt;
			if (arrPt->used)
				break;
			mcr_Array_deinit(arrPt);
			mcr_Array_pop(&ctx->extras.key_chars);
			rIt -= bytes;
		}
	}
	mcr_Array_trim(&ctx->extras.key_chars);
}

static int compare_args(const struct mcr_Command *lhs,
			const struct mcr_Command *rhs)
{
	int ret = 0;
	size_t i = 0, used = lhs->argv.used;
	struct mcr_SafeString *lss = MCR_ARR_FIRST(lhs->argv),
			       *rss = MCR_ARR_FIRST(rhs->argv);
	if (used != rhs->argv.used)
		return MCR_CMP_INTEGRAL(used, rhs->argv.used);
	while (!ret && i < used) {
		ret = mcr_SafeString_compare(lss + i, rss + i);
		++i;
	}
	return ret;
}

static int copy_args(struct mcr_Command *dstPt, struct mcr_Command *srcPt)
{
	mcr_String arr;
	struct mcr_SafeString *ssPt = MCR_ARR_FIRST(srcPt->argv);
	size_t i;
	int err = 0;
	mcr_String_init(&arr);
	MCR_ARR_FOR_EACH(dstPt->argv, mcr_SafeString_deinit);
	mcr_Array_clear(&dstPt->argv);
	for (i = 0; i < srcPt->argv.used; i++) {
		if (ssPt[i].cryptic) {
			if ((err = mcr_SafeString_text(ssPt + i, &arr)) ||
			    (err = mcr_Command_add_arg(dstPt, arr.array,
						       true))) {
				mcr_String_deinit(&arr);
				return err;
			}
			mcr_String_deinit(&arr);
		} else {
			if ((err = mcr_Command_add_arg(dstPt,
						       ssPt[i].text.array, 0)))
				return err;
		}
	}
	return err;
}

static int set_signal_delays(struct mcr_Array *sigArrPt,
			     struct mcr_ISignal *inoop, struct mcr_NoOp delay)
{
	char *itPt, *end;
	size_t bytes;
	struct mcr_Signal *sigPt;
	struct mcr_NoOp *dataPt;
	mcr_Array_iter(sigArrPt, &itPt, &end, &bytes);
	while (itPt < end) {
		sigPt = (struct mcr_Signal *)itPt;
		if (sigPt->isignal == inoop) {
			dataPt = mcr_NoOp_data(sigPt);
			if (!dataPt) {
				mcr_Instance_reset(sigPt);
				dataPt = mcr_NoOp_data(sigPt);
				if (!dataPt) {
					mset_error(ENOMEM);
					return ENOMEM;
				}
			}
			*dataPt = delay;
		}
		itPt += bytes;
	}
	return 0;
}

static int ssget_foreach(struct mcr_SafeString *ssPt,
			 struct mcr_Array *argArrPt, char **argsArr, size_t * argsItPt)
{
	mcr_String str;
	int err = 0;
	mcr_String_init(&str);
	err = mcr_SafeString_text(ssPt, &str);
	if (!err) {
		if (!MCR_STR_IS_EMPTY(str)) {
			err = mcr_Array_push(argArrPt, &str);
			argsArr[(*argsItPt)++] = str.array;
			return err;
		}
	}
	mcr_String_deinit(&str);
	return err;
}
