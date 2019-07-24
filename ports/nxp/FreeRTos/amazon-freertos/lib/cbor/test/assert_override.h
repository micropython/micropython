/*
 * Amazon FreeRTOS
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
#ifndef ASSERT_OVERRIDE_H
#define ASSERT_OVERRIDE_H

#include <assert.h>
#include <stdbool.h>

#define __assert_fail    TestAssertFail

#define xTEST_expect_assert( a ) \
    xTEST_expect_assert = true;  \
    if( TEST_PROTECT() ) {       \
        ( a );                   \
    }                            \
    else {                       \
    }                            \
    TEST_ASSERT_EQUAL_MESSAGE(   \
        false, xTEST_expect_assert, "assert() was not called" )

extern int lTEST_assert_fails;
extern bool xTEST_expect_assert;

void TestAssertFail( const char *,
                     const char *,
                     unsigned int,
                     const char * );

#endif /* end of include guard: ASSERT_OVERRIDE_H */
