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
#include "aws_cbor_internals.h"
#include "unity_fixture.h"
#include <assert.h>
#include <string.h>

CBORHandle_t xCborData;

TEST_GROUP( aws_cbor );
TEST_SETUP( aws_cbor )
{
    xCborData = CBOR_New( 0 );
}

TEST_TEAR_DOWN( aws_cbor )
{
    CBOR_Delete( &xCborData );
}

TEST_GROUP_RUNNER( aws_cbor )
{
    RUN_TEST_CASE( aws_cbor, New_returns_not_null );
    RUN_TEST_CASE( aws_cbor, New_returns_null_when_malloc_fails_for_struct );
    RUN_TEST_CASE( aws_cbor, New_returns_null_when_malloc_fails_for_buffer );
    RUN_TEST_CASE( aws_cbor, New_allocates_minimum_space_for_cbor_buffer );
    RUN_TEST_CASE( aws_cbor, New_allocates_specified_space_for_cbor_buffer );
    RUN_TEST_CASE( aws_cbor, New_initializes_buffer_with_empty_map );
    RUN_TEST_CASE( aws_cbor, Delete_frees_all_allocated_space );
    RUN_TEST_CASE( aws_cbor, GetRawBuffer_returns_pointer_to_cbor_buffer );
    RUN_TEST_CASE( aws_cbor, GetBufferSize_returns_size_of_empty_map_in_bytes );
    RUN_TEST_CASE( aws_cbor, GetBuffer_returns_size_of_map_in_bytes );
    RUN_TEST_CASE( aws_cbor, ClearError_sets_err_to_CBOR_ERR_NO_ERROR );
    RUN_TEST_CASE( aws_cbor, AppendMap );
}

TEST( aws_cbor, New_returns_not_null )
{
    CBORHandle_t xTestData = CBOR_New( 0 );

    TEST_ASSERT_NOT_NULL( xTestData );
    CBOR_Delete( &xTestData );
}

TEST( aws_cbor, New_returns_null_when_malloc_fails_for_struct )
{
    UnityMalloc_MakeMallocFailAfterCount( 0 );
    CBORHandle_t xNullBuffer = CBOR_New( 0 );
    TEST_ASSERT_NULL( xNullBuffer );
}

TEST( aws_cbor, New_returns_null_when_malloc_fails_for_buffer )
{
    UnityMalloc_MakeMallocFailAfterCount( 1 );
    CBORHandle_t xNullBuffer = CBOR_New( 0 );
    TEST_ASSERT_NULL( xNullBuffer );
}

TEST( aws_cbor, New_allocates_minimum_space_for_cbor_buffer )
{
    CBORHandle_t xTestData = CBOR_New( 0 );

    TEST_ASSERT_NOT_NULL( xTestData->pxBufferStart );
    cbor_ssize_t xSize = xTestData->pxBufferEnd - xTestData->pxBufferStart + 1;
    TEST_ASSERT_EQUAL( 32, xSize );
    CBOR_Delete( &xTestData );
}

TEST( aws_cbor, New_allocates_specified_space_for_cbor_buffer )
{
    CBORHandle_t xTestData = CBOR_New( 3845 );

    TEST_ASSERT_NOT_NULL( xTestData->pxBufferStart );
    cbor_ssize_t xSize = xTestData->pxBufferEnd - xTestData->pxBufferStart + 1;
    TEST_ASSERT_EQUAL( 3845, xSize );
    CBOR_Delete( &xTestData );
}

TEST( aws_cbor, New_initializes_buffer_with_empty_map )
{
    CBORHandle_t xTestData = CBOR_New( 0 );
    cbor_byte_t xEmptyMap[] = { CBOR_MAP_OPEN, CBOR_BREAK };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        xEmptyMap, xTestData->pxBufferStart, sizeof( xEmptyMap ) );
    CBOR_Delete( &xTestData );
}

TEST( aws_cbor, Delete_frees_all_allocated_space )
{
    CBORHandle_t xTestData = CBOR_New( 0 );

    CBOR_Delete( &xTestData );
    TEST_ASSERT_NULL( xTestData );
}

TEST( aws_cbor, GetRawBuffer_returns_pointer_to_cbor_buffer )
{
    cbor_byte_t const * const pxActualPtr = CBOR_GetRawBuffer( xCborData );
    cbor_byte_t * pxExpectedPtr = xCborData->pxBufferStart;

    TEST_ASSERT_EQUAL_PTR( pxExpectedPtr, pxActualPtr );
}

TEST( aws_cbor, GetBufferSize_returns_size_of_empty_map_in_bytes )
{
    cbor_ssize_t xExpectedSize = 2;
    cbor_ssize_t xActualSize = CBOR_GetBufferSize( xCborData );

    TEST_ASSERT_EQUAL( xExpectedSize, xActualSize );
}

TEST( aws_cbor, GetBuffer_returns_size_of_map_in_bytes )
{
    CBOR_AppendKeyWithInt( xCborData, "answer", 42 );
    cbor_ssize_t xExpectedSize = 11;
    cbor_ssize_t xActualSize = CBOR_GetBufferSize( xCborData );
    TEST_ASSERT_EQUAL( xExpectedSize, xActualSize );
}

TEST( aws_cbor, ClearError_sets_err_to_CBOR_ERR_NO_ERROR )
{
    xCborData->xError = eCborErrDefaultError;
    CBOR_ClearError( xCborData );
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );
}

TEST( aws_cbor, AppendMap )
{
    CBOR_AppendKeyWithInt( xCborData, "answer", 42 );

    CBORHandle_t xSrcData = CBOR_New( 0 );
    CBOR_AppendKeyWithString( xSrcData, "question", "unknown" );

    CBOR_AppendMap( xCborData, xSrcData );
    CBOR_Delete( &xSrcData );

    bool xAnswerFound = CBOR_FindKey( xCborData, "answer" );
    bool xQuestionFound = CBOR_FindKey( xCborData, "question" );

    /* Both keys should be in the first map */
    TEST_ASSERT_TRUE( xAnswerFound );
    TEST_ASSERT_TRUE( xQuestionFound );
}
