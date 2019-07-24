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

void CBOR_OpenMap( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    CBOR_AssignAndIncrementCursor( xCborData, CBOR_MAP_OPEN );
}

void CBOR_CloseMap( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    CBOR_AssignAndIncrementCursor( xCborData, CBOR_BREAK );
    xCborData->pxMapEnd = xCborData->pxCursor - 1;
}

bool CBOR_KeyIsMatch( CBORHandle_t xCborData,
                      const char * pcKey )
{
    assert( NULL != xCborData );
    assert( NULL != pcKey );

    bool xIs_match = false;

    if( 0 == CBOR_StringCompare( xCborData, pcKey ) )
    {
        xIs_match = true;
    }

    return xIs_match;
}
void CBOR_SearchForKey( CBORHandle_t xCborData,
                        const char * pcKey )
{
    assert( NULL != xCborData );
    assert( NULL != pcKey );
    assert( *( xCborData->pxCursor ) != 0 );
    assert( *( xCborData->pxCursor ) != CBOR_MAP_OPEN );

    while( !( ( CBOR_BREAK == *( xCborData->pxCursor ) ) ||
              true == CBOR_KeyIsMatch( xCborData, pcKey ) ) )
    {
        CBOR_NextKey( xCborData );
    }
}
void CBOR_AppendKey( CBORHandle_t xCborData,
                     const char * pcKey,
                     write_function_t xWriteFunction,
                     const void * pvValue )
{
    assert( NULL != xCborData );
    assert( NULL != pcKey );
    assert( NULL != xWriteFunction );
    assert( NULL != pvValue );

    xCborData->pxCursor = xCborData->pxMapEnd;
    /* Key not found, at end of map */
    assert( CBOR_BREAK == *( xCborData->pxCursor ) );
    /* This overwrites map close... */
    CBOR_WriteString( xCborData, pcKey );
    CBOR_CloseMap( xCborData );
    xCborData->pxCursor--;

    xWriteFunction( xCborData, pvValue );

    /* ...So need to reclose it here. */
    CBOR_CloseMap( xCborData );
}

void CBOR_AssignKey( CBORHandle_t xCborData,
                     const char * pcKey,
                     write_function_t xWriteFunction,
                     const void * pvValue )
{
    assert( NULL != xCborData );
    assert( NULL != pcKey );
    assert( NULL != xWriteFunction );
    assert( NULL != pvValue );

    ( xCborData->xError ) = eCborErrDefaultError;
    ( xCborData->pxCursor ) = ( xCborData->pxBufferStart );

    if( CBOR_MAP_OPEN == *( xCborData->pxCursor ) )
    {
        CBOR_OpenMap( xCborData );
        CBOR_SearchForKey( xCborData, pcKey );

        if( CBOR_KeyIsMatch( xCborData, pcKey ) )
        {
            /* Key was found */
            CBOR_Next( xCborData );
            xWriteFunction( xCborData, pvValue );
        }
        else
        {
            CBOR_AppendKey( xCborData, pcKey, xWriteFunction, pvValue );
        }
    }
    else
    {
        ( xCborData->xError ) = eCborErrUnsupportedReadOperation;
    }
}

cbor_ssize_t CBOR_MapSize( const cbor_byte_t * pxPtr )
{
    assert( NULL != pxPtr );

    const cbor_byte_t * pxCursor_start = pxPtr;

    while( CBOR_BREAK != *( pxPtr ) )
    {
        pxPtr = CBOR_NextKeyPtr( pxPtr );
    }

    cbor_ssize_t xSize = pxPtr - pxCursor_start + 1;

    return xSize;
}

void CBOR_WriteMap( CBORHandle_t xCborData,
                    const void * pvInput )
{
    assert( NULL != xCborData );
    assert( NULL != pvInput );

    const struct CborData_s * pxMap = pvInput;
    const cbor_byte_t * pxMap_start = pxMap->pxBufferStart;

    cbor_ssize_t xMap_size = CBOR_MapSize( pxMap_start );

    CBOR_ValueResize( xCborData, xMap_size );

    CBOR_MemCopy( xCborData, pxMap_start, xMap_size );
}

CBORHandle_t CBOR_ReadMap( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    cbor_byte_t xData_head = *( xCborData->pxCursor );
    cbor_byte_t xMajor_type = xData_head & CBOR_MAJOR_TYPE_MASK;

    if( CBOR_MAP != xMajor_type )
    {
        xCborData->xError = eCborErrReadTypeMismatch;

        return NULL;
    }

    CBORHandle_t xMap = CBOR_New( 0 );

    if( NULL == xMap )
    {
        xCborData->xError = eCborErrInsufficentSpace;

        return NULL;
    }

    cbor_ssize_t xMap_size = CBOR_MapSize( xCborData->pxCursor );
    CBOR_MemCopy( xMap, xCborData->pxCursor, xMap_size );
    xMap->pxMapEnd = xMap->pxCursor - 1;

    return xMap;
}
