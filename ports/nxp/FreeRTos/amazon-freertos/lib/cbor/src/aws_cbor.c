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
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** Minimum size (in bytes) that the CBOR buffer shall start with */
#define CBOR_MIN_BUFFER_SIZE    ( 32 )

CBORHandle_t CBOR_New( cbor_ssize_t xSize )
{
    CBORHandle_t xNewCborData = pxCBOR_malloc( sizeof( struct CborData_s ) );

    if( NULL == xNewCborData )
    {
        return NULL;
    }

    xSize = xSize < CBOR_MIN_BUFFER_SIZE ? CBOR_MIN_BUFFER_SIZE : xSize;
    cbor_byte_t * pxNew_buffer = pxCBOR_malloc( xSize );

    if( NULL == pxNew_buffer )
    {
        pxCBOR_free( xNewCborData );

        return NULL;
    }

    cbor_byte_t xEmpty_map[ 2 ] = { CBOR_MAP_OPEN, CBOR_BREAK };
    memcpy( pxNew_buffer, xEmpty_map, 2 );
    xNewCborData->pxBufferStart = pxNew_buffer;
    xNewCborData->pxCursor = pxNew_buffer;
    xNewCborData->pxBufferEnd = &pxNew_buffer[ xSize - 1 ];
    xNewCborData->pxMapEnd = &pxNew_buffer[ 1 ];

    return xNewCborData;
}

void CBOR_Delete( CBORHandle_t * ppxHandle )
{
    if( NULL == ppxHandle )
    {
        return;
    }

    if( NULL == *ppxHandle )
    {
        return;
    }

    pxCBOR_free( ( *ppxHandle )->pxBufferStart );
    pxCBOR_free( *ppxHandle );
    *ppxHandle = NULL;
}

cbor_byte_t const * const CBOR_GetRawBuffer( CBORHandle_t pxHandle )
{
    return pxHandle->pxBufferStart;
}

cbor_ssize_t const CBOR_GetBufferSize( CBORHandle_t pxHandle )
{
    return pxHandle->pxMapEnd - pxHandle->pxBufferStart + 1;
}

bool CBOR_FindKey( CBORHandle_t xCborData,
                   char const * pcKey )
{
    if( NULL == xCborData )
    {
        return false;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return false;
    }

    ( xCborData->pxCursor ) = ( xCborData->pxBufferStart );
    CBOR_OpenMap( xCborData );
    CBOR_SearchForKey( xCborData, pcKey );
    bool xFound = false;

    if( CBOR_KeyIsMatch( xCborData, pcKey ) )
    {
        xFound = true;
        CBOR_Next( xCborData );
    }

    return xFound;
}

void CBOR_AssignKeyWithString( CBORHandle_t xCborData,
                               const char * pcKey,
                               const char * pcValue )
{
    if( NULL == xCborData )
    {
        return;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return;
    }

    if( NULL == pcValue )
    {
        xCborData->xError = eCborErrNullValue;

        return;
    }

    CBOR_AssignKey( xCborData, pcKey, CBOR_WriteString, pcValue );
}

void CBOR_AppendKeyWithString( CBORHandle_t xCborData,
                               const char * pcKey,
                               const char * pcValue )
{
    if( NULL == xCborData )
    {
        return;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return;
    }

    if( NULL == pcValue )
    {
        xCborData->xError = eCborErrNullValue;

        return;
    }

    CBOR_AppendKey( xCborData, pcKey, CBOR_WriteString, pcValue );
}

char * CBOR_FromKeyReadString( CBORHandle_t xCborData,
                               const char * pcKey )
{
    if( NULL == xCborData )
    {
        return NULL;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return NULL;
    }

    CBOR_FindKey( xCborData, pcKey );
    char * pcStr = CBOR_ReadString( xCborData );

    return pcStr;
}

void CBOR_AssignKeyWithInt( CBORHandle_t xCborData,
                            const char * pcKey,
                            cbor_int_t xValue )
{
    if( NULL == xCborData )
    {
        return;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return;
    }

    CBOR_AssignKey( xCborData, pcKey, CBOR_WriteInt, &xValue );
}

void CBOR_AppendKeyWithInt( CBORHandle_t xCborData,
                            const char * pcKey,
                            cbor_int_t xValue )
{
    if( NULL == xCborData )
    {
        return;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return;
    }

    CBOR_AppendKey( xCborData, pcKey, CBOR_WriteInt, &xValue );
}

cbor_int_t CBOR_FromKeyReadInt( CBORHandle_t xCborData,
                                const char * pcKey )
{
    if( NULL == xCborData )
    {
        return 0;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return 0;
    }

    CBOR_FindKey( xCborData, pcKey );
    cbor_int_t xValue = CBOR_ReadInt( xCborData );

    return xValue;
}

void CBOR_AssignKeyWithMap( CBORHandle_t xCborData,
                            const char * pcKey,
                            CBORHandle_t xValue )
{
    if( NULL == xCborData )
    {
        return;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return;
    }

    if( NULL == xValue )
    {
        xCborData->xError = eCborErrNullValue;

        return;
    }

    CBOR_AssignKey( xCborData, pcKey, CBOR_WriteMap, xValue );
}

void CBOR_AppendKeyWithMap( CBORHandle_t xCborData,
                            const char * pcKey,
                            CBORHandle_t xValue )
{
    if( NULL == xCborData )
    {
        return;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return;
    }

    if( NULL == xValue )
    {
        xCborData->xError = eCborErrNullValue;

        return;
    }

    CBOR_AppendKey( xCborData, pcKey, CBOR_WriteMap, xValue );
}

CBORHandle_t CBOR_FromKeyReadMap( CBORHandle_t xCborData,
                                  const char * pcKey )
{
    if( NULL == xCborData )
    {
        return NULL;
    }

    if( NULL == pcKey )
    {
        xCborData->xError = eCborErrNullKey;

        return NULL;
    }

    CBOR_FindKey( xCborData, pcKey );
    CBORHandle_t xMap = CBOR_ReadMap( xCborData );

    return xMap;
}

void CBOR_AppendMap( CBORHandle_t xDest,
                     CBORHandle_t xSrc )
{
    xSrc->pxCursor = xSrc->pxBufferStart + 1;
    cbor_ssize_t xLength = xSrc->pxBufferEnd - xSrc->pxCursor;
    xDest->pxCursor = xDest->pxMapEnd;
    CBOR_MemCopy( xDest, xSrc->pxCursor, xLength );
    xDest->pxMapEnd = xDest->pxCursor - 1;
}

cborError_t CBOR_CheckError( CBORHandle_t xCborData )
{
    if( NULL == xCborData )
    {
        return eCborErrNullHandle;
    }

    return xCborData->xError;
}

void CBOR_ClearError( CBORHandle_t xCborData )
{
    if( NULL == xCborData )
    {
        return;
    }

    xCborData->xError = eCborErrNoError;
}

/*
 * End of File
 */
