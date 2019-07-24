/*
 * Amazon FreeRTOS CBOR Library V1.0.1
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
#include "unity_fixture.h"

static void runAllTests()
{
    RUN_TEST_GROUP(aws_cbor);
    RUN_TEST_GROUP(aws_cbor_acceptance);
    RUN_TEST_GROUP(aws_cbor_alloc);
    RUN_TEST_GROUP(aws_cbor_int);
    RUN_TEST_GROUP(aws_cbor_iter);
    RUN_TEST_GROUP(aws_cbor_map);
    RUN_TEST_GROUP(aws_cbor_mem);
    RUN_TEST_GROUP(aws_cbor_print);
    RUN_TEST_GROUP(aws_cbor_string);
}

int main(int argc, const char *argv[])
{
    UnityMain(argc, argv, runAllTests);
}
