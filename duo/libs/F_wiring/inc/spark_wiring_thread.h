/**
 ******************************************************************************
 * @file    spark_wiring_thread.h
 * @authors Matthew McGowan
 * @date    03 March 2015
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#ifndef SPARK_WIRING_THREAD_H
#define	SPARK_WIRING_THREAD_H

#if PLATFORM_THREADING

#include "concurrent_hal.h"
#include <stddef.h>
#include <mutex>
#include <functional>
#include <type_traits>

typedef std::function<os_thread_return_t(void)> wiring_thread_fn_t;

class Thread
{
private:
    mutable os_thread_t handle;
    mutable wiring_thread_fn_t *wrapper;

public:
    Thread() : handle(OS_THREAD_INVALID_HANDLE) {}

    Thread(const char* name, os_thread_fn_t function, void* function_param=NULL,
            os_thread_prio_t priority=OS_THREAD_PRIORITY_DEFAULT, size_t stack_size=OS_THREAD_STACK_SIZE_DEFAULT)
        : wrapper(NULL)
    {
        os_thread_create(&handle, name, priority, function, function_param, stack_size);
    }

    Thread(const char *name, wiring_thread_fn_t function,
            os_thread_prio_t priority=OS_THREAD_PRIORITY_DEFAULT, size_t stack_size=OS_THREAD_STACK_SIZE_DEFAULT)
        : handle(OS_THREAD_INVALID_HANDLE), wrapper(NULL)
    {
        if(function) {
            wrapper = new wiring_thread_fn_t(function);
            os_thread_create(&handle, name, priority, call_wiring_handler, wrapper, stack_size);
        }
    }

    void dispose()
    {
        if(wrapper) {
            delete wrapper;
            wrapper = NULL;
        }
        // This call will not return if handle is the current thread
        os_thread_cleanup(handle);
    }

    bool join()
    {
        return os_thread_join(handle)==0;
    }

    bool is_valid()
    {
        // TODO should this also check xTaskIsTaskFinished as well?
        return handle!=OS_THREAD_INVALID_HANDLE;
    }

    bool is_current()
    {
        return os_thread_is_current(handle);
    }

    Thread& operator = (const Thread& rhs)
    {
        if (this != &rhs)
        {
            this->handle = rhs.handle;
            this->wrapper = rhs.wrapper;
            rhs.handle = OS_THREAD_INVALID_HANDLE;
            rhs.wrapper = NULL;
        }
        return *this;
    }

private:

    static os_thread_return_t call_wiring_handler(void *param) {
        auto wrapper = (wiring_thread_fn_t*)(param);
        (*wrapper)();
        os_thread_cleanup(nullptr);
    }
};

class Mutex
{
	os_mutex_t handle_;
public:
	/**
	 * Creates a shared mutex from an existing handle.
	 * This is mainly used to share mutexes between dynamically linked modules.
	 */
	Mutex(os_mutex_t handle) : handle_(handle) {}

	/**
	 * Creates a new mutex.
	 */
	Mutex() : handle_(nullptr)
	{
		os_mutex_create(&handle_);
	}

	void dispose()
	{
		if (handle_) {
			os_mutex_destroy(handle_);
			handle_ = nullptr;
		}
	}

	void lock() { os_mutex_lock(handle_); }
	bool trylock() { return os_mutex_trylock(handle_)==0; }
	void unlock() { os_mutex_unlock(handle_); }

};


class RecursiveMutex
{
	os_mutex_recursive_t handle_;
public:
	/**
	 * Creates a shared mutex.
	 */
	RecursiveMutex(os_mutex_recursive_t handle) : handle_(handle) {}

	RecursiveMutex() : handle_(nullptr)
	{
		os_mutex_recursive_create(&handle_);
	}

	void dispose()
	{
		if (handle_) {
			os_mutex_recursive_destroy(handle_);
			handle_ = nullptr;
		}
	}

	void lock() { os_mutex_recursive_lock(handle_); }
	bool trylock() { return os_mutex_recursive_trylock(handle_)==0; }
	void unlock() { os_mutex_recursive_unlock(handle_); }

};


class SingleThreadedSection {
public:
	SingleThreadedSection() {
        os_thread_scheduling(false, NULL);
    }

    ~SingleThreadedSection() {
        os_thread_scheduling(true, NULL);
    }
};

#define SINGLE_THREADED_SECTION()  SingleThreadedSection __cs;

#define SINGLE_THREADED_BLOCK() for (bool __todo = true; __todo; ) for (SingleThreadedSection __cs; __todo; __todo=0)
#define WITH_LOCK(lock) for (bool __todo = true; __todo;) for (std::lock_guard<decltype(lock)> __lock##lock((lock)); __todo; __todo=0)
#define TRY_LOCK(lock) for (bool __todo = true; __todo; ) for (std::unique_lock<typename std::remove_reference<decltype(lock)>::type> __lock##lock((lock), std::try_to_lock); __todo &= bool(__lock##lock); __todo=0)

#else
#define SINGLE_THREADED_SECTION()
#define SINGLE_THREADED_BLOCK()
#define WITH_LOCK(x)
#define TRY_LOCK(x)

#endif

#endif	/* SPARK_WIRING_THREAD_H */

