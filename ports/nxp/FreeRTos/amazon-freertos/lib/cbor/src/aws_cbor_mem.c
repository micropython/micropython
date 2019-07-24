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

/*
 * Section: Included Files
 */

#include "aws_cbor_internals.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** @brief Checks if CBOR cursor is out of bounds */
#define CursorOutOfBounds( xCborData ) \
    OverflowOccured( ( xCborData ) ) || UnderflowOccured( ( xCborData ) )

/** @brief Checks if CBOR cursor is past the end of the buffer */
#define OverflowOccured( xCborData ) \
    ( ( xCborData )->pxBufferEnd < ( xCborData )->pxCursor )

/** @brief Checks if CBOR cursor is before the start of the buffer */
#define UnderflowOccured( xCborData ) \
    ( ( xCborData )->pxBufferStart > ( xCborData )->pxCursor )

/** @brief Calculates the size of the CBOR buffer */
#define BufferSize( xCborData ) \
    ( ( xCborData )->pxBufferEnd - ( xCborData )->pxBufferStart + 1 )

/**
 * @brief Doubles the size of the CBOR buffer
 *
 * Reallocates the CBOR buffer with double the size of the current buffer size.
 * Sets err if unable to reallocate the space.
 */
static void CBOR_Reallocate( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    /* Multiply by 1.5, more efficient on some compilers than just doing *1.5 */
    /* https://github.com/facebook/folly/blob/master/folly/docs/FBVector.md */
    cbor_ssize_t xNewSize = BufferSize( xCborData ) * 3;
    xNewSize /= 2;

    cbor_byte_t * pxNew_start = pxCBOR_realloc( xCborData->pxBufferStart, xNewSize );

    if( NULL == pxNew_start )
    {
        xCborData->xError = eCborErrInsufficentSpace;

        return;
    }

    /* Adjust pointers to new location */
    cbor_ssize_t xCursor_index = xCborData->pxCursor - xCborData->pxBufferStart;
    xCborData->pxBufferStart = pxNew_start;
    xCborData->pxBufferEnd = xCborData->pxBufferStart + xNewSize - 1;
    xCborData->pxCursor = xCborData->pxBufferStart + xCursor_index;
}

/**
 * @brief Copies memory into the CBOR buffer "front to back."
 *
 * Copies memory starting from the lowest address and ending with the highest
 * address.
 */
static void CBOR_MemCopyLowToHigh( CBORHandle_t xCborData,
                                   const cbor_byte_t * pxSource,
                                   cbor_ssize_t xLength )
{
    assert( NULL != xCborData );
    assert( NULL != pxSource );
    assert( 0 <= xLength );
    assert( xCborData->pxCursor < pxSource );

    for( cbor_int_t xI = 0; xI < xLength; xI++ )
    {
        CBOR_AssignAndIncrementCursor( xCborData, *pxSource++ );

        if( xCborData->xError )
        {
            return;
        }
    }
}

/**
 * @brief Copies memory into the CBOR buffer "back to front."
 *
 * Copies memory starting from the highest address and ending with the lowest
 * address.
 */
static void CBOR_MemCopyHighToLow( CBORHandle_t xCborData,
                                   const cbor_byte_t * pxSource,
                                   cbor_ssize_t xLength )
{
    assert( NULL != xCborData );
    assert( NULL != pxSource );
    assert( 0 <= xLength );
    assert( xCborData->pxCursor > pxSource );

    pxSource += ( xLength - 1 );
    xCborData->pxCursor += ( xLength - 1 );

    for( cbor_int_t xI = 0; xI < xLength; xI++ )
    {
        CBOR_AssignAndDecrementCursor( xCborData, *pxSource-- );
        /* assert, because else case is unreachable, unlike in 'LowToHigh' */
        assert( eCborErrNoError == xCborData->xError );
    }

    xCborData->pxCursor += xLength + 1;
}

void CBOR_AssignAndIncrementCursor( CBORHandle_t xCborData,
                                    cbor_byte_t xInput )
{
    assert( NULL != xCborData );
    assert( !UnderflowOccured( xCborData ) );

    while( OverflowOccured( xCborData ) )
    {
        CBOR_Reallocate( xCborData );

        if( eCborErrNoError != xCborData->xError )
        {
            return;
        }
    }

    *( xCborData->pxCursor )++ = xInput;
    ( xCborData->xError ) = eCborErrNoError;
}

void CBOR_AssignAndDecrementCursor( CBORHandle_t xCborData,
                                    cbor_byte_t xInput )
{
    assert( NULL != xCborData );
    assert( !UnderflowOccured( xCborData ) );

    while( OverflowOccured( xCborData ) )
    {
        CBOR_Reallocate( xCborData );

        if( eCborErrNoError != xCborData->xError )
        {
            return;
        }
    }

    *( xCborData->pxCursor )-- = xInput;
    ( xCborData->xError ) = eCborErrNoError;
}

void CBOR_MemCopy( CBORHandle_t xCborData,
                   const void * pvInput,
                   cbor_ssize_t xLength )
{
    assert( NULL != xCborData );
    assert( NULL != pvInput );
    assert( 0 <= xLength );

    const cbor_byte_t * pxSource = pvInput;

    if( xCborData->pxCursor < pxSource )
    {
        CBOR_MemCopyLowToHigh( xCborData, pxSource, xLength );
    }
    else if( xCborData->pxCursor > pxSource )
    {
        CBOR_MemCopyHighToLow( xCborData, pxSource, xLength );
    }
    else
    {
        /* Copying from a location into itself, so we can skip the copy part and
         * just move the cursor.  Maybe a user error, but not necessarily*/
        xCborData->pxCursor += xLength;
    }
}

cbor_ssize_t xCborDataItemSize( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    return xCborDataItemSizePtr( xCborData->pxCursor );
}

cbor_ssize_t xCborDataItemSizePtr( const cbor_byte_t * pxPtr )
{
    assert( NULL != pxPtr );

    cbor_byte_t xData_head = *pxPtr;
    cbor_byte_t xMajor_type = xData_head & CBOR_MAJOR_TYPE_MASK;

    switch( xMajor_type )
    {
        case CBOR_POS_INT:

            return CBOR_IntSize( pxPtr );

        case CBOR_STRING:

            return CBOR_StringSize( pxPtr );

        case CBOR_MAP:

            return CBOR_MapSize( pxPtr );

        default:
            break;
    }

    return 0;
}

void CBOR_ValueResize( CBORHandle_t xCborData,
                       cbor_ssize_t xNewSize )
{
    assert( NULL != xCborData );

    cbor_ssize_t xOld_size = xCborDataItemSize( xCborData );

    if( 0 == xOld_size )
    {
        return;
    }

    cbor_byte_t * pxCurrent_place = xCborData->pxCursor;
    cbor_byte_t * pxNext_key = pxCurrent_place + xOld_size;
    cbor_byte_t * pxKey_position = pxCurrent_place + xNewSize;
    cbor_ssize_t xRemaining_length = xCborData->pxMapEnd - pxNext_key + 1;
    assert( 0 <= xRemaining_length );
    xCborData->pxCursor = pxKey_position;
    CBOR_MemCopy( xCborData, pxNext_key, xRemaining_length );
    assert( eCborErrNoError == xCborData->xError );
    xCborData->pxCursor = pxCurrent_place;
}
