
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
	func ( arg ) ;
}

int thrd_create ( thrd_t * thr, thrd_start_t func, void * arg )
{
	 * thr = new std::thread ( thrd_function, func, arg ) ;
	return thrd_success ;
}

int thrd_equal ( thrd_t lhs, thrd_t rhs )
{
	return ( ( std::thread * ) lhs )->get_id ( ) ==
			( ( std::thread * ) rhs )->get_id ( ) ;
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
	( ( std::thread * ) thr )->detach ( ) ;
	delete ( std::thread * ) thr ;
	return thrd_success ;
}

int thrd_join ( thrd_t thr, int * )
{
	( ( std::thread * ) thr )->join ( ) ;
	delete ( std::thread * ) thr ;
	return thrd_success ;
}

//
// mutex
//
int mtx_init ( mtx_t * mutex, int type )
{
	switch ( type )
	{
	case mtx_recursive | mtx_timed :
		mutex->mtx = new std::recursive_timed_mutex ;
		mutex->type = mtx_recursive | mtx_timed ;
		return thrd_success ;
	case mtx_recursive :
		mutex->mtx = new std::recursive_mutex ;
		mutex->type = mtx_recursive ;
		return thrd_success ;
	case mtx_timed :
		mutex->mtx = new std::timed_mutex ;
		mutex->type = mtx_timed ;
		return thrd_success ;
	case mtx_plain :
		mutex->mtx = new std::mutex ;
		mutex->type = mtx_plain ;
		return thrd_success ;
	}
	DMSG ( "%s\n", "mtx_init, incorrect type." ) ;
	return thrd_error ;
}

int mtx_lock ( mtx_t * mutex )
{
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
	DMSG ( "%s\n", "mtx_lock, incorrect type." ) ;
	return thrd_error ;
}

int mtx_timedlock ( mtx_t * restrict mutex,
		const struct timespec * restrict time_point )
{
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
			return caster->try_lock_for ( nsec ) ;
	}
		break ;
	case mtx_timed :
	{
		std::timed_mutex * caster = ( std::timed_mutex * ) mutex->mtx ;
		locked = caster->try_lock_for ( sec ) ;
		if ( ! locked )
			return caster->try_lock_for ( nsec ) ;
	}
		break ;
	default :
		DMSG ( "%s\n", "mtx_timedlock, incorrect type." ) ;
		break ;
	}
	return locked ;
}

int mtx_trylock ( mtx_t * mutex )
{
	switch ( mutex->type )
	{
	case mtx_recursive | mtx_timed :
		return ( ( std::recursive_timed_mutex * ) mutex->mtx )->
				try_lock ( ) ;
	case mtx_recursive :
		return ( ( std::recursive_mutex * ) mutex->mtx )->try_lock ( ) ;
	case mtx_timed :
		return ( ( std::timed_mutex * ) mutex->mtx )->try_lock ( ) ;
	case mtx_plain :
		return ( ( std::mutex * ) mutex->mtx )->try_lock ( ) ;
	}
	DMSG ( "%s\n", "mtx_trylock, incorrect type." )
	return false ;
}

int mtx_unlock ( mtx_t * mutex )
{
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
	DMSG ( "%s\n", "mtx_unlock, incorrect type." )
	return thrd_error ;
}

void mtx_destroy ( mtx_t * mutex )
{
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
		DMSG ( "%s\n", "mtx_destroy, incorrect type." ) ;
		break ;
	}
	mutex->mtx = NULL ;
}

//
// condition
//
int cnd_init ( cnd_t * cond )
{
	 * cond = new std::condition_variable ;
	return thrd_success ;
}

int cnd_signal ( cnd_t * cond )
{
	( * ( std::condition_variable ** ) cond )->notify_one ( ) ;
	return thrd_success ;
}

int cnd_broadcast ( cnd_t * cond )
{
	( * ( std::condition_variable ** ) cond )->notify_all ( ) ;
	return thrd_success ;
}

int cnd_wait ( cnd_t * cond, mtx_t * mutex )
{
	if ( mutex->type == mtx_plain )
	{
		std::condition_variable * caster =
				 * ( std::condition_variable ** ) cond ;
		std::unique_lock<std::mutex> lock
				( * ( ( std::mutex * ) mutex->mtx ) ) ;
		caster->wait ( lock ) ;
		return thrd_success ;
	}
	DMSG ( "%s\n", "cnd_wait, incorrect mutex type." ) ;
	return thrd_error ;
}

int cnd_timedwait ( cnd_t * restrict cond, mtx_t * restrict mutex,
		const struct timespec * restrict time_point )
{
	if ( mutex->type != mtx_plain )
	{
		DMSG ( "%s\n", "cnd_timedwait, incorrect mutex type." ) ;
		return thrd_error ;
	}

	std::unique_lock < std::mutex > lock
			( * ( ( std::mutex * ) mutex->mtx ) ) ;
	std::chrono::seconds sec = std::chrono::seconds
			( time_point->tv_sec ) ;
	std::chrono::nanoseconds nsec = std::chrono::nanoseconds
			( time_point->tv_nsec ) ;
	std::cv_status ret = ( * ( std::condition_variable ** ) cond )->
			wait_for ( lock, sec ) ;
	if ( ret == std::cv_status::no_timeout )
		return thrd_success ;
	ret = ( * ( std::condition_variable ** ) cond )->wait_for
			( lock, nsec ) ;
	if ( ret == std::cv_status::no_timeout )
		return thrd_success ;
	return thrd_timeout ;
}

void cnd_destroy ( cnd_t * cond )
{
	delete * ( std::condition_variable ** ) cond ;
}
