/*
 * Amazon FreeRTOS Memory Leak Test V1.1.4
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* TCP/IP abstraction includes. */
#include "unity.h"
#include "unity_fixture.h"
#include "aws_test_runner.h"

/* Logging includes. */
#include "aws_logging_task.h"

#define memoryleakPRINTF( x )    vLoggingPrintf x

TEST_GROUP( Full_MemoryLeak );
TEST_SETUP( Full_MemoryLeak )
{
}

TEST_TEAR_DOWN( Full_MemoryLeak )
{
}

TEST_GROUP_RUNNER( Full_MemoryLeak )
{
    RUN_TEST_CASE( Full_MemoryLeak, CheckHeap );
}


TEST( Full_MemoryLeak, CheckHeap )
{
    size_t xHeapChange;

    xHeapChange = xHeapBefore - xHeapAfter;

    memoryleakPRINTF( ( "Heap Before: %d, Heap After: %d, Diff: %d \r\n",
                        xHeapBefore,
                        xHeapAfter,
                        xHeapChange ) );

    TEST_ASSERT_EQUAL_INT32_MESSAGE( 0,
                                     xHeapChange,
                                     "Free heap before and after tests was not the same." );
}
