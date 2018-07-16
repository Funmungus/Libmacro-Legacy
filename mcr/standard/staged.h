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

/*! \file
 *  \brief \ref mcr_Staged - Trigger that is triggered after activating all stages
 *  \ref mcr_BlockStyle - How to manage blocking values of all stages
 */

#ifndef MCR_STANDARD_STAGED_H_
#define MCR_STANDARD_STAGED_H_

#include "mcr/standard/stage.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Blocking style describes how to manage a set of stages'
 *  blocking values.
 */
enum mcr_BlockStyle {
	/*! Do not manage blocking for any stages. */
	MCR_BS_UNMANAGED = 0,
	/*! All stages are always set to not block. */
	MCR_BS_NOTHING,
	/*! All stages are always set to block. */
	MCR_BS_EVERYTHING,
	/*! Only manage the first stage. */
	MCR_BS_BEGIN,
	/*! Only manage the last stage. */
	MCR_BS_FINAL,
	/*! Manage all stages. */
	MCR_BS_ALL
};

/*! Trigger that is triggered after activating all stages */
struct mcr_Staged {
	/*! Blocking state of managed stages. */
	bool blocking;
	/*! Set of \ref mcr_Stage */
	struct mcr_Array stages;
	/*! How to manage the blocking value of stages. */
	enum mcr_BlockStyle style;
};

/*! \ref mcr_Staged ctor
 *
 *  \param stagedPt \ref opt \ref mcr_Staged
 *  \return 0
 */
MCR_API int mcr_Staged_init(void *stagedPt);
/*! \ref mcr_Staged dtor
 *
 *  \param stagedPt \ref opt \ref mcr_Staged
 *  \return 0
 */
MCR_API int mcr_Staged_deinit(void *stagedPt);
/*! Set initial values */
MCR_API void mcr_Staged_set_all(struct mcr_Staged *stagedPt, bool blocking,
								enum mcr_BlockStyle style);
MCR_API int mcr_Staged_compare(const void *lhs, const void *rhs);
MCR_API int mcr_Staged_copy(void *dstPt, const void *srcPt);
/*! \ref mcr_ITrigger.receive for staged trigger.
 *
 *  If all stages activate, all will be deactivated and
 *  \ref mcr_Trigger.trigger will be called.
 */
MCR_API bool mcr_Staged_receive(void *trigDataPt,
								struct mcr_Signal *interceptPt, unsigned int mods);

/* Stage manipulation */
MCR_API void mcr_Staged_set_style(struct mcr_Staged *trigPt,
								  enum mcr_BlockStyle style);
MCR_API bool mcr_Staged_is_blocking(const struct mcr_Staged *trigPt);
MCR_API void mcr_Staged_set_blocking(struct mcr_Staged *trigPt, bool blocking);
MCR_API void mcr_Staged_deactivate(struct mcr_Staged *trigPt);
MCR_API void mcr_Staged_clear(struct mcr_Staged *trigPt);

/*! Get the trigger interface for \ref mcr_Staged */
MCR_API struct mcr_ITrigger *mcr_iStaged(struct mcr_context *ctx);
/*! Get \ref mcr_Staged reference from a trigger */
#define mcr_Staged_data(trigPt) \
((struct mcr_Staged *)mcr_Instance_data(trigPt))

#ifdef __cplusplus
}
#endif

#endif
