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
 * @file aws_test_posix_stress.c
 * @brief Stress tests for FreeRTOS+POSIX.
 */

/* C standard library includes. */
#include <stddef.h>
#include <string.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/pthread.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/**
 * @brief How many times prvChangeErrnoThread modifies errno.
 */
#define posixtestERRNO_CHANGE_ITERATIONS    ( 100 )

/**
 * @defgroup Configuration constants for the mqueue stress test.
 */
/**@{ */
#define posixtestMQUEUE_STRESS_NUMBER_OF_THREADS    ( 10 )                                   /**< Number of sender and receiver threads. */
#define posixtestMQUEUE_STRESS_MESSAGE              "Hello world!"                           /**< Message sent to queue. */
#define posixtestMQUEUE_STRESS_MESSAGE_SIZE         sizeof( posixtestMQUEUE_STRESS_MESSAGE ) /**< Length of message. */
#define posixtestMQUEUE_STRESS_TIMEOUT_SECONDS      ( 1 )                                    /**< Relative timeout for mqueue functions. */
/**@} */

/**
 * @defgroup Configuration constants for the mutex stress test.
 */
/**@{ */
#define posixtestMUTEX_STRESS_ITERATIONS           ( 10 ) /**< How many times the test should be repeated. */
#define posixtestMUTEX_STRESS_NUMBER_OF_THREADS    ( 12 ) /**< Number of mutex test threads. */
/**@} */

/**
 * @defgroup Configuration constants for the barrier stress test.
 */
/**@{ */
#define posixtestBARRIER_STRESS_COUNT    ( 12 )    /**< The count argument for the barriers. */
/**@} */

/**
 * @brief The arguments to prvChangeErrnoThread.
 */
typedef struct ChangeErrnoThreadArgs
{
    int iInitialErrno;             /**< Initial value for this thread's errno. */
    pthread_barrier_t * pxBarrier; /**< Synchronization barrier. */
    BaseType_t xResult;            /**< Output parameter for result. */
} ChangeErrnoThreadArgs_t;

/**
 * @brief The arguments to all of the mutex test threads.
 */
typedef struct MutexTestThreadArgs
{
    volatile int * piSharedVariable; /**< Pointer to the shared variable to modify. */
    pthread_mutex_t * pxMutex;       /**< Mutex which protects the shared variable. */
} MutexTestThreadArgs_t;

/**
 * @brief The arguments to all of the barrier test threads.
 */
typedef struct BarrierTestThreadArgs
{
    pthread_barrier_t * pxBarrier;   /**< Barrier to test. */
    volatile int * piWaitingThreads; /**< How many threads are waiting on pxBarrier. */
} BarrierTestThreadArgs_t;

/*-----------------------------------------------------------*/

static void * prvChangeErrnoThread( void * pvArgs )
{
    int i = 0, iNewErrno = 0;
    ChangeErrnoThreadArgs_t * pxArgs = ( ChangeErrnoThreadArgs_t * ) pvArgs;

    /* Wait for the test runner thread to create both the pthread and FreeRTOS
     * task. */
    ( void ) pthread_barrier_wait( pxArgs->pxBarrier );

    /* Get the initial errno for this thread. */
    iNewErrno = pxArgs->iInitialErrno;

    /* Repeatedly change errno to test that it's thread-safe. */
    for( i = 0; i < posixtestERRNO_CHANGE_ITERATIONS; i++ )
    {
        /* Change errno. */
        errno = iNewErrno;

        /* Yield the processor to give the other thread time to change errno. */
        taskYIELD();

        /* Ensure that errno wasn't affected by the other thread. */
        if( errno != iNewErrno )
        {
            break;
        }

        /* Generate a new value for errno. */
        iNewErrno++;
    }

    /* Ensure that the test ran for the expected number of iterations. */
    if( i < posixtestERRNO_CHANGE_ITERATIONS )
    {
        pxArgs->xResult = pdFAIL;
    }
    else
    {
        pxArgs->xResult = pdPASS;
    }

    /* Enter the synchronization barrier again to let the test runner thread know
     * this thread is finished. */
    ( void ) pthread_barrier_wait( pxArgs->pxBarrier );

    /* Identify which thread is calling this function. The FreeRTOS task is
     * identified by a name that isn't posixconfigPTHREAD_TASK_NAME. */
    if( strcmp( pcTaskGetName( NULL ), posixconfigPTHREAD_TASK_NAME ) != 0 )
    {
        /* The FreeRTOS task suspends and waits for the test runner thread to
         * delete it. */
        vTaskSuspend( NULL );
    }

    /* The pthread returns NULL and waits for pthread_join to delete it. */
    return NULL;
}

/*-----------------------------------------------------------*/

static void * prvQueueSenderThread( void * pvArgs )
{
    int i = 0;
    intptr_t iStatus = 0;
    mqd_t xMqId = *( ( mqd_t * ) pvArgs );
    struct timespec xTimeout = { 0 };

    for( i = 0; i < posixconfigMQ_MAX_MESSAGES; i++ )
    {
        /* Set the timeout for sending. */
        ( void ) clock_gettime( CLOCK_REALTIME, &xTimeout );
        xTimeout.tv_sec += posixtestMQUEUE_STRESS_TIMEOUT_SECONDS;

        /* Send message. */
        if( mq_timedsend( xMqId,
                          posixtestMQUEUE_STRESS_MESSAGE,
                          posixtestMQUEUE_STRESS_MESSAGE_SIZE,
                          0,
                          &xTimeout ) == -1 )
        {
            break;
        }
    }

    /* If all messages successfully sent, set status to success. */
    if( i == posixconfigMQ_MAX_MESSAGES )
    {
        iStatus = 1;
    }

    return ( void * ) iStatus;
}

/*-----------------------------------------------------------*/

static void * prvQueueReceiverThread( void * pvArgs )
{
    int i = 0;
    intptr_t iStatus = 0;
    mqd_t xMqId = *( ( mqd_t * ) pvArgs );
    struct timespec xTimeout = { 0 };
    ssize_t xMessageSize = 0;
    char pcReceiveBuffer[ posixtestMQUEUE_STRESS_MESSAGE_SIZE ] = { 0 };

    for( i = 0; i < posixconfigMQ_MAX_MESSAGES; i++ )
    {
        /* Set the timeout for receiving. */
        ( void ) clock_gettime( CLOCK_REALTIME, &xTimeout );
        xTimeout.tv_sec += posixtestMQUEUE_STRESS_TIMEOUT_SECONDS;

        /* Receive message. */
        xMessageSize = mq_timedreceive( xMqId,
                                        pcReceiveBuffer,
                                        posixtestMQUEUE_STRESS_MESSAGE_SIZE,
                                        0,
                                        &xTimeout );

        /* Check that the message size is correct. */
        if( xMessageSize != posixtestMQUEUE_STRESS_MESSAGE_SIZE )
        {
            break;
        }

        /* Check that the correct message was received. */
        if( strcmp( pcReceiveBuffer, posixtestMQUEUE_STRESS_MESSAGE ) != 0 )
        {
            break;
        }
    }

    /* All messages successfully received, set status to success. */
    if( i == posixconfigMQ_MAX_MESSAGES )
    {
        iStatus = 1;
    }

    return ( void * ) iStatus;
}

/*-----------------------------------------------------------*/

static void * prvMutexTestThread( void * pvArgs )
{
    intptr_t iResult = 0;
    int iExpectedValue = 0;
    MutexTestThreadArgs_t * pxArgs = ( MutexTestThreadArgs_t * ) pvArgs;

    /* Lock the mutex to protect access to the shared variable. */
    iResult = ( intptr_t ) ( pthread_mutex_lock( pxArgs->pxMutex ) == 0 );

    if( iResult )
    {
        /* Increment the shared variable, then save its value. */
        ( *( pxArgs->piSharedVariable ) )++;
        iExpectedValue = *( pxArgs->piSharedVariable );

        /* Yield the processor to give other threads time to change the shared
         * variable. */
        ( void ) sched_yield();

        /* Ensure that the shared variable is unchanged, as it should be protected
         * by the mutex. */
        iResult = ( intptr_t ) ( *( pxArgs->piSharedVariable ) == iExpectedValue );
    }

    ( void ) pthread_mutex_unlock( pxArgs->pxMutex );

    return ( void * ) iResult;
}

/*-----------------------------------------------------------*/

static void * prvBarrierTestThread( void * pvArgs )
{
    intptr_t iResult = 0;
    BarrierTestThreadArgs_t * pxArgs = ( BarrierTestThreadArgs_t * ) pvArgs;

    /* Increment the number of threads waiting on the barrier atomically. */
    vTaskSuspendAll();
    ( *( pxArgs->piWaitingThreads ) )++;
    ( void ) xTaskResumeAll();

    /* Wait on a barrier. */
    iResult = pthread_barrier_wait( pxArgs->pxBarrier );

    /* A successful barrier wait should return PTHREAD_BARRIER_SERIAL_THREAD or
     * 0. Ensure that the barrier wait was successful. */
    iResult = ( ( iResult == 0 ) || ( iResult == PTHREAD_BARRIER_SERIAL_THREAD ) );

    /* Decrement the number of threads waiting on the barrier atomically. */
    vTaskSuspendAll();
    ( *( pxArgs->piWaitingThreads ) )--;
    ( void ) xTaskResumeAll();

    return ( void * ) iResult;
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_POSIX_STRESS );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_POSIX_STRESS )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_POSIX_STRESS )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_POSIX_STRESS )
{
    RUN_TEST_CASE( Full_POSIX_STRESS, errno_multithreaded );
    RUN_TEST_CASE( Full_POSIX_STRESS, mqueue );
    RUN_TEST_CASE( Full_POSIX_STRESS, pthread_mutex );
    RUN_TEST_CASE( Full_POSIX_STRESS, pthread_barrier_overflow );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_STRESS, errno_multithreaded )
{
    volatile BaseType_t xPthreadCreated = pdFALSE, xTaskCreated = pdFALSE;
    ChangeErrnoThreadArgs_t xPthreadArgs = { 0 }, xTaskArgs = { 0 };
    TaskHandle_t xTaskHandle;
    pthread_t xPthread;
    pthread_barrier_t xBarrier;

    /* Set errno to 0 for this thread. */
    errno = 0;

    /* Set the barrier for both the pthread and FreeRTOS task. */
    xPthreadArgs.pxBarrier = &xBarrier;
    xTaskArgs.pxBarrier = &xBarrier;

    /* Set different initial values for errno for the pthread and FreeRTOS task. */
    xPthreadArgs.iInitialErrno = posixtestERRNO_CHANGE_ITERATIONS;
    xTaskArgs.iInitialErrno = -posixtestERRNO_CHANGE_ITERATIONS;

    /* Create the barrier used to synchronize this thread and the two threads
     * it creates. */
    TEST_ASSERT_EQUAL_INT( 0, pthread_barrier_init( &xBarrier, NULL, 3U ) );

    if( TEST_PROTECT() )
    {
        /* Create a pthread to change errno. */
        TEST_ASSERT_EQUAL_INT( 0, pthread_create( &xPthread,
                                                  NULL,
                                                  prvChangeErrnoThread,
                                                  &xPthreadArgs ) );
        xPthreadCreated = pdTRUE;

        /* Create a FreeRTOS task to change errno. */
        TEST_ASSERT_EQUAL( pdTRUE, xTaskCreate( ( TaskFunction_t ) prvChangeErrnoThread,
                                                "TestErrno",
                                                configMINIMAL_STACK_SIZE,
                                                &xTaskArgs,
                                                tskIDLE_PRIORITY,
                                                &xTaskHandle ) );
        xTaskCreated = pdTRUE;

        /* Enter the synchronization barrier so the pthread and FreeRTOS task
         * begin their test at roughly the same time. */
        ( void ) pthread_barrier_wait( &xBarrier );

        /* Wait for both the FreeRTOS task and pthread to finish. */
        ( void ) pthread_barrier_wait( &xBarrier );

        /* Check that this thread's errno wasn't modified; check that both threads
         * successfully finished their test. */
        TEST_ASSERT_EQUAL_INT( 0, errno );
        TEST_ASSERT_EQUAL( pdPASS, xPthreadArgs.xResult );
        TEST_ASSERT_EQUAL( pdPASS, xTaskArgs.xResult );
    }

    /* Delete the FreeRTOS task. */
    if( xTaskCreated == pdTRUE )
    {
        vTaskDelete( xTaskHandle );
    }

    /* Join the pthread. */
    if( xPthreadCreated == pdTRUE )
    {
        pthread_join( xPthread, NULL );
    }

    /* Destroy the synchronization barrier. */
    ( void ) pthread_barrier_destroy( &xBarrier );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_STRESS, mqueue )
{
    int i = 0;
    mqd_t xMqId = ( mqd_t ) -1;
    /* Handles of the threads spawned by this test. */
    pthread_t pxSenders[ posixtestMQUEUE_STRESS_NUMBER_OF_THREADS ] = { ( pthread_t ) NULL },
              pxReceivers[ posixtestMQUEUE_STRESS_NUMBER_OF_THREADS ] = { ( pthread_t ) NULL };
    /* Return values of the threads spawned by this test. */
    intptr_t pxSenderStatus[ posixtestMQUEUE_STRESS_NUMBER_OF_THREADS ] = { 0 },
             pxReceiverStatus[ posixtestMQUEUE_STRESS_NUMBER_OF_THREADS ] = { 0 };

    struct mq_attr xQueueAttributes =
    {
        .mq_flags   =                                   0,
        .mq_maxmsg  = posixconfigMQ_MAX_MESSAGES,
        .mq_msgsize = posixtestMQUEUE_STRESS_MESSAGE_SIZE,
        .mq_curmsgs = 0
    };

    /* Create a message queue. */
    xMqId = mq_open( "/myqueue",
                     O_CREAT | O_RDWR,
                     0600,
                     &xQueueAttributes );
    TEST_ASSERT_NOT_EQUAL( ( mqd_t ) -1, xMqId );

    /* Spawn the sender and receiver threads. */
    if( TEST_PROTECT() )
    {
        for( i = 0; i < posixtestMQUEUE_STRESS_NUMBER_OF_THREADS; i++ )
        {
            TEST_ASSERT_EQUAL_INT( 0,
                                   pthread_create( &pxSenders[ i ], NULL, prvQueueSenderThread, &xMqId ) );
            TEST_ASSERT_EQUAL_INT( 0,
                                   pthread_create( &pxReceivers[ i ], NULL, prvQueueReceiverThread, &xMqId ) );
        }
    }

    /* Join any spawned threads. */
    for( i = 0; i < posixtestMQUEUE_STRESS_NUMBER_OF_THREADS; i++ )
    {
        if( pxSenders[ i ] != ( pthread_t ) NULL )
        {
            ( void ) pthread_join( pxSenders[ i ], ( void ** ) &pxSenderStatus[ i ] );
        }

        if( pxReceivers[ i ] != ( pthread_t ) NULL )
        {
            ( void ) pthread_join( pxReceivers[ i ], ( void ** ) &pxReceiverStatus[ i ] );
        }
    }

    /* Close and unlink message queue. */
    if( xMqId != ( mqd_t ) -1 )
    {
        ( void ) mq_close( xMqId );
        ( void ) mq_unlink( "/myqueue" );
    }

    /* Check results. */
    if( TEST_PROTECT() )
    {
        for( i = 0; i < posixtestMQUEUE_STRESS_NUMBER_OF_THREADS; i++ )
        {
            TEST_ASSERT_EQUAL_INT( 1, pxSenderStatus[ i ] );
            TEST_ASSERT_EQUAL_INT( 1, pxReceiverStatus[ i ] );
        }
    }
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_STRESS, pthread_mutex )
{
    int i = 0, j = 0;
    volatile int iSharedVariable = 0;
    pthread_mutex_t xMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t xMutexTestThreads[ posixtestMUTEX_STRESS_NUMBER_OF_THREADS ] = { ( pthread_t ) NULL };
    intptr_t xMutexThreadStatus[ posixtestMUTEX_STRESS_NUMBER_OF_THREADS ] = { 0 };
    MutexTestThreadArgs_t xThreadArguments = { 0 };

    /* Set the arguments for the test threads. */
    xThreadArguments.piSharedVariable = &iSharedVariable;
    xThreadArguments.pxMutex = &xMutex;

    for( i = 0; i < posixtestMUTEX_STRESS_ITERATIONS; i++ )
    {
        /* Create the mutex test threads. */
        for( j = 0; j < posixtestMUTEX_STRESS_NUMBER_OF_THREADS; j++ )
        {
            ( void ) pthread_create( &xMutexTestThreads[ j ], NULL, prvMutexTestThread, &xThreadArguments );
        }

        /* Wait for all test threads to finish. */
        for( j = 0; j < posixtestMUTEX_STRESS_NUMBER_OF_THREADS; j++ )
        {
            if( xMutexTestThreads[ j ] != ( pthread_t ) NULL )
            {
                ( void ) pthread_join( xMutexTestThreads[ j ], ( void ** ) &xMutexThreadStatus[ j ] );
            }
        }

        /* Check results. */
        TEST_ASSERT_EQUAL_INT( posixtestMUTEX_STRESS_NUMBER_OF_THREADS, iSharedVariable );

        for( j = 0; j < posixtestMUTEX_STRESS_NUMBER_OF_THREADS; j++ )
        {
            TEST_ASSERT_EQUAL_INT( 1, xMutexThreadStatus[ i ] );
        }

        /* Clear the results for the next iteration. */
        ( void ) memset( xMutexTestThreads, 0x00, sizeof( pthread_t ) * posixtestMUTEX_STRESS_NUMBER_OF_THREADS );
        ( void ) memset( xMutexThreadStatus, 0x00, sizeof( intptr_t ) * posixtestMUTEX_STRESS_NUMBER_OF_THREADS );
        iSharedVariable = 0;
    }
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_STRESS, pthread_barrier_overflow )
{
    int iResult = 0, i = 0;
    volatile int iWaitingThreads = 0;
    pthread_t xBarrierTestThreads[ 2 * posixtestBARRIER_STRESS_COUNT - 1 ] = { ( pthread_t ) NULL };
    intptr_t xThreadReturnStatus[ 2 * posixtestBARRIER_STRESS_COUNT - 1 ] = { 0 };
    pthread_barrier_t xBarrier;
    BarrierTestThreadArgs_t xThreadArguments = { 0 };

    /* Set the thread arguments. */
    xThreadArguments.piWaitingThreads = &iWaitingThreads;
    xThreadArguments.pxBarrier = &xBarrier;

    /* Create a barrier. This test will ensure that the barrier does not overflow
     * posixtestBARRIER_STRESS_COUNT. */
    iResult = pthread_barrier_init( &xBarrier, NULL, posixtestBARRIER_STRESS_COUNT );
    TEST_ASSERT_EQUAL_INT( 0, iResult );

    if( TEST_PROTECT() )
    {
        /* Create fewer threads than the barrier count. All the created threads
         * will block on the barrier. */
        for( i = 0; i < posixtestBARRIER_STRESS_COUNT - 1; i++ )
        {
            ( void ) pthread_create( &xBarrierTestThreads[ i ],
                                     NULL,
                                     prvBarrierTestThread,
                                     &xThreadArguments );
        }

        /* Wait half a second to allow all threads time to enter
         * pthread_barrier_wait. */
        vTaskDelay( pdMS_TO_TICKS( 500 ) );

        /* Ensure that the correct number of threads is waiting on the barrier. */
        TEST_ASSERT_EQUAL_INT( posixtestBARRIER_STRESS_COUNT - 1, iWaitingThreads );

        /* Create another set of threads to wait on the barrier. Upon entering
         * pthread_barrier_wait, this set of threads should cause the barrier count
         * to "roll over" and not overflow. */
        for( i = 0; i < posixtestBARRIER_STRESS_COUNT; i++ )
        {
            ( void ) pthread_create( &xBarrierTestThreads[ i + posixtestBARRIER_STRESS_COUNT - 1 ],
                                     NULL,
                                     prvBarrierTestThread,
                                     &xThreadArguments );
        }

        /* Wait half a second to allow all threads time to enter or exit
         * pthread_barrier_wait. */
        vTaskDelay( pdMS_TO_TICKS( 500 ) );

        /* Ensure that the correct number of threads is waiting on the barrier. */
        TEST_ASSERT_EQUAL_INT( posixtestBARRIER_STRESS_COUNT - 1, iWaitingThreads );

        /* Enter the barrier to release all the threads currently waiting. */
        ( void ) pthread_barrier_wait( &xBarrier );

        /* Join all the threads created by this test. */
        for( i = 0; i < 2 * posixtestBARRIER_STRESS_COUNT - 1; i++ )
        {
            ( void ) pthread_join( xBarrierTestThreads[ i ], ( void ** ) &xThreadReturnStatus[ i ] );
        }

        /* Ensure that all waiting threads were freed. */
        TEST_ASSERT_EQUAL_INT( 0, iWaitingThreads );

        /* Check the return values of all the threads. */
        for( i = 0; i < 2 * posixtestBARRIER_STRESS_COUNT - 1; i++ )
        {
            TEST_ASSERT_EQUAL_INT( 1, xThreadReturnStatus[ i ] );
        }
    }

    ( void ) pthread_barrier_destroy( &xBarrier );
}

/*-----------------------------------------------------------*/
