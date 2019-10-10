/* Libmacro - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

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
 *  Temporary solution. Until threading is implemented in C
 *  we have to use either C++ or native threading. For standard C reference
 *  please use a C11 reference.
 */

#ifndef MCR_UTIL_CPPTHREAD_H_
#define MCR_UTIL_CPPTHREAD_H_

#include "mcr/util/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Sleep current thread until given time. */
MCR_API int thrd_sleep_until(struct tm *time_point);

/* */
/* Temporary solution for C11 */
/* */
#ifndef _Noreturn
#define _Noreturn
#endif

/* Error defining key word restrict.  __restrict workaround. */
#ifndef __restrict
#ifdef restrict
#define __restrict restrict
#else
#define __restrict
#endif
#endif

/* */
/* enums */
/* */
enum {				/* undefined values. */
	thrd_success = 0,
	thrd_nomem,
	thrd_timedout,
	thrd_busy,
	thrd_error
};
enum {				/* undefined values. */
	mtx_plain = 0,
	mtx_recursive = 1 << 0,
	mtx_timed = 1 << 1
};

/* */
/* types */
/* */
typedef int once_flag;

typedef int (*thrd_start_t) (void *);

/* */
/* Temporary solution. Until threading is implemented in C we have */
/* to use C++. */
/* */
/* Handles to dynamically allocated C++ objects. ( Make sure to */
/* destruct on exit. ) */
typedef void *thrd_t;
typedef struct {
	void *mtx;
	int type;
} mtx_t;
typedef void *cnd_t;

/* */
/* C standard thread. */
/* */
MCR_API int thrd_create(thrd_t * thr, thrd_start_t func, void *arg);
MCR_API int thrd_equal(thrd_t lhs, thrd_t rhs);
MCR_API thrd_t thrd_current();
MCR_API int thrd_sleep(const struct timespec *time_point,
					   struct timespec *remaining);
MCR_API void thrd_yield();
_Noreturn void thrd_exit(int res);
MCR_API int thrd_detach(thrd_t thr);
MCR_API int thrd_join(thrd_t thr, int *res);
MCR_API void mcr_thrd_delete(thrd_t * thr);

/* */
/* mutex */
/* */
MCR_API int mtx_init(mtx_t * mutex, int type);
MCR_API int mtx_lock(mtx_t * mutex);
MCR_API int mtx_timedlock(mtx_t * __restrict mutex,
						  const struct timespec *__restrict time_point);
MCR_API int mtx_trylock(mtx_t * mutex);
MCR_API int mtx_unlock(mtx_t * mutex);
MCR_API void mtx_destroy(mtx_t * mutex);

/* */
/* condition */
/* */
MCR_API int cnd_init(cnd_t * cond);
MCR_API int cnd_signal(cnd_t * cond);
MCR_API int cnd_broadcast(cnd_t * cond);
/*! Pre: mutex must be mtx_plain. */
MCR_API int cnd_wait(cnd_t * cond, mtx_t * mutex);
/*! Pre: mutex must be mtx_plain. */
MCR_API int cnd_timedwait(cnd_t * __restrict cond, mtx_t * __restrict mutex,
						  const struct timespec *__restrict time_point);
MCR_API void cnd_destroy(cnd_t * cond);

/* */
/* Do not know how to do thread local storage */
/* */

#ifdef __cplusplus
}
#endif

#endif
