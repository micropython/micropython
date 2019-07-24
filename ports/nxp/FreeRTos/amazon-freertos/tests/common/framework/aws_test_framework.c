/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "aws_logging_task.h"

/* Test framework include */
#include "aws_test_framework.h"

/* Unity include */
#include "unity.h"

#define TEST_RESULT_BUFFER_CAPACITY    1024

/*-----------------------------------------------------------*/

/* The buffer to store test result. The content will be printed if an eol character
 * is received */
static char pcTestResultBuffer[ TEST_RESULT_BUFFER_CAPACITY ];
static int16_t xBufferSize = 0;

/*-----------------------------------------------------------*/

void TEST_CacheResult( char cResult )
{
    if( TEST_RESULT_BUFFER_CAPACITY - xBufferSize == 2 )
    {
        cResult = '\n';
    }

    pcTestResultBuffer[ xBufferSize++ ] = cResult;

    if( ( '\n' == cResult ) )
    {
        TEST_SubmitResultBuffer();
    }
}
/*-----------------------------------------------------------*/

void TEST_SubmitResultBuffer()
{
    if( 0 != xBufferSize )
    {
        TEST_SubmitResult( pcTestResultBuffer );
        memset( pcTestResultBuffer, 0, TEST_RESULT_BUFFER_CAPACITY );
        xBufferSize = 0;
    }
}
/*-----------------------------------------------------------*/

void TEST_NotifyTestStart()
{
    /* Wait for test script to open serial port before starting tests on the
     * board.*/
    vTaskDelay( pdMS_TO_TICKS( 5000UL ) );
    TEST_SubmitResult( "---------STARTING TESTS---------\n" );
}
/*-----------------------------------------------------------*/

void TEST_NotifyTestFinished()
{
    TEST_SubmitResult( "-------ALL TESTS FINISHED-------\n" );
}
/*-----------------------------------------------------------*/

void TEST_SubmitResult( const char * pcResult )
{
    /* We want to print test result no matter configPRINTF is defined or not */
    vLoggingPrint( pcResult );

    /* Wait for 0.1 seconds to let print task empty its buffer. */
    vTaskDelay( pdMS_TO_TICKS( 100UL ) );
}
