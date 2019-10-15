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

#include "mcr/standard/standard.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "mcr/libmacro.h"

static void set_matcher(struct mcr_Stage *stagePt, struct mcr_IsStage *matchPt)
{
	if (matchPt) {
		stagePt->matcher = *matchPt;
	} else {
		stagePt->matcher.equals = NULL;
		stagePt->matcher.resembles = NULL;
	}
}

int mcr_Stage_init(void *stagePt)
{
	if (stagePt)
		memset(stagePt, 0, sizeof(struct mcr_Stage));
	return 0;
}

int mcr_Stage_deinit(void *stagePt)
{
	if (stagePt)
		return mcr_Signal_deinit(&((struct mcr_Stage *)
								   stagePt)->intercept);
	return 0;
}

int mcr_Stage_set_all(struct mcr_context *ctx, struct mcr_Stage *stagePt,
					  bool blocking, const struct mcr_Signal *interceptPt,
					  unsigned int measurementError, unsigned int mods, int trigFlags)
{
	dassert(stagePt);
	stagePt->blocking = blocking;
	stagePt->measurement_error = measurementError;
	stagePt->modifiers = mods;
	stagePt->trigger_flags = trigFlags;
	return mcr_Stage_set_intercept(ctx, stagePt, interceptPt);
}

bool mcr_Stage_equals(struct mcr_Stage * stagePt,
					  struct mcr_Signal * interceptPt, unsigned int mods)
{
	bool isMods = false;
	struct mcr_Signal *sigPt = &stagePt->intercept;
	dassert(stagePt);
	if (!interceptPt) {
		/* Intercepted a non-signal, but we are looking for one */
		if (sigPt->isignal)
			return false;
		MCR_TF_IS_MOD(stagePt->modifiers, mods, stagePt->trigger_flags,
					  isMods);
		return isMods;
	}
	/* We have a type, and it is not the same */
	if (sigPt->isignal && sigPt->isignal != interceptPt->isignal)
		return false;
	MCR_TF_IS_MOD(stagePt->modifiers, mods, stagePt->trigger_flags, isMods);
	if (!isMods)
		return false;
	/* By default ignore dispatch flag */
	return stagePt->matcher.equals ?
		   stagePt->matcher.equals(stagePt, interceptPt) :
		   !mcr_Instance_compare(sigPt, interceptPt);
}

bool mcr_Stage_resembles(struct mcr_Stage * stagePt,
						 struct mcr_Signal * interceptPt)
{
	struct mcr_Signal *sigPt = &stagePt->intercept;
	dassert(stagePt);
	if (interceptPt) {
		/* Case 1, stage type: By default same type is a resemblance */
		if (sigPt->isignal) {
			if (sigPt->isignal == interceptPt->isignal)
				return stagePt->matcher.resembles ? stagePt->
					   matcher.resembles(stagePt,
										 interceptPt) : true;
			/* Not same type */
			return false;
		}
		/* Case 2, no stage type: By default always true */
		return stagePt->matcher.resembles ?
			   stagePt->matcher.resembles(stagePt, interceptPt) : true;
	}
	/* Case 3, no intercept: Only true if also no stage type */
	return !sigPt->isignal;
}

int mcr_Stage_set_intercept(struct mcr_context *ctx,
							struct mcr_Stage *stagePt, const struct mcr_Signal *interceptPt)
{
	size_t id;
	struct mcr_IsStage *matchPt;
	int err;
	dassert(stagePt);
	if (interceptPt && interceptPt->isignal) {
		id = mcr_Instance_id(interceptPt);
		if ((err = mcr_Signal_copy(&stagePt->intercept, interceptPt)))
			return err;
		matchPt = mcr_Stage_from_id(ctx, id);
		set_matcher(stagePt, matchPt);
	} else {
		return mcr_Stage_set_intercept_generic(ctx, stagePt,
											   interceptPt);
	}
	return 0;
}

int mcr_Stage_set_intercept_generic(struct mcr_context *ctx,
									struct mcr_Stage *stagePt, const struct mcr_Signal *interceptPt)
{
	struct mcr_IsStage *matchPt = ctx->standard.stage_generic;
	dassert(stagePt);
	mcr_Signal_deinit(&stagePt->intercept);
	stagePt->intercept.isignal = NULL;
	stagePt->intercept.instance.data.data = (void *)interceptPt;
	set_matcher(stagePt, matchPt);
	return 0;
}

int mcr_Stage_compare(const void *lhs, const void *rhs)
{
	const struct mcr_Stage *lPt = lhs, *rPt = rhs;
	int ret;
	if (rhs) {
		if (lhs) {
			if (lhs == rhs)
				return 0;
			if ((ret = mcr_Signal_compare(&lPt->intercept,
										  &rPt->intercept)))
				return ret;
			if ((ret = MCR_CMP(lPt->modifiers,
							   rPt->modifiers)))
				return ret;
			if ((ret = MCR_CMP(lPt->trigger_flags,
							   rPt->trigger_flags)))
				return ret;
			if ((ret = MCR_CMP(lPt->blocking, rPt->blocking)))
				return ret;
			return MCR_CMP(lPt->measurement_error,
						   rPt->measurement_error);
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_Stage_copy(void *dstPt, const void *srcPt)
{
	struct mcr_Stage *dPt = dstPt;
	const struct mcr_Stage *sPt = srcPt;
	dassert(dPt);
	dassert(sPt);
	if (dPt == sPt)
		return 1;
	mcr_Signal_deinit(&dPt->intercept);
	memcpy(dstPt, srcPt, sizeof(struct mcr_Stage));
	memset(&dPt->intercept, 0, sizeof(struct mcr_Signal));
	return mcr_Signal_copy(&dPt->intercept, &sPt->intercept);
}

struct mcr_IsStage *mcr_Stage_from_id(struct mcr_context *ctx, size_t id)
{
	if (id == (size_t) ~ 0)
		return ctx->standard.stage_generic;
	return MCR_ARR_ELEMENT(ctx->standard.stage_matchers, id);
}

size_t mcr_Stage_count(struct mcr_context * ctx)
{
	return ctx->standard.stage_matchers.used;
}

void mcr_Stage_all(struct mcr_context *ctx, struct mcr_IsStage *buffer,
				   size_t bufferLength)
{
	size_t count = ctx->standard.stage_matchers.used;
	if (bufferLength < count)
		count = bufferLength;
	dassert(ctx);
	if (buffer) {
		memcpy(buffer, ctx->standard.stage_matchers.array,
			   sizeof(struct mcr_IsStage) * count);
	}
}

/* Stage development */
int mcr_Stage_register(struct mcr_context *ctx, mcr_isme_fnc setIsme,
					   mcr_isme_fnc setResemble, size_t signalId)
{
	struct mcr_IsStage *matchPt;
	int err;
	dassert(ctx);
	if (signalId == (size_t) ~ 0) {
		matchPt = ctx->standard.stage_generic;
	} else {
		if ((err = mcr_Array_minfill(&ctx->standard.stage_matchers,
									 signalId + 1, NULL)))
			return err;
		matchPt =
			MCR_ARR_ELEMENT(ctx->standard.stage_matchers, signalId);
		if (!matchPt) {
			mset_error(ENOMEM);
			return ENOMEM;
		}
	}
	matchPt->equals = setIsme;
	matchPt->resembles = setResemble;
	return 0;
}

void mcr_Stage_clear(struct mcr_context *ctx)
{
	mcr_Array_clear(&ctx->standard.stage_matchers);
}

void mcr_Stage_trim(struct mcr_context *ctx)
{
	mcr_Array_trim(&ctx->standard.stage_matchers);
}
