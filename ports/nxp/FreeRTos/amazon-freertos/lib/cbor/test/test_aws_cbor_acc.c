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
#include "aws_cbor.h"
#include "aws_cbor_alloc.h"
#include "unity_fixture.h"
#include <string.h>

TEST_GROUP( aws_cbor_acceptance );
TEST_SETUP( aws_cbor_acceptance )
{
}

TEST_TEAR_DOWN( aws_cbor_acceptance )
{
}

TEST_GROUP_RUNNER( aws_cbor_acceptance )
{
    RUN_TEST_CASE( aws_cbor_acceptance, assign_and_read_supported_types );
    RUN_TEST_CASE( aws_cbor_acceptance, append_and_read_supported_types );
    RUN_TEST_CASE(
        aws_cbor_acceptance, public_functions_set_err_when_given_null_pointers );
}

TEST( aws_cbor_acceptance, assign_and_read_supported_types )
{
    /* Write and then read a the following key value pairs: */
    /* { */
    /*     "hello":"world", */
    /*     "model":"of the modern major general", */
    /*     "answer":42, */
    /*     "prime":1033, */
    /*     "map":{ */
    /*         "direction":"north", */
    /*         "miles":2000 */
    /*     }, */
    /*     "lorem": "Lorem ipsum..." */
    /* } */

    /* Write the key value pairs using Assign functions */
    CBORHandle_t xCborData = CBOR_New( 0 );

    CBOR_AssignKeyWithString( xCborData, "hello", "world" );
    char * pcModel = "of the modern major general";
    CBOR_AssignKeyWithString( xCborData, "model", pcModel );
    CBOR_AssignKeyWithInt( xCborData, "answer", 42 );
    int lPrime = 1033;
    CBOR_AssignKeyWithInt( xCborData, "prime", lPrime );

    CBORHandle_t xMap = CBOR_New( 0 );
    char * pcDirection = "north";
    CBOR_AssignKeyWithString( xMap, "direction", pcDirection );
    CBOR_AssignKeyWithInt( xMap, "miles", 2000 );
    CBOR_AssignKeyWithMap( xCborData, "map", xMap );
    CBOR_Delete( &xMap );

    char * pcLorem =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim "
        "ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";
    CBOR_AssignKeyWithString( xCborData, "lorem", pcLorem );

    /* Read the values from the key value pairs */

    char * pcReadLorem = CBOR_FromKeyReadString( xCborData, "lorem" );
    TEST_ASSERT_EQUAL_STRING( pcLorem, pcReadLorem );
    pxCBOR_free( pcReadLorem );

    char * pcReadHello = CBOR_FromKeyReadString( xCborData, "hello" );
    TEST_ASSERT_EQUAL_STRING( "world", pcReadHello );
    pxCBOR_free( pcReadHello );

    char * pcReadModel = CBOR_FromKeyReadString( xCborData, "model" );
    TEST_ASSERT_EQUAL_STRING( pcModel, pcReadModel );
    pxCBOR_free( pcReadModel );

    int lReadAnswer = CBOR_FromKeyReadInt( xCborData, "answer" );
    TEST_ASSERT_EQUAL( 42, lReadAnswer );

    int lReadPrime = CBOR_FromKeyReadInt( xCborData, "prime" );
    TEST_ASSERT_EQUAL( lPrime, lReadPrime );

    CBORHandle_t xReadMap = CBOR_FromKeyReadMap( xCborData, "map" );

    char * pcReadDirection = CBOR_FromKeyReadString( xReadMap, "direction" );
    TEST_ASSERT_EQUAL_STRING( pcDirection, pcReadDirection );
    pxCBOR_free( pcReadDirection );

    int lReadMiles = CBOR_FromKeyReadInt( xReadMap, "miles" );
    TEST_ASSERT_EQUAL( 2000, lReadMiles );

    CBOR_Delete( &xReadMap );

    CBOR_Delete( &xCborData );
}

TEST( aws_cbor_acceptance, append_and_read_supported_types )
{
    /* Write and then read a the following key value pairs: */
    /* { */
    /*     "hello":"world", */
    /*     "model":"of the modern major general", */
    /*     "answer":42, */
    /*     "prime":1033, */
    /*     "map":{ */
    /*         "direction":"north", */
    /*         "miles":2000 */
    /*     }, */
    /*     "lorem": "Lorem ipsum..." */
    /* } */

    /* Write the key value pairs using Assign functions */
    CBORHandle_t xCborData = CBOR_New( 0 );

    CBOR_AppendKeyWithString( xCborData, "hello", "world" );
    char * pcModel = "of the modern major general";
    CBOR_AppendKeyWithString( xCborData, "model", pcModel );
    CBOR_AppendKeyWithInt( xCborData, "answer", 42 );
    int lPrime = 1033;
    CBOR_AppendKeyWithInt( xCborData, "prime", lPrime );

    CBORHandle_t xMap = CBOR_New( 0 );
    char * pcDirection = "north";
    CBOR_AppendKeyWithString( xMap, "direction", pcDirection );
    CBOR_AppendKeyWithInt( xMap, "miles", 2000 );
    CBOR_AppendKeyWithMap( xCborData, "map", xMap );
    CBOR_Delete( &xMap );

    char * pcLorem =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim "
        "ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";
    CBOR_AppendKeyWithString( xCborData, "lorem", pcLorem );

    /* Read the values from the key value pairs */

    char * pcReadLorem = CBOR_FromKeyReadString( xCborData, "lorem" );
    TEST_ASSERT_EQUAL_STRING( pcLorem, pcReadLorem );
    pxCBOR_free( pcReadLorem );

    char * pcReadHello = CBOR_FromKeyReadString( xCborData, "hello" );
    TEST_ASSERT_EQUAL_STRING( "world", pcReadHello );
    pxCBOR_free( pcReadHello );

    char * pcReadModel = CBOR_FromKeyReadString( xCborData, "model" );
    TEST_ASSERT_EQUAL_STRING( pcModel, pcReadModel );
    pxCBOR_free( pcReadModel );

    int lReadAnswer = CBOR_FromKeyReadInt( xCborData, "answer" );
    TEST_ASSERT_EQUAL( 42, lReadAnswer );

    int lReadPrime = CBOR_FromKeyReadInt( xCborData, "prime" );
    TEST_ASSERT_EQUAL( lPrime, lReadPrime );

    CBORHandle_t xReadMap = CBOR_FromKeyReadMap( xCborData, "map" );

    char * pcReadDirection = CBOR_FromKeyReadString( xReadMap, "direction" );
    TEST_ASSERT_EQUAL_STRING( pcDirection, pcReadDirection );
    pxCBOR_free( pcReadDirection );

    int lReadMiles = CBOR_FromKeyReadInt( xReadMap, "miles" );
    TEST_ASSERT_EQUAL( 2000, lReadMiles );

    CBOR_Delete( &xReadMap );

    CBOR_Delete( &xCborData );
}

TEST( aws_cbor_acceptance, public_functions_set_err_when_given_null_pointers )
{
    /* All public functions that take pointers should set the appropriate error */
    /* when an invalid (e.g. NULL pointer) is passed in. */
    CBORHandle_t xCborData = CBOR_New( 0 );

    TEST_ASSERT_EQUAL( eCborErrNullHandle, CBOR_CheckError( NULL ) );

    CBOR_ClearError( NULL );

    CBOR_Delete( NULL );
    CBORHandle_t xNullCborObject = NULL;
    CBOR_Delete( &xNullCborObject );

    TEST_ASSERT_FALSE( CBOR_FindKey( NULL, "key" ) );
    TEST_ASSERT_FALSE( CBOR_FindKey( xCborData, NULL ) );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );

    CBOR_AssignKeyWithString( NULL, "key", "value" );
    CBOR_AssignKeyWithString( xCborData, NULL, "value" );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );
    CBOR_AssignKeyWithString( xCborData, "key", NULL );
    TEST_ASSERT_EQUAL( eCborErrNullValue, CBOR_CheckError( xCborData ) );

    CBOR_AppendKeyWithString( NULL, "key", "value" );
    CBOR_AppendKeyWithString( xCborData, NULL, "value" );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );
    CBOR_AppendKeyWithString( xCborData, "key", NULL );
    TEST_ASSERT_EQUAL( eCborErrNullValue, CBOR_CheckError( xCborData ) );

    ( void ) CBOR_FromKeyReadString( NULL, "key" );
    ( void ) CBOR_FromKeyReadString( xCborData, NULL );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );

    CBOR_AssignKeyWithInt( NULL, "key", 123 );
    CBOR_AssignKeyWithInt( xCborData, NULL, 123 );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );

    CBOR_AppendKeyWithInt( NULL, "key", 123 );
    CBOR_AppendKeyWithInt( xCborData, NULL, 123 );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );

    ( void ) CBOR_FromKeyReadInt( NULL, "key" );
    ( void ) CBOR_FromKeyReadInt( xCborData, NULL );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );

    CBORHandle_t xMap = CBOR_New( 0 );
    CBOR_AssignKeyWithMap( NULL, "key", xMap );
    CBOR_AssignKeyWithMap( xCborData, NULL, xMap );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );
    CBOR_AssignKeyWithMap( xCborData, "key", NULL );
    TEST_ASSERT_EQUAL( eCborErrNullValue, CBOR_CheckError( xCborData ) );

    CBOR_AppendKeyWithMap( NULL, "key", xMap );
    CBOR_AppendKeyWithMap( xCborData, NULL, xMap );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );
    CBOR_AppendKeyWithMap( xCborData, "key", NULL );
    TEST_ASSERT_EQUAL( eCborErrNullValue, CBOR_CheckError( xCborData ) );

    ( void ) CBOR_FromKeyReadMap( NULL, "key" );
    ( void ) CBOR_FromKeyReadMap( xCborData, NULL );
    TEST_ASSERT_EQUAL( eCborErrNullKey, CBOR_CheckError( xCborData ) );

    CBOR_Delete( &xMap );
    CBOR_Delete( &xCborData );
}
