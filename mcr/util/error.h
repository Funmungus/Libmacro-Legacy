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
 * Standard error codes memorized by Libmacro\n
 * Refer to \c errno.h
 */

#ifndef MCR_UTIL_ERROR_H
#define MCR_UTIL_ERROR_H

#include "mcr/util/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Last \ref errno known by Libmacro
 *
 * \return \ref reterr
 */
MCR_API int mcr_error();
/*! \brief Set a new error code for \ref mcr_error */
MCR_API void mcr_set_error(int errorNumber);
/*! \brief Use errno to set \ref mcr_error
 *
 * Also print a debugging message
 * \param defaultErrorNumber Use this error code if errno is not set
 * \param fileString File name
 * \param line Line number in file
 * \return Errno or defaultErrorNumber
 */
MCR_API int mcr_set_errno(int defaultErrorNumber, const char *fileString,
			  int line);

#ifdef __cplusplus
}
#endif
#endif
