/* util/threads.cpp
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

// Extern C
extern "C" {
# include "util/threads.h"
}
// C++ thread requirements
# include <thread>
# include <ctime>
# include <mutex>
# include <condition_variable>

int thrd_sleep_until ( struct tm * time_point )
{
	if ( ! time_point )
		return thrd_error ;
	std::chrono::system_clock::time_point until_time =
			std::chrono::system_clock::from_time_t
			( std::mktime ( time_point ) ) ;
	std::this_thread::sleep_until ( until_time ) ;
	return thrd_success ;
}


//
// C standard thread.
//
// C++ thread function has different syntax from C thread function.
static void thrd_function ( thrd_start_t func, void * arg )
{
	dassert ( func ) ;
	if ( func ( arg ) )
	{
		dmsg ( "Thread function returns failure.\n" ) ;
	}
}

int thrd_create ( thrd_t * thr, thrd_start_t func, void * arg )
{
	dassert ( thr ) ;
	 * thr = new std::thread ( thrd_function, func, arg ) ;
	return thrd_success ;
}

int thrd_equal ( thrd_t lhs, thrd_t rhs )
{
	if ( lhs && rhs )
		return ( ( std::thread * ) lhs )->get_id ( ) ==
				( ( std::thread * ) rhs )->get_id ( ) ;
	return ! ( lhs || rhs ) ;
}


// std:: impl unknown
//thrd_t thrd_current ( ) ;

int thrd_sleep ( const struct timespec * time_point,
		struct timespec * remaining )
{
	// Output seconds, nanoseconds unknown
	if ( remaining != NULL )
		remaining->tv_sec = std::time ( NULL ) ;
	std::this_thread::sleep_for
			( std::chrono::seconds ( time_point->tv_sec ) ) ;
	std::this_thread::sleep_for
			( std::chrono::nanoseconds ( time_point->tv_nsec ) ) ;
	if ( remaining != NULL )
		remaining->tv_sec = std::time ( NULL ) - remaining->tv_sec ;
	return thrd_success ;
}

void thrd_yield ( )
{
	std::this_thread::yield ( ) ;
}

// std:: impl unknown
//_Noreturn void thrd_exit ( int res ) ;

int thrd_detach ( thrd_t thr )
{
	dassert ( thr ) ;
	( ( std::thread * ) thr )->detach ( ) ;
	delete ( std::thread * ) thr ;
	return thrd_success ;
}

int thrd_join ( thrd_t thr, int * )
{
	dassert ( thr ) ;
	( ( std::thread * ) thr )->join ( ) ;
	delete ( std::thread * ) thr ;
	return thrd_success ;
}

//
// mutex
//
int mtx_init ( mtx_t * mutex, int type )
{
	dassert ( mutex ) ;
	switch ( type )
	{
	case mtx_recursive | mtx_timed :
		mutex->mtx = new std::recursive_timed_mutex ;
		mutex->type = type ;
		return thrd_success ;
	case mtx_recursive :
		mutex->mtx = new std::recursive_mutex ;
		mutex->type = type ;
		return thrd_success ;
	case mtx_timed :
		mutex->mtx = new std::timed_mutex ;
		mutex->type = type ;
		return thrd_success ;
	case mtx_plain :
		mutex->mtx = new std::mutex ;
		mutex->type = type ;
		return thrd_success ;
	}
	dmsg ( "mtx_init, incorrect type.\n" ) ;
	return thrd_error ;
}

int mtx_lock ( mtx_t * mutex )
{
	dassert ( mutex ) ;
	switch ( mutex->type )
	{
	case mtx_recursive | mtx_timed :
		( ( std::recursive_timed_mutex * ) mutex->mtx )->lock ( ) ;
		return thrd_success ;
	case mtx_recursive :
		( ( std::recursive_mutex * ) mutex->mtx )->lock ( ) ;
		return thrd_success ;
	case mtx_timed :
		( ( std::timed_mutex * ) mutex->mtx )->lock ( ) ;
		return thrd_success ;
	case mtx_plain :
		( ( std::mutex * ) mutex->mtx )->lock ( ) ;
		return thrd_success ;
	}
	dmsg ( "mtx_lock, incorrect type.\n" ) ;
	return thrd_error ;
}

int mtx_timedlock ( mtx_t * restrict mutex,
		const struct timespec * restrict time_point )
{
	dassert ( mutex ) ;
	dassert ( time_point ) ;
	std::chrono::seconds sec ( time_point->tv_sec ) ;
	std::chrono::nanoseconds nsec ( time_point->tv_nsec ) ;
	bool locked = false ;
	switch ( mutex->type )
	{
	case mtx_recursive | mtx_timed :
	{
		std::recursive_timed_mutex * caster =
				( std::recursive_timed_mutex * ) mutex->mtx ;
		locked = caster->try_lock_for ( sec ) ;
		if ( ! locked )
			locked = caster->try_lock_for ( nsec ) ;
	}
		break ;
	case mtx_timed :
	{
		std::timed_mutex * caster = ( std::timed_mutex * ) mutex->mtx ;
		locked = caster->try_lock_for ( sec ) ;
		if ( ! locked )
			locked = caster->try_lock_for ( nsec ) ;
	}
		break ;
	default :
		dmsg ( "mtx_timedlock, incorrect type.\n" ) ;
		break ;
	}
	return locked ? thrd_success : thrd_timeout ;
}

int mtx_trylock ( mtx_t * mutex )
{
	dassert ( mutex ) ;
	bool locked = false ;
	switch ( mutex->type )
	{
	case mtx_recursive | mtx_timed :
		locked = ( ( std::recursive_timed_mutex * ) mutex->mtx )->
				try_lock ( ) ;
		break ;
	case mtx_recursive :
		locked = ( ( std::recursive_mutex * ) mutex->mtx )->try_lock ( ) ;
		break ;
	case mtx_timed :
		locked = ( ( std::timed_mutex * ) mutex->mtx )->try_lock ( ) ;
		break ;
	case mtx_plain :
		locked = ( ( std::mutex * ) mutex->mtx )->try_lock ( ) ;
		break ;
	default :
		dmsg ( "mtx_trylock, incorrect type.\n" ) ;
		break ;
	}
	return locked ? thrd_success : thrd_timeout ;
}

int mtx_unlock ( mtx_t * mutex )
{
	dassert ( mutex ) ;
	switch ( mutex->type )
	{
	case mtx_recursive | mtx_timed :
		( ( std::recursive_timed_mutex * ) mutex->mtx )->unlock ( ) ;
		return thrd_success ;
	case mtx_recursive :
		( ( std::recursive_mutex * ) mutex->mtx )->unlock ( ) ;
		return thrd_success ;
	case mtx_timed :
		( ( std::timed_mutex * ) mutex->mtx )->unlock ( ) ;
		return thrd_success ;
	case mtx_plain :
		( ( std::mutex * ) mutex->mtx )->unlock ( ) ;
		return thrd_success ;
	}
	dmsg ( "mtx_unlock, incorrect type.\n" )
	return thrd_error ;
}

void mtx_destroy ( mtx_t * mutex )
{
	dassert ( mutex ) ;
	switch ( mutex->type )
	{
	case mtx_recursive | mtx_timed :
		delete ( std::recursive_timed_mutex * ) mutex->mtx ;
		break ;
	case mtx_recursive :
		delete ( std::recursive_mutex * ) mutex->mtx ;
		break ;
	case mtx_timed :
		delete ( std::timed_mutex * ) mutex->mtx ;
		break ;
	case mtx_plain :
		delete ( std::mutex * ) mutex->mtx ;
		break ;
	default :
		dmsg ( "mtx_destroy, incorrect type.\n" ) ;
		return ;
	}
	mutex->mtx = NULL ;
}

//
// condition
//
int cnd_init ( cnd_t * cond )
{
	dassert ( cond ) ;
	 * cond = new std::condition_variable ;
	return thrd_success ;
}

int cnd_signal ( cnd_t * cond )
{
	dassert ( cond ) ;
	( * ( std::condition_variable ** ) cond )->notify_one ( ) ;
	return thrd_success ;
}

int cnd_broadcast ( cnd_t * cond )
{
	dassert ( cond ) ;
	( * ( std::condition_variable ** ) cond )->notify_all ( ) ;
	return thrd_success ;
}

int cnd_wait ( cnd_t * cond, mtx_t * mutex )
{
	dassert ( cond ) ;
	dassert ( mutex ) ;
	if ( mutex->type != mtx_plain )
	{
		dmsg ( "cnd_wait, incorrect mutex type.\n" ) ;
		return thrd_error ;
	}

	std::condition_variable * caster =
			 * ( std::condition_variable ** ) cond ;
	std::unique_lock < std::mutex > lock
			( * ( ( std::mutex * ) mutex->mtx ),
			std::adopt_lock_t ( ) ) ;
	caster->wait ( lock ) ;
	return thrd_success ;
}

int cnd_timedwait ( cnd_t * restrict cond, mtx_t * restrict mutex,
		const struct timespec * restrict time_point )
{
	dassert ( cond ) ;
	dassert ( mutex ) ;
	dassert ( time_point ) ;
	if ( mutex->type != mtx_plain )
	{
		dmsg ( "cnd_timedwait, incorrect mutex type.\n" ) ;
		return thrd_error ;
	}

	std::condition_variable * caster =
			 * ( ( std::condition_variable ** ) cond ) ;
	std::chrono::seconds sec = std::chrono::seconds
			( time_point->tv_sec ) ;
	std::chrono::nanoseconds nsec = std::chrono::nanoseconds
			( time_point->tv_nsec ) ;
	std::cv_status ret = std::cv_status::no_timeout ;
	std::unique_lock < std::mutex > lock
			( * ( ( std::mutex * ) mutex->mtx ), std::adopt_lock_t ( ) ) ;
	ret = caster->wait_for ( lock, sec ) ;
	if ( ret != std::cv_status::no_timeout )
		ret = caster->wait_for ( lock, nsec ) ;
	if ( ret == std::cv_status::no_timeout )
		return thrd_success ;
	return thrd_timeout ;
}

void cnd_destroy ( cnd_t * cond )
{
	dassert ( cond ) ;
	delete * ( std::condition_variable ** ) cond ;
}
