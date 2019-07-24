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

extern int lTEST_assert_fails;
CBORHandle_t xCborData;

TEST_GROUP( aws_cbor_string );
TEST_SETUP( aws_cbor_string )
{
    lTEST_assert_fails = 0;
    xCborData = CBOR_New( 0 );
    char cEmptyMap[ 20 ] = { '{', '}' };
    CBOR_MemCopy( xCborData, cEmptyMap, 20 );
    CBOR_SetCursor( xCborData, 0 );
}

TEST_TEAR_DOWN( aws_cbor_string )
{
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );
    TEST_ASSERT_EQUAL( 0, lTEST_assert_fails );
    CBOR_Delete( &xCborData );
}

TEST_GROUP_RUNNER( aws_cbor_string )
{
    RUN_TEST_CASE( aws_cbor_string, WriteString_small_string );
    RUN_TEST_CASE(
        aws_cbor_string, WriteString_cursor_points_to_next_space_after_string );
    RUN_TEST_CASE( aws_cbor_string, WriteString_int8_sized_string );
    RUN_TEST_CASE( aws_cbor_string, WriteString_int16_sized_string );
    RUN_TEST_CASE( aws_cbor_string, WriteString_larger_than_supported_string );
    RUN_TEST_CASE( aws_cbor_string, WriteString_will_resize_data );
    RUN_TEST_CASE( aws_cbor_string, ReadString_read_short_string );
    RUN_TEST_CASE( aws_cbor_string, ReadString_read_int8_sized_string );
    RUN_TEST_CASE( aws_cbor_string, ReadString_read_int16_sized_string );
    RUN_TEST_CASE( aws_cbor_string, StringLength_Gets_xStringLength );
    RUN_TEST_CASE(
        aws_cbor_string, ReadString_gets_same_result_when_called_repeatedly );
    RUN_TEST_CASE(
        aws_cbor_string, StringLength_gets_same_result_when_called_repeatedly );
    RUN_TEST_CASE( aws_cbor_string, StringCompare_returns_0_when_strings_match );
    RUN_TEST_CASE( aws_cbor_string,
                   StringCompare_matches_strcmp_when_first_string_is_greater );
    RUN_TEST_CASE( aws_cbor_string,
                   StringCompare_matches_strcmp_when_first_string_is_less );
    RUN_TEST_CASE( aws_cbor_string, StringCompare_both_strings_empty );
    RUN_TEST_CASE( aws_cbor_string,
                   StringCompare_matches_strcmp_when_second_string_is_empty );
    RUN_TEST_CASE( aws_cbor_string, StringCompare_first_string_is_shorter );
    RUN_TEST_CASE( aws_cbor_string, StringCompare_second_string_is_shorter );

    RUN_TEST_CASE( aws_cbor_string, null_checks );
}

TEST( aws_cbor_string, WriteString_small_string )
{
    cbor_byte_t xExpected[] =
    {
        0x6A, /* 0 String, length 10 */
        'h',  /* 1                   */
        'i',  /* 2                   */
        ',',  /* 3                   */
        ' ',  /* 4                   */
        'w',  /* 5                   */
        '3',  /* 6                   */
        'r',  /* 7                   */
        'l',  /* 8                   */
        'd',  /* 9                   */
        '!',  /* 10                  */
    };

    CBOR_WriteString( xCborData, "hi, w3rld!" );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        xExpected, xCborData->pxBufferStart, sizeof( xExpected ) );
}

TEST( aws_cbor_string, WriteString_cursor_points_to_next_space_after_string )
{
    CBOR_WriteString( xCborData, "a" );
    TEST_ASSERT_EQUAL_PTR( &( xCborData->pxBufferStart )[ 2 ], xCborData->pxCursor );
}

TEST( aws_cbor_string, WriteString_int8_sized_string )
{
    char * pcTestString =
        "I need this string to be more than 23 characters, but "
        "less than 256 characters.  This will result in "
        "having a single byte store the length of the string.";
    int lTestLength = strlen( pcTestString );

    cbor_byte_t xExpected[ lTestLength + 2 ];

    xExpected[ 0 ] = 0x78; /* String with the length given in the next 1 byte */
    xExpected[ 1 ] = 153;  /* Length of the string*/
    memcpy( &xExpected[ 2 ], pcTestString, lTestLength );

    CBOR_WriteString( xCborData, pcTestString );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        xExpected, xCborData->pxBufferStart, sizeof( xExpected ) );
}

TEST( aws_cbor_string, WriteString_int16_sized_string )
{
    char * pcTestString =
        "For this test, I need a string that is greater than 256 characters.  "
        "This will require the length to be stored in 2 bytes.  This will also "
        "enable the storage of strings up to 64KB (~65kB) in length. Lorem "
        "ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod "
        "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
        "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        "ea commodo consequat.";
    int lTestLength = strlen( pcTestString );

    cbor_byte_t xExpected[ lTestLength + 3 ];

    xExpected[ 0 ] = 0x79; /* String with the length given in the next 2 bytes */
    /* Length of string is 431, or 0x01AF */
    xExpected[ 1 ] = 1;
    xExpected[ 2 ] = 0xAF;
    memcpy( &xExpected[ 3 ], pcTestString, lTestLength );

    CBOR_WriteString( xCborData, pcTestString );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        xExpected, xCborData->pxBufferStart, sizeof( xExpected ) );
}

TEST( aws_cbor_string, WriteString_larger_than_supported_string )
{
    cbor_ssize_t xStringLength = 1U << 16U;
    cbor_ssize_t xBufferLength = xStringLength + 1;
    char cStr[ xBufferLength ];

    for( int i = 0; i < xStringLength; i++ )
    {
        cStr[ i ] = 'a';
    }

    cStr[ xStringLength ] = 0;

    CBOR_WriteString( xCborData, cStr );
    TEST_ASSERT_EQUAL( eCborErrUnsupportedWriteOperation, xCborData->xError );
    xCborData->xError = 0;
}

TEST( aws_cbor_string, WriteString_will_resize_data )
{
    xCborData->pxMapEnd = xCborData->pxBufferStart + 10;
    CBOR_WriteString( xCborData, "hi" );
    CBOR_WriteString( xCborData, "world" );
    xCborData->pxCursor = xCborData->pxBufferStart;
    CBOR_WriteString( xCborData, "hello" );
    cbor_byte_t xExpected[] =
    {
        0x65, /* 0 String, length 5  */
        'h',  /* 1                   */
        'e',  /* 2                   */
        'l',  /* 3                   */
        'l',  /* 4                   */
        'o',  /* 5                   */
        0x65, /* 6 String, length 5  */
        'w',  /* 7                   */
        'o',  /* 8                   */
        'r',  /* 9                   */
        'l',  /* 10                  */
        'd',  /* 11                  */
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        xExpected, xCborData->pxBufferStart, sizeof( xExpected ) );
}

TEST( aws_cbor_string, ReadString_read_short_string )
{
    char cExpected[] = "A short string";

    CBOR_WriteString( xCborData, cExpected );
    xCborData->pxCursor = xCborData->pxBufferStart;

    char * pcStr = CBOR_ReadString( xCborData );

    TEST_ASSERT_EQUAL_STRING( cExpected, pcStr );
    free( pcStr );
}

TEST( aws_cbor_string, ReadString_read_int8_sized_string )
{
    char cExpected[] = "Lorem ipsum dolor sit amet, consectetur adipisicing "
                       "elit, sed do eiusmod tempor incididunt ut labore et "
                       "dolore magna aliqua.";

    CBOR_WriteString( xCborData, cExpected );
    xCborData->pxCursor = xCborData->pxBufferStart;

    char * pcStr = CBOR_ReadString( xCborData );

    TEST_ASSERT_EQUAL_STRING( cExpected, pcStr );
    free( pcStr );
}

TEST( aws_cbor_string, ReadString_read_int16_sized_string )
{
    char cExpected[] =
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris "
        "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";

    CBOR_WriteString( xCborData, cExpected );
    xCborData->pxCursor = xCborData->pxBufferStart;

    char * pcStr = CBOR_ReadString( xCborData );

    TEST_ASSERT_EQUAL_STRING( cExpected, pcStr );
    free( pcStr );
}

TEST( aws_cbor_string, ReadString_gets_same_result_when_called_repeatedly )
{
    char cStr[] = "Duis aute irure dolor in reprehenderit in voluptate velit "
                  "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "
                  "occaecat cupidatat non proident, sunt in culpa qui officia "
                  "deserunt mollit anim id est laborum.";

    CBOR_WriteString( xCborData, cStr );
    xCborData->pxCursor = xCborData->pxBufferStart;

    char * pcExpected = CBOR_ReadString( xCborData );
    char * pcResult = CBOR_ReadString( xCborData );

    TEST_ASSERT_EQUAL_STRING( pcExpected, pcResult );
    free( pcExpected );
    free( pcResult );
}

TEST( aws_cbor_string, StringLength_Gets_xStringLength )
{
    char cStr[] = "This string is 28 chars long";
    cbor_ssize_t xExpected = strlen( cStr );

    CBOR_WriteString( xCborData, cStr );
    xCborData->pxCursor = xCborData->pxBufferStart;
    cbor_ssize_t xResult = CBOR_StringLength( xCborData );
    TEST_ASSERT_EQUAL_INT( xExpected, xResult );
}

TEST( aws_cbor_string, StringLength_gets_same_result_when_called_repeatedly )
{
    char cStr[] = "This string is 29 chars long.";

    CBOR_WriteString( xCborData, cStr );
    xCborData->pxCursor = xCborData->pxBufferStart;
    cbor_ssize_t xExpected = CBOR_StringLength( xCborData );
    cbor_ssize_t xResult = CBOR_StringLength( xCborData );
    TEST_ASSERT_EQUAL_INT( xExpected, xResult );
}

TEST( aws_cbor_string, StringCompare_returns_0_when_strings_match )
{
    char * pcStr1 = "Matching Strings";
    char * pcStr2 = "Matching Strings";

    CBOR_WriteString( xCborData, pcStr1 );
    xCborData->pxCursor = xCborData->pxBufferStart;

    TEST_ASSERT_EQUAL_INT( 0, CBOR_StringCompare( xCborData, pcStr2 ) );
}

TEST( aws_cbor_string, StringCompare_matches_strcmp_when_first_string_is_greater )
{
    char * pcStr1 = "Mismatching Strings B <-- that's different";
    char * pcStr2 = "Mismatching Strings a <-- that's different";

    CBOR_WriteString( xCborData, pcStr1 );
    xCborData->pxCursor = xCborData->pxBufferStart;

    cbor_ssize_t xActual = CBOR_StringCompare( xCborData, pcStr2 );
    TEST_ASSERT_LESS_THAN( 0, xActual );
}

TEST( aws_cbor_string, StringCompare_matches_strcmp_when_first_string_is_less )
{
    char * pcStr1 = "Mismatching Strings a <-- that's different";
    char * pcStr2 = "Mismatching Strings B <-- that's different";

    CBOR_WriteString( xCborData, pcStr1 );
    xCborData->pxCursor = xCborData->pxBufferStart;

    cbor_ssize_t xActual = CBOR_StringCompare( xCborData, pcStr2 );
    TEST_ASSERT_GREATER_THAN( 0, xActual );
}

TEST( aws_cbor_string, StringCompare_both_strings_empty )
{
    char * pcStr1 = "";
    char * pcStr2 = "";

    CBOR_WriteString( xCborData, pcStr1 );
    xCborData->pxCursor = xCborData->pxBufferStart;
    cbor_ssize_t xActual = CBOR_StringCompare( xCborData, pcStr2 );
    TEST_ASSERT_EQUAL_INT( 0, xActual );
}

TEST( aws_cbor_string, StringCompare_matches_strcmp_when_second_string_is_empty )
{
    char * pcStr1 = "str1 is not empty";
    char * pcStr2 = "";

    CBOR_WriteString( xCborData, pcStr1 );
    xCborData->pxCursor = xCborData->pxBufferStart;
    cbor_ssize_t xActual = CBOR_StringCompare( xCborData, pcStr2 );
    TEST_ASSERT_GREATER_THAN( 0, xActual );
}

TEST( aws_cbor_string, StringCompare_first_string_is_shorter )
{
    char * pcStr1 = "str1 is shorter";
    char * pcStr2 = "str1 is shorter, and pcStr2 is longer";

    CBOR_WriteString( xCborData, pcStr1 );
    xCborData->pxCursor = xCborData->pxBufferStart;

    cbor_ssize_t xActual = CBOR_StringCompare( xCborData, pcStr2 );
    TEST_ASSERT_LESS_THAN( 0, xActual );
}

TEST( aws_cbor_string, StringCompare_second_string_is_shorter )
{
    char * pcStr1 = "str2 is shorter, and str1 is longer";
    char * pcStr2 = "str2 is shorter";

    CBOR_WriteString( xCborData, pcStr1 );
    xCborData->pxCursor = xCborData->pxBufferStart;

    cbor_ssize_t xActual = CBOR_StringCompare( xCborData, pcStr2 );
    TEST_ASSERT_GREATER_THAN( 0, xActual );
}

TEST( aws_cbor_string, null_checks )
{
    xTEST_expect_assert( CBOR_WriteString( NULL, "hello" ) );
    xTEST_expect_assert( CBOR_WriteString( xCborData, NULL ) );
    xTEST_expect_assert( CBOR_ReadString( NULL ) );
    xTEST_expect_assert( CBOR_StringSize( NULL ) );
    xTEST_expect_assert( CBOR_StringLength( NULL ) );
    xTEST_expect_assert( CBOR_StringCompare( NULL, "hello" ) );
    xTEST_expect_assert( CBOR_StringCompare( xCborData, NULL ) );
}
