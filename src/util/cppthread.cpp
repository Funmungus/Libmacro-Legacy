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

/* Extern C */
extern "C" {
#include "mcr/util/util.h"
#include "mcr/util/cppthread.h"
}

/* C++ thread requirements */
//#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <mutex>
#include <thread>

int thrd_sleep_until(struct tm *time_point)
{
	if (!time_point) {
		dmsg;
		return thrd_error;
	}
	std::chrono::system_clock::time_point until_time =
		std::chrono::system_clock::from_time_t(std::mktime(time_point));
	std::this_thread::sleep_until(until_time);
	return thrd_success;
}

/* */
/* C standard thread. */
/* */
/* thrd_start_t wrapper.  C thread functions have an error return, while
 * C++ thread functions have no return and variable arguments. */
static void thrd_function(thrd_start_t func, void *arg)
{
	dassert(func);
	if (func(arg) != thrd_success)
		dmsg;
}

int thrd_create(thrd_t * thr, thrd_start_t func, void *arg)
{
	dassert(thr);
	*thr = new std::thread(thrd_function, func, arg);
	return thrd_success;
}

int thrd_equal(thrd_t lhs, thrd_t rhs)
{
	if (lhs && rhs) {
		return static_cast<std::thread *>(lhs)->get_id() ==
			   static_cast<std::thread *>(rhs)->get_id();
	}
	return !(lhs || rhs);
}

/* std:: impl unknown */
thrd_t thrd_current()
{
	return nullptr;
}

int thrd_sleep(const struct timespec *time_point, struct timespec *remaining)
{
	/* Output seconds, nanoseconds unknown */
	std::this_thread::sleep_for(std::chrono::seconds(time_point->tv_sec));
	std::this_thread::sleep_for
	(std::chrono::nanoseconds(time_point->tv_nsec));
	if (remaining != nullptr) {
		time_t t = std::time(nullptr);
		std::tm tm = *std::localtime(&t);
		remaining->tv_sec = tm.tm_sec;
	}
	return thrd_success;
}

void thrd_yield()
{
	std::this_thread::yield();
}

/* std:: impl unknown */
/*_Noreturn void thrd_exit ( int res ) ; */

int thrd_detach(thrd_t thr)
{
	dassert(thr);
	static_cast<std::thread *>(thr)->detach();
	delete static_cast<std::thread *>(thr);
	return thrd_success;
}

int thrd_join(thrd_t thr, int *res)
{
	dassert(thr);
	static_cast<std::thread *>(thr)->join();
	delete static_cast<std::thread *>(thr);
	*res = thrd_success;
	return thrd_success;
}

void mcr_thrd_delete(thrd_t * thr)
{
	delete static_cast<std::thread *>(*thr);
	*thr = nullptr;
}

/* */
/* mutex */
/* */
int mtx_init(mtx_t * mutex, int type)
{
	dassert(mutex);
	switch (type) {
	case mtx_recursive | mtx_timed:
		mutex->mtx = new std::recursive_timed_mutex;
		mutex->type = type;
		return thrd_success;
	case mtx_recursive:
		mutex->mtx = new std::recursive_mutex;
		mutex->type = type;
		return thrd_success;
	case mtx_timed:
		mutex->mtx = new std::timed_mutex;
		mutex->type = type;
		return thrd_success;
	case mtx_plain:
		mutex->mtx = new std::mutex;
		mutex->type = type;
		return thrd_success;
	}
	dmsg;
	return thrd_error;
}

int mtx_lock(mtx_t * mutex)
{
	dassert(mutex);
	switch (mutex->type) {
	case mtx_recursive | mtx_timed:
		static_cast<std::recursive_timed_mutex *>(mutex->mtx)->lock();
		return thrd_success;
	case mtx_recursive:
		static_cast<std::recursive_mutex *>(mutex->mtx)->lock();
		return thrd_success;
	case mtx_timed:
		static_cast<std::timed_mutex *>(mutex->mtx)->lock();
		return thrd_success;
	case mtx_plain:
		static_cast<std::mutex *>(mutex->mtx)->lock();
		return thrd_success;
	}
	dmsg;
	return thrd_error;
}

int mtx_timedlock(mtx_t * __restrict mutex,
				  const struct timespec *__restrict time_point)
{
	dassert(mutex);
	dassert(time_point);
	std::chrono::seconds sec(time_point->tv_sec);
	std::chrono::nanoseconds nsec(time_point->tv_nsec);
	bool locked = false;
	switch (mutex->type) {
	case mtx_recursive | mtx_timed: {
		std::recursive_timed_mutex * caster =
			static_cast<std::recursive_timed_mutex *>(mutex->mtx);
		locked = caster->try_lock_for(sec);
		if (!locked)
			locked = caster->try_lock_for(nsec);
	}
	break;
	case mtx_timed: {
		std::timed_mutex * caster =
			static_cast<std::timed_mutex *>(mutex->mtx);
		locked = caster->try_lock_for(sec);
		if (!locked)
			locked = caster->try_lock_for(nsec);
	}
	break;
	default:
		dmsg;
		break;
	}
	return locked ? thrd_success : thrd_timedout;
}

int mtx_trylock(mtx_t * mutex)
{
	dassert(mutex);
	bool locked = false;
	switch (mutex->type) {
	case mtx_recursive | mtx_timed:
		locked = static_cast<std::recursive_timed_mutex *>(mutex->mtx)->
				 try_lock();
		break;
	case mtx_recursive:
		locked = static_cast<std::recursive_mutex *>(mutex->mtx)->try_lock();
		break;
	case mtx_timed:
		locked = static_cast<std::timed_mutex *>(mutex->mtx)->try_lock();
		break;
	case mtx_plain:
		locked = static_cast<std::mutex *>(mutex->mtx)->try_lock();
		break;
	default:
		dmsg;
		break;
	}
	return locked ? thrd_success : thrd_busy;
}

int mtx_unlock(mtx_t * mutex)
{
	dassert(mutex);
	switch (mutex->type) {
	case mtx_recursive | mtx_timed:
		static_cast<std::recursive_timed_mutex *>(mutex->mtx)->unlock();
		return thrd_success;
	case mtx_recursive:
		static_cast<std::recursive_mutex *>(mutex->mtx)->unlock();
		return thrd_success;
	case mtx_timed:
		static_cast<std::timed_mutex *>(mutex->mtx)->unlock();
		return thrd_success;
	case mtx_plain:
		static_cast<std::mutex *>(mutex->mtx)->unlock();
		return thrd_success;
	}
	dmsg;
	return thrd_error;
}

void mtx_destroy(mtx_t * mutex)
{
	int thrdErr;
	dassert(mutex);
	/// \todo Test mal-effects of unlocking before destroying mutex
	/* Wait for unlock, cannot destroy locked mutex. */
	if ((thrdErr = mtx_lock(mutex)) != thrd_success) {
		mcr_err = mcr_thrd_errno(thrdErr);
		dmsg;
	} else {
		mtx_unlock(mutex);
	}
	switch (mutex->type) {
	case mtx_recursive | mtx_timed:
		delete static_cast<std::recursive_timed_mutex *>(mutex->mtx);
		break;
	case mtx_recursive:
		delete static_cast<std::recursive_mutex *>(mutex->mtx);
		break;
	case mtx_timed:
		delete static_cast<std::timed_mutex *>(mutex->mtx);
		break;
	case mtx_plain:
		delete static_cast<std::mutex *>(mutex->mtx);
		break;
	default:
		dmsg;
		return;
	}
	mutex->mtx = nullptr;
}

/* */
/* condition */
/* */
int cnd_init(cnd_t * cond)
{
	dassert(cond);
	*cond = new std::condition_variable;
	return thrd_success;
}

int cnd_signal(cnd_t * cond)
{
	dassert(cond);
	static_cast<std::condition_variable *>(*cond)->notify_one();
	return thrd_success;
}

int cnd_broadcast(cnd_t * cond)
{
	dassert(cond);
	static_cast<std::condition_variable *>(*cond)->notify_all();
	return thrd_success;
}

int cnd_wait(cnd_t * cond, mtx_t * mutex)
{
	dassert(cond);
	dassert(mutex);
	if (mutex->type != mtx_plain) {
		dmsg;
		return thrd_error;
	}

	std::condition_variable * caster = static_cast<std::condition_variable *>
									   (*cond);
	std::unique_lock<std::mutex> lock
	(*static_cast<std::mutex *>(mutex->mtx), std::adopt_lock_t());
	caster->wait(lock);
	return thrd_success;
}

int cnd_timedwait(cnd_t * __restrict cond, mtx_t * __restrict mutex,
				  const struct timespec *__restrict time_point)
{
	dassert(cond);
	dassert(mutex);
	dassert(time_point);
	if (mutex->type != mtx_plain) {
		dmsg;
		return thrd_error;
	}

	std::condition_variable * caster = static_cast<std::condition_variable *>
									   (*cond);
	std::chrono::seconds sec = std::chrono::seconds(time_point->tv_sec);
	std::chrono::nanoseconds nsec = std::chrono::nanoseconds
									(time_point->tv_nsec);
	std::cv_status ret = std::cv_status::no_timeout;
	std::unique_lock < std::mutex > lock
	(*static_cast<std::mutex *>(mutex->mtx), std::adopt_lock_t());
	ret = caster->wait_for(lock, sec);
	/* Timed out seconds, try for nsec */
	if (ret != std::cv_status::no_timeout)
		ret = caster->wait_for(lock, nsec);
	/* Condition has been met for either sec or nsec. */
	if (ret == std::cv_status::no_timeout)
		return thrd_success;
	return thrd_timedout;
}

void cnd_destroy(cnd_t * cond)
{
	dassert(cond);
	delete static_cast<std::condition_variable *>(*cond);
	*cond = nullptr;
}
