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
#include "mcr/modules.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* All data here will be dynamically allocated. */
static int compare_args(const struct mcr_Command *lhs,
	const struct mcr_Command *rhs);
static int compare_env(const struct mcr_Command *lhs,
	const struct mcr_Command *rhs);
static int copy_args(struct mcr_Command *dstPt, struct mcr_Command *srcPt);
static int copy_env(struct mcr_Command *dstPt, struct mcr_Command *srcPt);

static int set_signal_delays(struct mcr_Array *sigArrPt,
	struct mcr_ISignal *inoop, struct timespec delay);
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

void mcr_Command_init(void *cmdDataPt)
{
	struct mcr_Command *cmdPt = cmdDataPt;
	if (cmdPt) {
		memset(cmdDataPt, 0, sizeof(struct mcr_Command));
		mcr_SafeString_init(&cmdPt->file);
		mcr_Array_init(&cmdPt->argv);
		mcr_Array_set_all(&cmdPt->argv, NULL,
			sizeof(struct mcr_SafeString));
		mcr_Array_init(&cmdPt->env);
		mcr_Array_set_all(&cmdPt->env, NULL,
			sizeof(struct mcr_SafeString));
	}
}

void mcr_Command_free(void *cmdDataPt)
{
	struct mcr_Command *cmdPt = cmdDataPt;
	if (cmdPt) {
		mcr_SafeString_free(&cmdPt->file);
		MCR_ARR_FOR_EACH(cmdPt->argv,
			MCR_EXP(mcr_SafeString_free_foreach),);
		MCR_ARR_FOR_EACH(cmdPt->env,
			MCR_EXP(mcr_SafeString_free_foreach),);
		mcr_Array_free(&cmdPt->argv);
		mcr_Array_free(&cmdPt->env);
	}
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
			if ((ret = compare_args(lPt, rPt)))
				return ret;
			return compare_env(lPt, rPt);
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_Command_copy(void *dstPt, void *srcPt)
{
	struct mcr_Command *dPt = dstPt, *sPt = srcPt;
	dassert(dstPt);
	if (sPt) {
		return mcr_SafeString_copy(&dPt->file, &sPt->file) &&
			copy_args(dPt, sPt) && copy_env(dPt, sPt);
	}
	mcr_Command_free(dPt);
	mcr_Command_init(dPt);
	return 0;
}

void mcr_StringKey_init(void *skDataPt)
{
	struct mcr_StringKey *skPt = skDataPt;
	if (skPt) {
		memset(skDataPt, 0, sizeof(struct mcr_StringKey));
		skPt->interval.tv_nsec = 1000 * 1000 * 100;
	}
}

void mcr_StringKey_free(void *skDataPt)
{
	struct mcr_StringKey *skPt = skDataPt;
	if (skPt) {
		mcr_SafeString_free(&skPt->string);
	}
}

int mcr_StringKey_compare(const void *lhs, const void *rhs)
{
	const struct mcr_StringKey *lPt = lhs, *rPt = rhs;
	int ret;
	if (rhs) {
		if (lhs) {
			if (lhs == rhs)
				return 0;
			if ((ret = MCR_CMP_INTEGRAL(lPt->interval.tv_sec,
						rPt->interval.tv_sec)))
				return ret;
			if ((ret = MCR_CMP_INTEGRAL(lPt->interval.tv_nsec,
						rPt->interval.tv_nsec)))
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
	mcr_StringKey_free(dPt);
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
	MCR_ARR_FOR_EACH(cmdPt->argv, MCR_EXP(mcr_SafeString_free_foreach),);
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
			(err = mcr_Array_push(&cmdPt->argv, &addString, 0))) {
			mcr_SafeString_free(&addString);
			return err;
		}
	}
	return err;
}

void mcr_Command_clear_args(struct mcr_Command *cmdPt)
{
	dassert(cmdPt);
	MCR_ARR_FOR_EACH(cmdPt->argv, MCR_EXP(mcr_SafeString_free_foreach),);
	mcr_Array_clear(&cmdPt->argv);
}

int mcr_Command_set_env(struct mcr_Command *cmdPt, char *const envp[],
	bool cryptic)
{
	size_t i;
	int err = 0;
	dassert(cmdPt);
	MCR_ARR_FOR_EACH(cmdPt->env, MCR_EXP(mcr_SafeString_free_foreach),);
	cmdPt->env.used = 0;
	for (i = 0; envp[i] != NULL; i++) {
		if ((err = mcr_Command_add_env(cmdPt, envp[i], cryptic)))
			return err;
	}
	mcr_Array_trim(&cmdPt->env);
	return err;
}

int mcr_Command_add_env(struct mcr_Command *cmdPt, char *const env,
	bool cryptic)
{
	struct mcr_SafeString addString;
	int err = 0;
	dassert(cmdPt);
	if (env) {
		mcr_SafeString_init(&addString);
		if ((err = mcr_SafeString_set_text(&addString, env, cryptic)) ||
			(err = mcr_Array_push(&cmdPt->env, &addString, 0))) {
			mcr_SafeString_free(&addString);
			return err;
		}
	}
	return err;
}

void mcr_Command_clear_env(struct mcr_Command *cmdPt)
{
	dassert(cmdPt);
	MCR_ARR_FOR_EACH(cmdPt->env, MCR_EXP(mcr_SafeString_free_foreach),);
	mcr_Array_clear(&cmdPt->env);
}

/* TODO: Unicode support. */
fixme;
int mcr_StringKey_send(struct mcr_Signal *sigPt)
{
	struct mcr_StringKey *skPt = mcr_SK_data(sigPt);
	struct mcr_context *ctx = ((struct mcr_CtxISignal *)sigPt->isig)->ctx;
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
			mcr_String_free(&string);
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
				end = MCR_ARR_END(*found);
				for (sigPt = MCR_ARR_FIRST(*found);
					sigPt < end; sigPt++) {
					if ((err = mcr_send(ctx, sigPt)))
						lastErr = err;
					if ((err = thrd_sleep(&skPt->interval,
								NULL)) !=
						thrd_success) {
						lastErr = mcr_thrd_errno(err);
						mset_error(lastErr);
					}
				}
			}
		}
	}
	mcr_String_free(&string);
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
	size_t i, argsUsed = cmdPt->argv.used, envUsed = cmdPt->env.used;
	char **args = NULL, **envies = NULL;
	mcr_String file;
	struct mcr_Array argsArr, envArr;
	mcr_String_init(&file);
	mcr_Array_init(&envArr);
	mcr_Array_set_all(&envArr, NULL, sizeof(mcr_String));
	argsArr = envArr;
	if ((err = mcr_SafeString_text(&cmdPt->file, &file))) {
		goto cleanup;
	}
	if (MCR_STR_IS_EMPTY(file)) {
		goto cleanup;
	}
	/* +1, extra space for NULL termination
	 * args + 1, file is required first */
	args = malloc(sizeof(char *) * (argsUsed + 2));
	envies = malloc(sizeof(char *) * (envUsed + 1));
	if (!args || !envies) {
		err = ENOMEM;
		mset_error(ENOMEM);
		goto cleanup;
	}
	if ((err = mcr_Array_resize(&argsArr, argsUsed)) ||
		(err = mcr_Array_resize(&envArr, envUsed))) {
		goto cleanup;
	}
	/* For each argument, push argument and set into arg array */
	/* Also null terminate. */
	args[0] = file.array;
	i = 1;
	MCR_ARR_FOR_EACH(cmdPt->argv, ssget_foreach, &argsArr, args, &i);
	args[argsUsed + 1] = NULL;
	i = 0;
	MCR_ARR_FOR_EACH(cmdPt->env, ssget_foreach, &envArr, envies, &i);
	envies[envUsed] = NULL;

	err = mcr_execvpe(file.array, args, envies);
 cleanup:
	if (args)
		free(args);
	if (envies)
		free(envies);
	MCR_ARR_FOR_EACH(argsArr, MCR_EXP(mcr_String_free_foreach),);
	MCR_ARR_FOR_EACH(envArr, MCR_EXP(mcr_String_free_foreach),);
	mcr_Array_free(&argsArr);
	mcr_Array_free(&envArr);
	mcr_String_free(&file);

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
	MCR_ARR_FOR_EACH(*arrPt, MCR_EXP(mcr_Signal_free_foreach),);
	if (charSignalArr && arrLen) {
		mcr_Array_clear(arrPt);
		if ((err = mcr_Array_resize(arrPt, arrLen)))
			return err;
		mcr_Signal_init(&emptySig);
		mcr_Signal_init(&cpy);
		for (i = 0; i < arrLen; i++) {
			if ((err = mcr_Signal_copy(&cpy, charSignalArr + i)) ||
				(err = mcr_Array_push(arrPt, &cpy, false))) {
				mcr_Signal_free(&cpy);
				return err;
			}
			cpy = emptySig;
		}
	} else {
		mcr_Array_free(arrPt);
	}
	return err;
}

void mcr_StringKey_remove_char(struct mcr_context *ctx, int character)
{
	struct mcr_Array *arrPt = mcr_Array_element(&ctx->extras.key_chars,
		(unsigned)character);
	if (arrPt) {
		MCR_ARR_FOR_EACH(*arrPt, MCR_EXP(mcr_Signal_free_foreach),);
		if ((unsigned)character ==
			MCR_ARR_LAST_INDEX(ctx->extras.key_chars)) {
			mcr_Array_free(arrPt);
			mcr_Array_pop(&ctx->extras.key_chars);
		} else
			mcr_Array_clear(arrPt);
	}
}

int mcr_StringKey_set_char_keys(struct mcr_context *ctx, int character,
	int key, long nsec, bool shiftFlag)
{
	int i, ret;
	int shift = mcr_Key_mod_key(ctx, MCR_SHIFT);
	mcr_NoOp delay = { 0 };
	struct mcr_Signal sigs[7];
	struct mcr_ISignal *ikey = mcr_iKey(ctx), *inoop = mcr_iNoOp(ctx);
	if (shift == -1) {
		mset_error(EINVAL);
		return -EINVAL;
	}
	delay.tv_nsec = nsec;
	memset(sigs, 0, sizeof(sigs));
	// Evens are keys
	for (i = 0; i < 7; i += 2) {
		sigs[i].isig = ikey;
		MCR_INST_RESET(sigs[i].inst);
		if (!MCR_INST_IS_HEAP(sigs[i].inst)) {
			mset_error(ENOMEM);
			ret = ENOMEM;
			goto cleanout;
		}
	}
	// Odds are delays
	for (i = 1; i < 7; i += 2) {
		sigs[i].isig = inoop;
		sigs[i].inst.data.data = &delay;
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
		mcr_Signal_free(sigs + i);
	}
	return ret;
}

int mcr_StringKey_set_delays(struct mcr_context *ctx, struct timespec delay)
{
	int err = 0;
	char *itPt, *end;
	size_t bytes;
	struct mcr_ISignal *inoop = mcr_iNoOp(ctx);
	MCR_ARR_ITER(ctx->extras.key_chars, itPt, end, bytes);
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
	MCR_ARR_ITER(ctx->extras.key_chars, itPt, end, bytes);
	while (itPt < end) {
		arrPt = (struct mcr_Array *)itPt;
		MCR_ARR_FOR_EACH(*arrPt, MCR_EXP(mcr_Signal_free_foreach),);
		mcr_Array_free(arrPt);
		itPt += bytes;
	}
	mcr_Array_clear(&ctx->extras.key_chars);
}

void mcr_StringKey_char_trim(struct mcr_context *ctx)
{
	struct mcr_Array *arrPt;
	char *first, *rIt;
	size_t bytes;
	MCR_ARR_ITER(ctx->extras.key_chars, first, rIt, bytes);
	if (first) {
		rIt -= bytes;
		while (rIt >= first) {
			arrPt = (struct mcr_Array *)rIt;
			if (arrPt->used)
				break;
			mcr_Array_free(arrPt);
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

static int compare_env(const struct mcr_Command *lhs,
	const struct mcr_Command *rhs)
{
	size_t i = 0, used = lhs->env.used;
	int ret = 0;
	struct mcr_SafeString *lss = MCR_ARR_FIRST(lhs->env),
		*rss = MCR_ARR_FIRST(rhs->env);
	if (used != rhs->env.used)
		return MCR_CMP_INTEGRAL(used, rhs->env.used);
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
	MCR_ARR_FOR_EACH(dstPt->argv, MCR_EXP(mcr_SafeString_free_foreach),)
		mcr_Array_clear(&dstPt->argv);
	for (i = 0; i < srcPt->argv.used; i++) {
		if (ssPt[i].cryptic) {
			if ((err = mcr_SafeString_text(ssPt + i, &arr)) ||
				(err = mcr_Command_add_arg(dstPt, arr.array,
						true))) {
				mcr_String_free(&arr);
				return err;
			}
			mcr_String_free(&arr);
		} else {
			if ((err = mcr_Command_add_arg(dstPt,
						ssPt[i].text.array, 0)))
				return err;
		}
	}
	return err;
}

static int copy_env(struct mcr_Command *dstPt, struct mcr_Command *srcPt)
{
	mcr_String arr;
	struct mcr_SafeString *ssPt = MCR_ARR_FIRST(srcPt->env);
	size_t i;
	int err = 0;
	mcr_String_init(&arr);
	MCR_ARR_FOR_EACH(dstPt->env, MCR_EXP(mcr_SafeString_free_foreach),)
		mcr_Array_clear(&dstPt->env);
	for (i = 0; i < srcPt->env.used; i++) {
		if (ssPt[i].cryptic) {
			if ((err = mcr_SafeString_text(ssPt + i, &arr)) ||
				(err = mcr_Command_add_env(dstPt, arr.array,
						true))) {
				mcr_String_free(&arr);
				return err;
			}
			mcr_String_free(&arr);
		} else {
			if ((err = mcr_Command_add_env(dstPt,
						ssPt[i].text.array, false)))
				return err;
		}
	}
	return err;
}

int mcr_signal_extras_initialize(struct mcr_context *ctx)
{
	struct mcr_ISignal *isigPt = mcr_iStringKey(ctx);
	int err = 0;
	mcr_ISignal_init(isigPt);
	mcr_iset_all(isigPt, mcr_StringKey_compare,
		mcr_StringKey_copy, sizeof(struct mcr_StringKey),
		mcr_StringKey_init, mcr_StringKey_free);
	isigPt->send = mcr_StringKey_send;
	isigPt = mcr_iCommand(ctx);
	mcr_ISignal_init(isigPt);
	mcr_iset_all(isigPt, mcr_Command_compare,
		mcr_Command_copy, sizeof(struct mcr_Command),
		mcr_Command_init, mcr_Command_free);
	isigPt->send = mcr_Command_send;
	mcr_Array_init(&ctx->extras.key_chars);
	mcr_Array_set_all(&ctx->extras.key_chars, NULL,
		sizeof(struct mcr_Array));

	err = mcr_register(mcr_ISignal_reg(ctx), mcr_iStringKey(ctx), NULL,
		NULL, 0);
	if (!err)
		err = mcr_register(mcr_ISignal_reg(ctx), mcr_iCommand(ctx),
			NULL, NULL, 0);
	return err;
}

void mcr_signal_extras_cleanup(struct mcr_context *ctx)
{
	mcr_StringKey_char_clear(ctx);
	mcr_Array_free(&ctx->extras.key_chars);
}

static int set_signal_delays(struct mcr_Array *sigArrPt,
	struct mcr_ISignal *inoop, struct timespec delay)
{
	char *itPt, *end;
	size_t bytes;
	struct mcr_Signal *sigPt;
	mcr_NoOp *dataPt;
	MCR_ARR_ITER(*sigArrPt, itPt, end, bytes);
	while (itPt < end) {
		sigPt = (struct mcr_Signal *)itPt;
		if (sigPt->isig == inoop) {
			dataPt = mcr_NoOp_data(sigPt);
			if (!dataPt) {
				mcr_inst_reset(sigPt);
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
			err = mcr_Array_push(argArrPt, &str, false);
			argsArr[(*argsItPt)++] = str.array;
		}
	}
	mcr_String_free(&str);
	return err;
}
