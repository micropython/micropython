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

#define CBOR_IsOpenMap( a )    ( CBOR_MAP_OPEN == ( a ) )
#define CBOR_IsBreak( a )      ( CBOR_BREAK == ( a ) )

/**
 * @file aws_cbor_iter.h
 * @brief Provides interface to iterate over CBOR buffer
 */

cbor_byte_t * CBOR_NextPtr( const cbor_byte_t * pxPtr )
{
    assert( NULL != pxPtr );

    cbor_byte_t xJump_type = *pxPtr;
    cbor_byte_t xMajor_type = ( CBOR_MAJOR_TYPE_MASK & xJump_type );

    switch( xMajor_type )
    {
        case CBOR_POS_INT:
        case CBOR_NEG_INT:
           {
               cbor_ssize_t xJump_amount = CBOR_IntSize( pxPtr );
               pxPtr += xJump_amount;
               break;
           }

        case CBOR_BYTE_STRING:
        case CBOR_STRING:
           {
               cbor_ssize_t xJump_amount = CBOR_StringSize( pxPtr );
               pxPtr += xJump_amount;
               break;
           }

        case CBOR_MAP:
            pxPtr++;
            break;

        case CBOR_OTHER:
            pxPtr++;
            break;

        default:
           {
               bool xDefault_case_major_type = false;
               assert( xDefault_case_major_type );
           }
    }

    return ( cbor_byte_t * ) pxPtr;
}

void CBOR_Next( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    xCborData->pxCursor = CBOR_NextPtr( xCborData->pxCursor );
}

cbor_byte_t * CBOR_NextKeyPtr( const cbor_byte_t * pxPtr )
{
    assert( NULL != pxPtr );

    cbor_byte_t xJump_type = *( pxPtr );

    if( CBOR_IsOpenMap( xJump_type ) )
    {
        pxPtr = CBOR_NextPtr( pxPtr );

        return ( cbor_byte_t * ) pxPtr;
    }

    pxPtr = CBOR_NextPtr( pxPtr );
    xJump_type = *( pxPtr );

    if( CBOR_IsOpenMap( xJump_type ) )
    {
        int lDepth = 1;

        while( 0 < lDepth )
        {
            pxPtr = CBOR_NextPtr( pxPtr );
            xJump_type = *( pxPtr );

            if( CBOR_IsBreak( xJump_type ) )
            {
                lDepth--;
            }
            else if( CBOR_IsOpenMap( xJump_type ) )
            {
                lDepth++;
            }
        }
    }

    pxPtr = CBOR_NextPtr( pxPtr );

    return ( cbor_byte_t * ) pxPtr;
}

void CBOR_NextKey( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    cbor_byte_t * pxPtr = CBOR_NextKeyPtr( xCborData->pxCursor );

    assert( xCborData->pxBufferStart <= pxPtr );
    assert( xCborData->pxBufferEnd >= pxPtr );
    assert( ( xCborData->pxMapEnd + 1 ) >= pxPtr );

    xCborData->pxCursor = pxPtr;
}

void CBOR_SetCursor( CBORHandle_t xCborData,
                     cbor_ssize_t xPos )
{
    assert( NULL != xCborData );

    xCborData->pxCursor = &( xCborData->pxBufferStart[ xPos ] );
}
