/**7
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

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

#ifndef SYSTEM_THREADING_H
#define	SYSTEM_THREADING_H

#if PLATFORM_THREADING

#include "concurrent_hal.h"
#include "active_object.h"
#include <stddef.h>
#include <functional>
#include <mutex>
#include <future>

#ifndef PARTICLE_GTHREAD_INCLUDED
#error "GTHREAD header not included. This is required for correct mutex implementation on embedded platforms."
#endif

/**
 * System thread runs on a separate thread
 */
extern ActiveObjectThreadQueue SystemThread;

/**
 * Application queue runs on the calling thread (main)
 */
extern ActiveObjectCurrentThreadQueue ApplicationThread;

#endif


// execute synchronously on the system thread. Since the parameter lifetime is
// assumed to be bound by the caller, the parameters don't need marshalling
// fn: the function call to perform. This is textually substitued into a lambda, with the
// parameters passed by copy.
#if PLATFORM_THREADING

template<typename T>
struct memfun_type
{
    using type = void;
};

template<typename Ret, typename Class, typename... Args>
struct memfun_type<Ret(Class::*)(Args...) const>
{
    using type = std::function<Ret(Args...)>;
};

template<typename F>
typename memfun_type<decltype(&F::operator())>::type
FFL(F const &func)
{ // Function from lambda !
    return func;
}

#define _THREAD_CONTEXT_ASYNC_RESULT(thread, fn, result) \
    if (thread.isStarted() && !thread.isCurrentThread()) { \
        auto lambda = [=]() { (fn); }; \
        thread.invoke_async(FFL(lambda)); \
        return result; \
    }

#define _THREAD_CONTEXT_ASYNC(thread, fn) \
    if (thread.isStarted() && !thread.isCurrentThread()) { \
        auto lambda = [=]() { (fn); }; \
        thread.invoke_async(FFL(lambda)); \
        return; \
    }

#define SYSTEM_THREAD_CONTEXT_SYNC(fn) \
    if (SystemThread.isStarted() && !SystemThread.isCurrentThread()) { \
        auto callable = FFL([=]() { return (fn); }); \
        auto future = SystemThread.invoke_future(callable); \
        auto result = future ? future->get() : 0;  \
        delete future; \
        return result; \
    }

#else

#define _THREAD_CONTEXT_ASYNC(thread, fn)
#define _THREAD_CONTEXT_ASYNC_RESULT(thread, fn, result)
#define SYSTEM_THREAD_CONTEXT_SYNC(fn) 
#endif

#define SYSTEM_THREAD_CONTEXT_ASYNC(fn) _THREAD_CONTEXT_ASYNC(SystemThread, fn)
#define SYSTEM_THREAD_CONTEXT_ASYNC_RESULT(fn, result) _THREAD_CONTEXT_ASYNC_RESULT(SystemThread, fn, result)
#define APPLICATION_THREAD_CONTEXT_ASYNC(fn) _THREAD_CONTEXT_ASYNC(ApplicationThread, fn)
#define APPLICATION_THREAD_CONTEXT_ASYNC_RESULT(fn, result) _THREAD_CONTEXT_ASYNC_RESULT(ApplicationThread, fn, result)

// Perform an asynchronous function call if not on the system thread,
// or execute directly if on the system thread
#define SYSTEM_THREAD_CONTEXT_ASYNC_CALL(fn) \
    SYSTEM_THREAD_CONTEXT_ASYNC(fn); \
    fn;

#define SYSTEM_THREAD_CONTEXT_SYNC_CALL(fn) \
    SYSTEM_THREAD_CONTEXT_SYNC(fn); \
    fn;

#define SYSTEM_THREAD_CONTEXT_SYNC_CALL_RESULT(fn) \
    SYSTEM_THREAD_CONTEXT_SYNC(fn); \
    return fn;



#if PLATFORM_THREADING
#define SYSTEM_THREAD_CURRENT() (SystemThread.isCurrentThread())
#define APPLICATION_THREAD_CURRENT() (ApplicationThread.isCurrentThread())
#else
#define SYSTEM_THREAD_CURRENT() (1)
#define APPLICATION_THREAD_CURRENT() (1)
#endif


#endif	/* SYSTEM_THREADING_H */

