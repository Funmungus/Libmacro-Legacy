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

/*! \file
 * \brief \ref mcr_TriggerFlags enumerates behavior of triggering modifiers.
 */

#ifndef MCR_TRIGGER_FLAGS_H
#define MCR_TRIGGER_FLAGS_H

#include "mcr/standard/def.h"

/*!
 * \brief Logical ing bahavior of modifiers.
 *
 * This will define behaviour of how modifiers may or may not trigger a
 * dispatch to receivers.\n
 * These flags act as a function between the registered modifier,
 * and triggering modifier.  The end result is a multi-function machine
 * of the two modifier variables.\n
 * Given none=001, some=010, all=100 the following is the function table:\n
 * 000 defaulted to all flags set\n
 * 001 none of\n
 * 010 some of\n
 * 011 none or some of\n
 * 100 all of\n
 * 101 none or all of\n
 * 110 some or all of\n
 * 111 none, some, or all of (trigger any)
 */
enum mcr_TriggerFlags {
	/*! \brief Invalid flags */
	MCR_TF_UNDEFINED = 0,
	/*! \brief None of this modifier may be included. */
	MCR_TF_NONE = 1,
	/*! \brief Some flag of this modifier must be included, but not all. */
	MCR_TF_SOME = 1 << 1,
	/*! \brief Anything except an exact match. */
	MCR_TF_INEQUAL = MCR_TF_NONE | MCR_TF_SOME,
	/*! \brief All flags of this modifier must be included. */
	MCR_TF_ALL = 1 << 2,
	/*! \brief All or nothing match. */
	MCR_TF_ALL_OR_NOTHING = MCR_TF_NONE | MCR_TF_ALL,
	/*! \brief At least one mod flag matches. */
	MCR_TF_MATCH = MCR_TF_SOME | MCR_TF_ALL,
	/*! \brief All flags set (-1 also works) */
	MCR_TF_ANY = MCR_TF_NONE | MCR_TF_SOME | MCR_TF_ALL,
	/*! \brief At this number and above may be used to extend flag logic. */
	MCR_TF_USER = MCR_TF_ANY + 1
};
/*! \brief Make valid \ref mcr_TriggerFlags from any number */
#define MCR_TF_mask(number) (MCR_TF_ANY & (number))
/*!
 * \brief Remove valid \ref mcr_TriggerFlags from any number, leaving
 * only user defined flags.
 */
#define MCR_TF_user_mask(number) ((number) & (~MCR_TF_ANY))
/*! \brief bool, \ref MCR_TF_NONE */
#define MCR_TF_IS_NONE(lhs, rhs) (!((lhs) & (rhs)))
/*! \brief bool, \ref MCR_TF_SOME */
#define MCR_TF_IS_SOME(lhs, rhs) \
((lhs) != (rhs) && ((lhs) & (rhs)))
/*! \brief bool, \ref MCR_TF_INEQUAL */
#define MCR_TF_IS_INEQUAL(lhs, rhs) ((lhs) != (rhs))
/*! \brief bool, \ref MCR_TF_ALL */
#define MCR_TF_IS_ALL(lhs, rhs) ((lhs) == (rhs))
/*! \brief bool, \ref MCR_TF_ALL_OR_NOTHING */
#define MCR_TF_IS_ALL_OR_NOTHING(lhs, rhs) \
(MCR_TF_IS_ALL(lhs, rhs) || MCR_TF_IS_NONE(lhs, rhs))
/*! \brief bool, \ref MCR_TF_MATCH */
#define MCR_TF_IS_MATCH(lhs, rhs) (!!((lhs) & (rhs)))
/*! \brief bool, \ref MCR_TF_ANY */
#define MCR_TF_IS_ANY(lhs, rhs) true

/*!
 * \brief Match modifiers using \ref mcr_TriggerFlags logic.
 *
 * \param lhs Modifiers that must be matched
 * \param rhs Modifiers that are used to match
 * \param flags \ref mcr_TriggerFlags Logic flags
 * \param outIsVal bool True if modifiers match
 */
#define MCR_TF_IS_MOD(lhs, rhs, flags, outIsVal) \
switch (flags) { \
case MCR_TF_NONE: \
	outIsVal = MCR_TF_IS_NONE(lhs, rhs); \
	break; \
case MCR_TF_SOME: \
	outIsVal = MCR_TF_IS_SOME(lhs, rhs); \
	break; \
case MCR_TF_INEQUAL: \
	outIsVal = MCR_TF_IS_INEQUAL(lhs, rhs); \
	break; \
case MCR_TF_ALL: \
	outIsVal = MCR_TF_IS_ALL(lhs, rhs); \
	break; \
case MCR_TF_ALL_OR_NOTHING: \
	outIsVal = MCR_TF_IS_ALL_OR_NOTHING(lhs, rhs); \
	break; \
case MCR_TF_MATCH: \
	outIsVal = MCR_TF_IS_MATCH(lhs, rhs); \
	break; \
case MCR_TF_ANY: \
	outIsVal = MCR_TF_IS_ANY(lhs, rhs); \
	break; \
default: \
	outIsVal = false; \
	dmsg; \
	fprintf(stderr, "Error: Invalid " \
			MCR_STR(enum mcr_TriggerFlags) "\n"); \
	break; \
}

#endif
