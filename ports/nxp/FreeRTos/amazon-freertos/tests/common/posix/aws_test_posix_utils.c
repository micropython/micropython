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
 * @file aws_test_posix_utils.c
 * @brief Tests for POSIX utility functions.
 *
 * Some utility functions are considered adequately tested by other functions
 * and therefore aren't tested here.
 */

/* C standard library includes. */
#include <string.h>
#include <stddef.h>

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/utils.h"
#include "FreeRTOS_POSIX/errno.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

TEST_GROUP( Full_POSIX_UTILS );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_POSIX_UTILS )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_POSIX_UTILS )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_POSIX_UTILS )
{
    RUN_TEST_CASE( Full_POSIX_UTILS, UTILS_TimespecAdd );
    RUN_TEST_CASE( Full_POSIX_UTILS, UTILS_TimespecSubtract );
    RUN_TEST_CASE( Full_POSIX_UTILS, UTILS_TimespecAddNanoseconds );
    RUN_TEST_CASE( Full_POSIX_UTILS, UTILS_strnlen );
    RUN_TEST_CASE( Full_POSIX_UTILS, UTILS_TimespecCompare );
    RUN_TEST_CASE( Full_POSIX_UTILS, UTILS_TimespecToTicks );
    RUN_TEST_CASE( Full_POSIX_UTILS, UTILS_AbsoluteTimespecToDeltaTicks )
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_UTILS, UTILS_TimespecAdd )
{
    struct timespec xResult = { .tv_sec = 0, .tv_nsec = 0 };
    struct timespec x = { .tv_sec = 0, .tv_nsec = 0 };
    struct timespec y = { .tv_sec = 0, .tv_nsec = 0 };

    /* Check return value with NULL parameters. */
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecAdd( &x, &y, NULL ) );
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecAdd( NULL, &y, &xResult ) );
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecAdd( &x, NULL, &xResult ) );

    /* Simple timespec add. */
    x.tv_sec = 1;
    y.tv_sec = 2;
    x.tv_nsec = 3;
    y.tv_nsec = 4;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecAdd( &x, &y, &xResult ) );
    TEST_ASSERT_EQUAL_INT( 3, xResult.tv_sec );
    TEST_ASSERT_EQUAL_INT( 7, xResult.tv_nsec );

    /* Add timespec with carry. */
    x.tv_sec = y.tv_sec = 0;
    x.tv_nsec = y.tv_nsec = 600000000;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecAdd( &x, &y, &xResult ) );
    TEST_ASSERT_EQUAL_INT( 1, xResult.tv_sec );
    TEST_ASSERT_EQUAL_INT( 200000000, xResult.tv_nsec );

    /* Add timespec with negative y. */
    x.tv_sec = 5;
    x.tv_nsec = 0;
    y.tv_sec = -6;
    y.tv_nsec = 100000000;
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecAdd( &x, &y, &xResult ) );
    TEST_ASSERT_EQUAL_INT( -1, xResult.tv_sec );
    TEST_ASSERT_EQUAL_INT( 100000000, xResult.tv_nsec );

    /* Add non-compliant timespec where y.tv_nsec < 0. */
    x.tv_sec = y.tv_sec = 1;
    x.tv_nsec = 100000000;
    y.tv_nsec = -2100000000;
    /* should return 1 for negative value */
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecAdd( &x, &y,  &xResult ) );

    /* Add non-compliant timespec where y.tv_nsec > 1 billion. */
    x.tv_sec = y.tv_sec = 0;
    x.tv_nsec = y.tv_nsec = 2100000000;
    /* Overflow should be indicated by returning 1 */
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecAdd( &x, &y, &xResult ) );

    /* Test for correct xResult even with pointer aliasing. */
    x.tv_sec = x.tv_nsec = 5;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecAdd( &x, &x, &x ) );
    TEST_ASSERT_EQUAL_INT( 10, x.tv_sec );
    TEST_ASSERT_EQUAL_INT( 10, x.tv_nsec );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_UTILS, UTILS_TimespecSubtract )
{
    struct timespec xResult = { .tv_sec = 0, .tv_nsec = 0 };
    struct timespec x = { .tv_sec = 0, .tv_nsec = 0 };
    struct timespec y = { .tv_sec = 0, .tv_nsec = 0 };

    /* Check return value with NULL parameters. */
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecSubtract( &x, &y, NULL ) );
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecSubtract( NULL, &y, &xResult ) );
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecSubtract( &x, NULL, &xResult ) );

    /* Subtract timespec.tv_sec where x > y. */
    x.tv_sec = 5;
    y.tv_sec = 3;
    x.tv_nsec = y.tv_nsec = 0;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecSubtract( &x, &y, &xResult ) );
    TEST_ASSERT_EQUAL_INT( 2, xResult.tv_sec );
    TEST_ASSERT_EQUAL_INT( 0, xResult.tv_nsec );

    /* Subtract timespec.tv_sec where x < y. */
    x.tv_sec = 1;
    y.tv_sec = 3;
    x.tv_nsec = y.tv_nsec = 0;
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecSubtract( &x, &y, &xResult ) );

    /* Subtract timespec.tv_nsec where x > y. */
    x.tv_sec = y.tv_sec = 0;
    x.tv_nsec = 5;
    y.tv_nsec = 3;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecSubtract( &x, &y,  &xResult ) );
    TEST_ASSERT_EQUAL_INT( 0, xResult.tv_sec );
    TEST_ASSERT_EQUAL_INT( 2, xResult.tv_nsec );

    /* Subtract timespec.tv_nsec where x < y. */
    x.tv_sec = y.tv_sec = 0;
    x.tv_nsec = 0;
    y.tv_nsec = 5;
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecSubtract( &x, &y, &xResult ) );

    /* Subtract non-compliant timespec where x.tv_nsec < 0. */
    x.tv_sec = y.tv_sec = 0;
    x.tv_nsec = -5; /* Equivalent to tv_sec = -1, tv_nsec = 999999995. */
    y.tv_nsec = 3;
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecSubtract( &x, &y, &xResult ) );

    /* Subtract non-compliant timespec where y.tv_nsec > 1 billion. */
    x.tv_sec = 1;
    y.tv_sec = 0;
    x.tv_nsec = 0;
    y.tv_nsec = 2100000000; /* Equivalent to tv_sec = 2, tv_nsec = 100000000. */
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecSubtract( &x, &y, &xResult ) );

    /* Test for correct xResult even with pointer aliasing. */
    x.tv_sec = x.tv_nsec = 5;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecSubtract( &x, &x, &x ) );
    TEST_ASSERT_EQUAL_INT( 0, x.tv_sec );
    TEST_ASSERT_EQUAL_INT( 0, x.tv_nsec );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_UTILS, UTILS_TimespecAddNanoseconds )
{
    struct timespec xResult = { .tv_sec = 0, .tv_nsec = 0 };
    struct timespec x = { .tv_sec = 0, .tv_nsec = 0 };
    int64_t nanosec = 0;

    /* Check return value with NULL parameters. */
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecAddNanoseconds( &x, nanosec, NULL ) );
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecAddNanoseconds( NULL, nanosec, &xResult ) );

    /* Test with nanosec positive. */
    x.tv_sec = 5;
    x.tv_nsec = 500000000;
    nanosec = 4500000000LL;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecAddNanoseconds( &x, nanosec, &xResult ) );
    TEST_ASSERT_EQUAL_INT( 10, xResult.tv_sec );
    TEST_ASSERT_EQUAL_INT( 0, xResult.tv_nsec );

    /* Test with nanosec negative. */
    x.tv_sec = 5;
    x.tv_nsec = 500000000;
    nanosec = -9500000000LL;
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecAddNanoseconds( &x, nanosec, &xResult ) );

    /* Test for correct xResult even with pointer aliasing. */
    x.tv_sec = x.tv_nsec = 5;
    nanosec = 4999999995;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecAddNanoseconds( &x, nanosec, &x ) );
    TEST_ASSERT_EQUAL_INT( 10, x.tv_sec );
    TEST_ASSERT_EQUAL_INT( 0, x.tv_nsec );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_UTILS, UTILS_strnlen )
{
    static const char * const pcString = "This is a string used to test UTILS_strnlen.";
    size_t len = strlen( pcString );

    /* Check return value with NULL parameters. */
    TEST_ASSERT_EQUAL_UINT32( 0, UTILS_strnlen( NULL, len ) );

    /* Check return value with empty string. */
    TEST_ASSERT_EQUAL_UINT32( 0, UTILS_strnlen( "", len ) );

    /* Check for correct value when maxlen = 0. */
    TEST_ASSERT_EQUAL_UINT32( 0, UTILS_strnlen( pcString, 0 ) );

    /* Check for correct value when maxlen < len. */
    TEST_ASSERT_EQUAL_UINT32( len - 1UL, UTILS_strnlen( pcString, len - 1UL ) );

    /* Check for correct value even when maxlen > len. */
    TEST_ASSERT_EQUAL_UINT32( len, UTILS_strnlen( pcString, len + 1UL ) );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_UTILS,  UTILS_TimespecCompare )
{
    struct timespec xTimeX = { .tv_sec = 0, .tv_nsec = 0 };
    struct timespec xTimeY = { .tv_sec = 0, .tv_nsec = 0 };

    /* Check return value with NULL parameters. */
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecCompare( NULL, NULL) );
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecCompare( NULL, &xTimeY ) );
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecCompare( &xTimeX, NULL ) );

    /* check equal value */
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecCompare( &xTimeX, &xTimeY) );

    /* check greater value */
    xTimeX.tv_nsec = 1;
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecCompare( &xTimeX, &xTimeY ) );
    xTimeX.tv_sec = 1;
    TEST_ASSERT_EQUAL_INT( 1, UTILS_TimespecCompare( &xTimeX, &xTimeY ) );

    /* check less than value */
    xTimeY.tv_sec = 1;
    xTimeY.tv_nsec = 2;
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecCompare( &xTimeX, &xTimeY ) );
    xTimeX.tv_sec = 0;
    TEST_ASSERT_EQUAL_INT( -1, UTILS_TimespecCompare( &xTimeX, &xTimeY ) );
}

/*-----------------------------------------------------------*/

TEST(  Full_POSIX_UTILS, UTILS_TimespecToTicks )
{
    struct timespec xTimeX = { .tv_sec = 0, .tv_nsec = 0 };
    TickType_t xResult;

    /* Check return value with NULL parameters. */
    TEST_ASSERT_EQUAL_INT( EINVAL, UTILS_TimespecToTicks( NULL, &xResult ) );
    TEST_ASSERT_EQUAL_INT( EINVAL, UTILS_TimespecToTicks( &xTimeX, NULL ) );

    /* Check return value with invalid parameters */
    xTimeX.tv_nsec = NANOSECONDS_PER_SECOND + 1;
    TEST_ASSERT_EQUAL_INT( EINVAL, UTILS_TimespecToTicks( &xTimeX, &xResult ) );
    xTimeX.tv_nsec = -1;
    TEST_ASSERT_EQUAL_INT( EINVAL, UTILS_TimespecToTicks( &xTimeX, &xResult ) );

    /* Check return value and results with valid parameters*/
    xTimeX.tv_sec = 1;
    xTimeX.tv_nsec = NANOSECONDS_PER_TICK + 1;
    TickType_t xExpectedVal = xTimeX.tv_sec * configTICK_RATE_HZ + 2;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_TimespecToTicks( &xTimeX, &xResult ) );
    TEST_ASSERT_EQUAL_INT( ( int ) xExpectedVal, ( int ) xResult );
}

/*-----------------------------------------------------------*/

TEST(  Full_POSIX_UTILS, UTILS_AbsoluteTimespecToDeltaTicks )
{
    struct timespec xTimeX = { .tv_sec = 0, .tv_nsec = 0 };
    struct timespec xTimeY = { .tv_sec = 0, .tv_nsec = 0 };
    TickType_t xResult;

    /* Check return value with NULL parameters. */
    TEST_ASSERT_EQUAL_INT( EINVAL, UTILS_AbsoluteTimespecToDeltaTicks( &xTimeX, &xTimeY, NULL ) );
    TEST_ASSERT_EQUAL_INT( EINVAL, UTILS_AbsoluteTimespecToDeltaTicks( NULL, &xTimeY, &xResult ) );
    TEST_ASSERT_EQUAL_INT( EINVAL, UTILS_AbsoluteTimespecToDeltaTicks( &xTimeX, NULL, &xResult ) );

    /* Check return value with for the event in the past */
    xTimeY.tv_sec = 1;
    TEST_ASSERT_EQUAL_INT( ETIMEDOUT, UTILS_AbsoluteTimespecToDeltaTicks( &xTimeX, &xTimeY, &xResult ) );

    /* check return value for positive delta */
    xTimeX.tv_sec = 2;
    TEST_ASSERT_EQUAL_INT( 0, UTILS_AbsoluteTimespecToDeltaTicks( &xTimeX, &xTimeY, &xResult ) );

    /* check return value for invalid nano seconds */
    xTimeY.tv_nsec = NANOSECONDS_PER_SECOND + 1;
    TEST_ASSERT_EQUAL_INT( EINVAL, UTILS_AbsoluteTimespecToDeltaTicks( &xTimeX, &xTimeY, &xResult ) );

}

/*-----------------------------------------------------------*/
