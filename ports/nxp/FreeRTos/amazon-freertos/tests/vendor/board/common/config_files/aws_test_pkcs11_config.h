/*
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
 * @file aws_test_pkcs11_config.h
 * @brief Port-specific variables for PKCS11 tests. 
 */
#ifndef _AWS_TEST_PKCS11_CONFIG_H_
#define _AWS_TEST_PKCS11_CONFIG_H_

/**
 * @brief Number of simultaneous tasks for SignVerifyRoundTrip_MultitaskLoop test.
 *
 * Each task consumes both stack and heap space, which may cause memory allocation
 * failures if too many tasks are created. 
 */
#define pkcs11testSIGN_VERIFY_TASK_COUNT    ( 2 )    /* FIX ME. */

/**
 * @brief The number of iterations in SignVerifyRoundTrip_MultitaskLoop.
 *
 * A single iteration of SignVerifyRoundTrip may take up to a minute on some
 * boards. Ensure that pkcs11testEVENT_GROUP_TIMEOUT is long enough to accommodate
 * all iterations of the loop.
 */
#define pkcs11testSIGN_VERIFY_LOOP_COUNT    ( 10 )    /* FIX ME. */

/**
 * @brief
 *
 * All tasks of the SignVerifyRoundTrip_MultitaskLoop test must finish within
 * this timeout, or the test will fail.
 */
#define pkcs11testEVENT_GROUP_TIMEOUT_MS    ( pdMS_TO_TICKS( 1000000UL ) )    /* FIX ME. */

#endif /* _AWS_TEST_PKCS11_CONFIG_H_ */
