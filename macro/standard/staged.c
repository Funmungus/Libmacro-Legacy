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
#include "mcr/modules.h"
#include <string.h>

int mcr_Staged_init(void *stagedPt)
{
	struct mcr_Staged *localPt = stagedPt;
	if (localPt) {
		memset(localPt, 0, sizeof(struct mcr_Staged));
		mcr_Array_init(&localPt->stages);
		localPt->stages.element_size = sizeof(struct mcr_Stage);
		localPt->style = MCR_BS_UNMANAGED;
	}
	return 0;
}

int mcr_Staged_deinit(void *stagedPt)
{
	struct mcr_Staged *localPt = stagedPt;
	if (localPt) {
		MCR_ARR_FOR_EACH(localPt->stages, mcr_Stage_deinit);
		mcr_Array_deinit(&localPt->stages);
	}
	return 0;
}

void mcr_Staged_set_all(struct mcr_Staged *stagedPt, bool blocking,
			enum mcr_BlockStyle style)
{
	dassert(stagedPt);
	mcr_Staged_deinit(stagedPt);
	mcr_Staged_set_blocking(stagedPt, blocking);
	mcr_Staged_set_style(stagedPt, style);
}

int mcr_Staged_compare(const void *lhs, const void *rhs)
{
	const struct mcr_Staged *lPt = lhs, *rPt = rhs;
	int ret;
	struct mcr_Stage *lSt, *rSt, *end;
	if (rhs) {
		if (lhs) {
			if (lhs == rhs)
				return 0;
			if ((ret = MCR_CMP_INTEGRAL(lPt->stages.used,
						    rPt->stages.used)))
				return ret;
			if ((ret = MCR_CMP_INTEGRAL(lPt->style, rPt->style)))
				return ret;
			lSt = MCR_ARR_FIRST(lPt->stages);
			rSt = MCR_ARR_FIRST(rPt->stages);
			end = MCR_ARR_END(lPt->stages);
			while (lSt < end) {
				if ((ret = mcr_Stage_compare(lSt++, rSt++)))
					return ret;
			}
			return 0;
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_Staged_copy(void *dstPt, const void *srcPt)
{
	struct mcr_Staged *dPt = dstPt;
	const struct mcr_Staged *sPt = srcPt;
	struct mcr_Stage initial, *dSt, *sSt, *end;
	int err;
	dassert(dstPt);
	if (dstPt == srcPt)
		return 0;
	if (!srcPt) {
		mcr_Staged_clear(dPt);
		dPt->block = false;
		dPt->style = MCR_BS_UNMANAGED;
		return 0;
	}
	mcr_Stage_init(&initial);
	dPt->block = sPt->block;
	mcr_Staged_clear(dPt);
	dPt->style = sPt->style;
	if (!sPt->stages.used)
		return 0;
	if ((err = mcr_Array_minfill(&dPt->stages, sPt->stages.used, &initial)))
		return err;
	dSt = MCR_ARR_FIRST(dPt->stages);
	sSt = MCR_ARR_FIRST(sPt->stages);
	end = MCR_ARR_END(dPt->stages);
	while (dSt < end) {
		if ((err = mcr_Stage_copy(dSt++, sSt++)))
			return err;
	}
	return 0;
}

bool mcr_Staged_receive(void *trigDataPt,
			struct mcr_Signal * interceptPt, unsigned int mods)
{
	struct mcr_Trigger *trigPt = trigDataPt;
	struct mcr_Staged *stagedPt = mcr_Staged_data(trigPt);
	bool isToBlock = false;
	struct mcr_Array *stages;
	struct mcr_Stage *first, *rit, *prev;
	dassert(trigDataPt);
#define localOnComplete \
(trigPt->trigger ? \
	trigPt->trigger(trigPt, interceptPt, mods) || isToBlock : \
isToBlock)
	if (!stagedPt)
		return false;
	stages = &stagedPt->stages;
	if (!stages->used)
		return localOnComplete;
	first = mcr_Array_first(stages);
	/* At last element. */
	rit = mcr_Array_prev(stages, mcr_Array_end(stages));
	/* One before last. */
	prev = mcr_Array_prev(stages, rit);
	/* One element or previous activated, this may complete. */
	if (stages->used == 1 || prev->activated) {
		if (mcr_Stage_equals(rit, interceptPt, mods)) {
			if (rit->block)
				++isToBlock;
			mcr_Staged_deactivate(stagedPt);
			return localOnComplete;
		}
		if (stages->used == 1)
			return false;
		rit = prev;
		prev = mcr_Array_prev(stages, prev);
	}
	/* Do not check first item for conditional activation, but
	 *always check it at the end. */
	while (rit != first) {
		if (rit->activated) {
			if (mcr_Stage_resembles(rit, interceptPt)) {
				if (!isToBlock && rit->block)
					++isToBlock;
			} else {
				rit->activated = false;
			}
		} else if (prev->activated) {
			if (mcr_Stage_equals(rit, interceptPt, mods)) {
				rit->activated = true;
				if (!isToBlock && rit->block)
					++isToBlock;
			}
		}
		rit = prev;
		prev = mcr_Array_prev(stages, prev);
	}
	/* Assert: At first element, always reset current activated state */
	dassert(rit == first);
	if (mcr_Stage_equals(rit, interceptPt, mods)) {
		rit->activated = true;
		if (!isToBlock && rit->block)
			++isToBlock;
	} else {
		rit->activated = true;
	}
	return isToBlock;
#undef localOnComplete
}

void mcr_Staged_set_style(struct mcr_Staged *stagedPt,
			  enum mcr_BlockStyle style)
{
	bool wasBlocking = mcr_Staged_is_blocking(stagedPt);
	dassert(stagedPt);
	if (wasBlocking)
		mcr_Staged_set_blocking(stagedPt, false);
	stagedPt->style = style;
	if (wasBlocking)
		mcr_Staged_set_blocking(stagedPt, true);
}

bool mcr_Staged_is_blocking(const struct mcr_Staged *stagedPt)
{
	dassert(stagedPt);
	return stagedPt->block;
}

void mcr_Staged_set_blocking(struct mcr_Staged *stagedPt, bool blocking)
{
	struct mcr_Stage *it, *end;
	dassert(stagedPt);
	stagedPt->block = blocking;
	if (!stagedPt->stages.used)
		return;
	it = MCR_ARR_FIRST(stagedPt->stages);
	end = MCR_ARR_END(stagedPt->stages);
#define localSetAll(blockVal) \
for (; it < end; it = MCR_ARR_NEXT(stagedPt->stages, it)) \
{ it->block = blockVal; }
	switch (stagedPt->style) {
	case MCR_BS_UNMANAGED:
		break;
	case MCR_BS_NOTHING:
		localSetAll(false);
		break;
	case MCR_BS_EVERYTHING:
		localSetAll(true);
		break;
	case MCR_BS_BEGIN:
		it->block = blocking;
		break;
	case MCR_BS_FINAL:
		it = MCR_ARR_PREV(stagedPt->stages, end);
		it->block = blocking;
		break;
	case MCR_BS_ALL:
		localSetAll(blocking);
		break;
	}
#undef localSetAll
}

void mcr_Staged_deactivate(struct mcr_Staged *stagedPt)
{
#define localDeactivate(stageIt) \
((struct mcr_Stage *)stageIt)->activated = false;
	MCR_ARR_FOR_EACH(stagedPt->stages, localDeactivate);
#undef localDeactivate
}

void mcr_Staged_clear(struct mcr_Staged *stagedPt)
{
	dassert(stagedPt);
	MCR_ARR_FOR_EACH(stagedPt->stages, mcr_Stage_deinit);
	mcr_Array_clear(&stagedPt->stages);
}

struct mcr_ITrigger *mcr_iStaged(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.istaged;
}
