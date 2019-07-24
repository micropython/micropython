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

CBORHandle_t xCborData;

TEST_GROUP( aws_cbor_map );

TEST_SETUP( aws_cbor_map )
{
    xCborData = CBOR_New( 0 );
}

TEST_TEAR_DOWN( aws_cbor_map )
{
    CBOR_Delete( &xCborData );
}

TEST_GROUP_RUNNER( aws_cbor_map )
{
    RUN_TEST_CASE( aws_cbor_map, AssignKey_writes_byte_to_key );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_update_key_with_larger_int );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_update_key_with_smaller_int );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_will_return_err_if_not_map_or_empty );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_with_string );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_with_map );

    RUN_TEST_CASE( aws_cbor_map, FindKey_read_string_at_1_char_key );
    RUN_TEST_CASE( aws_cbor_map, FindKey_read_int_at_longer_key );
    RUN_TEST_CASE( aws_cbor_map, FindKey_returns_true_when_key_is_found );
    RUN_TEST_CASE( aws_cbor_map, FindKey_returns_false_when_key_not_found );

    RUN_TEST_CASE( aws_cbor_map, WriteMap );

    RUN_TEST_CASE( aws_cbor_map, ReadMap_returns_cbor_object_with_map_at_cursor );
    RUN_TEST_CASE( aws_cbor_map, ReadMap_returned_map_has_correct_map_end );
    RUN_TEST_CASE( aws_cbor_map, ReadMap_sets_err_when_wrong_type );
    RUN_TEST_CASE( aws_cbor_map, ReadMap_sets_err_when_new_fails );

    RUN_TEST_CASE( aws_cbor_map, AppendKey );

    RUN_TEST_CASE( aws_cbor_map, null_checks );
}

TEST( aws_cbor_map, AssignKey_writes_byte_to_key )
{
    uint8_t ucExpected[] =
    {
        0xBF, /* 0  Open Map         */
        0x66, /* 1  Key of length 6  */
        'a',  /* 2                   */
        'n',  /* 3                   */
        's',  /* 4                   */
        'w',  /* 5                   */
        'e',  /* 6                   */
        'r',  /* 7                   */
        0x18, /* 8  Start 8-bit int  */
        42,   /* 9                   */
        0xFF, /* 10 End of Map       */
    };

    int lMyAnswer = 42;

    CBOR_AssignKey( xCborData, "answer", CBOR_WriteInt, &lMyAnswer );
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, xCborData->pxBufferStart, sizeof( ucExpected ) );
}

TEST( aws_cbor_map, AssignKey_update_key_with_larger_int )
{
    CBOR_AssignKey( xCborData, "1", CBOR_WriteInt, &( int ) { 0x23 } );
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );

    CBOR_AssignKey( xCborData, "2", CBOR_WriteInt, &( int ) { 0x7392 } );
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );

    /* Note, this test assert is here for documentation.  It demonstrates the
     * state of the CBOR buffer before re-writing the value for key '1'. */
    uint8_t ucExpected[] =
    {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 6  */
        0x31, /* 2 '1'               */
        0x18, /* 3  Start 8-bit int  */
        0x23, /* 4                   */
        0x61, /* 5  Key of length 6  */
        0x32, /* 6 '2'               */
        0x19, /* 7  Start 16-bit int */
        0x73, /* 8                   */
        0x92, /* 9                   */
        0xFF, /* 10 End of Map       */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, xCborData->pxBufferStart, sizeof( ucExpected ) );

    CBOR_AssignKey( xCborData, "1", CBOR_WriteInt, &( int ) { 0x1234 } );
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );

    uint8_t ucExpectedRewrite[] =
    {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 6  */
        0x31, /* 2 '1'               */
        0x19, /* 3  Start 16-bit int */
        0x12, /* 4                   */
        0x34, /* 5                   */
        0x61, /* 6  Key of length 6  */
        0x32, /* 7 '2'               */
        0x19, /* 8  Start 16-bit int */
        0x73, /* 9                   */
        0x92, /* 10                  */
        0xFF, /* 11 End of Map       */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpectedRewrite,
        xCborData->pxBufferStart,
        sizeof( ucExpectedRewrite ) );
}

TEST( aws_cbor_map, AssignKey_update_key_with_smaller_int )
{
    CBOR_AssignKey( xCborData, "1", CBOR_WriteInt, &( int ) { 0x5CA1AB1E } );
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );

    CBOR_AssignKey( xCborData, "2", CBOR_WriteInt, &( int ) { 0xCAFE } );
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );

    CBOR_AssignKey( xCborData, "1", CBOR_WriteInt, &( int ) { 0xED } );
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );

    uint8_t ucExpectedRewrite[] =
    {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 6  */
        0x31, /* 2 '1'               */
        0x18, /* 3  Start 8-bit int  */
        0xED, /* 4                   */
        0x61, /* 5  Key of length 6  */
        0x32, /* 6 '2'               */
        0x19, /* 7  Start 16-bit int */
        0xCA, /* 8                   */
        0xFE, /* 9                   */
        0xFF, /* 10 End of Map       */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpectedRewrite, xCborData->pxBufferStart, sizeof( ucExpectedRewrite ) );
}

TEST( aws_cbor_map, AssignKey_will_return_err_if_not_map_or_empty )
{
    *( xCborData->pxBufferStart ) = 2;
    CBOR_AssignKey( xCborData, "1", CBOR_WriteInt, &( int ) { 0x5CA1AB1E } );
    TEST_ASSERT_EQUAL( eCborErrUnsupportedReadOperation, xCborData->xError );
}

TEST( aws_cbor_map, AssignKey_with_string )
{
    CBOR_AssignKey( xCborData, "1", CBOR_WriteString, "Hi" );

    uint8_t ucExpectedRewrite[] =
    {
        0xBF, /* 0  Open Map           */
        0x61, /* 1  Key of length 1    */
        0x31, /* 2 '1'                 */
        0x62, /* 3  String of length 2 */
        'H',  /* 4                     */
        'i',  /* 5                     */
        0xFF, /* 6  End of Map         */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpectedRewrite, xCborData->pxBufferStart, sizeof( ucExpectedRewrite ) );
}

TEST( aws_cbor_map, AssignKey_with_map )
{
    uint8_t ucMapBuffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of Map         */
    };

    CBORHandle_t xMap = CBOR_New( 0 );

    CBOR_MemCopy( xMap, ucMapBuffer, sizeof( ucMapBuffer ) );
    CBOR_AssignKey( xCborData, "1", CBOR_WriteMap, xMap );
    CBOR_Delete( &xMap );

    uint8_t ucExpectedRewrite[] =
    {
        0xBF, /* 0  Open Map        */
        0x61, /* 1  Key of length 1 */
        0x31, /* 2' 1'              */
        0xBF, /* 3  Open Map        */
        0x63, /* 4  Key of length 3 */
        'k',  /* 5                  */
        'e',  /* 6                  */
        'y',  /* 7                  */
        0x18, /* 8  1 byte int      */
        78,   /* 9                  */
        0xFF, /* 10 End of Map      */
        0xFF, /* 11 End of Map      */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpectedRewrite, xCborData->pxBufferStart, sizeof( ucExpectedRewrite ) );
}

TEST( aws_cbor_map, FindKey_read_string_at_1_char_key )
{
    uint8_t ucBuffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x61, /* 1  Key of length 1    */
        '1',  /* 2                     */
        0x62, /* 3  String of length 2 */
        'H',  /* 4                     */
        'i',  /* 5                     */
        0xFF, /* 6  End of Map         */
    };

    CBOR_SetCursor( xCborData, 0 );
    CBOR_MemCopy( xCborData, ucBuffer, sizeof( ucBuffer ) );
    CBOR_FindKey( xCborData, "1" );
    char * pcStr = CBOR_ReadString( xCborData );
    TEST_ASSERT_EQUAL_STRING( "Hi", pcStr );
    free( pcStr );
}

TEST( aws_cbor_map, FindKey_read_int_at_longer_key )
{
    uint8_t ucBuffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of Map         */
    };

    CBOR_SetCursor( xCborData, 0 );
    CBOR_MemCopy( xCborData, ucBuffer, sizeof( ucBuffer ) );
    CBOR_FindKey( xCborData, "key" );
    int lResult = CBOR_ReadInt( xCborData );
    TEST_ASSERT_EQUAL_INT( 78, lResult );
}

TEST( aws_cbor_map, FindKey_returns_true_when_key_is_found )
{
    uint8_t ucBuffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of Map         */
    };

    CBOR_SetCursor( xCborData, 0 );
    CBOR_MemCopy( xCborData, ucBuffer, sizeof( ucBuffer ) );
    bool xResult = CBOR_FindKey( xCborData, "key" );
    TEST_ASSERT_TRUE( xResult );
}

TEST( aws_cbor_map, FindKey_returns_false_when_key_not_found )
{
    uint8_t ucBuffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of Map         */
    };

    CBOR_SetCursor( xCborData, 0 );
    CBOR_MemCopy( xCborData, ucBuffer, sizeof( ucBuffer ) );
    xCborData->pxMapEnd = xCborData->pxCursor - 1;
    bool xResult = CBOR_FindKey( xCborData, "lock" );
    TEST_ASSERT_FALSE( xResult );
}

TEST( aws_cbor_map, WriteMap )
{
    uint8_t ucMapBuffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of Map         */
    };
    CBORHandle_t xMap = CBOR_New( 0 );

    CBOR_MemCopy( xMap, ucMapBuffer, sizeof( ucMapBuffer ) );
    CBOR_WriteMap( xCborData, xMap );
    CBOR_Delete( &xMap );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucMapBuffer, xCborData->pxBufferStart, sizeof( ucMapBuffer ) );
}

TEST( aws_cbor_map, ReadMap_returns_cbor_object_with_map_at_cursor )
{
    uint8_t ucFullBuffer[] =
    {
        0xBF, /* 0  Open Map          */
        0x61, /* 1  Key of length 1   */
        'z',  /* 2                    */
        0xBF, /* 3  Open Map          */
        0x61, /* 4  Key of length 1   */
        'z',  /* 5                    */
        0x18, /* 6  1 byte int        */
        87,   /* 7                    */
        0xFF, /* 8  End of Map        */
        0x18, /* 9  1 byte int        */
        78,   /* 10                   */
        0xFF, /* 11 End of Map        */
    };

    CBOR_MemCopy( xCborData, ucFullBuffer, sizeof( ucFullBuffer ) );
    xCborData->pxMapEnd = xCborData->pxBufferStart + sizeof( ucFullBuffer );
    CBOR_FindKey( xCborData, "z" );
    CBORHandle_t xMap = CBOR_ReadMap( xCborData );

    CBOR_FindKey( xMap, "z" );
    int lResult = CBOR_ReadInt( xMap );
    TEST_ASSERT_EQUAL( 87, lResult );

    CBOR_Delete( &xMap );
}

TEST( aws_cbor_map, ReadMap_returned_map_has_correct_map_end )
{
    uint8_t ucFullBuffer[] =
    {
        0xBF, /* 0  Open Map          */
        0x61, /* 1  Key of length 1   */
        'z',  /* 2                    */
        0xBF, /* 3  Open Map          */
        0x61, /* 4  Key of length 1   */
        'y',  /* 5                    */
        22,   /* 6  small int (22)    */
        0xFF, /* 7  End of Map        */
        0x18, /* 8  1 byte int        */
        78,   /* 9                    */
        0xFF, /* 10 End of Map        */
    };

    CBOR_MemCopy( xCborData, ucFullBuffer, sizeof( ucFullBuffer ) );
    xCborData->pxMapEnd = xCborData->pxBufferStart + sizeof( ucFullBuffer );
    CBOR_FindKey( xCborData, "z" );
    CBORHandle_t xMap = CBOR_ReadMap( xCborData );

    int lExpected = 4;
    int lActual = xMap->pxMapEnd - xMap->pxBufferStart;
    TEST_ASSERT_EQUAL( lExpected, lActual );

    CBOR_Delete( &xMap );
}

TEST( aws_cbor_map, ReadMap_sets_err_when_wrong_type )
{
    CBOR_AssignKeyWithInt( xCborData, "1", 834 );
    CBOR_FindKey( xCborData, "1" );
    ( void ) CBOR_ReadMap( xCborData );
    TEST_ASSERT_EQUAL( eCborErrReadTypeMismatch, xCborData->xError );
}

TEST( aws_cbor_map, ReadMap_sets_err_when_new_fails )
{
    uint8_t ucFullBuffer[] =
    {
        0xBF, /* 0  Open Map          */
        0x61, /* 1  Key of length 1   */
        'z',  /* 2                    */
        0xBF, /* 3  Open Map          */
        0x61, /* 4  Key of length 1   */
        'z',  /* 5                    */
        0x18, /* 6  1 byte int        */
        87,   /* 7                    */
        0xFF, /* 8  End of Map        */
        0x18, /* 9  1 byte int        */
        78,   /* 10                   */
        0xFF, /* 11 End of Map        */
    };

    CBOR_MemCopy( xCborData, ucFullBuffer, sizeof( ucFullBuffer ) );
    xCborData->pxMapEnd = xCborData->pxBufferStart + sizeof( ucFullBuffer );

    UnityMalloc_MakeMallocFailAfterCount( 0 );
    CBOR_FindKey( xCborData, "z" );
    CBORHandle_t xMap = CBOR_ReadMap( xCborData );
    TEST_ASSERT_NULL( xMap );
    TEST_ASSERT_EQUAL( eCborErrInsufficentSpace, xCborData->xError );
}

TEST( aws_cbor_map, AppendKey )
{
    uint8_t ucInit[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of Map         */
    };

    CBOR_MemCopy( xCborData, ucInit, sizeof( ucInit ) );
    CBOR_SetCursor( xCborData, 7 );
    xCborData->pxMapEnd = xCborData->pxCursor;

    CBOR_AppendKey( xCborData, "lock", CBOR_WriteInt, &( int ) { 0x0347 } );

    uint8_t ucExpected[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0x64, /* 7  Key of length 4    */
        'l',  /* 8                     */
        'o',  /* 9                     */
        'c',  /* 10                    */
        'k',  /* 11                    */
        0x19, /* 12 2 byte int follows */
        0x03, /* 13                    */
        0x47, /* 14                    */
        0xFF, /* 15 End of Map         */
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, xCborData->pxBufferStart, sizeof( ucExpected ) );
}

TEST( aws_cbor_map, null_checks )
{
    xTEST_expect_assert( CBOR_OpenMap( NULL ) );

    xTEST_expect_assert( CBOR_CloseMap( NULL ) );

    xTEST_expect_assert( CBOR_KeyIsMatch( NULL, "key" ) );
    xTEST_expect_assert( CBOR_KeyIsMatch( xCborData, NULL ) );

    xTEST_expect_assert( CBOR_SearchForKey( NULL, "key" ) );
    xTEST_expect_assert( CBOR_SearchForKey( xCborData, NULL ) );

    xTEST_expect_assert( CBOR_AppendKey( NULL, "1", CBOR_WriteString, "a" ) );
    xTEST_expect_assert( CBOR_AppendKey( xCborData, NULL, CBOR_WriteString, "a" ) );
    xTEST_expect_assert( CBOR_AppendKey( xCborData, "1", NULL, "a" ) );
    xTEST_expect_assert( CBOR_AppendKey( xCborData, "1", CBOR_WriteString, NULL ) );

    xTEST_expect_assert( CBOR_AssignKey( NULL, "1", CBOR_WriteString, "a" ) );
    xTEST_expect_assert( CBOR_AssignKey( xCborData, NULL, CBOR_WriteString, "a" ) );
    xTEST_expect_assert( CBOR_AssignKey( xCborData, "1", NULL, "a" ) );
    xTEST_expect_assert( CBOR_AssignKey( xCborData, "1", CBOR_WriteString, NULL ) );

    xTEST_expect_assert( CBOR_MapSize( NULL ) );

    xTEST_expect_assert( CBOR_WriteMap( NULL, xCborData ) );
    xTEST_expect_assert( CBOR_WriteMap( xCborData, NULL ) );

    xTEST_expect_assert( CBOR_ReadMap( NULL ) );
}
