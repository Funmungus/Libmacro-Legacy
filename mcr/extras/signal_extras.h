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

#ifndef MCR_SIGNALEXTRAS_H
#define MCR_SIGNALEXTRAS_H

#include "mcr/extras/safe_string.h"

/*!
 *
 * All members are arrays of safe strings.
 */
struct mcr_Command {
	struct mcr_SafeString file;
	/* Internal */
	struct mcr_Array argv;
	struct mcr_Array env;
};

/* TODO: unicode support */
struct mcr_StringKey {
	struct timespec interval;
	struct mcr_SafeString string;
};

MCR_API struct mcr_ISignal *mcr_iCommand(struct mcr_context *ctx);
#define mcr_iCmd mcr_iCommand
MCR_API struct mcr_ISignal *mcr_iStringKey(struct mcr_context *ctx);
#define mcr_iSK mcr_iStringKey

MCR_API void mcr_Command_init(void *cmdDataPt);
MCR_API void mcr_Command_free(void *cmdDataPt);
MCR_API int mcr_Command_compare(const void *lhs, const void *rhs);
MCR_API int mcr_Command_copy(void *dstPt, void *srcPt);

MCR_API void mcr_StringKey_init(void *skDataPt);
MCR_API void mcr_StringKey_free(void *skDataPt);
MCR_API int mcr_StringKey_compare(const void *lhs, const void *rhs);
MCR_API int mcr_StringKey_copy(void *dstPt, void *srcPt);

MCR_API int mcr_Command_set_file(struct mcr_Command *cmdPt,
	const char *file, bool cryptic);
MCR_API int mcr_Command_set_args(struct mcr_Command *cmdPt,
	char *const argv[], bool cryptic);
MCR_API int mcr_Command_add_arg(struct mcr_Command *cmdPt,
	char *const arg, bool cryptic);
MCR_API void mcr_Command_clear_args(struct mcr_Command *cmdPt);
MCR_API int mcr_Command_set_env(struct mcr_Command *cmdPt,
	char *const envp[], bool cryptic);
MCR_API int mcr_Command_add_env(struct mcr_Command *cmdPt,
	char *const env, bool cryptic);
MCR_API void mcr_Command_clear_env(struct mcr_Command *cmdPt);

MCR_API int mcr_StringKey_send(struct mcr_Signal *sigPt);
MCR_API int mcr_StringKey_send_data(struct mcr_context *ctx,
	struct mcr_StringKey *skPt);

MCR_API int mcr_Command_send(struct mcr_Signal *sigPt);
MCR_API int mcr_Command_send_data(struct mcr_Command *cmdPt);

/* */
/* String key development */
/* */
MCR_API struct mcr_Array *mcr_StringKey_char(struct mcr_context *ctx,
	int character);
MCR_API int mcr_StringKey_set_char(struct mcr_context *ctx, int character,
	struct mcr_Signal *charSignalArr, size_t arrLen);
MCR_API void mcr_StringKey_remove_char(struct mcr_context *ctx, int character);
/* TODO: mcr_StringKey_set_char, shifted flag */
MCR_API int mcr_StringKey_set_char_keys(struct mcr_context *ctx, int character,
	int key, long nsec, bool shiftFlag);
MCR_API int mcr_StringKey_set_delays(struct mcr_context *ctx,
	struct timespec delay);
MCR_API size_t mcr_StringKey_char_count(struct mcr_context *ctx);
MCR_API void mcr_StringKey_char_clear(struct mcr_context *ctx);
MCR_API void mcr_StringKey_char_trim(struct mcr_context *ctx);

#define mcr_Command_data(sigPt) \
((struct mcr_Command *)((sigPt) ? (sigPt)->inst.data.data : NULL))
#define mcr_Cmd_data(sigPt) mcr_Command_data (sigPt)
#define mcr_StringKey_data(sigPt) \
((struct mcr_StringKey *)((sigPt) ? (sigPt)->inst.data.data : NULL))
#define mcr_SK_data(sigPt) mcr_StringKey_data (sigPt)

/* */
/* Implemented in native directory */
/* */
/*!
 * \brief Same as execvpe, except a new process is created to not kill
 * this one.
 *
 * \return \ref reterr
 */
MCR_API int mcr_execvpe(const char *file, char *const *args, char *const *env);

#endif
