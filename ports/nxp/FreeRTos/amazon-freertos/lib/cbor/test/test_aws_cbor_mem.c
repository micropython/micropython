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
#include <string.h>

typedef struct test_size_s
{
    uint8_t ucBuffer[ 32 ];
    cbor_ssize_t xCursorIndex;
    cbor_ssize_t xExpectedSize;
    char * pcMessage;
} test_size_t;

static CBORHandle_t xCborData;

TEST_GROUP( aws_cbor_mem );
TEST_SETUP( aws_cbor_mem )
{
    xCborData = CBOR_New( 0 );
}

TEST_TEAR_DOWN( aws_cbor_mem )
{
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );
    CBOR_Delete( &xCborData );
}

TEST_GROUP_RUNNER( aws_cbor_mem )
{
    RUN_TEST_CASE( aws_cbor_mem, test_setup );

    RUN_TEST_CASE( aws_cbor_mem, AssignAndIncrement );
    RUN_TEST_CASE(
        aws_cbor_mem, AssignAndIncrement_reallocates_when_buffer_is_full );
    RUN_TEST_CASE(
        aws_cbor_mem, AssignAndIncrement_returns_err_when_out_of_memory );
    RUN_TEST_CASE( aws_cbor_mem,
                   AssignAndIncrement_asserts_when_cursor_before_start_of_buffer );

    RUN_TEST_CASE( aws_cbor_mem, AssignAndDecrement );
    RUN_TEST_CASE( aws_cbor_mem,
                   AssignAndDecrement_asserts_when_cursor_before_start_of_buffer );
    RUN_TEST_CASE(
        aws_cbor_mem, AssignAndDecrement_returns_err_when_past_end_of_buffer );

    RUN_TEST_CASE( aws_cbor_mem, MemCopy_will_forward_copy_overlapping_strings );
    RUN_TEST_CASE( aws_cbor_mem, MemCopy_will_reverse_copy_overlapping_strings );
    RUN_TEST_CASE( aws_cbor_mem, MemCopy_will_do_nothing_if_src_matches_dest );
    RUN_TEST_CASE( aws_cbor_mem, MemCopy_returns_err_when_out_of_memory );

    RUN_TEST_CASE( aws_cbor_mem, GetValueSize );

    RUN_TEST_CASE( aws_cbor_mem, ValueResize_will_shrink_value );
    RUN_TEST_CASE( aws_cbor_mem, ValueResize_will_grow_value );
    RUN_TEST_CASE(
        aws_cbor_mem, ValueResize_makes_no_change_if_data_size_unknown );
    RUN_TEST_CASE( aws_cbor_mem, ValueResize_asserts_on_map_end_errors );

    RUN_TEST_CASE( aws_cbor_mem, null_checks );
}

TEST( aws_cbor_mem, test_setup )
{
    /* Test to ensure tests are setup correctly */
    TEST_ASSERT_NOT_NULL( xCborData );
}

TEST( aws_cbor_mem, AssignAndIncrement )
{
    CBOR_AssignAndIncrementCursor( xCborData, 0x42 );
    CBOR_AssignAndIncrementCursor( xCborData, 0x24 );
    TEST_ASSERT_EQUAL_HEX8( 0x42, ( xCborData->pxBufferStart )[ 0 ] );
    TEST_ASSERT_EQUAL_HEX8( 0x24, ( xCborData->pxBufferStart )[ 1 ] );
}

TEST( aws_cbor_mem, AssignAndIncrement_reallocates_when_buffer_is_full )
{
    int lInitialBufferSize =
        xCborData->pxBufferEnd - xCborData->pxBufferStart + 1;

    xCborData->pxCursor = xCborData->pxBufferEnd;
    CBOR_AssignAndIncrementCursor( xCborData, 0xF0 );
    CBOR_AssignAndIncrementCursor( xCborData, 0x01 );
    int lFinalBufferSize =
        xCborData->pxBufferEnd - xCborData->pxBufferStart + 1;
    TEST_ASSERT_GREATER_THAN( lInitialBufferSize, lFinalBufferSize );
}

TEST( aws_cbor_mem, AssignAndIncrement_returns_err_when_out_of_memory )
{
    xCborData->pxCursor = xCborData->pxBufferEnd;
    UnityMalloc_MakeMallocFailAfterCount( 0 );
    CBOR_AssignAndIncrementCursor( xCborData, 0xDE );
    CBOR_AssignAndIncrementCursor( xCborData, 0xAD );
    TEST_ASSERT_EQUAL( eCborErrInsufficentSpace, xCborData->xError );

    xCborData->xError = eCborErrNoError;
}

TEST(
    aws_cbor_mem, AssignAndIncrement_asserts_when_cursor_before_start_of_buffer )
{
    xCborData->pxCursor = xCborData->pxBufferStart - 1;
    xTEST_expect_assert( CBOR_AssignAndIncrementCursor( xCborData, 0xBC ) );
}

TEST( aws_cbor_mem, AssignAndDecrement )
{
    xCborData->pxCursor = &( xCborData->pxBufferStart )[ 1 ];
    CBOR_AssignAndDecrementCursor( xCborData, 0x73 );
    CBOR_AssignAndDecrementCursor( xCborData, 0x24 );
    TEST_ASSERT_EQUAL_HEX8( 0x73, ( xCborData->pxBufferStart )[ 1 ] );
    TEST_ASSERT_EQUAL_HEX8( 0x24, ( xCborData->pxBufferStart )[ 0 ] );
}

TEST(
    aws_cbor_mem, AssignAndDecrement_asserts_when_cursor_before_start_of_buffer )
{
    CBOR_AssignAndDecrementCursor( xCborData, 0xEF );
    xTEST_expect_assert( CBOR_AssignAndDecrementCursor( xCborData, 0xBE ) );
}

TEST( aws_cbor_mem, AssignAndDecrement_returns_err_when_past_end_of_buffer )
{
    xCborData->pxCursor = xCborData->pxBufferEnd + 1;

    UnityMalloc_MakeMallocFailAfterCount( 0 );
    CBOR_AssignAndDecrementCursor( xCborData, 0xCE );
    TEST_ASSERT_EQUAL( eCborErrInsufficentSpace, xCborData->xError );

    xCborData->xError = eCborErrNoError;
}

TEST( aws_cbor_mem, MemCopy_will_forward_copy_overlapping_strings )
{
    char cInit[] = "Helllo, World!";
    char cExpected[] = "Hello, World!";

    /* Copy the string into the CBOR ucBuffer. */
    memcpy( ( xCborData->pxBufferStart ), cInit, strlen( cInit ) + 1 );

    /* Point the cursor to the third l. */
    /* This is where the data will be copied to. */
    xCborData->pxCursor = &( xCborData->pxBufferStart )[ 4 ];

    /* Starting from the 'o', copy back to the cursor. */
    CBOR_MemCopy( xCborData, &( xCborData->pxBufferStart )[ 5 ], 10 );

    TEST_ASSERT_EQUAL_STRING( cExpected, ( xCborData->pxBufferStart ) );

    /* Cursor will now be pointing to the position after the null terminator for
     * the string "Hello, World!" */
    uint8_t * pucExpectedCursorPosition =
        &( xCborData->pxBufferStart )[ strlen( cExpected ) + 1 ];
    TEST_ASSERT_EQUAL_PTR( pucExpectedCursorPosition, xCborData->pxCursor );
}

TEST( aws_cbor_mem, MemCopy_will_reverse_copy_overlapping_strings )
{
    char cInit[] = "Helo, World!";
    char cExpected[] = "Hello, World!";

    /* Copy the string into the CBOR ucBuffer. */
    memcpy( ( xCborData->pxBufferStart ), cInit, strlen( cInit ) + 1 );

    /* Point the cursor at the 'o' */
    xCborData->pxCursor = &( xCborData->pxBufferStart )[ 3 ];

    /* Starting from the 'l' in "Helo", copy to the cursor */
    CBOR_MemCopy( xCborData, &( xCborData->pxBufferStart )[ 2 ], 11 );

    TEST_ASSERT_EQUAL_STRING( cExpected, ( xCborData->pxBufferStart ) );

    /* Cursor will now be pointing to the position after the null terminator for */
    /* the string "Hello, World!" */
    uint8_t * pucExpectedCursorPosition =
        &( xCborData->pxBufferStart )[ strlen( cExpected ) + 1 ];
    TEST_ASSERT_EQUAL_PTR( pucExpectedCursorPosition, xCborData->pxCursor );
}

TEST( aws_cbor_mem, MemCopy_will_do_nothing_if_src_matches_dest )
{
    char cInit[] = "Hello, World!";
    char cExpected[] = "Hello, World!";

    /* Copy the string into the CBOR ucBuffer. */
    memcpy( ( xCborData->pxBufferStart ), cInit, sizeof( cInit ) );

    /* Point the cursor to the start of the ucBuffer */
    xCborData->pxCursor = ( xCborData->pxBufferStart );

    /* From the start of the ucBuffer, copy to the cursor */
    /* (Which is at the same position (the start of the ucBuffer)). */
    CBOR_MemCopy( xCborData, ( xCborData->pxBufferStart ), sizeof( cInit ) );

    TEST_ASSERT_EQUAL_STRING( cExpected, ( xCborData->pxBufferStart ) );

    /* Cursor will now be pointing to the position after the null terminator for */
    /* the string "Hello, World!" */
    uint8_t * pucExpectedCursorPosition =
        &( xCborData->pxBufferStart )[ strlen( cExpected ) + 1 ];
    TEST_ASSERT_EQUAL( 0, pucExpectedCursorPosition - xCborData->pxCursor );
}

TEST( aws_cbor_mem, MemCopy_returns_err_when_out_of_memory )
{
    char cLorem[] =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim "
        "ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum "
        "dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor "
        "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
        "commodo consequat. Duis aute irure dolor in reprehenderit in "
        "voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
        "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui "
        "officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit "
        "amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt "
        "ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis "
        "nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo "
        "consequat. Duis aute irure dolor in reprehenderit in voluptate velit "
        "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat "
        "cupidatat non proident, sunt in culpa qui officia deserunt mollit "
        "anim id est laborum.";

    UnityMalloc_MakeMallocFailAfterCount( 0 );
    CBOR_MemCopy( xCborData, cLorem, sizeof( cLorem ) );
    TEST_ASSERT_EQUAL( eCborErrInsufficentSpace, xCborData->xError );
    xCborData->xError = eCborErrNoError;
}

const test_size_t * pxTestCase;
const test_size_t xSizeTestCases[] =
{
    {
        .pcMessage = "smallest integer in additional data",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x00, /* 3  0                */
            0xFF, /* 4  End of map       */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 1,
    },
    {
        .pcMessage = "largest integer in additional data",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map         */
            0x61, /* 1  Key of length 1  */
            'a',  /* 2                   */
            0x17, /* 3  23                */
            0xFF, /* 4  End of map       */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 1,
    },
    {
        .pcMessage = "integer in 1 following byte",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map              */
            0x61, /* 1  Key of length 1       */
            'a',  /* 2                        */
            0x18, /* 3  int follows in 1 byte */
            24,   /* 4                        */
            0xFF, /* 5  End of map            */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 2,
    },
    {
        .pcMessage = "integer in 2 following bytes",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map              */
            0x61, /* 1  Key of length 1       */
            'a',  /* 2                        */
            0x19, /* 3  int follows in 2 byte */
            0xDE, /* 4                        */
            0xAD, /* 5                        */
            0xFF, /* 6  End of map            */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 3,
    },
    {
        .pcMessage = "integer in 4 following bytes",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map              */
            0x61, /* 1  Key of length 1       */
            'a',  /* 2                        */
            0x1A, /* 3  int follows in 4 byte */
            0xDE, /* 4                        */
            0xAD, /* 5                        */
            0xBE, /* 6                        */
            0xEF, /* 7                        */
            0xFF, /* 8  End of map            */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 5,
    },
    {
        .pcMessage = "zero length string",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map              */
            0x61, /* 1  Key of length 1       */
            'a',  /* 2                        */
            0x60, /* 3  zero length string    */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 1,
    },
    {
        .pcMessage = "23 char string",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map              */
            0x61, /* 1  Key of length 1       */
            'a',  /* 2                        */
            0x77, /* 3  23 char string        */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 24,
    },
    {
        .pcMessage = "24 char string",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map                      */
            0x61, /* 1  Key of length 1               */
            'a',  /* 2                                */
            0x78, /* 3  string size follows in 1 byte */
            24,   /* 4  24 char string                */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 24 + 2,
    },
    {
        .pcMessage = "256 char string",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map                      */
            0x61, /* 1  Key of length 1               */
            'a',  /* 2                                */
            0x79, /* 3  string size follows in 2 byte */
            0x01, /* 4  256 char string               */
            0x00, /* 5                                */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 256 + 3,
    },
    {
        .pcMessage = "empty map",
        .ucBuffer =
        {
            0xBF, /* 0 Open Map   */
            0xFF, /* 1 End of map */
        },
        .xCursorIndex = 0,
        .xExpectedSize = 2,
    },
    {
        .pcMessage = "simple map",
        .ucBuffer =
        {
            0xBF, /* 0 Open Map        */
            0x61, /* 1 Key of length 1 */
            'a',  /* 2                 */
            0x00, /* 3 0               */
            0xFF, /* 4 End of map      */
        },
        .xCursorIndex = 0,
        .xExpectedSize = 5,
    },
    {
        .pcMessage = "map inside of map",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map               */
            0x61, /* 1  Key of length 1        */
            'm',  /* 2                         */
            0xBF, /* 3  Open Map               */
            0x61, /* 4  Key, length 1          */
            '1',  /* 5                         */
            0x19, /* 6  int follows in 2 bytes */
            0xDE, /* 7                         */
            0xAD, /* 8                         */
            0xFF, /* 9  End of map             */
            0x61, /* 10 Key, length 1          */
            'a',  /* 11                        */
            0x10, /* 12 16                     */
            0xFF, /* 13 End of map             */
        },
        .xCursorIndex = 3,
        .xExpectedSize = 7,
    },
    {
        .pcMessage = "map containing a map",
        .ucBuffer =
        {
            0xBF, /* 0  Open Map               */
            0x61, /* 1  Key of length 1        */
            'm',  /* 2                         */
            0xBF, /* 3  Open Map               */
            0x61, /* 4  Key, length 1          */
            '1',  /* 5                         */
            0x19, /* 6  int follows in 2 bytes */
            0xDE, /* 7                         */
            0xAD, /* 8                         */
            0xFF, /* 9  End of map             */
            0x61, /* 10 Key, length 1          */
            'a',  /* 11                        */
            0x10, /* 12 16                     */
            0xFF, /* 13 End of map             */
        },
        .xCursorIndex = 0,
        .xExpectedSize = 14,
    },
};

void test_GetSize( void )
{
    CBOR_SetCursor( xCborData, 0 );
    CBOR_MemCopy( xCborData, pxTestCase->ucBuffer, sizeof( pxTestCase->ucBuffer ) );
    CBOR_SetCursor( xCborData, pxTestCase->xCursorIndex );
    cbor_ssize_t xActualSize = xCborDataItemSize( xCborData );
    TEST_ASSERT_EQUAL_MESSAGE(
        pxTestCase->xExpectedSize, xActualSize, pxTestCase->pcMessage );
}

TEST( aws_cbor_mem, GetValueSize )
{
    int lTestCaseCount =
        sizeof( xSizeTestCases ) / sizeof( xSizeTestCases[ 0 ] );

    for( int i = 0; i < lTestCaseCount; i++ )
    {
        pxTestCase = &xSizeTestCases[ i ];
        RUN_TEST( test_GetSize );
    }
}

TEST( aws_cbor_mem, ValueResize_will_shrink_value )
{
    uint8_t ucInitial[] =
    {
        0xBF, /* 0  Open Map          */
        0x61, /* 1  Key of length 1   */
        0x31, /* 2 '1'                */
        0x1A, /* 3  Start 32-bit int  */
        0x5C, /* 4                    */
        0xA1, /* 5                    */
        0xAB, /* 6                    */
        0x1E, /* 7                    */
        0x61, /* 8  Key of length 1   */
        0x32, /* 9 '2'                */
        0x19, /* 10  Start 16-bit int */
        0x73, /* 11                   */
        0x92, /* 12                   */
        0xFF, /* 13 End of map        */
    };

    uint8_t ucExpected[] =
    {
        0xBF, /* 0      */
        0x61, /* 1      */
        0x31, /* 2      */
        0x1A, /* 3      */
        0x5C, /* 4      */
        0x61, /* 5      */
        0x32, /* 6      */
        0x19, /* 7      */
        0x73, /* 8      */
        0x92, /* 9      */
        0xFF, /* 10     */
    };

    memcpy( ( xCborData->pxBufferStart ), ucInitial, sizeof( ucInitial ) );
    xCborData->pxCursor = &( xCborData->pxBufferStart )[ 3 ];
    xCborData->pxMapEnd = &( xCborData->pxBufferStart )[ 13 ];
    cbor_ssize_t xNewSize = 2;
    CBOR_ValueResize( xCborData, xNewSize );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, ( xCborData->pxBufferStart ), sizeof( ucExpected ) );
}

TEST( aws_cbor_mem, ValueResize_will_grow_value )
{
    uint8_t ucInitial[] =
    {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 1  */
        0x31, /* 2 '1'               */
        0x18, /* 3  Start 8-bit int  */
        0x23, /* 4                   */
        0x61, /* 5  Key of length 1  */
        0x32, /* 6 '2'               */
        0x19, /* 7  Start 16-bit int */
        0x73, /* 8                   */
        0x92, /* 9                   */
        0xFF, /* 10 End of map       */
    };

    uint8_t ucExpected[] =
    {
        0xBF, /* 0               */
        0x61, /* 1               */
        0x31, /* 2               */
        0x18, /* 3               */
        0x23, /* 4               */
        0x61, /* 5               */
        0x61, /* 6               */
        0x32, /* 7               */
        0x19, /* 8               */
        0x73, /* 9               */
        0x92, /* 10              */
        0xFF, /* 11              */
    };

    memcpy( ( xCborData->pxBufferStart ), ucInitial, sizeof( ucInitial ) );
    xCborData->pxCursor = &( xCborData->pxBufferStart )[ 3 ];
    xCborData->pxMapEnd = &( xCborData->pxBufferStart )[ 10 ];
    cbor_ssize_t xNewSize = 3;
    CBOR_ValueResize( xCborData, xNewSize );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, ( xCborData->pxBufferStart ), sizeof( ucExpected ) );
}

TEST( aws_cbor_mem, ValueResize_makes_no_change_if_data_size_unknown )
{
    uint8_t ucExpected[] =
    {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 1  */
        0x31, /* 2 '1'               */
        0x20, /* 3  unkown data type */
        0x23, /* 4                   */
        0x61, /* 5  Key of length 1  */
        0x32, /* 6 '2'               */
        0x19, /* 7  Start 16-bit int */
        0x73, /* 8                   */
        0x92, /* 9                   */
        0xFF, /* 10 End of map       */
    };

    memcpy( ( xCborData->pxBufferStart ), ucExpected, sizeof( ucExpected ) );
    CBOR_SetCursor( xCborData, 3 );
    xCborData->pxMapEnd = xCborData->pxBufferStart + 10;
    cbor_ssize_t xNewSize = 2;
    CBOR_ValueResize( xCborData, xNewSize );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, ( xCborData->pxBufferStart ), sizeof( ucExpected ) );
}

TEST( aws_cbor_mem, ValueResize_asserts_on_map_end_errors )
{
    uint8_t ucInitial[] =
    {
        0xBF, /* 0 Open Map        */
        0x61, /* 1 Key of length 1 */
        0x31, /* 2 '1'             */
        0x1A, /* 3 32-bit int      */
        0x23, /* 4                 */
        0x23, /* 5                 */
        0x23, /* 6                 */
        0x23, /* 7                 */
        0xFF, /* 8 End of map      */
    };

    memcpy( ( xCborData->pxBufferStart ), ucInitial, sizeof( ucInitial ) );
    xCborData->pxCursor = &( xCborData->pxBufferStart )[ 3 ];

    /* point map_end to a spot before the end of the map */
    xCborData->pxMapEnd = &( xCborData->pxBufferStart )[ 4 ];
    cbor_ssize_t xNewSize = 1;
    xTEST_expect_assert( CBOR_ValueResize( xCborData, xNewSize ) );
}
TEST( aws_cbor_mem, null_checks )
{
    xTEST_expect_assert( CBOR_AssignAndIncrementCursor( NULL, 0 ) );
    xTEST_expect_assert( CBOR_AssignAndDecrementCursor( NULL, 0 ) );
    xTEST_expect_assert( CBOR_MemCopy( NULL, "hello", 0 ) );
    xTEST_expect_assert( CBOR_MemCopy( xCborData, NULL, 0 ) );
    xTEST_expect_assert( xCborDataItemSize( NULL ) );
    xTEST_expect_assert( xCborDataItemSizePtr( NULL ) );
    xTEST_expect_assert( CBOR_ValueResize( NULL, 0 ) );
}
