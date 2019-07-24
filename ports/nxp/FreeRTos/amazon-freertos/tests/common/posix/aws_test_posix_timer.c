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
 * @file aws_test_posix_timer.c
 * @brief Tests for POSIX timer functions.
 */

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/semaphore.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/utils.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/**
 * @brief A short delay used in these tests.
 */
#define posixtestSHORT_TIMER_DELAY_NANOSECONDS    ( 50000000L ) /* 50 ms. */

/**
 * @brief Callback thread that notifies of timer expiration.
 *
 * @param[out] pvArg A pointer to a TimerCallbackThreadArgs_t struct.
 *
 * @return NULL.
 */
static void * prvTimerCallbackThread( void * pvArg );

/**
 * @brief Callback function that notifies of timer expiration. (no thread)
 *
 * @param[out] pvArg A pointer to a TimerCallbackThreadArgs_t struct.
 *
 * @return NULL.
 */
static void * prvTimerCallback( void * pvArg );

/*-----------------------------------------------------------*/

/**
 * @brief The argument for prvTimerCallbackThread, used to return values from
 * the thread.
 */
typedef struct TimerCallbackThreadReturn
{
    int iStatus;                     /**< The status reported by the callback thread. */
    pthread_t xTimerCallbackThread;  /**< The identifier of the timer callback. */
    struct timespec xExpirationTime; /* Records the time the callback thread runs. */
} TimerCallbackThreadReturn_t;

/*-----------------------------------------------------------*/

/**
 * @brief Used to synchronize the timer callback thread and test runner thread.
 */
static sem_t xSemaphore = { 0 };

/**
 * @brief The default event notification structure.
 */
static struct sigevent xDefaultSigevent =
{
    .sigev_notify            = SIGEV_THREAD,
    .sigev_notify_function   = ( void ( * )( union sigval ) )prvTimerCallback,
    .sigev_signo             = 0,
    .sigev_notify_attributes = NULL,
    .sigev_value.sival_ptr   = NULL
};

/*-----------------------------------------------------------*/

static void * prvTimerCallbackThread( void * pvArg )
{
    TimerCallbackThreadReturn_t * pxReturn = ( TimerCallbackThreadReturn_t * ) pvArg;

    if( pxReturn != NULL )
    {
        /* Get the time that this function was called. */
        pxReturn->iStatus = clock_gettime( CLOCK_REALTIME, &pxReturn->xExpirationTime );

        /* Record this thread's ID so that it can be joined by the main test thread. */
        pxReturn->xTimerCallbackThread = pthread_self();
    }

    /* Post to the semaphore and unblock the main test thread. */
    ( void ) sem_post( &xSemaphore );

    return NULL;
}

/*-----------------------------------------------------------*/

static void * prvTimerCallback( void * pvArg )
{
    TimerCallbackThreadReturn_t * pxReturn = ( TimerCallbackThreadReturn_t * ) pvArg;

    if( pxReturn != NULL )
    {
        /* Get the time that this function was called. */
        pxReturn->iStatus = clock_gettime( CLOCK_REALTIME, &pxReturn->xExpirationTime );
    }

    /* Post to the semaphore and unblock the main test thread. */
    ( void ) sem_post( &xSemaphore );

    return NULL;
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_POSIX_TIMER );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_POSIX_TIMER )
{
    /* Create the locked synchronization semaphore in test setup. */
    TEST_ASSERT_EQUAL_INT( 0, sem_init( &xSemaphore, 0, 0 ) );
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_POSIX_TIMER )
{
    /* Clean up the synchronization semaphore in test teardown. */
    ( void ) sem_destroy( &xSemaphore );
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_POSIX_TIMER )
{
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_create );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_delete_active_timer );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_settime_invalid_params );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_settime_min_resolution );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_settime_disarm );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_settime_abstime );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_settime_abstime_with_pthread_attr );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_settime_abstime_in_past );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_settime_ovalue );
    RUN_TEST_CASE( Full_POSIX_TIMER, timer_periodic );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_create )
{
    int iStatus = 0;
    timer_t xTimer = NULL;
    struct sigevent xNotificationEvent = xDefaultSigevent;

    /* Creation of a timer with NULL sigevent, which implies SIGEV_SIGNAL,
     * should fail. */
    iStatus = timer_create( CLOCK_REALTIME, NULL, &xTimer );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( ENOTSUP, errno );

    /* Creation of a timer with SIGEV_SIGNAL notification should fail. */
    xNotificationEvent.sigev_notify = SIGEV_SIGNAL;
    iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( ENOTSUP, errno );

    /* Create a timer with valid parameters. */
    iStatus = timer_create( CLOCK_REALTIME, &xDefaultSigevent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Delete timer. */
    iStatus = timer_delete( xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_delete_active_timer )
{
    int iStatus = 0;
    volatile BaseType_t xTimerCreated = pdFALSE;
    timer_t xTimer = NULL;
    struct itimerspec xInterval =
    {
        .it_value.tv_sec  = 0,
        .it_value.tv_nsec = posixtestSHORT_TIMER_DELAY_NANOSECONDS,
        .it_interval      = { 0 }
    };

    /* Create a timer. */
    iStatus = timer_create( CLOCK_REALTIME, &xDefaultSigevent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );
    xTimerCreated = pdTRUE;

    if( TEST_PROTECT() )
    {
        /* Set the timeout. */
        iStatus = timer_settime( xTimer, 0, &xInterval, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Immediately delete the timer before it expires. */
        iStatus = timer_delete( xTimer );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        xTimerCreated = pdFALSE;

        /* Wait 3 times the timer period to ensure the timer never fires. */
        xInterval.it_interval.tv_nsec *= 3;
        ( void ) clock_nanosleep( CLOCK_REALTIME, 0, &xInterval.it_value, NULL );

        /* The semaphore should still be locked, since the timer never fired. */
        iStatus = sem_trywait( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EAGAIN, errno );
    }

    if( xTimerCreated == pdTRUE )
    {
        ( void ) timer_delete( xTimer );
    }
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_settime_min_resolution )
{
    int iStatus = 0;
    timer_t xTimer = NULL;
    struct itimerspec xInterval = { { 0 }, { 0 } };
    struct sigevent xNotificationEvent = xDefaultSigevent;
    TimerCallbackThreadReturn_t xThreadReturnValues = { 0 };
    struct timespec xStartTime = { 0 }, xElapsedTime = { 0 };

    /* Set pointer for thread return values. */
    xNotificationEvent.sigev_value.sival_ptr = &xThreadReturnValues;

    /* Create a timer. */
    iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    if( TEST_PROTECT() )
    {
        /* Set an interval of the minimum clock resolution. */
        iStatus = clock_getres( CLOCK_REALTIME, &xInterval.it_value );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Record the time before the timer is armed. */
        iStatus = clock_gettime( CLOCK_REALTIME, &xStartTime );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Start the timer */
        iStatus = timer_settime( xTimer, 0, &xInterval, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Wait for the timer callback to release the semaphore. */
        ( void ) sem_wait( &xSemaphore );

        /* Check the timer callback thread's return value. */
        TEST_ASSERT_EQUAL_INT( 0, xThreadReturnValues.iStatus );

        /* Calculate the elapsed time since the timer was started. Ensure that
         * it's positive. */
        iStatus = UTILS_TimespecSubtract( &xThreadReturnValues.xExpirationTime, &xStartTime, &xElapsedTime );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_TRUE( ( xElapsedTime.tv_sec > 0 ) || ( xElapsedTime.tv_nsec > 0 ) );

        /* Ensure the timer did not fire before the minimum sleep resolution. */
        iStatus = UTILS_TimespecSubtract( &xElapsedTime, &xInterval.it_value, &xElapsedTime );
        TEST_ASSERT_TRUE( iStatus == 0 );
    }

    /* Delete timer. */
    ( void ) timer_delete( xTimer );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_settime_invalid_params )
{
    int iStatus = 0;
    timer_t xTimer = NULL;
    struct itimerspec xInterval = { { 0 }, { 0 } };
    struct sigevent xNotificationEvent = xDefaultSigevent;

    /* Create a timer. */
    iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    if( TEST_PROTECT() )
    {
        /* Set it_interval to an invalid value. */
        xInterval.it_interval.tv_nsec = NANOSECONDS_PER_SECOND;

        /* It's ok to pass an invalid it_interval as long as it_value is 0, i.e.
         * timer is not armed. */
        iStatus = timer_settime( xTimer, 0, &xInterval, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Change it_value to nonzero. */
        xInterval.it_value.tv_nsec = posixtestSHORT_TIMER_DELAY_NANOSECONDS;

        /* Passing an invalid it_interval when it_value is not 0 should fail. */
        iStatus = timer_settime( xTimer, 0, &xInterval, NULL );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EINVAL, errno );

        /* Wait 3 times the timer period to ensure the timer never fires. */
        xInterval.it_interval.tv_nsec *= 3;
        ( void ) clock_nanosleep( CLOCK_REALTIME, 0, &xInterval.it_value, NULL );

        /* The semaphore should still be locked, since the timer never fired. */
        iStatus = sem_trywait( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EAGAIN, errno );
    }

    /* Delete timer. */
    ( void ) timer_delete( xTimer );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_settime_disarm )
{
    int iStatus = 0;
    timer_t xTimer = NULL;
    struct itimerspec xInterval = { { 0 }, { 0 } };
    struct sigevent xNotificationEvent = xDefaultSigevent;

    /* Create a timer. */
    iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Set a short relative timer expiration. */
    xInterval.it_value.tv_nsec = posixtestSHORT_TIMER_DELAY_NANOSECONDS;

    if( TEST_PROTECT() )
    {
        /* Arm the timer. */
        iStatus = timer_settime( xTimer, 0, &xInterval, NULL );
        TEST_ASSERT_EQUAL( 0, iStatus );

        /* Change the timer it_value to 0, then disarm the timer. */
        xInterval.it_value.tv_nsec = 0L;
        iStatus = timer_settime( xTimer, 0, &xInterval, NULL );

        /* Wait 3 times the timer period to ensure the timer never fires. */
        xInterval.it_interval.tv_nsec = posixtestSHORT_TIMER_DELAY_NANOSECONDS * 3;
        ( void ) clock_nanosleep( CLOCK_REALTIME, 0, &xInterval.it_value, NULL );

        /* The semaphore should still be locked, since the timer never fired. */
        iStatus = sem_trywait( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EAGAIN, errno );
    }

    /* Delete timer. */
    ( void ) timer_delete( xTimer );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_settime_abstime_with_pthread_attr )
{
    int iStatus = 0;
    timer_t xTimer = NULL;
    struct itimerspec xAbsoluteTimeout = { { 0 }, { 0 } };
    struct sigevent xNotificationEvent = xDefaultSigevent;
    TimerCallbackThreadReturn_t xThreadReturnValues = { 0 };
    struct timespec xTimeDifference = { 0 };
    pthread_attr_t xTimerCallbackAttributes;
    volatile BaseType_t xTimerCreated = pdFALSE, xThreadAttributesCreated = pdFALSE;

    /* Set pointer for thread attributes, which should create callback thread  */
    xNotificationEvent.sigev_notify_attributes = &xTimerCallbackAttributes;

    /* Set pointer for thread return values. */
    xNotificationEvent.sigev_value.sival_ptr = &xThreadReturnValues;

    /* Update notification event callback function */
    xNotificationEvent.sigev_notify_function = ( void ( * )( union sigval ) )prvTimerCallbackThread;

    /* Initialize pthread attributes */
    iStatus = pthread_attr_init( &xTimerCallbackAttributes );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );
    xThreadAttributesCreated = pdTRUE;

    /* Create a timer. */
    iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );
    xTimerCreated = pdTRUE;

    if( TEST_PROTECT() )
    {
        /* Set an absolute timeout. */
        iStatus = clock_gettime( CLOCK_REALTIME, &xAbsoluteTimeout.it_value );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = UTILS_TimespecAddNanoseconds( &xAbsoluteTimeout.it_value,
                                                posixtestSHORT_TIMER_DELAY_NANOSECONDS,
                                                &xAbsoluteTimeout.it_value );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = timer_settime( xTimer, TIMER_ABSTIME, &xAbsoluteTimeout, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Wait for the timer callback to release the semaphore. */
        ( void ) sem_wait( &xSemaphore );

        /* Join the timer callback thread. */
        ( void ) pthread_join( xThreadReturnValues.xTimerCallbackThread, NULL );

        /* Check the timer callback thread's return value. */
        TEST_ASSERT_EQUAL_INT( 0, xThreadReturnValues.iStatus );

        /* Calculate the difference between the timeout expiration and the absolute
         * timeout. Ensure that it's small and positive. */
        iStatus = UTILS_TimespecSubtract( &xThreadReturnValues.xExpirationTime, &xAbsoluteTimeout.it_value, &xTimeDifference );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_TRUE( ( xTimeDifference.tv_sec == 0 ) && ( xTimeDifference.tv_nsec >= 0 ) );
    }

    /* Clean up resources. */
    if( xThreadAttributesCreated == pdTRUE )
    {
        ( void ) pthread_attr_destroy( &xTimerCallbackAttributes );
    }

    if( xTimerCreated == pdTRUE )
    {
        ( void ) timer_delete( xTimer );
    }
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_settime_abstime )
{
    int iStatus = 0;
    timer_t xTimer = NULL;
    struct itimerspec xAbsoluteTimeout = { { 0 }, { 0 } };
    struct sigevent xNotificationEvent = xDefaultSigevent;
    TimerCallbackThreadReturn_t xThreadReturnValues = { 0 };
    struct timespec xTimeDifference = { 0 };

    /* Set pointer for thread return values. */
    xNotificationEvent.sigev_value.sival_ptr = &xThreadReturnValues;

    /* Create a timer. */
    iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    if( TEST_PROTECT() )
    {
        /* Set an absolute timeout. */
        iStatus = clock_gettime( CLOCK_REALTIME, &xAbsoluteTimeout.it_value );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = UTILS_TimespecAddNanoseconds( &xAbsoluteTimeout.it_value,
                                                posixtestSHORT_TIMER_DELAY_NANOSECONDS,
                                                &xAbsoluteTimeout.it_value );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = timer_settime( xTimer, TIMER_ABSTIME, &xAbsoluteTimeout, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Wait for the timer callback to release the semaphore. */
        ( void ) sem_wait( &xSemaphore );

        /* Check the timer callback thread's return value. */
        TEST_ASSERT_EQUAL_INT( 0, xThreadReturnValues.iStatus );

        /* Calculate the difference between the timeout expiration and the absolute
         * timeout. Ensure that it's small and positive. */
        iStatus = UTILS_TimespecSubtract( &xThreadReturnValues.xExpirationTime, &xAbsoluteTimeout.it_value, &xTimeDifference );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_TRUE( ( xTimeDifference.tv_sec == 0 ) && ( xTimeDifference.tv_nsec >= 0 ) );
    }

    /* Delete timer. */
    ( void ) timer_delete( xTimer );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_settime_abstime_in_past )
{
    int iStatus = 0;
    timer_t xTimer = NULL;
    struct itimerspec xAbsoluteTimeout = { { 0 }, { 0 } };
    struct sigevent xNotificationEvent = xDefaultSigevent;
    TimerCallbackThreadReturn_t xThreadReturnValues = { 0 };
    struct timespec xTimeDifference = { 0 }, xStartTime = { 0 };

    /* Set pointer for thread return values. */
    xNotificationEvent.sigev_value.sival_ptr = &xThreadReturnValues;

    /* Create a timer. */
    iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Set a nonzero absolute timeout in the past, e.g. 1 Epoch time. */
    xAbsoluteTimeout.it_value.tv_sec = 1;

    if( TEST_PROTECT() )
    {
        /* Record the time before the timer is armed. */
        iStatus = clock_gettime( CLOCK_REALTIME, &xStartTime );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Start the timer with an absolute timeout in the past. */
        iStatus = timer_settime( xTimer, TIMER_ABSTIME, &xAbsoluteTimeout, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Wait for the timer callback to release the semaphore. */
        ( void ) sem_wait( &xSemaphore );

        /* Check the timer callback thread's return value. */
        TEST_ASSERT_EQUAL_INT( 0, xThreadReturnValues.iStatus );

        /* Calculate the difference between the timeout expiration and the start time.
         * Ensure that it's small and positive. */
        iStatus = UTILS_TimespecSubtract( &xThreadReturnValues.xExpirationTime, &xStartTime, &xTimeDifference );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_TRUE( ( xTimeDifference.tv_sec == 0 ) && ( xTimeDifference.tv_nsec >= 0 ) );
    }

    /* Delete timer. */
    ( void ) timer_delete( xTimer );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_settime_ovalue )
{
    int iStatus = 0;
    timer_t xTimer = NULL;
    struct itimerspec xInterval = { { 0 }, { 0 } }, xOldValue = { { 0 }, { 0 } };
    struct sigevent xNotificationEvent = xDefaultSigevent;

    /* Don't notify of timer expiration for this test. */
    xNotificationEvent.sigev_notify = SIGEV_NONE;

    /* Create a timer. */
    iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Set a periodic timer. */
    xInterval.it_value.tv_nsec = posixtestSHORT_TIMER_DELAY_NANOSECONDS;
    xInterval.it_interval.tv_nsec = posixtestSHORT_TIMER_DELAY_NANOSECONDS;

    if( TEST_PROTECT() )
    {
        /* Arm the timer with a relative timeout. */
        iStatus = timer_settime( xTimer, 0, &xInterval, &xOldValue );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Ensure that the correct old value was retrieved. */
        TEST_ASSERT_EQUAL_INT( 0, xOldValue.it_value.tv_sec );
        TEST_ASSERT_EQUAL_INT( 0, xOldValue.it_value.tv_nsec );
        TEST_ASSERT_EQUAL_INT( 0, xOldValue.it_interval.tv_sec );
        TEST_ASSERT_EQUAL_INT( 0, xOldValue.it_interval.tv_nsec );

        /* Change the relative timeout and re-arm the timer. */
        xInterval.it_value.tv_nsec = 2 * posixtestSHORT_TIMER_DELAY_NANOSECONDS;
        iStatus = timer_settime( xTimer, 0, &xInterval, &xOldValue );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Ensure that the correct old value was retrieved. */
        TEST_ASSERT_EQUAL_INT( 0, xOldValue.it_value.tv_sec );
        TEST_ASSERT_TRUE( xOldValue.it_value.tv_nsec <= posixtestSHORT_TIMER_DELAY_NANOSECONDS );
        TEST_ASSERT_EQUAL_INT( 0, xOldValue.it_interval.tv_sec );
        TEST_ASSERT_EQUAL_INT( posixtestSHORT_TIMER_DELAY_NANOSECONDS, xOldValue.it_interval.tv_nsec );
    }

    /* Delete timer. */
    ( void ) timer_delete( xTimer );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_TIMER, timer_periodic )
{
    int iStatus = 0, iExpectedTimerExpirations = 0, iNumberOfTimerExpirations = 0;
    volatile BaseType_t xTimerCreated = pdFALSE, xThreadAttributesCreated = pdFALSE;
    timer_t xTimer = ( timer_t ) NULL;
    struct itimerspec xInterval = { { 0 }, { 0 } };
    struct sigevent xNotificationEvent = xDefaultSigevent;
    struct timespec xSleepTime = { .tv_sec = 1, .tv_nsec = 0 };
    pthread_attr_t xTimerCallbackAttributes;

    /* Set pointer for thread attributes. */
    xNotificationEvent.sigev_notify_attributes = &xTimerCallbackAttributes;

    /* Update notification event callback function */
    xNotificationEvent.sigev_notify_function = ( void ( * )( union sigval ) )prvTimerCallbackThread;

    /* This test will check how many timer expirations occurred in 1 second. */
    iExpectedTimerExpirations = NANOSECONDS_PER_SECOND / posixtestSHORT_TIMER_DELAY_NANOSECONDS;

    /* Periodic timer interval. */
    xInterval.it_value.tv_nsec = posixtestSHORT_TIMER_DELAY_NANOSECONDS;
    xInterval.it_interval.tv_nsec = posixtestSHORT_TIMER_DELAY_NANOSECONDS;

    /* This test will use detached threads to eliminate calls to pthread_join. */
    iStatus = pthread_attr_init( &xTimerCallbackAttributes );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );
    xThreadAttributesCreated = pdTRUE;

    if( TEST_PROTECT() )
    {
        /* Set detached thread attribute. */
        iStatus = pthread_attr_setdetachstate( &xTimerCallbackAttributes, PTHREAD_CREATE_DETACHED );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Create a timer. */
        iStatus = timer_create( CLOCK_REALTIME, &xNotificationEvent, &xTimer );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        xTimerCreated = pdTRUE;

        /* Arm the periodic timer. */
        iStatus = timer_settime( xTimer, 0, &xInterval, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Sleep for 1 second. */
        iStatus = clock_nanosleep( CLOCK_REALTIME, 0, &xSleepTime, NULL );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Retrieve how many times the timer callback semaphore was incremented,
         * i.e. how many times the timer expiration callback ran. */
        iStatus = sem_getvalue( &xSemaphore, &iNumberOfTimerExpirations );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Because of unpredictable context switches, the number of timer expirations
         * may not be exactly as expected. */
        if( iExpectedTimerExpirations != iNumberOfTimerExpirations )
        {
            /* The number of timer expirations should not be greater than expected,
             * i.e. the timer should not have fired more frequently. */
            TEST_ASSERT_TRUE( iNumberOfTimerExpirations < iExpectedTimerExpirations );

            /* Ensure that at least 90% of expected timer expirations happened. */
            TEST_ASSERT_TRUE( iNumberOfTimerExpirations >= 9 * iExpectedTimerExpirations / 10 );
        }
    }

    /* Clean up resources. */
    if( xThreadAttributesCreated == pdTRUE )
    {
        ( void ) pthread_attr_destroy( &xTimerCallbackAttributes );
    }

    if( xTimerCreated == pdTRUE )
    {
        ( void ) timer_delete( xTimer );
    }
}
