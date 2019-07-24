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
 * @file FreeRTOS_POSIX_pthread_barrier.c
 * @brief Implementation of barrier functions in pthread.h
 */

/* C standard library includes. */
#include <stddef.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/pthread.h"

/*
 * Barriers are implemented on FreeRTOS event groups, of which 8 bits are usable
 * when configUSE_16_BIT_TICKS is 1. Otherwise, 24 bits are usable.
 */
#if ( configUSE_16_BIT_TICKS == 1 )
    #define posixPTHREAD_BARRIER_MAX_COUNT        ( 8 )
#else
    #define posixPTHREAD_BARRIER_MAX_COUNT        ( 24 )
#endif

/*-----------------------------------------------------------*/

int pthread_barrier_destroy( pthread_barrier_t * barrier )
{
    pthread_barrier_internal_t * pxBarrier = ( pthread_barrier_internal_t * ) ( barrier );

    /* Free all resources used by the barrier. */
    ( void ) vEventGroupDelete( ( EventGroupHandle_t ) &pxBarrier->xBarrierEventGroup );
    ( void ) vSemaphoreDelete( ( SemaphoreHandle_t ) &pxBarrier->xThreadCountMutex );
    ( void ) vSemaphoreDelete( ( SemaphoreHandle_t ) &pxBarrier->xThreadCountSemaphore );

    return 0;
}

/*-----------------------------------------------------------*/

int pthread_barrier_init( pthread_barrier_t * barrier,
                          const pthread_barrierattr_t * attr,
                          unsigned count )
{
    int iStatus = 0;
    pthread_barrier_internal_t * pxNewBarrier = ( pthread_barrier_internal_t * ) ( barrier );

    /* Silence warnings about unused parameters. */
    ( void ) attr;

    /* Ensure count is greater than 0. */
    if( count == 0 )
    {
        iStatus = EINVAL;
    }

    /* Ensure that count will fit in a FreeRTOS event group. */
    if( iStatus == 0 )
    {
        if( count > posixPTHREAD_BARRIER_MAX_COUNT )
        {
            /* No memory exists in the event group for more than
             * posixPTHREAD_BARRIER_MAX_COUNT threads. */
            iStatus = ENOMEM;
        }
    }

    if( iStatus == 0 )
    {
        /* Set the current thread count and threshold. */
        pxNewBarrier->uThreadCount = 0;
        pxNewBarrier->uThreshold = count;

        /* Create the FreeRTOS event group. This call will not fail when its
         * argument isn't NULL. */
        ( void ) xEventGroupCreateStatic( &pxNewBarrier->xBarrierEventGroup );

        /* Create the mutex that guards access to uThreadCount. This call
         * will not fail when its argument isn't NULL. */
        ( void ) xSemaphoreCreateMutexStatic( &pxNewBarrier->xThreadCountMutex );

        /* Create the semaphore that prevents more than count threads from being
         * unblocked by a single successful pthread_barrier_wait. This semaphore
         * counts down from count and cannot decrement below 0. */
        ( void ) xSemaphoreCreateCountingStatic( ( UBaseType_t ) count, /* Max count. */
                                                 ( UBaseType_t ) count, /* Initial count. */
                                                 &pxNewBarrier->xThreadCountSemaphore );
    }

    return iStatus;
}

/*-----------------------------------------------------------*/

int pthread_barrier_wait( pthread_barrier_t * barrier )
{
    int iStatus = 0;
    unsigned i = 0; /* Loop iterator. */
    pthread_barrier_internal_t * pxBarrier = ( pthread_barrier_internal_t * ) ( barrier );
    unsigned uThreadNumber = 0;

    /* Decrement the number of threads waiting on this barrier. This will prevent more
     * than pxBarrier->uThreshold threads from being unblocked by a single successful
     * pthread_barrier_wait call.
     *
     * This call will never fail because it blocks forever.
     */
    ( void ) xSemaphoreTake( ( SemaphoreHandle_t ) &pxBarrier->xThreadCountSemaphore, portMAX_DELAY );

    /* Lock the mutex so that this thread can change uThreadCount. This call will
     * never fail because it blocks forever.*/
    ( void ) xSemaphoreTake( ( SemaphoreHandle_t ) &pxBarrier->xThreadCountMutex, portMAX_DELAY );

    /* Increment thread count. This is the order that this thread entered the
     * barrier, i.e. uThreadNumber-1 threads entered the barrier before this one. */
    pxBarrier->uThreadCount++;
    uThreadNumber = pxBarrier->uThreadCount;
    configASSERT( uThreadNumber > 0 );

    /* Unlock the thread count mutex to allow other threads to change thread count.
     * This call will never fail because xThreadCountMutex is owned by this thread. */
    ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) &pxBarrier->xThreadCountMutex );

    /* Set the bit in the event group representing this thread, then wait for the other
     * threads to set their bit. This call should wait forever until all threads have set
     * their bit, so the return value is ignored. */
    ( void ) xEventGroupSync( ( EventGroupHandle_t ) &pxBarrier->xBarrierEventGroup,
                              1 << ( uThreadNumber - 1 ),         /* Which bit in the event group to set. */
                              ( 1 << pxBarrier->uThreshold ) - 1, /* Wait for all threads to set their bits. */
                              portMAX_DELAY );

    /* The first thread to enter the barrier gets PTHREAD_BARRIER_SERIAL_THREAD as its
     * return value and resets xThreadCountSemaphore. */
    if( uThreadNumber == 1 )
    {
        iStatus = PTHREAD_BARRIER_SERIAL_THREAD;

        /* uThreadCount can be safely changed without locking xThreadCountMutex
         * because xThreadCountSemaphore is currently 0. */
        pxBarrier->uThreadCount = 0;

        /* Reset xThreadCountSemaphore. This allows more threads to enter the
         * barrier, starting a new cycle. */
        for( i = 0; i < pxBarrier->uThreshold; i++ )
        {
            xSemaphoreGive( ( SemaphoreHandle_t ) &pxBarrier->xThreadCountSemaphore );
        }
    }

    return iStatus;
}
