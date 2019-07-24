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
 * @file aws_test_posix_clock.c
 * @brief Tests for POSIX clock functions.
 */

/* C standard library includes. */
#include <stddef.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/utils.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/**
 * @brief Size of the buffer in the strftime test.
 */
#define posixtestTIME_BUFFER_SIZE    ( 16 )

/*-----------------------------------------------------------*/

static void prvClockSleep( const struct timespec * const pxSleepTime,
                           struct timespec * const pxElapsedTime,
                           int iFlags )
{
    int iStatus = 0;
    struct timespec xStartTime = { 0 }, xEndTime = { 0 };

    /* Get start time of function. */
    iStatus = clock_gettime( CLOCK_MONOTONIC, &xStartTime );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Sleep for the given time. */
    iStatus = clock_nanosleep( CLOCK_MONOTONIC, iFlags, pxSleepTime, NULL );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Get time after sleeping. */
    iStatus = clock_gettime( CLOCK_MONOTONIC, &xEndTime );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Verify that clock_gettime returned valid timespecs. */
    TEST_ASSERT_TRUE( 0 <= xStartTime.tv_sec );
    TEST_ASSERT_EQUAL_INT( true, UTILS_ValidateTimespec( &xStartTime ) );
    TEST_ASSERT_TRUE( 0 <= xEndTime.tv_sec );
    TEST_ASSERT_EQUAL_INT( true, UTILS_ValidateTimespec( &xEndTime ) );

    /* Verify that xEndTime > xStartTime. Also calculates elapsed time. */
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecSubtract( &xEndTime, &xStartTime, pxElapsedTime ) );
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_POSIX_CLOCK );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_POSIX_CLOCK )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_POSIX_CLOCK )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_POSIX_CLOCK )
{
    RUN_TEST_CASE( Full_POSIX_CLOCK, clock_gettime_nanosleep );
    RUN_TEST_CASE( Full_POSIX_CLOCK, clock_getres );
    RUN_TEST_CASE( Full_POSIX_CLOCK, clock_nanosleep_absolute );
    RUN_TEST_CASE( Full_POSIX_CLOCK, clock_nanosleep_min_resolution );
    RUN_TEST_CASE( Full_POSIX_CLOCK, clock_nanosleep_invalid_rqtp );
    RUN_TEST_CASE( Full_POSIX_CLOCK, clock_nanosleep_absolute_in_past );
    RUN_TEST_CASE( Full_POSIX_CLOCK, nanosleep );
    RUN_TEST_CASE( Full_POSIX_CLOCK, nanosleep_invalid_params );
    RUN_TEST_CASE( Full_POSIX_CLOCK, localtime_r );
    RUN_TEST_CASE( Full_POSIX_CLOCK, strftime );
    RUN_TEST_CASE( Full_POSIX_CLOCK, time );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, clock_gettime_nanosleep )
{
    struct timespec xSleepTime = { .tv_sec = 0, .tv_nsec = 1000000 }, /* 1 millisecond. */
                    xElapsedTime = { 0 };

    /* Sleep for 1 millisecond. */
    prvClockSleep( &xSleepTime, &xElapsedTime, 0 );

    /* Verify that at least 1 millisecond elapsed. */
    TEST_ASSERT_TRUE( 0 <= xElapsedTime.tv_sec );
    TEST_ASSERT_TRUE( 1 > xElapsedTime.tv_sec );
    TEST_ASSERT_TRUE( 1000000 <= xElapsedTime.tv_nsec );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, clock_getres )
{
    int iStatus = 0;
    struct timespec xResolution = { 0 };

    /* Get the resolution of the clock. */
    iStatus = clock_getres( CLOCK_REALTIME, &xResolution );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Test that clock_getres returned a valid timespec. */
    TEST_ASSERT_EQUAL_INT( true, UTILS_ValidateTimespec( &xResolution ) );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, clock_nanosleep_absolute )
{
    int iStatus = 0;
    struct timespec xWakeTime = { 0 }, xElapsedTime = { 0 };

    /* Get the current time. */
    iStatus = clock_gettime( CLOCK_REALTIME, &xWakeTime );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Wake up 5 ms from now. */
    iStatus = UTILS_TimespecAddNanoseconds( &xWakeTime, 5000000, &xWakeTime );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    prvClockSleep( &xWakeTime, &xElapsedTime, TIMER_ABSTIME );

    /* Verify that at least 5 milliseconds elapsed. */
    TEST_ASSERT_TRUE( 0 <= xElapsedTime.tv_sec );
    TEST_ASSERT_TRUE( 1 > xElapsedTime.tv_sec );
    TEST_ASSERT_TRUE( 5000000 <= xElapsedTime.tv_nsec );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, clock_nanosleep_min_resolution )
{
    int iStatus = 0;
    struct timespec xResolution = { 0 }, xSleepTime = { 0 }, xElapsedTime = { 0 };

    /* Get the clock resolution. */
    iStatus = clock_getres( CLOCK_REALTIME, &xResolution );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Specify a sleep time less than the clock resolution. */
    xSleepTime.tv_sec = xResolution.tv_sec / 2;
    xSleepTime.tv_nsec = xResolution.tv_nsec / 2;

    prvClockSleep( &xSleepTime, &xElapsedTime, 0 );

    /* Verify that at least the minimum resolution elapsed, i.e. a requested
     * sleep for smaller than the minimum resolution slept for the minimum
     * resolution. */
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecSubtract( &xElapsedTime,
                                                      &xResolution,
                                                      &xElapsedTime ) );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, clock_nanosleep_invalid_rqtp )
{
    int iStatus = 0;
    struct timespec xSleepTime;

    /* Invalid sleep time with tv_nec < 0. */
    xSleepTime.tv_sec = 0;
    xSleepTime.tv_nsec = -3;

    iStatus = clock_nanosleep( CLOCK_REALTIME, 0, &xSleepTime, NULL );
    TEST_ASSERT_EQUAL_INT( EINVAL, iStatus );

    /* Invalid sleep time with tv_nec = 1000000000. */
    xSleepTime.tv_sec = 0;
    xSleepTime.tv_nsec = NANOSECONDS_PER_SECOND;

    iStatus = clock_nanosleep( CLOCK_REALTIME, 0, &xSleepTime, NULL );
    TEST_ASSERT_EQUAL_INT( EINVAL, iStatus );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, clock_nanosleep_absolute_in_past )
{
    int iStatus = 0;
    struct timespec xWakeTime = { 0 }, xElapsedTime = { 0 };

    /* Get the current time. */
    iStatus = clock_gettime( CLOCK_REALTIME, &xWakeTime );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Set a timeout 5 ms in the past. */
    iStatus = UTILS_TimespecAddNanoseconds( &xWakeTime, -5000000, &xWakeTime );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    prvClockSleep( &xWakeTime, &xElapsedTime, TIMER_ABSTIME );

    /* Verify that the clock_nanosleep returned quickly. */
    TEST_ASSERT_TRUE( 0 == xElapsedTime.tv_sec );
    TEST_ASSERT_TRUE( 2000000 > xElapsedTime.tv_nsec );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, nanosleep )
{
    int iStatus = 0;
    struct timespec xSleepTime = { .tv_sec = 0, .tv_nsec = 1000000 }, /* 1 millisecond. */
                    xStartTime = { 0 }, xEndTime = { 0 }, xElapsedTime = { 0 };

    /* Get the time when the test started. */
    iStatus = clock_gettime( CLOCK_REALTIME, &xStartTime );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Sleep for 1 ms. */
    iStatus = nanosleep( &xSleepTime, NULL );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Get the time after sleeping. */
    clock_gettime( CLOCK_REALTIME, &xEndTime );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    /* Verify that xEndTime > xStartTime. Also calculates elapsed time. */
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecSubtract( &xEndTime, &xStartTime, &xElapsedTime ) );

    /* Verify that at least 1 ms elapsed. */
    TEST_ASSERT_TRUE( 0 <= xElapsedTime.tv_sec );
    TEST_ASSERT_TRUE( 1 > xElapsedTime.tv_sec );
    TEST_ASSERT_TRUE( 1000000 <= xElapsedTime.tv_nsec );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, nanosleep_invalid_params )
{
    /* An invalid timespec where tv_nsec = 1000000000. */
    struct timespec xSleepTime = { .tv_sec = 0, .tv_nsec = NANOSECONDS_PER_SECOND };

    /* Check for error with NULL parameter. */
    TEST_ASSERT_EQUAL_INT( -1, nanosleep( NULL, NULL ) );
    TEST_ASSERT_EQUAL_INT( EINVAL, errno );

    /* Check for error with invalid rqtp. */
    TEST_ASSERT_EQUAL_INT( -1, nanosleep( &xSleepTime, NULL ) );
    TEST_ASSERT_EQUAL_INT( EINVAL, errno );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, localtime_r )
{
    time_t xTime = time( NULL );
    struct tm xLocalTime = { 0 };

    /* Check the return value of localtime. */
    TEST_ASSERT_EQUAL_PTR( &xLocalTime,
                           localtime_r( &xTime, &xLocalTime ) );

    /* Only check the stored tick value only if using "custom" FreeRTOS+POSIX
     * tm struct. */
    #if !defined( posixconfigENABLE_TM ) || ( posixconfigENABLE_TM == 1 )
        TEST_ASSERT_TRUE( xLocalTime.tm_tick > 0 );
    #endif
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, strftime )
{
    time_t xCurrentTime = time( NULL );
    struct tm xLocalTime = { 0 };
    char pcTimeBuffer[ posixtestTIME_BUFFER_SIZE ] = { 0 };

    /* Convert current time to struct tm. */
    TEST_ASSERT_EQUAL_PTR( &xLocalTime,
                           localtime_r( &xCurrentTime, &xLocalTime ) );

    /* Print the time. Check that the return value is positive. */
    TEST_ASSERT_GREATER_THAN( 0,
                              strftime( pcTimeBuffer,
                                        posixtestTIME_BUFFER_SIZE,
                                        "%y",
                                        &xLocalTime ) );

    /* Check that at least one character was added to the buffer. */
    TEST_ASSERT_NOT_EQUAL( '\0', pcTimeBuffer[ 0 ] );

    /* Check that buffer size is respected. */
    TEST_ASSERT_EQUAL( 0,
                       strftime( pcTimeBuffer,
                                 1,
                                 "%y",
                                 &xLocalTime ) );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_CLOCK, time )
{
    time_t xStartTime = 0, xEndTime = 0;

    /* Get time using return value. Check that it's nonzero. */
    xStartTime = time( NULL );
    TEST_ASSERT_TRUE( xStartTime >= 0 );

    /* Get time using output parameter. Check that time did not decrease. */
    ( void ) time( &xEndTime );
    TEST_ASSERT_TRUE( xEndTime >= xStartTime );
}

/*-----------------------------------------------------------*/
