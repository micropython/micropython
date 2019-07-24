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

#ifndef _FREERTOS_POSIX_INTERNAL_H_
#define _FREERTOS_POSIX_INTERNAL_H_

/**
 * @file FreeRTOS_POSIX_internal.h
 * @brief Internal structs and initializers for FreeRTOS+POSIX.
 */

/* Amazon FreeRTOS includes. */
#include "aws_doubly_linked_list.h"

/**
 * @brief Mutex attribute object.
 */
#if posixconfigENABLE_PTHREAD_MUTEXATTR_T == 1
    typedef struct pthread_mutexattr_internal
    {
        int iType; /**< Mutex type. */
    } pthread_mutexattr_internal_t;
#endif

#if posixconfigENABLE_PTHREAD_MUTEX_T == 1
    /**
     * @brief Mutex.
     */
    typedef struct pthread_mutex_internal
    {
        BaseType_t xIsInitialized;          /**< Set to pdTRUE if this mutex is initialized, pdFALSE otherwise. */
        StaticSemaphore_t xMutex;           /**< FreeRTOS mutex. */
        TaskHandle_t xTaskOwner;            /**< Owner; used for deadlock detection and permission checks. */
        pthread_mutexattr_internal_t xAttr; /**< Mutex attributes. */
    } pthread_mutex_internal_t;

    /**
     * @brief Compile-time initializer of pthread_mutex_internal_t.
     */
    #define FREERTOS_POSIX_MUTEX_INITIALIZER \
        ( ( ( pthread_mutex_internal_t )     \
        {                                    \
            .xIsInitialized = pdFALSE,       \
            .xMutex = { { 0 } },             \
            .xTaskOwner = NULL,              \
            .xAttr = { .iType = 0 }          \
        }                                    \
          )                                  \
        )
#endif

#if posixconfigENABLE_PTHREAD_COND_T == 1
    /**
     * @brief Condition variable.
     */
    typedef struct pthread_cond_internal
    {
        BaseType_t xIsInitialized;            /**< Set to pdTRUE if this condition variable is initialized, pdFALSE otherwise. */
        StaticSemaphore_t xCondMutex;         /**< Prevents concurrent accesses to iWaitingThreads. */
        StaticSemaphore_t xCondWaitSemaphore; /**< Threads block on this semaphore in pthread_cond_wait. */
        int iWaitingThreads;                  /**< The number of threads currently waiting on this condition variable. */
    } pthread_cond_internal_t;

    /**
     * @brief Compile-time initializer of pthread_cond_internal_t.
     */
    #define FREERTOS_POSIX_COND_INITIALIZER \
        ( ( ( pthread_cond_internal_t )     \
        {                                   \
            .xIsInitialized = pdFALSE,      \
            .xCondMutex = { { 0 } },        \
            .xCondWaitSemaphore = { { 0 } },\
            .iWaitingThreads = 0            \
        }                                   \
          )                                 \
        )
#endif

#if posixconfigENABLE_SEM_T == 1
    /**
     * @brief Semaphore type.
     */
    typedef struct
    {
        StaticSemaphore_t xSemaphore; /**< FreeRTOS semaphore. */
    } sem_internal_t;
#endif

#if posixconfigENABLE_PTHREAD_BARRIER_T == 1
    /**
     * @brief Barrier object.
     */
    typedef struct pthread_barrier_internal
    {
        unsigned uThreadCount;                   /**< Current number of threads that have entered barrier. */
        unsigned uThreshold;                     /**< The count argument of pthread_barrier_init. */
        StaticSemaphore_t xThreadCountMutex;     /**< Guards access to uThreadCount. */
        StaticSemaphore_t xThreadCountSemaphore; /**< Prevents more than uThreshold threads from exiting pthread_barrier_wait at once. */
        StaticEventGroup_t xBarrierEventGroup;   /**< FreeRTOS event group that blocks to wait on threads entering barrier. */
    } pthread_barrier_internal_t;
#endif

#endif /* _FREERTOS_POSIX_INTERNAL_H_ */
