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

/*!
 * \file
 * \brief \ref mcr_Staged - Trigger that is triggered after activating all stages
 * \ref mcr_BlockStyle - How to manage blocking values of all stages
 */

#ifndef MCR_STANDARD_STAGED_H
#define MCR_STANDARD_STAGED_H

#include "mcr/standard/stage.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Blocking style describes how to manage a set of stages'
 * blocking values.
 */
enum mcr_BlockStyle {
	/*! \brief Do not manage blocking for any stages. */
	MCR_BS_UNMANAGED = 0,
	/*! \brief All stages are always set to not block. */
	MCR_BS_NOTHING,
	/*! \brief All stages are always set to block. */
	MCR_BS_EVERYTHING,
	/*! \brief Only manage the first stage. */
	MCR_BS_BEGIN,
	/*! \brief Only manage the last stage. */
	MCR_BS_FINAL,
	/*! \brief Manage all stages. */
	MCR_BS_ALL
};

/*! \brief Trigger that is triggered after activating all stages */
struct mcr_Staged {
	/*! \brief Blocking state of managed stages. */
	bool block;
	/*! \brief Set of \ref mcr_Stage */
	struct mcr_Array stages;
	/*! \brief How to manage the blocking value of stages. */
	enum mcr_BlockStyle style;
};

/*!
 * \brief \ref mcr_Staged ctor
 *
 * \param stagedPt \ref opt \ref mcr_Staged
 * \return 0
 */
MCR_API int mcr_Staged_init(void *stagedPt);
/*!
 * \brief \ref mcr_Staged dtor
 *
 * \param stagedPt \ref opt \ref mcr_Staged
 * \return 0
 */
MCR_API int mcr_Staged_deinit(void *stagedPt);
/*! \brief Set initial values */
MCR_API void mcr_Staged_set_all(struct mcr_Staged *stagedPt, bool blocking,
				enum mcr_BlockStyle style);
MCR_API int mcr_Staged_compare(const void *lhs, const void *rhs);
MCR_API int mcr_Staged_copy(void *dstPt, const void *srcPt);
/*!
 * \brief \ref mcr_ITrigger.receive for staged trigger.
 *
 * If all stages activate, all will be deactivated and
 * \ref mcr_Trigger.trigger will be called.
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

/*! \brief Get the trigger interface for \ref mcr_Staged */
MCR_API struct mcr_ITrigger *mcr_iStaged(struct mcr_context *ctx);
/*! \brief Get \ref mcr_Staged reference from a trigger */
#define mcr_Staged_data(trigPt) \
((struct mcr_Staged *)mcr_Instance_data(trigPt))

#ifdef __cplusplus
}
#endif
#endif
