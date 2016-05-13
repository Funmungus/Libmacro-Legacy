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
 * \brief \ref mcr_Key
 */

#ifndef MCR_KEY_H
#define MCR_KEY_H

#include "mcr/standard/def.h"

/*!
 * \brief Simulate keyboard keys
 *
 * Keyboard signals have a scan code, specified by the hardware,
 * or a key code, specified by the operating system. One or both may
 * be used.
 * <br>
 * A 0 value of both key code and scan code should be logically
 * interpreted as either, "no key available, do not send,"
 * or "either a key code or scan code of any value."
 */
struct mcr_Key;

/*!
 * \brief Initialize \ref mcr_Key
 *
 * Native
 */
MCR_API void mcr_Key_init(void *keyDataPt);
/*! \brief Set key, scan and key up type. */
MCR_API void mcr_Key_set_all(struct mcr_Key *keyPt, int key, int scan,
	enum mcr_KeyUpType keyUp);
/*! \brief Get key code */
MCR_API int mcr_Key_key(const struct mcr_Key *keyPt);
/*! \brief Set key code */
MCR_API void mcr_Key_set_key(struct mcr_Key *keyPt, int key);
/*! \brief Get Scan code */
MCR_API int mcr_Key_scan(const struct mcr_Key *keyPt);
/*! \brief Set scan code */
MCR_API void mcr_Key_set_scan(struct mcr_Key *keyPt, int scan);
/*! \brief Get key up type */
MCR_API enum mcr_KeyUpType mcr_Key_up_type(const struct mcr_Key *keyPt);
/*! \brief Set key up type */
MCR_API void mcr_Key_set_up_type(struct mcr_Key *keyPt,
	const enum mcr_KeyUpType keyUp);
/*!
 * \brief Simulate keyboard keys.
 *
 * \param signalData Has data member \ref mcr_Key
 * \return \ref reterr
 */
MCR_API int mcr_Key_send(struct mcr_Signal *signalData);
/*!
 * \brief \ref mcr_Key_send
 *
 * Native
 */
MCR_API int mcr_Key_send_data(struct mcr_Key *dataPt);
/*! \brief Compare \ref mcr_Key */
MCR_API int mcr_Key_compare(const void *lhs, const void *rhs);
/*! \brief Copy \ref mcr_Key */
MCR_API int mcr_Key_copy(void *dstPt, void *srcPt);
/* Default free */

/* Key names */
/*!
 * \brief Get a key code from its name.
 *
 * \param keyName Name of key code
 * \return Key code, or \ref MCR_KEY_ANY if it is not registered
 */
MCR_API int mcr_Key_name_key(struct mcr_context *ctx, const char *keyName);
/*! \brief Get the name of a key code. */
MCR_API const char *mcr_Key_name(struct mcr_context *ctx, int keyCode);
/*! \brief Get the number of registered key codes. */
MCR_API size_t mcr_Key_count(struct mcr_context *ctx);
/*!
 * \brief Set the name mapped by a key code.
 *
 * The name will also map to this key code.
 * \return \ref reterr
 */
MCR_API int mcr_Key_set_name(struct mcr_context *ctx, int keyCode,
	const char *newName);
/*!
 * \brief Add a set of names to map to a key code.
 *
 * \return \ref reterr
 */
MCR_API int mcr_Key_add(struct mcr_context *ctx, int keyCode,
	const char **addNames, size_t bufferLen);
/*!
 * \brief \ref mcr_Key_set_name and \ref mcr_Key_add_names
 *
 * \return \ref reterr
 */
MCR_API int mcr_Key_map(struct mcr_context *ctx, int keyCode,
	const char *newName, const char **addNames, size_t bufferLen);
/*!
 * \brief Rename a key code.
 *
 * \return \ref reterr
 */
MCR_API int mcr_Key_rekey(struct mcr_context *ctx, int keyCode, int newCode);
/*!
 * \brief Rename a key code. The code is first found from its
 * old name.
 *
 * \return \ref reterr
 */
MCR_API int mcr_Key_rename(struct mcr_context *ctx, const char *oldName,
	const char *newName);
MCR_API void mcr_Key_trim(struct mcr_context *ctx);
/*! \brief Remove all mapped key codes. */
MCR_API void mcr_Key_clear(struct mcr_context *ctx);

/* Modifier mapping */
MCR_API unsigned int mcr_Key_mod(struct mcr_context *ctx, int key);
MCR_API int mcr_Key_mod_key(struct mcr_context *ctx, unsigned int modifier);
MCR_API size_t mcr_Key_mod_count(struct mcr_context *ctx);

MCR_API void mcr_Key_mod_all(struct mcr_context *ctx,
	unsigned int *modBuffer, size_t bufferLength);
MCR_API void mcr_Key_mod_clear(struct mcr_context *ctx);
MCR_API int mcr_Key_mod_set_key(struct mcr_context *ctx,
	unsigned int modifiers, int key);
MCR_API int mcr_Key_mod_add(struct mcr_context *ctx, unsigned int modifiers,
	int key);
MCR_API int mcr_Key_mod_map(struct mcr_context *ctx, unsigned int modifiers,
	int key, const int *addKeys, size_t bufferLen);
MCR_API int mcr_Key_mod_rekey(struct mcr_context *ctx, int oldKey, int newKey);
MCR_API int mcr_Key_mod_remod(struct mcr_context *ctx,
	unsigned int modifiers, unsigned int newMods);
MCR_API void mcr_Key_mod_trim(struct mcr_context *ctx);

MCR_API struct mcr_ISignal *mcr_iKey(struct mcr_context *ctx);
/*! \brief Signal data casted \ref mcr_Key * */
#define mcr_Key_data(sigPt) \
((struct mcr_Key *)mcr_inst_data(sigPt))
/*! \brief Signal data casted \ref mcr_Key * */
#define MCR_KEY_DATA(sig) \
((struct mcr_Key *)(sig).inst.data.data)

#endif
