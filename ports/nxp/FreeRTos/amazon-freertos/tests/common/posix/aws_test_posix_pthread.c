/*
 * Amazon FreeRTOS POSIX Test V1.1.4
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
 * @file aws_test_posix_pthread.c
 * @brief Tests for POSIX pthread functions.
 */

/* C standard library includes. */
#include <stddef.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/utils.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/**
 * @defgroup Configuration constants for the pthread tests that aren't
 * platform-specific.
 */
/**@{ */
#define posixtestPTHREAD_DETACHED_WAIT_MILLISECONDS          ( 100000000 ) /**< How long to wait for a detached thread to finish. */
#define posixtestPTHREAD_COND_BROADCAST_NUMBER_OF_THREADS    ( 4 )         /**< Number of threads that wait on a pthread_cond_broadcast. */
/**@} */

/**
 * @brief The arguments to prvSignalCondThread.
 */
typedef struct SignalCondThreadArgs
{
    int iStatus;             /**< The status the thread reports. */
    int iSharedVariable;     /**< Shared variable for condition. */
    pthread_cond_t * pxCond; /**< Condition variable. */
} SignalCondThreadArgs_t;

/*-----------------------------------------------------------*/

static void * prvComputeSquareThread( void * pvArgs )
{
    int iNumberToSquare = *( ( int * ) pvArgs );

    /* Perform computation, then exit the thread. */
    *( ( int * ) pvArgs ) = iNumberToSquare * iNumberToSquare;
    pthread_exit( pvArgs );

    /* Silence compiler warnings about return values. This line will never be
     * reached. */
    return NULL;
}

/*-----------------------------------------------------------*/

static void * prvUnlockMutexThread( void * pvArgs )
{
    pthread_mutex_t * pxMutex = ( pthread_mutex_t * ) pvArgs;
    intptr_t xStatus = 0;

    /* Attempt to unlock the mutex, then exit. */
    xStatus = ( intptr_t ) pthread_mutex_unlock( pxMutex );
    pthread_exit( ( void * ) xStatus );

    /* Silence compiler warnings about return values. This line will never be
     * reached. */
    return NULL;
}

/*-----------------------------------------------------------*/

static void * prvBarrierThread( void * pvArgs )
{
    pthread_barrier_t * pxBarrier = ( pthread_barrier_t * ) pvArgs;
    intptr_t xStatus = 0;

    /* Enter the barrier, then exit. */
    xStatus = ( intptr_t ) pthread_barrier_wait( pxBarrier );
    pthread_exit( ( void * ) xStatus );

    /* Silence compiler warnings about return values. This line will never be
     * reached. */
    return NULL;
}

/*-----------------------------------------------------------*/

static void * prvSignalCondThread( void * pvArgs )
{
    SignalCondThreadArgs_t * pxArgs = ( SignalCondThreadArgs_t * ) pvArgs;

    /* Increment the shared variable and signal cond. */
    pxArgs->iSharedVariable++;
    pxArgs->iStatus = pthread_cond_signal( pxArgs->pxCond );

    return NULL;
}

/*-----------------------------------------------------------*/

static void * prvWaitCondThread( void * pvArgs )
{
    intptr_t xStatus = 0;
    BaseType_t xMutexAttrCreated = pdFALSE, xMutexCreated = pdFALSE;
    pthread_cond_t * pxCond = ( pthread_cond_t * ) pvArgs;
    pthread_mutexattr_t xMutexAttr;
    pthread_mutex_t xMutex;

    /* Create an error-checking mutex. This mutex types allows verification
     * of mutex owner. */
    xStatus = ( intptr_t ) pthread_mutexattr_init( &xMutexAttr );

    if( xStatus == 0 )
    {
        xMutexAttrCreated = pdTRUE;
        xStatus = ( intptr_t ) pthread_mutexattr_settype( &xMutexAttr,
                                                          PTHREAD_MUTEX_ERRORCHECK );
    }

    if( xStatus == 0 )
    {
        xStatus = ( intptr_t ) pthread_mutex_init( &xMutex, &xMutexAttr );
        xMutexCreated = pdTRUE;
    }

    /* Lock the mutex. */
    if( xStatus == 0 )
    {
        xStatus = ( intptr_t ) pthread_mutex_lock( &xMutex );
    }

    /* Wait for the signal on cond. */
    if( xStatus == 0 )
    {
        xStatus = ( intptr_t ) pthread_cond_wait( pxCond, &xMutex );
    }

    /* Unlock the mutex. This will only succeed if this thread owns the mutex
     * because the mutex type is PTHREAD_MUTEX_ERRORCHECK. */
    if( xStatus == 0 )
    {
        xStatus = ( intptr_t ) pthread_mutex_unlock( &xMutex );
    }

    /* Clean up resources. */
    if( xMutexAttrCreated == pdTRUE )
    {
        ( void ) pthread_mutexattr_destroy( &xMutexAttr );
    }

    if( xMutexCreated == pdTRUE )
    {
        ( void ) pthread_mutex_unlock( &xMutex );
        ( void ) pthread_mutex_destroy( &xMutex );
    }

    pthread_exit( ( void * ) xStatus );

    /* Silence compiler warnings about return values. This line will never be
     * reached. */
    return NULL;
}

/*-----------------------------------------------------------*/

static void prvTestMutexLockUnlock( int iMutexType )
{
    int iStatus = 0, iType = -1;
    volatile BaseType_t xAttrCreated = pdFALSE, xMutexCreated = pdFALSE;
    volatile intptr_t xThreadReturnValue = 0;
    pthread_mutex_t xMutex;
    pthread_mutexattr_t xMutexAttr;
    pthread_t xNewThread;

    if( TEST_PROTECT() )
    {
        /* Create a pthread_mutex_attr_t. */
        iStatus = pthread_mutexattr_init( &xMutexAttr );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        xAttrCreated = pdTRUE;

        /* Set mutex to given type. */
        iStatus = pthread_mutexattr_settype( &xMutexAttr, iMutexType );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Check that the correct type was set. */
        iStatus = pthread_mutexattr_gettype( &xMutexAttr, &iType );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_EQUAL_INT( iMutexType, iType );

        /* Create a new mutex. */
        iStatus = pthread_mutex_init( &xMutex, &xMutexAttr );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        xMutexCreated = pdTRUE;

        /* Lock the mutex. */
        iStatus = pthread_mutex_lock( &xMutex );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Lock an already locked mutex. */
        iStatus = pthread_mutex_lock( &xMutex );

        /* Check behavior based on mutex type. */
        if( iMutexType == PTHREAD_MUTEX_ERRORCHECK )
        {
            /* PTHREAD_MUTEX_ERRORCHECK should return EDEADLK. */
            TEST_ASSERT_EQUAL_INT( EDEADLK, iStatus );
        }
        else
        {
            /* PTHREAD_MUTEX_RECURSIVE should be successfully locked. */
            TEST_ASSERT_EQUAL_INT( 0, iStatus );

            /* Unlock the mutex once. */
            iStatus = pthread_mutex_unlock( &xMutex );
            TEST_ASSERT_EQUAL_INT( 0, iStatus );
        }

        /* Spawn a thread to unlock the mutex. That thread should fail to unlock
         * the mutex because it doesn't own it. */
        ( void ) pthread_create( &xNewThread, NULL, prvUnlockMutexThread, &xMutex );

        /* Wait for the thread to finish, then check that it failed to unlock the
         * mutex. */
        ( void ) pthread_join( xNewThread, ( void ** ) &xThreadReturnValue );
        TEST_ASSERT_EQUAL_INT( EPERM, ( int ) xThreadReturnValue );

        /* Unlock the mutex so it may be destroyed. */
        iStatus = pthread_mutex_unlock( &xMutex );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
    }

    if( xAttrCreated == pdTRUE )
    {
        ( void ) pthread_mutexattr_destroy( &xMutexAttr );
    }

    if( xMutexCreated == pdTRUE )
    {
        ( void ) pthread_mutex_destroy( &xMutex );
    }
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_POSIX_PTHREAD );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_POSIX_PTHREAD )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_POSIX_PTHREAD )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_POSIX_PTHREAD )
{
    RUN_TEST_CASE( Full_POSIX_PTHREAD, pthread_create_join );
    RUN_TEST_CASE( Full_POSIX_PTHREAD, pthread_attr_init_destroy );
    RUN_TEST_CASE( Full_POSIX_PTHREAD, pthread_mutex_lock_unlock );
    RUN_TEST_CASE( Full_POSIX_PTHREAD, pthread_mutex_trylock_timedlock );
    RUN_TEST_CASE( Full_POSIX_PTHREAD, pthread_barrier );
    RUN_TEST_CASE( Full_POSIX_PTHREAD, pthread_cond_signal );
    RUN_TEST_CASE( Full_POSIX_PTHREAD, pthread_cond_broadcast );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_PTHREAD, pthread_create_join )
{
    int iStatus = 0;
    volatile int iNumberToSquare = 5;
    volatile BaseType_t xAttrInitDone = pdFALSE;
    void * pvThreadReturnValue = NULL;
    pthread_t xNewThread = NULL;
    pthread_attr_t xThreadAttr;
    struct timespec xDelay =
    {
        .tv_sec  = 0,
        .tv_nsec = posixtestPTHREAD_DETACHED_WAIT_MILLISECONDS
    };

    if( TEST_PROTECT() )
    {
        /* Create a pthread_attr_t. */
        iStatus = pthread_attr_init( &xThreadAttr );
        TEST_ASSERT_EQUAL_INT_MESSAGE( 0,
                                       iStatus,
                                       "pthread_attr_init failed!" );
        xAttrInitDone = pdTRUE;

        /* Create a detached thread. */
        iStatus = pthread_attr_setdetachstate( &xThreadAttr, PTHREAD_CREATE_DETACHED );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = pthread_create( &xNewThread, &xThreadAttr, prvComputeSquareThread, ( void * ) &iNumberToSquare );
        TEST_ASSERT_EQUAL_INT_MESSAGE( 0,
                                       iStatus,
                                       "Thread creation failed!" );

        /* Attempt to join a detached thread. This should fail with EDEADLK. */
        iStatus = pthread_join( xNewThread, &pvThreadReturnValue );
        TEST_ASSERT_EQUAL_INT( EDEADLK, iStatus );

        /* Wait some time for prvComputeSquareThread to exit and ensure it completed
         * the calculation. */
        nanosleep( &xDelay, NULL );
        TEST_ASSERT_EQUAL_INT( 25, iNumberToSquare );

        /* Create a joinable thread. */
        iStatus = pthread_attr_setdetachstate( &xThreadAttr, PTHREAD_CREATE_JOINABLE );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = pthread_create( &xNewThread, &xThreadAttr, prvComputeSquareThread, ( void * ) &iNumberToSquare );
        TEST_ASSERT_EQUAL_INT_MESSAGE( 0,
                                       iStatus,
                                       "Thread creation failed!" );

        /* Join the created thread. Check it completed its calculation and
         * returned the correct value. */
        iStatus = pthread_join( xNewThread, &pvThreadReturnValue );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        TEST_ASSERT_EQUAL_INT( 625, iNumberToSquare );
        TEST_ASSERT_EQUAL_PTR( &iNumberToSquare, pvThreadReturnValue );
    }

    /* Destroy the thread attribute object if it was created. */
    if( xAttrInitDone == pdTRUE )
    {
        ( void ) pthread_attr_destroy( &xThreadAttr );
    }
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_PTHREAD, pthread_attr_init_destroy )
{
    int iStatus = 0;
    size_t xStackSize1 = 0, xStackSize2 = 0;
    volatile BaseType_t xAttrInitDone = pdFALSE;
    pthread_attr_t xAttr;

    if( TEST_PROTECT() )
    {
        /* Create a pthread_attr_t. */
        iStatus = pthread_attr_init( &xAttr );
        TEST_ASSERT_EQUAL_INT_MESSAGE( 0,
                                       iStatus,
                                       "pthread_attr_init failed!" );
        xAttrInitDone = pdTRUE;

        /* Check we can get and set the stack size. */
        iStatus = pthread_attr_getstacksize( &xAttr, &xStackSize1 );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        iStatus = pthread_attr_setstacksize( &xAttr, xStackSize1 );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        iStatus = pthread_attr_getstacksize( &xAttr, &xStackSize2 );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_EQUAL_INT( xStackSize2, xStackSize1 );

        /* Attempt to set an invalid stack size. */
        xStackSize1 = PTHREAD_STACK_MIN - 1;
        iStatus = pthread_attr_setstacksize( &xAttr, xStackSize1 );
        TEST_ASSERT_EQUAL_INT( EINVAL, iStatus );
    }

    /* Destroy the thread attribute object if it was created. */
    if( xAttrInitDone == pdTRUE )
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE( 0,
                                       pthread_attr_destroy( &xAttr ),
                                       "pthread_attr_destroy failed!" );
    }
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_PTHREAD, pthread_mutex_lock_unlock )
{
    int iStatus = 0;

    /* Test the NORMAL (DEFAULT) mutex type. */
    pthread_mutex_t xMutex = PTHREAD_MUTEX_INITIALIZER;

    /* Check mutex can be locked. */
    iStatus = pthread_mutex_lock( &xMutex );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Check mutex can be unlocked. */
    iStatus = pthread_mutex_unlock( &xMutex );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Run tests on the ERRORCHECK and RECURSIVE mutex types. */
    prvTestMutexLockUnlock( PTHREAD_MUTEX_ERRORCHECK );
    prvTestMutexLockUnlock( PTHREAD_MUTEX_RECURSIVE );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_PTHREAD, pthread_mutex_trylock_timedlock )
{
    int iStatus = 0;
    volatile BaseType_t xMutexCreated = pdFALSE;
    pthread_mutex_t xMutex;
    struct timespec xTimeout;

    if( TEST_PROTECT() )
    {
        /* Create a new DEFAULT mutex. */
        iStatus = pthread_mutex_init( &xMutex, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        xMutexCreated = pdTRUE;

        /* Lock the mutex. */
        iStatus = pthread_mutex_trylock( &xMutex );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Attempt to lock a locked mutex. */
        iStatus = pthread_mutex_trylock( &xMutex );
        TEST_ASSERT_EQUAL_INT( EBUSY, iStatus );

        /* Set an absolute timeout of 100 ms. */
        ( void ) clock_gettime( CLOCK_REALTIME, &xTimeout );
        ( void ) UTILS_TimespecAddNanoseconds( &xTimeout, 100000000LL, &xTimeout );

        /* Attempt to lock mutex with timeout while mutex is still locked. */
        iStatus = pthread_mutex_timedlock( &xMutex, &xTimeout );
        TEST_ASSERT_EQUAL_INT( ETIMEDOUT, iStatus );

        /* Unlock mutex so it can be destroyed. */
        iStatus = pthread_mutex_unlock( &xMutex );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
    }

    if( xMutexCreated == pdTRUE )
    {
        ( void ) pthread_mutex_destroy( &xMutex );
    }
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_PTHREAD, pthread_barrier )
{
    int iStatus = 0;
    intptr_t xThreadReturnValue = 0;
    pthread_t xNewThread;
    pthread_barrier_t xBarrier = { 0 };

    /* Ensure that calling pthread_barrier_init with count=0 returns EINVAL. */
    iStatus = pthread_barrier_init( &xBarrier, NULL, 0U );
    TEST_ASSERT_EQUAL_INT( EINVAL, iStatus );

    /* Initialize a barrier for 2 threads. */
    iStatus = pthread_barrier_init( &xBarrier, NULL, 2U );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    if( TEST_PROTECT() )
    {
        /* Create a thread to wait in the barrier. */
        iStatus = pthread_create( &xNewThread, NULL, prvBarrierThread, ( void * ) &xBarrier );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Wait in the barrier, which will release prvBarrierThread. */
        iStatus = pthread_barrier_wait( &xBarrier );

        /* Join prvBarrierThread and get its return value. */
        TEST_ASSERT_EQUAL_INT( 0, pthread_join( xNewThread, ( void ** ) &xThreadReturnValue ) );

        /* If this thread received 0 from pthread_barrier_wait, then prvBarrierThread
         * should have received PTHREAD_BARRIER_SERIAL_THREAD. */
        if( iStatus == 0 )
        {
            TEST_ASSERT_EQUAL_INT( PTHREAD_BARRIER_SERIAL_THREAD, ( int ) xThreadReturnValue );
        }

        /* Otherwise, if this thread received PTHREAD_BARRIER_SERIAL_THREAD, then
         * prvBarrierThread should have received 0. */
        else if( iStatus == PTHREAD_BARRIER_SERIAL_THREAD )
        {
            TEST_ASSERT_EQUAL_INT( 0, ( int ) xThreadReturnValue );
        }
        else
        {
            /* Bad return values. */
            TEST_FAIL();
        }
    }

    /* Destroy the barrier. */
    iStatus = pthread_barrier_destroy( &xBarrier );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_PTHREAD, pthread_cond_signal )
{
    int iStatus = 0;
    BaseType_t xMutexCreated = pdFALSE;
    volatile BaseType_t xThreadCreated = pdFALSE;
    pthread_cond_t xCond = PTHREAD_COND_INITIALIZER;
    pthread_mutexattr_t xMutexAttr;
    pthread_mutex_t xMutex;
    pthread_t xNewThread;
    SignalCondThreadArgs_t xThreadArgs = { 0 };
    struct timespec xWaitTime = { 0 };

    /* Create an error-checking mutex. This mutex types allows verification
     * of mutex owner. */
    iStatus = pthread_mutexattr_init( &xMutexAttr );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    if( TEST_PROTECT() )
    {
        iStatus = pthread_mutexattr_settype( &xMutexAttr,
                                             PTHREAD_MUTEX_ERRORCHECK );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = pthread_mutex_init( &xMutex, &xMutexAttr );
    }

    ( void ) pthread_mutexattr_destroy( &xMutexAttr );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );
    xMutexCreated = pdTRUE;

    if( TEST_PROTECT() )
    {
        /* Lock mutex and set shared variable to 0. */
        iStatus = pthread_mutex_trylock( &xMutex );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        xThreadArgs.iSharedVariable = 0;

        /* Set an absolute timeout of 100 ms. */
        ( void ) clock_gettime( CLOCK_REALTIME, &xWaitTime );
        ( void ) UTILS_TimespecAddNanoseconds( &xWaitTime, 100000000LL, &xWaitTime );

        /* Waiting on a condition variable that is never signaled should time out. */
        iStatus = pthread_cond_timedwait( &xCond, &xMutex, &xWaitTime );
        TEST_ASSERT_EQUAL_INT( ETIMEDOUT, iStatus );

        /* Create a thread that will signal the condition variable. */
        xThreadArgs.pxCond = &xCond;
        iStatus = pthread_create( &xNewThread, NULL, prvSignalCondThread, &xThreadArgs );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        xThreadCreated = pdTRUE;

        /* Wait for the signal. */
        iStatus = pthread_cond_wait( &xCond, &xMutex );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Check that the signaling thread incremented the shared variable
         * and check its status. */
        TEST_ASSERT_EQUAL_INT( 1, xThreadArgs.iSharedVariable );
        TEST_ASSERT_EQUAL_INT( 0, xThreadArgs.iStatus );

        /* Unlock the mutex. This will only succeed if this thread owns the mutex
         * because the mutex type is PTHREAD_MUTEX_ERRORCHECK. */
        iStatus = pthread_mutex_unlock( &xMutex );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
    }

    if( xMutexCreated == pdTRUE )
    {
        ( void ) pthread_mutex_destroy( &xMutex );
    }

    if( xThreadCreated == pdTRUE )
    {
        ( void ) pthread_join( xNewThread, NULL );
    }
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_PTHREAD, pthread_cond_broadcast )
{
    int i = 0, iCondBroadcastStatus = 0;
    pthread_cond_t xCond = PTHREAD_COND_INITIALIZER;
    pthread_t xThreads[ posixtestPTHREAD_COND_BROADCAST_NUMBER_OF_THREADS ];
    BaseType_t xThreadsCreated[ posixtestPTHREAD_COND_BROADCAST_NUMBER_OF_THREADS ] = { pdFALSE };
    intptr_t xThreadReturnValues[ posixtestPTHREAD_COND_BROADCAST_NUMBER_OF_THREADS ] = { 0 };

    /* Create the threads that wait for pthread_cond_broadcast. */
    for( i = 0; i < posixtestPTHREAD_COND_BROADCAST_NUMBER_OF_THREADS; i++ )
    {
        if( pthread_create( &xThreads[ i ], NULL, prvWaitCondThread, &xCond ) == 0 )
        {
            xThreadsCreated[ i ] = pdTRUE;
        }
    }

    /* Wait half a second for all spawned threads to initialize their mutexes
     * and enter pthread_cond_wait. */
    vTaskDelay( pdMS_TO_TICKS( 500 ) );

    /* Broadcast on cond. */
    iCondBroadcastStatus = pthread_cond_broadcast( &xCond );

    /* Wait for all threads to finish. */
    for( i = 0; i < posixtestPTHREAD_COND_BROADCAST_NUMBER_OF_THREADS; i++ )
    {
        if( xThreadsCreated[ i ] == pdTRUE )
        {
            ( void ) pthread_join( xThreads[ i ], ( void ** ) &xThreadReturnValues[ i ] );
        }
    }

    /* Check results. */
    TEST_ASSERT_EQUAL_INT( 0, iCondBroadcastStatus );

    for( i = 0; i < posixtestPTHREAD_COND_BROADCAST_NUMBER_OF_THREADS; i++ )
    {
        TEST_ASSERT_EQUAL( pdTRUE, xThreadsCreated[ i ] );
        TEST_ASSERT_EQUAL_INT( 0, ( int ) xThreadReturnValues[ i ] );
    }
}

/*-----------------------------------------------------------*/
