/*! \file util/threads.h
 * Temporary solution. Until threading is implemented in C
 * we have to use C++.
 * */

# ifndef MCR_THREADS_H
# define MCR_THREADS_H

# include "util/def.h"

MCR_API int thrd_sleep_until ( struct tm * time_point ) ;

//
// Temporary solution for C11
//
# ifndef _Noreturn
# define _Noreturn
# endif

# ifndef restrict
# define restrict
# endif

//
// enums
//
enum	// undefined values.
{
	thrd_success = 0,
	thrd_nomem,
	thrd_timeout,
	thrd_busy,
	thrd_error
} ;
enum	// undefined values.
{
	mtx_plain = 0,
	mtx_recursive = 1,
	mtx_timed = 2
} ;

//
// types
//
typedef int once_flag ;

typedef int ( * thrd_start_t ) ( void * ) ;

//
// Temporary solution. Until threading is implemented in C we have
// to use C++.
//
// Handles to dynamically allocated C++ objects. ( Make sure to
// destruct on exit. )
typedef void * thrd_t ;
typedef struct mtx_t
{
	void * mtx ;
	int type ;
} mtx_t ;
typedef void * cnd_t ;

//
// C standard thread.
//
MCR_API int thrd_create ( thrd_t * thr, thrd_start_t func, void * arg ) ;
MCR_API int thrd_equal ( thrd_t lhs, thrd_t rhs ) ;
MCR_API thrd_t thrd_current ( ) ;
MCR_API int thrd_sleep ( const struct timespec * time_point,
		struct timespec * remaining ) ;
MCR_API void thrd_yield ( ) ;
_Noreturn void thrd_exit ( int res ) ;
MCR_API int thrd_detach ( thrd_t thr ) ;
MCR_API int thrd_join ( thrd_t thr, int * res ) ;

//
// mutex
//
MCR_API int mtx_init ( mtx_t * mutex, int type ) ;
MCR_API int mtx_lock ( mtx_t * mutex ) ;
MCR_API int mtx_timedlock ( mtx_t * restrict mutex,
		const struct timespec * restrict time_point ) ;
MCR_API int mtx_trylock ( mtx_t * mutex ) ;
MCR_API int mtx_unlock ( mtx_t * mutex ) ;
MCR_API void mtx_destroy ( mtx_t * mutex ) ;

//
// condition
//
MCR_API int cnd_init ( cnd_t * cond ) ;
MCR_API int cnd_signal ( cnd_t * cond ) ;
MCR_API int cnd_broadcast ( cnd_t * cond ) ;
MCR_API int cnd_wait ( cnd_t * cond, mtx_t * mutex ) ;
MCR_API int cnd_timedwait ( cnd_t * restrict cond, mtx_t * restrict mutex,
		const struct timespec * restrict time_point ) ;
MCR_API void cnd_destroy ( cnd_t * cond ) ;

//
// Do not know how to do thread local storage
//

# endif	// MCR_THREADS_H
