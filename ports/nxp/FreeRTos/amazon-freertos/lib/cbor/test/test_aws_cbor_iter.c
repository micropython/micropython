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
#include "assert_override.h"
#include "aws_cbor_internals.h"
#include "unity_fixture.h"
#include <assert.h>
#include <string.h>

typedef struct test_next_s
{
    cbor_byte_t xBuffer[ 32 ];
    int lInitialIndex;
    int lExpectedIndex;
    char * pcMessage;
} test_next_t;

CBORHandle_t xCborData;
const test_next_t * pxTestCase;

TEST_GROUP( aws_cbor_iter );
TEST_SETUP( aws_cbor_iter )
{
    xCborData = CBOR_New( 0 );
    pxTestCase = NULL;
}

TEST_TEAR_DOWN( aws_cbor_iter )
{
    CBOR_Delete( &xCborData );
    pxTestCase = NULL;
}

TEST_GROUP_RUNNER( aws_cbor_iter )
{
    RUN_TEST_CASE( aws_cbor_iter, Next );
    RUN_TEST_CASE( aws_cbor_iter, Next_asserts_default_case );
    RUN_TEST_CASE( aws_cbor_iter, NextKey );
    RUN_TEST_CASE( aws_cbor_iter, SetCursor );

    RUN_TEST_CASE( aws_cbor_iter, null_checks );
}

#define NEXT_TEST_CASE_COUNT    ( 12 )
const test_next_t xNextTestCases[ NEXT_TEST_CASE_COUNT ] =
{
    {
        .pcMessage = "next from open map goes to first key",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x00, /* 3  0                */
            0xFF, /* 4  End of map       */
        },
        .lInitialIndex = 0,
        .lExpectedIndex = 1,
    },
    {
        .pcMessage = "next from key goes to its value",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x00, /* 3  0                */
            0xFF, /* 4  End of map       */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 3,
    },
    {
        .pcMessage = "next from value goes to next key",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x00, /* 3  0                */
            0x61, /* 4  Key of length 1  */
            'b',  /* 5                   */
            0x00, /* 6  0                */
            0xFF, /* 7  End of map       */
        },
        .lInitialIndex = 3,
        .lExpectedIndex = 4,
    },
    {
        .pcMessage = "next from last value goes to break",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x02, /* 3  0                */
            0x61, /* 4  Key of length 1  */
            'b',  /* 5                   */
            0x17, /* 6  0x17, 23         */
            0xFF, /* 7  End of map       */
        },
        .lInitialIndex = 6,
        .lExpectedIndex = 7,
    },
    {
        .pcMessage = "1 byte int jump",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x18, /* 3  1-byte uint      */
            0x41, /* 4                   */
            0x61, /* 5  Key of length 1  */
            'b',  /* 6                   */
            0x00, /* 7  0                */
            0xFF, /* 8  End of map       */
        },
        .lInitialIndex = 3,
        .lExpectedIndex = 5,
    },
    {
        .pcMessage = "2 byte int jump",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x19, /* 3  2-byte uint      */
            0xDE, /* 4                   */
            0xA1, /* 5                   */
            0x61, /* 6  Key of length 1  */
            'b',  /* 7                   */
            0x00, /* 8  0                */
            0xFF, /* 9  End of map       */
        },
        .lInitialIndex = 3,
        .lExpectedIndex = 6,
    },
    {
        .pcMessage = "4 byte int jump",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x1A, /* 3  4-byte uint      */
            0xDE, /* 4                   */
            0xAD, /* 5                   */
            0xBE, /* 6                   */
            0xEF, /* 7                   */
            0x61, /* 8  Key of length 1  */
            'b',  /* 9                   */
            0x00, /* 10 0                */
            0xFF, /* 11 End of map       */
        },
        .lInitialIndex = 3,
        .lExpectedIndex = 8,
    },
    {
        .pcMessage = "small negative integer",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x24, /* 3  -5               */
            0xFF, /* 4  End of map       */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 3,
    },
    {
        .pcMessage = "key of size 3",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x63, /* 1  Key of length 3  */
            'f',  /* 2                   */
            'o',  /* 3                   */
            'o',  /* 4                   */
            0x24, /* 5  -5               */
            0xFF, /* 6  End of map       */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 5,
    },
    {
        .pcMessage = "Iterate into map",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0xBF, /* 3  Open Map         */
            0x61, /* 4  Key of length 1  */
            'b',  /* 5                   */
            0x24, /* 6  -5               */
            0xFF, /* 7  End of map       */
            0x61, /* 8  Key of length 1  */
            'b',  /* 9                   */
            0x25, /* 10 -6               */
            0xFF, /* 11 End of map       */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 3,
    },
    {
        .pcMessage = "Iterate into map part 2",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0xBF, /* 3  Open Map         */
            0x61, /* 4  Key of length 1  */
            'b',  /* 5                   */
            0x24, /* 6  -5               */
            0xFF, /* 7  End of map       */
            0x61, /* 8  Key of length 1  */
            'b',  /* 9                   */
            0x25, /* 10 -6               */
            0xFF, /* 11 End of map       */
        },
        .lInitialIndex = 3,
        .lExpectedIndex = 4,
    },
    {
        .pcMessage = "Iterate into map part 3",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0xBF, /* 3  Open Map         */
            0x61, /* 4  Key of length 1  */
            'b',  /* 5                   */
            0x24, /* 6  -5               */
            0xFF, /* 7  End of map       */
            0x61, /* 8  Key of length 1  */
            'b',  /* 9                   */
            0x25, /* 10 -6               */
            0xFF, /* 11 End of map       */
        },
        .lInitialIndex = 7,
        .lExpectedIndex = 8,
    },
};

void test_Next( void )
{
    CBOR_SetCursor( xCborData, 0 );
    CBOR_MemCopy( xCborData, pxTestCase->xBuffer, sizeof( pxTestCase->xBuffer ) );
    CBOR_SetCursor( xCborData, pxTestCase->lInitialIndex );
    CBOR_Next( xCborData );
    cbor_ssize_t xActualIndex = xCborData->pxCursor - xCborData->pxBufferStart;
    TEST_ASSERT_EQUAL_MESSAGE(
        pxTestCase->lExpectedIndex, xActualIndex, pxTestCase->pcMessage );
}

TEST( aws_cbor_iter, Next )
{
    for( int i = 0; i < NEXT_TEST_CASE_COUNT; i++ )
    {
        pxTestCase = &xNextTestCases[ i ];
        RUN_TEST( test_Next );
    }
}

TEST( aws_cbor_iter, Next_asserts_default_case )
{
    CBOR_SetCursor( xCborData, 0 );
    *xCborData->pxCursor = CBOR_TAG;
    xTEST_expect_assert( CBOR_Next( xCborData ) );
}

#define NEXT_KEY_TEST_CASE_COUNT    ( 7 )
const test_next_t xNextKeyTestCases[ NEXT_KEY_TEST_CASE_COUNT ] =
{
    {
        .pcMessage = "next from open map goes to first key",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x00, /* 3  0                */
            0x61, /* 4  Key of length 1  */
            'b',  /* 5                   */
            0x00, /* 6  0                */
            0xFF, /* 7  End of map       */
        },
        .lInitialIndex = 0,
        .lExpectedIndex = 1,
    },
    {
        .pcMessage = "next key first key goes to second key",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x00, /* 3  0                */
            0x61, /* 4  Key of length 1  */
            'b',  /* 5                   */
            0x00, /* 6  0                */
            0xFF, /* 7  End of map       */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 4,
    },
    {
        .pcMessage = "Next key after map",
        .xBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0xBF, /* 3  Open Map         */
            0x61, /* 4  Key of length 1  */
            'b',  /* 5                   */
            0x24, /* 6  -5               */
            0xFF, /* 7  End of map       */
            0x61, /* 8  Key of length 1  */
            'b',  /* 9                   */
            0x25, /* 10 -6               */
            0xFF, /* 11 End of map       */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 8,
    },
    {
        .pcMessage = "Next key after string",
        .xBuffer =
        {
            0xBF, /* 0 Open Map           */
            0x61, /* 1 Key of length 1    */
            'a',  /* 2                    */
            0x61, /* 3 String of length 1 */
            'f',  /* 4                    */
            0x61, /* 5 Key of length 1    */
            'b',  /* 6                    */
            0x25, /* 7 -6                 */
            0xFF, /* 8 End of map         */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 5,
    },
    {
        .pcMessage = "Next key after byte string",
        .xBuffer =
        {
            0xBF, /* 0 Open Map                */
            0x61, /* 1 Key of length 1         */
            'a',  /* 2                         */
            0x41, /* 3 Byte String of length 1 */
            0x10, /* 4                         */
            0x61, /* 5 Key of length 1         */
            'b',  /* 6                         */
            0x25, /* 7 -6                      */
            0xFF, /* 8 End of map              */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 5,
    },
    {
        .pcMessage = "Next key after last key",
        .xBuffer =
        {
            0xBF, /* 0 Open Map                */
            0x61, /* 1 Key of length 1         */
            'a',  /* 2                         */
            0x41, /* 3 Byte String of length 1 */
            0x10, /* 4                         */
            0x61, /* 5 Key of length 1         */
            'b',  /* 6                         */
            0x25, /* 7 -6                      */
            0xFF, /* 8 End of map              */
        },
        .lInitialIndex = 5,
        .lExpectedIndex = 8,
    },
    {
        .pcMessage = "Next key after nested map",
        .xBuffer =
        {
            0xBF, /* 0  Open Map 1      */
            0x61, /* 1  Key of length 1 */
            'm',  /* 2                  */
            0xBF, /* 3  Open Map 2      */
            0x61, /* 4  Key of length 1 */
            'n',  /* 5                  */
            0xBF, /* 6  Open Map 3      */
            0xFF, /* 7  End of map 3    */
            0xFF, /* 8  End of map 2    */
            0x61, /* 9  Key of length 1 */
            'b',  /* 10                 */
            0x25, /* 11 -6              */
            0xFF, /* 12 End of map 1    */
        },
        .lInitialIndex = 1,
        .lExpectedIndex = 9,
    },
};

void test_NextKey( void )
{
    CBOR_SetCursor( xCborData, 0 );
    CBOR_MemCopy( xCborData, pxTestCase->xBuffer, sizeof( pxTestCase->xBuffer ) );
    xCborData->pxMapEnd = xCborData->pxCursor - 1;
    CBOR_SetCursor( xCborData, pxTestCase->lInitialIndex );
    CBOR_NextKey( xCborData );
    cbor_ssize_t xActualIndex = xCborData->pxCursor - xCborData->pxBufferStart;
    TEST_ASSERT_EQUAL_MESSAGE(
        pxTestCase->lExpectedIndex, xActualIndex, pxTestCase->pcMessage );
}

TEST( aws_cbor_iter, NextKey )
{
    for( int i = 0; i < NEXT_KEY_TEST_CASE_COUNT; i++ )
    {
        pxTestCase = &xNextKeyTestCases[ i ];
        RUN_TEST( test_NextKey );
    }
}

TEST( aws_cbor_iter, SetCursor )
{
    int lExpectedPosition = 0;

    CBOR_SetCursor( xCborData, lExpectedPosition );
    TEST_ASSERT_EQUAL_PTR(
        &( xCborData->pxBufferStart[ lExpectedPosition ] ), xCborData->pxCursor );

    lExpectedPosition = 3;
    CBOR_SetCursor( xCborData, lExpectedPosition );
    TEST_ASSERT_EQUAL_PTR(
        &( xCborData->pxBufferStart[ lExpectedPosition ] ), xCborData->pxCursor );
}

TEST( aws_cbor_iter, null_checks )
{
    xTEST_expect_assert( CBOR_NextPtr( NULL ) );
    xTEST_expect_assert( CBOR_Next( NULL ) );
    xTEST_expect_assert( CBOR_NextKeyPtr( NULL ) );
    xTEST_expect_assert( CBOR_NextKey( NULL ) );
    xTEST_expect_assert( CBOR_SetCursor( NULL, 0 ) );
}
