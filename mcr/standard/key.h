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
 * \brief \ref mcr_Key - Simulate keyboard keys
 */

#ifndef MCR_KEY_H
#define MCR_KEY_H

#include "mcr/standard/def.h"

/*! \brief Simulate keyboard keys
 *
 * One or both of key and scan code may be used.\n
 * A 0 value of both key code and scan code should be logically
 * interpreted as either, "no key available, do not send,"
 * or "either a key code or scan code of any value."
 */
struct mcr_Key {
	/*! \brief Key code, specified by platform */
	int key;
	/*! \brief Scan code, specified by hardware */
	int scan;
	/*! \brief Key press or release */
	enum mcr_KeyUpType up_type;
};

/*! \brief Set key, scan and key up type. */
MCR_API void mcr_Key_set_all(struct mcr_Key *keyPt, int key, int scan,
			     enum mcr_KeyUpType keyUp);
/*! \pre Signal instance data is \ref mcr_Key
 * \brief Simulate keyboard keys.
 *
 * \param sigPt Has data member \ref mcr_Key
 * \return \ref reterr
 */
MCR_API int mcr_Key_send(struct mcr_Signal *sigPt);
/*! \brief \ref mcr_Key_send
 *
 * \ref mcr_is_platform
 * \return \ref reterr
 */
MCR_API int mcr_Key_send_data(struct mcr_Key *dataPt);
/* Default init, deinit, compare, and copy */

/* Key names */
/*! \brief Get a key code from its name.
 *
 * \param keyName \ref opt Name of key code
 * \return Key code, or \ref MCR_KEY_ANY if not found
 */
MCR_API int mcr_Key_name_key(struct mcr_context *ctx, const char *keyName);
/*! \brief Get the name of a key code.
 *
 * \return Key name, or null if not found
 */
MCR_API const char *mcr_Key_name(struct mcr_context *ctx, int keyCode);
/*! \brief Get the number of registered key codes. */
MCR_API size_t mcr_Key_count(struct mcr_context *ctx);
/*! \brief Set the name mapped by a key code.
 *
 * The name will also map to this key code.
 * \param newName \ref opt Name to set
 * \return \ref reterr
 */
MCR_API int mcr_Key_set_name(struct mcr_context *ctx, int keyCode,
			     const char *newName);
/*! \brief Add a set of names to map to a key code.
 *
 * \param addNames \ref opt Set of names to add to key code
 * \return \ref reterr
 */
MCR_API int mcr_Key_add(struct mcr_context *ctx, int keyCode,
			const char **addNames, size_t bufferLen);
/*! \brief \ref mcr_Key_set_name and \ref mcr_Key_add_names
 *
 * \return \ref reterr
 */
MCR_API int mcr_Key_map(struct mcr_context *ctx, int keyCode,
			const char *newName, const char **addNames, size_t bufferLen);
/*! \brief Rename a key code.
 *
 * \return \ref reterr
 */
MCR_API int mcr_Key_rekey(struct mcr_context *ctx, int keyCode, int newCode);
/*! \brief Rename a key code. The code is first found from its
 * old name.
 *
 * \return \ref reterr
 */
MCR_API int mcr_Key_rename(struct mcr_context *ctx, const char *oldName,
			   const char *newName);
/*! \brief Minimize key allocation */
MCR_API void mcr_Key_trim(struct mcr_context *ctx);
/*! \brief Remove all mapped key codes. */
MCR_API void mcr_Key_clear(struct mcr_context *ctx);

/* Modifier mapping */
/*! \brief Get the modifier from a key code */
MCR_API unsigned int mcr_Key_mod(struct mcr_context *ctx, int key);
/*! \brief Get the key code from a modifier */
MCR_API int mcr_Key_mod_key(struct mcr_context *ctx, unsigned int modifier);
/*! \brief Get the number of mapped key codes to modifiers */
MCR_API size_t mcr_Key_mod_count(struct mcr_context *ctx);

MCR_API void mcr_Key_mod_all(struct mcr_context *ctx,
			     unsigned int *modBuffer, size_t bufferLength);
MCR_API void mcr_Key_mod_clear(struct mcr_context *ctx);
MCR_API int mcr_Key_mod_set_key(struct mcr_context *ctx,
				unsigned int modifiers, int key);
MCR_API int mcr_Key_mod_add(struct mcr_context *ctx, unsigned int modifiers,
			    int key);
MCR_API int mcr_Key_mod_map(struct mcr_context *ctx, unsigned int modifiers,
			    int key, int *addKeys, size_t bufferLen);
MCR_API int mcr_Key_mod_rekey(struct mcr_context *ctx, int oldKey, int newKey);
MCR_API int mcr_Key_mod_remod(struct mcr_context *ctx,
			      unsigned int modifiers, unsigned int newMods);
MCR_API void mcr_Key_mod_trim(struct mcr_context *ctx);

/*! \brief Get the Signal interface of \ref mcr_Key */
MCR_API struct mcr_ISignal *mcr_iKey(struct mcr_context *ctx);
/*! \brief Signal data casted \ref mcr_Key * */
#define mcr_Key_data(sigPt) \
((struct mcr_Key *)mcr_Instance_data(sigPt))
/*! \brief Signal data casted \ref mcr_Key * */
#define MCR_KEY_DATA(sig) \
((struct mcr_Key *)(sig).instance.data.data)

MCR_API int mcr_Key_Dispatcher_add(void *dispDataPt,
				   struct mcr_Signal *signalPt, void *newTrigger,
				   mcr_Dispatcher_receive_fnc receiveFnc);
MCR_API int mcr_Key_Dispatcher_clear(void *dispDataPt);
MCR_API bool mcr_Key_Dispatcher_dispatch(void *dispDataPt,
		struct mcr_Signal *signalPt, unsigned int mods);
MCR_API void mcr_Key_Dispatcher_modifier(void *dispDataPt,
		struct mcr_Signal *signalPt, unsigned int *modsPt);
MCR_API int mcr_Key_Dispatcher_remove(void *dispDataPt, void *delTrigger);
MCR_API int mcr_Key_Dispatcher_trim(void *dispDataPt);

#endif
