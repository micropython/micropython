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
#include <string.h>

/**
 * @brief reads the specified byte from the number
 *
 * Reads the byte specified by the index from the number, with 1 being the most
 * significant and n being the least significant (where n is the number of
 * bytes in the integer).
 *
 * @param  number Number to extract byte from
 * @param  index  Byte to extract (1 being most significant)
 * @return        The extracted byte
 */
#define CBOR_ReadByte( number, index ) \
    number >> ( CBOR_BYTE_WIDTH * ( sizeof( number ) - ( index ) ) )

void CBOR_WriteInt( CBORHandle_t xCborData,
                    const void * pvInput )
{
    assert( NULL != xCborData );
    assert( NULL != pvInput );

    cbor_int_t xNum = *( cbor_int_t * ) pvInput;

    if( CBOR_IsSmallInt( xNum ) )
    {
        CBOR_ValueResize( xCborData, CBOR_SMALL_INT_SIZE );
        CBOR_WriteSmallInt( xCborData, xNum );
    }
    else if( CBOR_Is8BitInt( xNum ) )
    {
        CBOR_ValueResize( xCborData, CBOR_INT8_SIZE );
        CBOR_WriteInt8( xCborData, ( cbor_byte_t ) xNum );
    }
    else if( CBOR_Is16BitInt( xNum ) )
    {
        CBOR_ValueResize( xCborData, CBOR_INT16_SIZE );
        CBOR_WriteInt16( xCborData, ( uint16_t ) xNum );
    }
    else if( CBOR_Is32BitInt( xNum ) )
    {
        CBOR_ValueResize( xCborData, CBOR_INT32_SIZE );
        CBOR_WriteInt32( xCborData, ( uint32_t ) xNum );
    }
}

cbor_ssize_t CBOR_IntSize( const cbor_byte_t * pxPtr )
{
    assert( NULL != pxPtr );

    cbor_byte_t xDataHead = *( pxPtr );
    cbor_byte_t xMajorType = xDataHead & CBOR_MAJOR_TYPE_MASK;
    assert( xMajorType == CBOR_POS_INT || xMajorType == CBOR_NEG_INT );
    cbor_byte_t xAdditional_detail = xDataHead & CBOR_ADDITIONAL_DATA_MASK;

    cbor_ssize_t xSize = 0;

    if( CBOR_IsSmallInt( xAdditional_detail ) )
    {
        xSize = CBOR_SMALL_INT_SIZE;
    }
    else if( CBOR_INT8_FOLLOWS == xAdditional_detail )
    {
        xSize = CBOR_INT8_SIZE;
    }
    else if( CBOR_INT16_FOLLOWS == xAdditional_detail )
    {
        xSize = CBOR_INT16_SIZE;
    }
    else if( CBOR_INT32_FOLLOWS == xAdditional_detail )
    {
        xSize = CBOR_INT32_SIZE;
    }

    return xSize;
}

void CBOR_WriteInt32( CBORHandle_t xCborData,
                      uint32_t ulNum )
{
    assert( NULL != xCborData );

    CBOR_AssignAndIncrementCursor( xCborData, CBOR_INT32_FOLLOWS );

    for( cbor_int_t xI = 1; xI <= sizeof( ulNum ); xI++ )
    {
        cbor_byte_t xByte = CBOR_ReadByte( ulNum, xI );
        CBOR_AssignAndIncrementCursor( xCborData, xByte );
    }
}

void CBOR_WriteInt16( CBORHandle_t xCborData,
                      uint16_t usNum )
{
    assert( NULL != xCborData );

    CBOR_AssignAndIncrementCursor( xCborData, CBOR_INT16_FOLLOWS );

    for( cbor_int_t xI = 1; xI <= sizeof( usNum ); xI++ )
    {
        cbor_byte_t xByte = CBOR_ReadByte( usNum, xI );
        CBOR_AssignAndIncrementCursor( xCborData, xByte );
    }
}

void CBOR_WriteInt8( CBORHandle_t xCborData,
                     cbor_byte_t xNum )
{
    assert( NULL != xCborData );

    CBOR_AssignAndIncrementCursor( xCborData, CBOR_INT8_FOLLOWS );
    CBOR_AssignAndIncrementCursor( xCborData, xNum );
}

void CBOR_WriteSmallInt( CBORHandle_t xCborData,
                         cbor_byte_t xNum )
{
    assert( NULL != xCborData );

    CBOR_AssignAndIncrementCursor( xCborData, xNum );
}

cbor_int_t CBOR_ReadInt( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    cbor_byte_t xDataHead = *( xCborData->pxCursor );
    cbor_byte_t xMajorType = xDataHead & CBOR_MAJOR_TYPE_MASK;

    if( CBOR_POS_INT != xMajorType )
    {
        xCborData->xError = eCborErrReadTypeMismatch;
    }

    cbor_int_t xNum = 0;
    cbor_byte_t xAdditional_detail = xDataHead & CBOR_ADDITIONAL_DATA_MASK;

    switch( xAdditional_detail )
    {
        case CBOR_INT32_FOLLOWS:
            xNum = CBOR_ReadPositiveInt32( xCborData );
            break;

        case CBOR_INT16_FOLLOWS:
            xNum = CBOR_ReadPositiveInt16( xCborData );
            break;

        case CBOR_INT8_FOLLOWS:
            xNum = CBOR_ReadPositiveInt8( xCborData );
            break;

        default:
            xNum = xAdditional_detail;
            break;
    }

    return xNum;
}

uint32_t CBOR_ReadPositiveInt32( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    uint32_t ulNum = 0;
    cbor_byte_t * pxPtr = xCborData->pxCursor + 1;

    /* Scan byte by byte and reassemble the integer */
    for( cbor_int_t xI = 0; xI < sizeof( ulNum ); xI++ )
    {
        cbor_byte_t xByte = *( pxPtr )++;
        ulNum <<= CBOR_BYTE_WIDTH;
        ulNum += xByte;
    }

    return ulNum;
}

uint16_t CBOR_ReadPositiveInt16( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    uint16_t usNum = 0;
    cbor_byte_t * pxPtr = xCborData->pxCursor + 1;

    /* Scan byte by byte and reassemble the integer */
    for( cbor_int_t xI = 0; xI < sizeof( usNum ); xI++ )
    {
        cbor_byte_t xByte = *( pxPtr )++;
        usNum <<= CBOR_BYTE_WIDTH;
        usNum += xByte;
    }

    return usNum;
}

cbor_byte_t CBOR_ReadPositiveInt8( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    return xCborData->pxCursor[ 1 ];
}

cbor_byte_t CBOR_ReadSmallPositiveInt( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    return *xCborData->pxCursor;
}
