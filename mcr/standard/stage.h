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
 * \brief \ref mcr_Stage - Stages of activation for \ref mcr_Staged
 * \ref mcr_IsStage - Stage matching functions
 */

#ifndef MCR_STAGE_H
#define MCR_STAGE_H

#include "mcr/standard/def.h"

struct mcr_Stage;
/*! \brief Comparison for stages, which may be more lenient than
 * regular \ref mcr_Signal_compare used for exact comparison.
 */
typedef bool(*mcr_isme_fnc)
(struct mcr_Stage *, struct mcr_Signal *);
/*! \brief Function table for \ref mcr_Stage
 *
 * Matching functions to activate stage, or keep activated.
 */
struct mcr_IsStage {
	/*! \brief If true, activate stage */
	mcr_isme_fnc equals;
	/*! \brief If true, stage will remain activated */
	mcr_isme_fnc resembles;
};

/*! \brief Stages of activition for triggers */
struct mcr_Stage {
	/*! \brief Functions to match intercepted signal and activation */
	struct mcr_IsStage matcher;
	/*! \brief Make trigger block if this stage is activated */
	bool block;
	/*! \brief The signal compared for activation */
	struct mcr_Signal intercept;
	/*! \brief Measurement error for absolute values */
	unsigned int measurement_error;
	/*! \brief Modifiers compared for activation */
	unsigned int modifiers;
	/*! \brief Determine how modifiers are compared */
	int trigger_flags;
	/* Internal */
	/*! false if this stage is not triggered, otherwise this stage
	 * is triggered */
	bool activated;
};

/*! \brief \ref mcr_Stage ctor
 *
 * \param stagePt \ref opt
 * \return 0
 */
MCR_API int mcr_Stage_init(void *stagePt);
/*! \brief \ref mcr_Stage dtor
 *
 * \param stagePt \ref opt
 * \return 0
 */
MCR_API int mcr_Stage_deinit(void *stagePt);
/*! \brief Set initial values
 *
 * \param intercepPt \ref opt Signal to copy as comparison for activation
 * \return \ref reterr
 */
MCR_API int mcr_Stage_set_all(struct mcr_context *ctx,
			      struct mcr_Stage *stagePt, bool blocking,
			      struct mcr_Signal *interceptPt, unsigned int measurementError,
			      unsigned int mods, int trigFlags);
/*! \brief Match an unactivated stage
 *
 * If intercepting a real signal: If signal interface is the same and
 * modifiers match, the intercepted signal is matched by
 * \ref mcr_IsStage.equals or \ref mcr_Instance_compare\n
 * If intercepting a fake signal: Signal matches if stage does not have
 * a signal interface and modifiers match.\n
 * Otherwise the intercepted signal does not match.
 * \param interceptPt \ref opt Intercepted signal
 * \param mods Intercepted modifiers
 * \return Do activate if true
 */
MCR_API bool mcr_Stage_equals(struct mcr_Stage *stagePt,
			      struct mcr_Signal *interceptPt, unsigned int mods);
/*! \brief Match an activated stage to be the new activated state
 *
 * If intercepting a real signal: Match with \ref mcr_IsStage.resembles, or
 * signal interface.  If stage has an interface, it must be the same.\n
 * If intercepting a fake signal: Matches if stage does not have an interface.
 * \param interceptPt \ref opt Intercepted signal
 * \param mods Intercepted modifiers
 * \return Deactivate if false
 */
MCR_API bool mcr_Stage_resembles(struct mcr_Stage *stagePt,
				 struct mcr_Signal *interceptPt);
MCR_API int mcr_Stage_set_intercept(struct mcr_context *ctx,
				    struct mcr_Stage *stagePt, struct mcr_Signal *interceptPt);
MCR_API int mcr_Stage_set_intercept_generic(struct mcr_context *ctx,
		struct mcr_Stage *stagePt, struct mcr_Signal *interceptPt);
MCR_API int mcr_Stage_compare(const void *lhs, const void *rhs);
MCR_API int mcr_Stage_copy(void *dstPt, void *srcPt);

/* Interface development. */
MCR_API struct mcr_IsStage *mcr_Stage_from_id(struct mcr_context *ctx,
		size_t id);
MCR_API size_t mcr_Stage_count(struct mcr_context *ctx);
MCR_API void mcr_Stage_all(struct mcr_context *ctx,
			   struct mcr_IsStage *buffer, size_t bufferLength);
MCR_API int mcr_Stage_register(struct mcr_context *ctx, mcr_isme_fnc setIsme,
			       mcr_isme_fnc setResemble, size_t signalId);
MCR_API void mcr_Stage_clear(struct mcr_context *ctx);
MCR_API void mcr_Stage_trim(struct mcr_context *ctx);

#endif
