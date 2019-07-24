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

static CBORHandle_t xCborData;

extern int lTEST_assert_fails;

TEST_GROUP( aws_cbor_int );
TEST_SETUP( aws_cbor_int )
{
    lTEST_assert_fails = 0;
    xCborData = CBOR_New( 0 );
    uint8_t ucInit[ 32 ] = { 0 };

    CBOR_MemCopy( xCborData, ucInit, sizeof( ucInit ) );
    CBOR_SetCursor( xCborData, 0 );
}

TEST_TEAR_DOWN( aws_cbor_int )
{
    TEST_ASSERT_EQUAL( eCborErrNoError, xCborData->xError );
    TEST_ASSERT_EQUAL( 0, lTEST_assert_fails );
    CBOR_Delete( &xCborData );
}

TEST_GROUP_RUNNER( aws_cbor_int )
{
    RUN_TEST_CASE( aws_cbor_int, IntSize_SmallInt );
    RUN_TEST_CASE( aws_cbor_int, IntSize_Int8 );
    RUN_TEST_CASE( aws_cbor_int, IntSize_Int16 );
    RUN_TEST_CASE( aws_cbor_int, IntSize_Int32 );

    RUN_TEST_CASE( aws_cbor_int, WriteSmallInt );
    RUN_TEST_CASE( aws_cbor_int, WriteInt8 );
    RUN_TEST_CASE( aws_cbor_int, WriteInt16 );
    RUN_TEST_CASE( aws_cbor_int, WriteInt32 );

    RUN_TEST_CASE( aws_cbor_int, ReadSmallPositiveInt );
    RUN_TEST_CASE( aws_cbor_int, ReadPositiveInt8 );
    RUN_TEST_CASE( aws_cbor_int, ReadPositiveInt16 );
    RUN_TEST_CASE( aws_cbor_int, ReadPositiveInt32 );

    RUN_TEST_CASE( aws_cbor_int, WriteInt );

    RUN_TEST_CASE( aws_cbor_int, ReadInt );

    RUN_TEST_CASE( aws_cbor_int, ReadInt_sets_error_if_wrong_type );

    RUN_TEST_CASE( aws_cbor_int, null_checks );
}

TEST( aws_cbor_int, IntSize_SmallInt )
{
    int lNum = 12;

    CBOR_WriteInt( xCborData, &lNum );
    CBOR_SetCursor( xCborData, 0 );
    int lResult = CBOR_IntSize( xCborData->pxCursor );
    TEST_ASSERT_EQUAL( 1, lResult );
}

TEST( aws_cbor_int, IntSize_Int8 )
{
    int lNum = 42;

    CBOR_WriteInt( xCborData, &lNum );
    CBOR_SetCursor( xCborData, 0 );
    int lResult = CBOR_IntSize( xCborData->pxCursor );
    TEST_ASSERT_EQUAL( 2, lResult );
}

TEST( aws_cbor_int, IntSize_Int16 )
{
    int lNum = 1234;

    CBOR_WriteInt( xCborData, &lNum );
    CBOR_SetCursor( xCborData, 0 );
    int lResult = CBOR_IntSize( xCborData->pxCursor );
    TEST_ASSERT_EQUAL( 3, lResult );
}

TEST( aws_cbor_int, IntSize_Int32 )
{
    int lNum = INT32_MAX;

    CBOR_WriteInt( xCborData, &lNum );
    CBOR_SetCursor( xCborData, 0 );
    int lResult = CBOR_IntSize( xCborData->pxCursor );
    TEST_ASSERT_EQUAL( 5, lResult );
}

TEST( aws_cbor_int, WriteSmallInt )
{
    int lExpected = 23;

    CBOR_WriteSmallInt( xCborData, lExpected );
    TEST_ASSERT_EQUAL_INT8( lExpected, *xCborData->pxBufferStart );
}

TEST( aws_cbor_int, WriteInt8 )
{
    CBOR_WriteInt8( xCborData, 0x21 );
    uint8_t ucExpected[] = { CBOR_INT8_FOLLOWS, 0x21 };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, xCborData->pxBufferStart, sizeof( ucExpected ) );
}

TEST( aws_cbor_int, WriteInt16 )
{
    CBOR_WriteInt16( xCborData, 0x1221 );
    uint8_t ucExpected[] = { CBOR_INT16_FOLLOWS, 0x12, 0x21 };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, xCborData->pxBufferStart, sizeof( ucExpected ) );
}

TEST( aws_cbor_int, WriteInt32 )
{
    CBOR_WriteInt32( xCborData, 0xBA5EBA11 );
    uint8_t ucExpected[] = { CBOR_INT32_FOLLOWS, 0xBA, 0x5E, 0xBA, 0x11 };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        ucExpected, xCborData->pxBufferStart, sizeof( ucExpected ) );
}

TEST( aws_cbor_int, ReadSmallPositiveInt )
{
    uint8_t ucInit[] =
    {
        19,
    };

    CBOR_MemCopy( xCborData, ucInit, sizeof( ucInit ) );
    CBOR_SetCursor( xCborData, 0 );
    int lResult = CBOR_ReadSmallPositiveInt( xCborData );
    int lExpected = 19;
    TEST_ASSERT_EQUAL( lExpected, lResult );
}

TEST( aws_cbor_int, ReadPositiveInt8 )
{
    uint8_t ucInit[] =
    {
        CBOR_INT8_FOLLOWS,
        42,
    };

    CBOR_MemCopy( xCborData, ucInit, sizeof( ucInit ) );
    CBOR_SetCursor( xCborData, 0 );
    int lResult = CBOR_ReadPositiveInt8( xCborData );
    int lExpected = 42;
    TEST_ASSERT_EQUAL( lExpected, lResult );
}

TEST( aws_cbor_int, ReadPositiveInt16 )
{
    uint8_t ucInit[] =
    {
        CBOR_INT16_FOLLOWS,
        0x01,
        0x02,
    };

    CBOR_MemCopy( xCborData, ucInit, sizeof( ucInit ) );
    CBOR_SetCursor( xCborData, 0 );
    int lResult = CBOR_ReadPositiveInt16( xCborData );
    int lExpected = 258;
    TEST_ASSERT_EQUAL( lExpected, lResult );
}

TEST( aws_cbor_int, ReadPositiveInt32 )
{
    uint8_t ucInit[] =
    {
        CBOR_INT16_FOLLOWS,
        0x01,
        0x02,
        0x03,
        0x04,
    };

    CBOR_MemCopy( xCborData, ucInit, sizeof( ucInit ) );
    CBOR_SetCursor( xCborData, 0 );
    int lResult = CBOR_ReadPositiveInt32( xCborData );
    int lExpected = 16909060;
    TEST_ASSERT_EQUAL( lExpected, lResult );
}

typedef struct write_test_s
{
    char * pcMessage;
    int lInput;
    uint8_t ucExpected[ 32 ];
} write_test_t;

write_test_t xWriteTests[] =
{
    {
        .pcMessage = "write small int",
        .lInput = 8,
        .ucExpected ={ 8                   },
    },
    {
        .pcMessage = "write 8 bit int",
        .lInput = 56,
        .ucExpected ={ CBOR_INT8_FOLLOWS, 56 },
    },
    {
        .pcMessage = "write 16 bit int",
        .lInput = 0x11FE,
        .ucExpected ={ CBOR_INT16_FOLLOWS, 0x11, 0xFE },
    },
    {
        .pcMessage = "write 32 bit int",
        .lInput = 0x1337C0DE,
        .ucExpected ={ CBOR_INT32_FOLLOWS, 0x13, 0x37, 0xC0, 0xDE },
    },
    {
        .pcMessage = "write largest small positive int",
        .lInput = 23,
        .ucExpected ={ 23                  },
    },
    {
        .pcMessage = "write smallest 8-bit int",
        .lInput = 24,
        .ucExpected ={ CBOR_INT8_FOLLOWS, 24 },
    },
    {
        .pcMessage = "write largest 8-bit int",
        .lInput = 255,
        .ucExpected ={ CBOR_INT8_FOLLOWS, 255 },
    },
    {
        .pcMessage = "write smallest 16-bit int",
        .lInput = 256,
        .ucExpected ={ CBOR_INT16_FOLLOWS, 0x01, 0x00 },
    },
    {
        .pcMessage = "write largest 16-bit int",
        .lInput = 0xFFFF,
        .ucExpected ={ CBOR_INT16_FOLLOWS, 0xFF, 0xFF },
    },
    {
        .pcMessage = "write smallest 32-bit int",
        .lInput = 0x010000,
        .ucExpected =
        {
            CBOR_INT32_FOLLOWS,
            0x00,
            0x01,
            0x00,
            0x00,
        },
    },
    {
        .pcMessage = "write largest positive signed 32-bit int",
        .lInput = INT32_MAX,
        .ucExpected =
        {
            CBOR_INT32_FOLLOWS,
            0x7F,
            0xFF,
            0xFF,
            0xFF,
        },
    },
};

int lWriteTestCount = ( sizeof( xWriteTests ) / sizeof( xWriteTests[ 0 ] ) );

write_test_t * pxWriteTest;

void test_WriteInt( void )
{
    CBOR_SetCursor( xCborData, 0 );
    uint8_t ucInit[ 32 ] = { 0 };
    CBOR_MemCopy( xCborData, ucInit, sizeof( ucInit ) );
    CBOR_SetCursor( xCborData, 0 );

    CBOR_WriteInt( xCborData, &pxWriteTest->lInput );
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        pxWriteTest->ucExpected, xCborData->pxBufferStart, 32, pxWriteTest->pcMessage );
}

TEST( aws_cbor_int, WriteInt )
{
    for( int i = 0; i < lWriteTestCount; i++ )
    {
        pxWriteTest = &xWriteTests[ i ];
        RUN_TEST( test_WriteInt );
    }
}

typedef struct read_test_s
{
    char * pcMessage;
    uint8_t ucBuffer[ 32 ];
    int lExpected;
} read_test_t;

read_test_t xReadTests[] =
{
    {
        .pcMessage = "read small int",
        .ucBuffer ={ 13                 },
        .lExpected = 13,
    },
    {
        .pcMessage = "read 8-bit int",
        .ucBuffer ={ CBOR_INT8_FOLLOWS, 78 },
        .lExpected = 78,
    },
    {
        .pcMessage = "read 16-bit int",
        .ucBuffer ={ CBOR_INT16_FOLLOWS, 0x12, 0x34 },
        .lExpected = 0x1234,
    },
    {
        .pcMessage = "read 32-bit int",
        .ucBuffer ={ CBOR_INT32_FOLLOWS, 0x09, 0x87, 0x65, 0x43 },
        .lExpected = 0x09876543,
    },
    {
        .pcMessage = "read largest small positive int",
        .lExpected = 23,
        .ucBuffer ={ 23                 },
    },
    {
        .pcMessage = "read smallest 8-bit int",
        .lExpected = 24,
        .ucBuffer ={ CBOR_INT8_FOLLOWS, 24 },
    },
    {
        .pcMessage = "read largest 8-bit int",
        .lExpected = 255,
        .ucBuffer ={ CBOR_INT8_FOLLOWS, 255 },
    },
    {
        .pcMessage = "read smallest 16-bit int",
        .lExpected = 256,
        .ucBuffer ={ CBOR_INT16_FOLLOWS, 0x01, 0x00 },
    },
    {
        .pcMessage = "read largest 16-bit int",
        .lExpected = 0xFFFF,
        .ucBuffer ={ CBOR_INT16_FOLLOWS, 0xFF, 0xFF },
    },
    {
        .pcMessage = "read smallest 32-bit int",
        .lExpected = 0x010000,
        .ucBuffer =
        {
            CBOR_INT32_FOLLOWS,
            0x00,
            0x01,
            0x00,
            0x00,
        },
    },
    {
        .pcMessage = "read largest positive signed 32-bit int",
        .lExpected = INT32_MAX,
        .ucBuffer =
        {
            CBOR_INT32_FOLLOWS,
            0x7F,
            0xFF,
            0xFF,
            0xFF,
        },
    },
};

int lReadTestCount = ( sizeof( xReadTests ) / sizeof( xReadTests[ 0 ] ) );

read_test_t * pxReadTest;

void test_ReadInt( void )
{
    CBOR_SetCursor( xCborData, 0 );
    CBOR_MemCopy( xCborData, pxReadTest->ucBuffer, sizeof( pxReadTest->ucBuffer ) );
    CBOR_SetCursor( xCborData, 0 );

    int lResult = CBOR_ReadInt( xCborData );
    TEST_ASSERT_EQUAL_MESSAGE( pxReadTest->lExpected, lResult, pxReadTest->pcMessage );
}

TEST( aws_cbor_int, ReadInt )
{
    for( int i = 0; i < lReadTestCount; i++ )
    {
        pxReadTest = &xReadTests[ i ];
        RUN_TEST( test_ReadInt );
    }
}

TEST( aws_cbor_int, ReadInt_sets_error_if_wrong_type )
{
    CBOR_AssignAndIncrementCursor( xCborData, 0xFF );
    CBOR_SetCursor( xCborData, 0 );
    ( void ) CBOR_ReadInt( xCborData );
    TEST_ASSERT_EQUAL( eCborErrReadTypeMismatch, xCborData->xError );
    xCborData->xError = eCborErrNoError;
}

TEST( aws_cbor_int, null_checks )
{
    uint8_t ucFoo[] = { CBOR_INT8_FOLLOWS, 42 };

    xTEST_expect_assert( CBOR_IntSize( NULL ) );

    xTEST_expect_assert( CBOR_WriteInt( NULL, ucFoo ) );
    xTEST_expect_assert( CBOR_WriteInt( xCborData, NULL ) );

    xTEST_expect_assert( CBOR_WriteInt32( NULL, 0 ) );
    xTEST_expect_assert( CBOR_WriteInt16( NULL, 0 ) );
    xTEST_expect_assert( CBOR_WriteInt8( NULL, 0 ) );
    xTEST_expect_assert( CBOR_WriteSmallInt( NULL, 0 ) );

    xTEST_expect_assert( CBOR_ReadInt( NULL ) );

    xTEST_expect_assert( CBOR_ReadPositiveInt32( NULL ) );
    xTEST_expect_assert( CBOR_ReadPositiveInt16( NULL ) );
    xTEST_expect_assert( CBOR_ReadPositiveInt8( NULL ) );
    xTEST_expect_assert( CBOR_ReadSmallPositiveInt( NULL ) );
}
