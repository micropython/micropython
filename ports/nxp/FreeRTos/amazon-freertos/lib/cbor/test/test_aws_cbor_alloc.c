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
#include "aws_cbor_alloc.h"
#include "unity_fixture.h"
#include <string.h>

TEST_GROUP( aws_cbor_alloc );

TEST_SETUP( aws_cbor_alloc )
{
}

TEST_TEAR_DOWN( aws_cbor_alloc )
{
}

TEST_GROUP_RUNNER( aws_cbor_alloc )
{
    RUN_TEST_CASE( aws_cbor_alloc, ReallocImpl_returns_new_pointer );
    RUN_TEST_CASE( aws_cbor_alloc, ReallocImpl_copies_content_from_old_location );
    RUN_TEST_CASE(
        aws_cbor_alloc, ReallocImpl_returns_null_when_allocation_fails );
}

TEST( aws_cbor_alloc, ReallocImpl_returns_new_pointer )
{
    char * malloced_ptr = pxCBOR_malloc( 10 );
    char * realloced_ptr = CBOR_ReallocImpl( malloced_ptr, 15 );

    TEST_ASSERT_NOT_EQUAL( malloced_ptr, realloced_ptr );
    pxCBOR_free( realloced_ptr );
}

TEST( aws_cbor_alloc, ReallocImpl_copies_content_from_old_location )
{
    char expected[] = "Original string";
    char * malloced_copy = pxCBOR_malloc( sizeof( expected ) );

    strncpy( malloced_copy, expected, sizeof( expected ) );
    char * realloced_copy =
        CBOR_ReallocImpl( malloced_copy, sizeof( expected ) * 3 / 2 );
    TEST_ASSERT_EQUAL_STRING( expected, realloced_copy );
    pxCBOR_free( realloced_copy );
}

TEST( aws_cbor_alloc, ReallocImpl_returns_null_when_allocation_fails )
{
    char * malloced_ptr = pxCBOR_malloc( 10 );

    UnityMalloc_MakeMallocFailAfterCount( 0 );
    char * realloced_ptr = CBOR_ReallocImpl( malloced_ptr, 15 );
    pxCBOR_free( malloced_ptr );
    TEST_ASSERT_EQUAL_PTR( NULL, realloced_ptr );
}
