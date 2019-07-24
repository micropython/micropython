/*
 * Amazon FreeRTOS+POSIX V1.0.2
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file pthread.h
 * @brief Threads.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html
 */

#ifndef _FREERTOS_POSIX_PTHREAD_H_
#define _FREERTOS_POSIX_PTHREAD_H_

/* FreeRTOS+POSIX includes. POSIX states that this header shall make symbols
 * defined in sched.h and time.h visible. */
#include "FreeRTOS_POSIX/sched.h"
#include "FreeRTOS_POSIX/time.h"

/**
 * @defgroup pthread detach state.
 */
/**@{ */
#define PTHREAD_CREATE_DETACHED    0       /**< Detached. */
#define PTHREAD_CREATE_JOINABLE    1       /**< Joinable (default). */
/**@} */

/**
 * @brief Returned to a single thread after a successful pthread_barrier_wait.
 *
 * POSIX specifies that this value should be distinct from any other value returned
 * by pthread_barrier_wait, so it's defined as negative to distinguish it from the
 * errnos, which are positive.
 */
#define PTHREAD_BARRIER_SERIAL_THREAD    ( -2 )

/**
 * @defgroup Mutex types.
 */
/**@{ */
#ifndef PTHREAD_MUTEX_NORMAL
    #define PTHREAD_MUTEX_NORMAL        0                        /**< Non-robust, deadlock on relock, does not remember owner. */
#endif
#ifndef PTHREAD_MUTEX_ERRORCHECK
    #define PTHREAD_MUTEX_ERRORCHECK    1                        /**< Non-robust, error on relock,  remembers owner. */
#endif
#ifndef PTHREAD_MUTEX_RECURSIVE
    #define PTHREAD_MUTEX_RECURSIVE     2                        /**< Non-robust, recursive relock, remembers owner. */
#endif
#ifndef PTHREAD_MUTEX_DEFAULT
    #define PTHREAD_MUTEX_DEFAULT       PTHREAD_MUTEX_NORMAL     /**< PTHREAD_MUTEX_NORMAL (default). */
#endif
/**@} */

/**
 * @defgroup Compile-time initializers.
 */
/**@{ */
#if posixconfigENABLE_PTHREAD_COND_T == 1
    #define PTHREAD_COND_INITIALIZER         FREERTOS_POSIX_COND_INITIALIZER  /**< pthread_cond_t. */
#else
    /**
     * pthread cond initializer place holder for compilation to go through
     * for the ports that don't define PTHREAD_COND_INITIALIZER (for example: esp)
     */
    #define PTHREAD_COND_INITIALIZER  ((pthread_cond_t) 0xFFFFFFFF)
#endif

#if posixconfigENABLE_PTHREAD_MUTEX_T == 1
    #define PTHREAD_MUTEX_INITIALIZER    FREERTOS_POSIX_MUTEX_INITIALIZER /**< pthread_mutex_t. */
#endif

/**@} */

/**
 * @brief Destroy the thread attributes object.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_destroy.html
 */
int pthread_attr_destroy( pthread_attr_t * attr );

/**
 * @brief Get detachstate attribute.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_getdetachstate.html
 */
int pthread_attr_getdetachstate( const pthread_attr_t * attr,
                                 int * detachstate );

/**
 * @brief Get schedparam attribute.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_getschedparam.html
 */
int pthread_attr_getschedparam( const pthread_attr_t * attr,
                                struct sched_param * param );

/**
 * @brief Get stacksize attribute.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_getstacksize.html
 */
int pthread_attr_getstacksize( const pthread_attr_t * attr,
                               size_t * stacksize );

/**
 * @brief Initialize the thread attributes object.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_init.html
 *
 * @note Currently, only stack size, sched_param, and detach state attributes
 * are supported.
 */
int pthread_attr_init( pthread_attr_t * attr );

/**
 * @brief Set detachstate attribute.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_setdetachstate.html
 */
int pthread_attr_setdetachstate( pthread_attr_t * attr,
                                 int detachstate );

/**
 * @brief Set schedparam attribute.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_setschedparam.html
 */
int pthread_attr_setschedparam( pthread_attr_t * attr,
                                const struct sched_param * param );

/**
 * @brief Set stacksize attribute.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_setstacksize.html
 */
int pthread_attr_setstacksize( pthread_attr_t * attr,
                               size_t stacksize );

/**
 * @brief Destroy a barrier object.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_barrier_destroy.html
 */
int pthread_barrier_destroy( pthread_barrier_t * barrier );

/**
 * @brief Initialize a barrier object.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_barrier_init.html
 *
 * @note attr is ignored. count may be at most 8 when configUSE_16_BIT_TICKS is 1;
 * it may be at most 24 otherwise.
 */
int pthread_barrier_init( pthread_barrier_t * barrier,
                          const pthread_barrierattr_t * attr,
                          unsigned count );

/**
 * @brief Synchronize at a barrier.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_barrier_wait.html
 */
int pthread_barrier_wait( pthread_barrier_t * barrier );

/**
 * @brief Thread creation.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_create.html
 */
int pthread_create( pthread_t * thread,
                    const pthread_attr_t * attr,
                    void *( *startroutine )( void * ),
                    void * arg );

/**
 * @brief Broadcast a condition.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_broadcast.html
 */
int pthread_cond_broadcast( pthread_cond_t * cond );

/**
 * @brief Destroy condition variables.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_destroy.html
 */
int pthread_cond_destroy( pthread_cond_t * cond );

/**
 * @brief Initialize condition variables.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_init.html
 *
 * @note attr is ignored.
 */
int pthread_cond_init( pthread_cond_t * cond,
                       const pthread_condattr_t * attr );

/**
 * @brief Signal a condition.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_signal.html
 */
int pthread_cond_signal( pthread_cond_t * cond );

/**
 * @brief Wait on a condition with a timeout.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_timedwait.html
 */
int pthread_cond_timedwait( pthread_cond_t * cond,
                            pthread_mutex_t * mutex,
                            const struct timespec * abstime );

/**
 * @brief Wait on a condition.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_wait.html
 */
int pthread_cond_wait( pthread_cond_t * cond,
                       pthread_mutex_t * mutex );

/**
 * @brief Compare thread IDs.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_equal.html
 */
int pthread_equal( pthread_t t1,
                   pthread_t t2 );

/**
 * @brief Thread termination.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_exit.html
 */
void pthread_exit( void * value_ptr );

/**
 * @brief Dynamic thread scheduling parameters access.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_getschedparam.html
 *
 * @note policy is always set to SCHED_OTHER by this function.
 */
int pthread_getschedparam( pthread_t thread,
                           int * policy,
                           struct sched_param * param );

/**
 * @brief Wait for thread termination.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_join.html
 */
int pthread_join( pthread_t thread,
                  void ** retval );

/**
 * @brief Destroy a mutex.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_destroy.html
 */
int pthread_mutex_destroy( pthread_mutex_t * mutex );

/**
 * @brief Initialize a mutex.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_init.html
 */
int pthread_mutex_init( pthread_mutex_t * mutex,
                        const pthread_mutexattr_t * attr );

/**
 * @brief Lock a mutex.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_lock.html
 */
int pthread_mutex_lock( pthread_mutex_t * mutex );

/**
 * @brief Lock a mutex with timeout.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_timedlock.html
 */
int pthread_mutex_timedlock( pthread_mutex_t * mutex,
                             const struct timespec * abstime );

/**
 * @brief Attempt to lock a mutex. Fail immediately if mutex is already locked.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_trylock.html
 */
int pthread_mutex_trylock( pthread_mutex_t * mutex );

/**
 * @brief Unlock a mutex.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_unlock.html
 */
int pthread_mutex_unlock( pthread_mutex_t * mutex );

/**
 * @brief Destroy the mutex attributes object.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutexattr_destroy.html
 */
int pthread_mutexattr_destroy( pthread_mutexattr_t * attr );

/**
 * @brief Get the mutex type attribute.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutexattr_gettype.html
 */
int pthread_mutexattr_gettype( const pthread_mutexattr_t * attr,
                               int * type );

/**
 * @brief Initialize the mutex attributes object.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutexattr_init.html
 *
 * @note Currently, only the type attribute is supported.
 */
int pthread_mutexattr_init( pthread_mutexattr_t * attr );

/**
 * @brief Set the mutex type attribute.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutexattr_settype.html
 */
int pthread_mutexattr_settype( pthread_mutexattr_t * attr,
                               int type );

/**
 * @brief Get the calling thread ID.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_self.html
 */
pthread_t pthread_self( void );

/**
 * @brief Dynamic thread scheduling parameters access.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_setschedparam.html
 *
 * @note policy is ignored; only priority (param.sched_priority) may be changed.
 */
int pthread_setschedparam( pthread_t thread,
                           int policy,
                           const struct sched_param * param );

#endif /* _FREERTOS_POSIX_PTHREAD_H_ */
