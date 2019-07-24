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
 * @file aws_test_posix_semaphore.c
 * @brief Tests for POSIX semaphore functions.
 */

/* FreeRTOS+POSIX includes. */
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX/semaphore.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/*-----------------------------------------------------------*/

TEST_GROUP( Full_POSIX_SEMAPHORE );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_POSIX_SEMAPHORE )
{
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_POSIX_SEMAPHORE )
{
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_POSIX_SEMAPHORE )
{
    RUN_TEST_CASE( Full_POSIX_SEMAPHORE, sem_wait_trywait );
    RUN_TEST_CASE( Full_POSIX_SEMAPHORE, sem_timedwait );
    RUN_TEST_CASE( Full_POSIX_SEMAPHORE, sem_value );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_SEMAPHORE, sem_wait_trywait )
{
    int iStatus = 0;
    sem_t xSemaphore = { 0 };

    /* Initialize a semaphore with value 1. */
    iStatus = sem_init( &xSemaphore, 0, 1 );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    if( TEST_PROTECT() )
    {
        /* sem_wait should immediately succeed, as the semaphore is available. */
        iStatus = sem_wait( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* trywait should fail immediately, as the semaphore is not available. */
        iStatus = sem_trywait( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EAGAIN, errno );
    }

    ( void ) sem_destroy( &xSemaphore );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_SEMAPHORE, sem_timedwait )
{
    int iStatus = 0;
    sem_t xSemaphore = { 0 };
    struct timespec xDelay = { 0 };

    /* Initialize a semaphore with value 2. */
    iStatus = sem_init( &xSemaphore, 0, 2 );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    if( TEST_PROTECT() )
    {
        /* sem_timedwait should always succeed when the semaphore is available,
         * event if the delay is in the past or invalid. */
        xDelay.tv_nsec = -1;
        iStatus = sem_timedwait( &xSemaphore, &xDelay );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        xDelay.tv_nsec = 0; /* Timeout in the past. */
        iStatus = sem_timedwait( &xSemaphore, &xDelay );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* When the semaphore is not available, sem_timedwait should fail. */
        iStatus = sem_timedwait( &xSemaphore, &xDelay );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( ETIMEDOUT, errno );

        xDelay.tv_nsec = -1; /* Invalid delay */
        iStatus = sem_timedwait( &xSemaphore, &xDelay );
        TEST_ASSERT_EQUAL_INT( -1, iStatus );
        TEST_ASSERT_EQUAL_INT( EINVAL, errno );
    }

    ( void ) sem_destroy( &xSemaphore );
}

/*-----------------------------------------------------------*/

TEST( Full_POSIX_SEMAPHORE, sem_value )
{
    int iStatus = 0, iSemaphoreValue = 0;
    sem_t xSemaphore = { 0 };

    /* Creating a semaphore with a value more than SEM_VALUE_MAX should fail. */
    iStatus = sem_init( &xSemaphore, 0, SEM_VALUE_MAX + 1 );
    TEST_ASSERT_EQUAL_INT( -1, iStatus );
    TEST_ASSERT_EQUAL_INT( EINVAL, errno );

    /* Create a semaphore with value 0. */
    iStatus = sem_init( &xSemaphore, 0, 0 );
    TEST_ASSERT_EQUAL_INT( 0, iStatus );

    if( TEST_PROTECT() )
    {
        /* Ensure that the correct semaphore value is returned. */
        iStatus = sem_getvalue( &xSemaphore, &iSemaphoreValue );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_EQUAL_INT( 0, iSemaphoreValue );

        /* Post to the semaphore twice. */
        iStatus = sem_post( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = sem_post( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        /* Ensure that the semaphore has a value of 2. */
        iStatus = sem_getvalue( &xSemaphore, &iSemaphoreValue );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_EQUAL_INT( 2, iSemaphoreValue );

        /* Wait on the semaphore and ensure that its value decrements. */
        iStatus = sem_wait( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = sem_getvalue( &xSemaphore, &iSemaphoreValue );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_EQUAL_INT( 1, iSemaphoreValue );

        /* Post to the semaphore again to ensure its value increments. */
        iStatus = sem_post( &xSemaphore );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );

        iStatus = sem_getvalue( &xSemaphore, &iSemaphoreValue );
        TEST_ASSERT_EQUAL_INT( 0, iStatus );
        TEST_ASSERT_EQUAL_INT( 2, iSemaphoreValue );
    }

    ( void ) sem_destroy( &xSemaphore );
}
