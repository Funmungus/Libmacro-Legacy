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
 * \brief \ref mcr_Command - Execute a shell command with \ref mcr_execvp
 * \ref mcr_StringKey - Type or send signals from text
 *
 * \ref mcr_execvp
 * \ref execvp
 */

#ifndef MCR_SIGNALEXTRAS_H
#define MCR_SIGNALEXTRAS_H

#include "mcr/extras/safe_string.h"

/*! \brief Execute a shell command with \ref mcr_execvp */
struct mcr_Command {
	/*! \brief Executable file */
	struct mcr_SafeString file;
	/* Internal */
	/*! \brief Array of \ref mcr_SafeString, command arguments */
	struct mcr_Array argv;
};

/* TODO: unicode support */
/*! \brief Type or send signals from text */
struct mcr_StringKey {
	/*! \brief Pause time between sending each character in string */
	struct timespec interval;
	/*! \brief Text to parse, each character maps to a set of signals
	 * to send */
	struct mcr_SafeString string;
};

/*! \brief Get the \ref mcr_ISignal for \ref mcr_Command */
MCR_API struct mcr_ISignal *mcr_iCommand(struct mcr_context *ctx);
/*! \brief Get the \ref mcr_ISignal for \ref mcr_Command */
#define mcr_iCmd mcr_iCommand
/*! \brief Get the \ref mcr_ISignal for \ref mcr_StringKey */
MCR_API struct mcr_ISignal *mcr_iStringKey(struct mcr_context *ctx);
/*! \brief Get the \ref mcr_ISignal for \ref mcr_StringKey */
#define mcr_iSK mcr_iStringKey

/*! \brief \ref mcr_Command ctor
 *
 * \param cmdPt \ref opt \ref mcr_Command *
 * \return 0
 */
MCR_API int mcr_Command_init(void *cmdPt);
/*! \brief Construct an empty command */
MCR_API struct mcr_Command mcr_Command_new();
/*! \brief \ref mcr_Command dtor
 *
 * \param cmdPt \ref opt \ref mcr_Command *
 * \return 0
 */
MCR_API int mcr_Command_deinit(void *cmdPt);
/*! \brief Compare two commands
 *
 * \param lhs \ref opt mcr_Command *
 * \param rhs \ref opt mcr_Command *
 * \return \ref retcmp
 */
MCR_API int mcr_Command_compare(const void *lhs, const void *rhs);
/*! \brief Copy command from source to destination
 *
 * \param dstPt \ref mcr_Command * Destination to copy to
 * \param srcPt \ref opt \ref mcr_Command * Source to copy from
 * \return \ref reterr
 */
MCR_API int mcr_Command_copy(void *dstPt, void *srcPt);

/*! \brief \ref mcr_StringKey ctor
 *
 * \param skPt \ref opt \ref mcr_StringKey *
 * \return 0
 */
MCR_API int mcr_StringKey_init(void *skPt);
/*! \brief Construct an empty string key */
MCR_API struct mcr_StringKey mcr_StringKey_new();
/*! \brief \ref mcr_StringKey dtor
 *
 * \param skPt \ref opt \ref mcr_StringKey *
 * \return 0
 */
MCR_API int mcr_StringKey_deinit(void *skPt);
/*! \brief Compare two string keys
 *
 * \param lhs \ref opt mcr_StringKey *
 * \param rhs \ref opt mcr_StringKey *
 * \return \ref retcmp
 */
MCR_API int mcr_StringKey_compare(const void *lhs, const void *rhs);
/*! \brief Copy string key from source to destination
 *
 * \param dstPt \ref mcr_StringKey * Destination to copy to
 * \param srcPt \ref opt \ref mcr_StringKey * Source to copy from
 * \return \ref reterr
 */
MCR_API int mcr_StringKey_copy(void *dstPt, void *srcPt);

/*! \brief Set executable file
 *
 * \param file \ref opt \ref mcr_Command.file
 * \param cryptic \ref mcr_SafeString.cryptic
 * \return \ref reterr
 */
MCR_API int mcr_Command_set_file(struct mcr_Command *cmdPt,
	const char *file, bool cryptic);
/*! \brief Set command arguments
 *
 * \param argv \ref opt \ref mcr_Command.argv
 * \param cryptic \ref mcr_SafeString.cryptic
 * \return \ref reterr
 */
MCR_API int mcr_Command_set_args(struct mcr_Command *cmdPt,
	char *const argv[], bool cryptic);
/*! \brief Add command arguments
 *
 * \param arg \ref opt Command to push
 * \param cryptic \ref mcr_SafeString.cryptic
 * \return \ref reterr
 */
MCR_API int mcr_Command_add_arg(struct mcr_Command *cmdPt,
	char *const arg, bool cryptic);
/*! \brief Remove all command arguments */
MCR_API void mcr_Command_clear_args(struct mcr_Command *cmdPt);

/*! \brief \ref mcr_send function for \ref mcr_StringKey
 *
 * \return \ref reterr
 */
MCR_API int mcr_StringKey_send(struct mcr_Signal *sigPt);
/*! \brief Send string key without a signal
 *
 * \return \ref reterr
 */
MCR_API int mcr_StringKey_send_data(struct mcr_context *ctx,
	struct mcr_StringKey *skPt);

/*! \brief \ref mcr_send function for \ref mcr_Command
 *
 * \return \ref reterr
 */
MCR_API int mcr_Command_send(struct mcr_Signal *sigPt);
/*! \brief Send command without a signal
 *
 * \return \ref reterr
 */
MCR_API int mcr_Command_send_data(struct mcr_Command *cmdPt);

/* String key development */
/*! \brief Get an array of signals to send for this character
 *
 * \param character Character to find signals for
 * \return Array of \ref mcr_Signal, or null if not found
 */
MCR_API struct mcr_Array *mcr_StringKey_char(struct mcr_context *ctx,
	int character);
/*! \brief Set signals to send for a character
 *
 * \param character Character to set signals for
 * \param charSignalArr \ref opt Set of signals
 * \param arrLen Number of signals to set
 * \return \ref reterr
 */
MCR_API int mcr_StringKey_set_char(struct mcr_context *ctx, int character,
	struct mcr_Signal *charSignalArr, size_t arrLen);
/*! \brief Remove signals for a character
 *
 * \param character Character to remove
 */
MCR_API void mcr_StringKey_remove_char(struct mcr_context *ctx, int character);
/* TODO: mcr_StringKey_set_char, shifted flag */
/*! \brief Set signals for a character that will press, pause, and release a
 * key.
 *
 * \param character Character to set signals for
 * \param key Keyboard key to press and release
 * \param shiftFlag If true a shift key will press before the key, and
 * release after the key.
 * \return \ref reterr
 */
MCR_API int mcr_StringKey_set_char_keys(struct mcr_context *ctx, int character,
	int key, long nsec, bool shiftFlag);
/*! \brief Set default string key pause delay between press and release
 *
 * This will also replace all \ref mcr_NoOp already set.
 * \param delay Apply this pause time.
 * \return \ref reterr
 */
MCR_API int mcr_StringKey_set_delays(struct mcr_context *ctx,
	struct timespec delay);
/*! \brief Get the number of characters set to send signals for
 *
 * \return \ref retind
 */
MCR_API size_t mcr_StringKey_char_count(struct mcr_context *ctx);
/*! \brief Remove all mapped string key characters */
MCR_API void mcr_StringKey_char_clear(struct mcr_context *ctx);
/*! \brief Minimize string key characters allocation */
MCR_API void mcr_StringKey_char_trim(struct mcr_context *ctx);

/*! \brief Get command from signal
 *
 * \param sigPt \ref opt \ref mcr_Signal *
 * \return \ref mcr_Command *
 */
#define mcr_Command_data(sigPt) \
((struct mcr_Command *)((sigPt) ? (sigPt)->instance.data.data : NULL))
/*! \brief Get command from signal
 *
 * \param sigPt \ref opt \ref mcr_Signal *
 * \return \ref mcr_Command *
 */
#define mcr_Cmd_data(sigPt) mcr_Command_data (sigPt)
/*! \brief Get string key from signal
 *
 * \param sigPt \ref opt \ref mcr_Signal *
 * \return \ref mcr_StringKey *
 */
#define mcr_StringKey_data(sigPt) \
((struct mcr_StringKey *)((sigPt) ? (sigPt)->instance.data.data : NULL))
/*! \brief Get string key from signal
 *
 * \param sigPt \ref opt \ref mcr_Signal *
 * \return \ref mcr_StringKey *
 */
#define mcr_SK_data(sigPt) mcr_StringKey_data (sigPt)

/* native directory */
/*! \brief Same as execvp, except a new process is created to not kill
 * this one.
 *
 * \param file Executable file
 * \param args \ref opt Executable command arguments
 * \return \ref reterr
 */
MCR_API int mcr_execvp(const char *file, char *const *args);

#endif
