/*
 * Amazon FreeRTOS OTA AFQP V1.1.4
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
/* Standard includes. */
#include <stdint.h>
#include <Windows.h>
#include <strsafe.h>
#include <stdbool.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* MQTT include. */
#include "aws_mqtt_agent.h"

/* CBOR and OTA includes. */
#include "aws_ota_agent.h"
#include "aws_ota_cbor.h"
#include "aws_ota_cbor_internal.h"
#include "aws_ota_agent_test_access_declare.h"
#include "cbor.h"

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/* Crypto includes. */
#include "aws_crypto.h"
#include "mbedTLS/sha1.h"

/*-----------------------------------------------------------*/

/* Global symbol required by OTA agent. */
MQTTPublishData_t xMsgMetaData;

/*-----------------------------------------------------------*/

static void prvSaveCborTestFile( char * pcFileName,
                                 uint8_t * pucCborData,
                                 uint32_t ulCborDataSize )
{
    uint32_t ulStatus = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD lpNumberOfBytesWritten;

    /* Open the file. */
    hFile = CreateFileA(
        pcFileName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        ulStatus = GetLastError();
    }

    /* Write the data. */
    if( ERROR_SUCCESS == ulStatus )
    {
        if( FALSE == WriteFile( hFile, pucCborData, ulCborDataSize, &lpNumberOfBytesWritten, NULL ) )
        {
            ulStatus = GetLastError();
        }
    }

    /* Clean up. */
    if( INVALID_HANDLE_VALUE != hFile )
    {
        CloseHandle( hFile );
    }
}

/*-----------------------------------------------------------*/

static BaseType_t prvReadCborTestFile( char * pcFileName,
                                       uint8_t ** ppucCborData,
                                       uint32_t * pulCborDataSize )
{
    uint32_t ulStatus = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    uint32_t ulSize = 0;

    /* Open the file. */
    hFile = CreateFileA(
        pcFileName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        ulStatus = GetLastError();
    }

    if( 0 == ulStatus )
    {
        /* Get the file size. */
        *pulCborDataSize = GetFileSize( hFile, &ulSize );

        /* Create a buffer. */
        *ppucCborData = pvPortMalloc( *pulCborDataSize );

        if( NULL == *ppucCborData )
        {
            ulStatus = ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    /* Read the file. */
    if( 0 == ulStatus )
    {
        if( FALSE == ReadFile(
                hFile,
                *ppucCborData,
                *pulCborDataSize,
                &ulSize,
                NULL ) )
        {
            ulStatus = GetLastError();
        }
    }

    /* Confirm the amount of data read. */
    if( 0 == ulStatus )
    {
        if( ulSize != *pulCborDataSize )
        {
            ulStatus = ERROR_INVALID_DATA;
        }
    }

    /* Clean up. */
    if( INVALID_HANDLE_VALUE != hFile )
    {
        CloseHandle( hFile );
    }

    return 0 == ulStatus;
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_OTA_CBOR );

CHAR szOriginalWorkingDirectory[ MAX_PATH ];

TEST_SETUP( Full_OTA_CBOR )
{
    uint32_t ulStatus = 0;
    CHAR szFullPath[ MAX_PATH ];

    /* Get the current working directory. */
    if( 0 == GetCurrentDirectoryA(
            sizeof( szOriginalWorkingDirectory ) / sizeof( szOriginalWorkingDirectory[ 0 ] ),
            szOriginalWorkingDirectory ) )
    {
        ulStatus = GetLastError();
        TEST_ASSERT_EQUAL( ulStatus, 0 );
    }

    /* Get the user %TEMP% directory. */
    if( 0 == GetTempPathA(
            sizeof( szFullPath ) / sizeof( szFullPath[ 0 ] ),
            szFullPath ) )
    {
        ulStatus = GetLastError();
        TEST_ASSERT_EQUAL( ulStatus, 0 );
    }
    else
    {
        /* Set the working directory of the host process. */
        if( FALSE == SetCurrentDirectoryA( szFullPath ) )
        {
            ulStatus = GetLastError();
            TEST_ASSERT_EQUAL( ulStatus, 0 );
        }
    }
}

TEST_TEAR_DOWN( Full_OTA_CBOR )
{
    uint32_t ulStatus = 0;

    /* Restore the working directory. */
    if( FALSE == SetCurrentDirectoryA( szOriginalWorkingDirectory ) )
    {
        ulStatus = GetLastError();
        TEST_ASSERT_EQUAL( ulStatus, 0 );
    }
}

TEST_GROUP_RUNNER( Full_OTA_CBOR )
{
    RUN_TEST_CASE( Full_OTA_CBOR, CborOtaApi );
    RUN_TEST_CASE( Full_OTA_CBOR, CborOtaAgentIngest );
    RUN_TEST_CASE( Full_OTA_CBOR, CborOtaServerFiles );
}

#define CBOR_TEST_MESSAGE_BUFFER_SIZE                     2048
#define CBOR_TEST_SERVER_CHUNK_COUNT                      16
#define CBOR_TEST_BITMAP_VALUE                            0xAAAAAAAA
#define CBOR_TEST_GETSTREAMRESPONSE_MESSAGE_ITEM_COUNT    4
#define CBOR_TEST_CLIENTTOKEN_VALUE                       "ThisIsAClientToken"
#define CBOR_TEST_STREAMVERSION_VALUE                     2
#define CBOR_TEST_STREAMDESCRIPTION_VALUE                 "ThisIsAStream"
#define CBOR_TEST_FILEIDENTITY_VALUE                      2
#define CBOR_TEST_BLOCKIDENTITY_VALUE                     0
#define CBOR_TEST_STREAMFILES_COUNT                       3
#define CBOR_TEST_STREAMFILE_FIELD_COUNT                  2

/*-----------------------------------------------------------*/

BaseType_t prvCreateSampleDescribeStreamResponseMessage( uint8_t * pucMessageBuffer,
                                                         size_t xMessageBufferSize,
                                                         size_t * pxEncodedSize )
{
    CborError xCborResult = CborNoError;
    CborEncoder
        xCborEncoder,
        xCborOuterMapEncoder,
        xCborInnerArrayEncoder,
        xCborStreamFileMapEncoder;

    /* Initialize the CBOR encoder. */
    cbor_encoder_init(
        &xCborEncoder,
        pucMessageBuffer,
        xMessageBufferSize,
        0 );
    xCborResult = cbor_encoder_create_map(
        &xCborEncoder,
        &xCborOuterMapEncoder,
        CBOR_TEST_GETSTREAMRESPONSE_MESSAGE_ITEM_COUNT );

    /* Encode the client token. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborOuterMapEncoder,
            OTA_CBOR_CLIENTTOKEN_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborOuterMapEncoder,
            CBOR_TEST_CLIENTTOKEN_VALUE );
    }

    /* Encode the stream description. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborOuterMapEncoder,
            OTA_CBOR_STREAMDESCRIPTION_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborOuterMapEncoder,
            CBOR_TEST_STREAMDESCRIPTION_VALUE );
    }

    /* Start the stream files array. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborOuterMapEncoder,
            OTA_CBOR_STREAMFILES_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encoder_create_array(
            &xCborOuterMapEncoder,
            &xCborInnerArrayEncoder,
            CBOR_TEST_STREAMFILES_COUNT );
    }

    /* Add the stream files. */
    for( uint32_t ulFile = 0;
         CborNoError == xCborResult && ulFile < CBOR_TEST_STREAMFILES_COUNT;
         ulFile++ )
    {
        xCborResult = cbor_encoder_create_map(
            &xCborInnerArrayEncoder,
            &xCborStreamFileMapEncoder,
            CBOR_TEST_STREAMFILE_FIELD_COUNT );

        /* Encode the file ID. */
        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encode_text_stringz(
                &xCborStreamFileMapEncoder,
                OTA_CBOR_FILEID_KEY );
        }

        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encode_int(
                &xCborStreamFileMapEncoder,
                1 + ulFile );
        }

        /* Encode the file size. */
        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encode_text_stringz(
                &xCborStreamFileMapEncoder,
                OTA_CBOR_FILESIZE_KEY );
        }

        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encode_int(
                &xCborStreamFileMapEncoder,
                100 * ( 1 + ulFile ) );
        }

        /* Done with the stream file map. */
        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encoder_close_container_checked(
                &xCborInnerArrayEncoder,
                &xCborStreamFileMapEncoder );
        }
    }

    /* Done with the inner array. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encoder_close_container_checked(
            &xCborOuterMapEncoder,
            &xCborInnerArrayEncoder );
    }

    /* Done with the outer map. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encoder_close_container_checked(
            &xCborEncoder,
            &xCborOuterMapEncoder );
    }

    /* Get the encoded size. */
    if( CborNoError == xCborResult )
    {
        *pxEncodedSize = cbor_encoder_get_buffer_size(
            &xCborEncoder,
            pucMessageBuffer );
    }

    return CborNoError == xCborResult;
}

/*-----------------------------------------------------------*/

BaseType_t prvCreateSampleGetStreamResponseMessage( uint8_t * pucMessageBuffer,
                                                    size_t xMessageBufferSize,
                                                    int lBlockIndex,
                                                    uint8_t * pucBlockPayload,
                                                    size_t xBlockPayloadSize,
                                                    size_t * pxEncodedSize )
{
    CborError xCborResult = CborNoError;
    CborEncoder xCborEncoder, xCborMapEncoder;

    /* Initialize the CBOR encoder. */
    cbor_encoder_init(
        &xCborEncoder,
        pucMessageBuffer,
        xMessageBufferSize,
        0 );
    xCborResult = cbor_encoder_create_map(
        &xCborEncoder,
        &xCborMapEncoder,
        CBOR_TEST_GETSTREAMRESPONSE_MESSAGE_ITEM_COUNT );

    /* Encode the file identity. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_FILEID_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_int(
            &xCborMapEncoder,
            CBOR_TEST_FILEIDENTITY_VALUE );
    }

    /* Encode the block identity. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_BLOCKID_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_int(
            &xCborMapEncoder,
            lBlockIndex );
    }

    /* Encode the block size. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_BLOCKSIZE_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_int(
            &xCborMapEncoder,
            xBlockPayloadSize );
    }

    /* Encode the block payload. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_BLOCKPAYLOAD_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_byte_string(
            &xCborMapEncoder,
            pucBlockPayload,
            xBlockPayloadSize );
    }

    /* Done with the encoder. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encoder_close_container_checked(
            &xCborEncoder,
            &xCborMapEncoder );
    }

    /* Get the encoded size. */
    if( CborNoError == xCborResult )
    {
        *pxEncodedSize = cbor_encoder_get_buffer_size(
            &xCborEncoder,
            pucMessageBuffer );
    }

    return CborNoError == xCborResult;
}

TEST( Full_OTA_CBOR, CborOtaApi )
{
    BaseType_t xResult = pdFALSE;
    uint8_t ucBlockPayload[OTA_FILE_BLOCK_SIZE] = { 0 };
    uint8_t ucCborWork[ CBOR_TEST_MESSAGE_BUFFER_SIZE ];
    size_t xEncodedSize = 0;
    uint32_t ulBitmap = CBOR_TEST_BITMAP_VALUE;
    void * pvDecodeContext = NULL;
    char * pcClientToken = NULL;
    int lStreamVersion = 0;
    char * pcStreamDescription = NULL;
    int lFileId = 0;
    int lFileSize = 0;
    int lBlockIndex = 0;
    int lBlockSize = 0;
    uint8_t * pucPayload = NULL;
    size_t xPayloadSize = 0;

    /* Test OTA_CBOR_Encode_GetStreamRequestMessage( ). */
    xResult = OTA_CBOR_Encode_GetStreamRequestMessage(
        ucCborWork,
        sizeof( ucCborWork ),
        &xEncodedSize,
        CBOR_TEST_CLIENTTOKEN_VALUE,
        1,
        OTA_FILE_BLOCK_SIZE,
        0,
        ( uint8_t * ) &ulBitmap,
        sizeof( ulBitmap ) );
    TEST_ASSERT_TRUE( xResult );

    prvSaveCborTestFile( "tempGetStreamRequest.cbor", ucCborWork, xEncodedSize );

    /* Test OTA_CBOR_Decode_GetStreamResponseMessage( ). */
    for( int l = 0; l < sizeof( ucBlockPayload ); l++ )
    {
        ucBlockPayload[ l ] = l;
    }

    xResult = prvCreateSampleGetStreamResponseMessage(
        ucCborWork,
        sizeof( ucCborWork ),
        CBOR_TEST_BLOCKIDENTITY_VALUE,
        ucBlockPayload,
        sizeof( ucBlockPayload ),
        &xEncodedSize );
    TEST_ASSERT_TRUE( xResult );

    prvSaveCborTestFile( "tempGetStreamResponse.cbor", ucCborWork, xEncodedSize );

    xResult = OTA_CBOR_Decode_GetStreamResponseMessage(
        ucCborWork,
        xEncodedSize,
        &lFileId,
        &lBlockIndex,
        &lBlockSize,
        &pucPayload,
        &xPayloadSize );
    TEST_ASSERT_TRUE( xResult );

    if( NULL != pucPayload )
    {
        vPortFree( pucPayload );
        pucPayload = NULL;
    }
}

TEST( Full_OTA_CBOR, CborOtaAgentIngest )
{
    BaseType_t xResultBool = pdFALSE;
    IngestResult_t xResultIngest = 0;
    uint8_t ucCborWork[ CBOR_TEST_MESSAGE_BUFFER_SIZE ];
    size_t xChunkSize = 0;
    size_t xEncodedSize = 0;
    OTA_FileContext_t xOTAFileContext = { 0 };
    Sig256_t xSig = { 0 };
    uint8_t *pucInFile = NULL;
    size_t xBlockBitmapSize = 0;
    uint8_t ucSignature[] =
    {
        0x38, 0x78, 0xf9, 0xb0, 0xd8, 0xf1, 0xa8, 0xc3, 0x4a, 0xdd, 0x63, 0x44, 0xc1, 0xbc, 0x9f, 0xb3,
        0xf3, 0xde, 0x49, 0x24, 0xb5, 0x93, 0x32, 0xe4, 0x01, 0x0e, 0x0c, 0x4a, 0xed, 0x32, 0x28, 0xd1,
        0x68, 0x1b, 0x12, 0x6b, 0x50, 0xab, 0x88, 0x5a, 0x18, 0xd0, 0x6d, 0x08, 0x8f, 0x95, 0x77, 0x03,
        0xb5, 0x79, 0x52, 0x5c, 0x4f, 0x32, 0x74, 0x0c, 0x28, 0xc6, 0x79, 0xf0, 0xe6, 0x57, 0x30, 0xa4,
        0x9b, 0x1b, 0x5f, 0x10, 0xf5, 0x2b, 0x53, 0xf0, 0x64, 0x9a, 0x1c, 0xff, 0x79, 0xaf, 0xe5, 0x71,
        0xd7, 0x6e, 0xfb, 0xc4, 0xa0, 0x5a, 0xf8, 0xc8, 0x0d, 0x2b, 0x85, 0x8e, 0x2d, 0xca, 0xb3, 0x03,
        0x05, 0xeb, 0x04, 0xb4, 0xe5, 0x6b, 0x5b, 0x4c, 0x88, 0xe2, 0x63, 0x38, 0x2d, 0xd7, 0xb9, 0x3a,
        0xd7, 0x48, 0xc6, 0x8f, 0x2c, 0x8d, 0x34, 0x8f, 0x19, 0x7a, 0x36, 0x12, 0x3b, 0xa1, 0x9f, 0xe2,
        0xc3, 0x44, 0x3d, 0xe0, 0x29, 0xd6, 0xf5, 0x82, 0xd3, 0xe9, 0xa0, 0x9f, 0xd8, 0x05, 0x09, 0x98,
        0x29, 0x71, 0xc5, 0x43, 0x94, 0x16, 0xe7, 0xc1, 0x8e, 0x4a, 0x50, 0x7d, 0xa6, 0xba, 0xb9, 0xbf,
        0xe3, 0x25, 0xa1, 0x50, 0x80, 0x4e, 0x39, 0xb3, 0x6f, 0xdb, 0x6e, 0xe2, 0x6b, 0x12, 0x71, 0x76,
        0x18, 0xcb, 0x8d, 0x62, 0x90, 0x48, 0x4a, 0xd9, 0xec, 0x9f, 0x97, 0xbf, 0xef, 0xa5, 0xcd, 0xaf,
        0x30, 0xd5, 0xfa, 0xba, 0x1c, 0xb2, 0x79, 0x98, 0x64, 0xbb, 0xd9, 0xda, 0x98, 0x8e, 0x0e, 0x66,
        0x6b, 0x29, 0xef, 0x6b, 0x4b, 0x2f, 0x80, 0xf8, 0xa4, 0x5b, 0x78, 0xfe, 0x70, 0xd6, 0x61, 0x20,
        0x28, 0xf2, 0xc4, 0x00, 0xc2, 0x7b, 0x35, 0x44, 0xd6, 0x3e, 0x8f, 0x9d, 0x8a, 0x7e, 0xf8, 0x2f,
        0x28, 0xa3, 0x77, 0xbb, 0xa1, 0xb7, 0xb2, 0xe1, 0x72, 0x55, 0x0a, 0x31, 0x58, 0x9b, 0xb7, 0x68
    };

    /* Read the test signed file. */
    xResultBool = prvReadCborTestFile(
        "payload.bin",
        &pucInFile,
        &xOTAFileContext.ulFileSize );

    /* Test OTA agent data block handling internals. Needs to be read/write
     * since we read it back in order to ensure that the blocks are hashed in
     * order. */
    xOTAFileContext.pxFile = fopen( "testOtaFile.bin", "w+b" );
    TEST_ASSERT_NOT_NULL( xOTAFileContext.pxFile );
    xOTAFileContext.ulBlocksRemaining =
        xOTAFileContext.ulFileSize / OTA_FILE_BLOCK_SIZE;
    if( 0 != xOTAFileContext.ulFileSize % OTA_FILE_BLOCK_SIZE )
    {
        xOTAFileContext.ulBlocksRemaining++;
    }

    xBlockBitmapSize = 1 + ( xOTAFileContext.ulFileSize / BITS_PER_BYTE );
    xOTAFileContext.pucRxBlockBitmap = pvPortMalloc( xBlockBitmapSize );
    TEST_ASSERT_NOT_NULL( xOTAFileContext.pucRxBlockBitmap );
    memset( xOTAFileContext.pucRxBlockBitmap, 0xFF, xBlockBitmapSize );

    xOTAFileContext.pucCertFilepath = "rsasigner.crt";
    xOTAFileContext.pxSignature = &xSig;
    memcpy( xOTAFileContext.pxSignature->ucData, ucSignature, sizeof( ucSignature ) );
    xOTAFileContext.pxSignature->usSize = sizeof( ucSignature );

    /* Process the signed file by chunks. */
    for(    size_t xBlock = 0;
            ( xBlock * OTA_FILE_BLOCK_SIZE ) < xOTAFileContext.ulFileSize;
            xBlock++ )
    {
        /* Create the encoded data block response. */
        xChunkSize = min(
            OTA_FILE_BLOCK_SIZE,
            xOTAFileContext.ulFileSize - ( xBlock * OTA_FILE_BLOCK_SIZE ) );
        xResultBool = prvCreateSampleGetStreamResponseMessage(
            ucCborWork,
            sizeof( ucCborWork ),
            xBlock,
            pucInFile + ( xBlock * OTA_FILE_BLOCK_SIZE ),
            xChunkSize,
            &xEncodedSize );
        TEST_ASSERT_TRUE( xResultBool );

        /* Handle the encoded data block message. */
        OTA_Err_t xCloseResult = kOTA_Err_None;
        xResultIngest = TEST_OTA_prvIngestDataBlock(
            &xOTAFileContext,
            ucCborWork,
            xEncodedSize,
            &xCloseResult );
        if( ( xBlock * OTA_FILE_BLOCK_SIZE ) + xChunkSize == xOTAFileContext.ulFileSize )
        {
            TEST_ASSERT_EQUAL_INT32( xResultIngest, eIngest_Result_FileComplete );
        }
        else
        {
            // eIngest_Result_Accepted_Continue = 0
            // eIngest_Result_Duplicate_Continue = 1,  /* The block was a duplicate but that's OK. Continue. */
            if ( (xResultIngest != eIngest_Result_Accepted_Continue) && (xResultIngest != eIngest_Result_Duplicate_Continue) )
            {
                TEST_ASSERT_EQUAL_INT32_MESSAGE( eIngest_Result_Accepted_Continue, xResultIngest, "Result was not accepted or duplicate continue" );
            }
        }
    }

    /* Clean-up. */
    if( NULL != xOTAFileContext.pxFile )
    {
        fclose( xOTAFileContext.pxFile );
    }

    if( NULL != pucInFile )
    {
        vPortFree( pucInFile );
    }
}

TEST( Full_OTA_CBOR, CborOtaServerFiles )
{
    BaseType_t xResultBool = pdFALSE;
    uint8_t * pucInFile = NULL;
    size_t xBufferSize = 0;
    void * pvDecodeContext = NULL;
    char * pcClientToken = NULL;
    int lStreamVersion = 0;
    char * pcStreamDescription = NULL;
    int lFileId = 0;
    int lFileSize = 0;
    int lBlockIndex = 0;
    int lBlockSize = 0;
    uint8_t * pucPayload = NULL;
    size_t xPayloadSize = 0;
    char pcChunkFileName[ MAX_PATH ];
    uint32_t ulBitmap = CBOR_TEST_BITMAP_VALUE;

    /* Read sample file chunks. */
    for( uint32_t ulChunk = 0;
         ulChunk < CBOR_TEST_SERVER_CHUNK_COUNT;
         ulChunk++ )
    {
        /* Build the chunk file name. */
        StringCbPrintfA(
            pcChunkFileName,
            sizeof( pcChunkFileName ),
            "getStreamResponse_%d.cbor",
            ulChunk );

        /* Free the previous content buffer. */
        if( NULL != pucInFile )
        {
            vPortFree( pucInFile );
        }

        /* Read in the next chunk message. */
        xResultBool = prvReadCborTestFile(
            pcChunkFileName,
            &pucInFile,
            &xBufferSize );
        TEST_ASSERT_TRUE( xResultBool );

        /* Free the previous chunk buffer. */
        if( NULL != pucPayload )
        {
            vPortFree( pucPayload );
        }

        /* Parse the chunk message. */
        xResultBool = OTA_CBOR_Decode_GetStreamResponseMessage(
            pucInFile,
            xBufferSize,
            &lFileId,
            &lBlockIndex,
            &lBlockSize,
            &pucPayload,
            &xPayloadSize );
        TEST_ASSERT_TRUE( xResultBool );
        TEST_ASSERT_EQUAL( lFileId, 1 );
        TEST_ASSERT_EQUAL( lBlockSize, xPayloadSize );

        /* Mark the chunk as received. */
        ulBitmap &= ~( 0x1 << lBlockIndex );
    }

    /* Confirm that all requested chunks were received. */
    TEST_ASSERT_EQUAL( ulBitmap, 0 );

    /* Clean-up. */
    if( NULL != pucInFile )
    {
        vPortFree( pucInFile );
    }

    if( NULL != pucPayload )
    {
        vPortFree( pucPayload );
    }
}
