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
#include <stdlib.h>
#include <string.h>

void CBOR_WriteShortString( CBORHandle_t xCborData,
                            const char * pcStr,
                            cbor_ssize_t xStringLength );

void CBOR_WriteInt8String( CBORHandle_t xCborData,
                           const char * pcStr,
                           cbor_ssize_t xStringLength );

void CBOR_WriteInt16String( CBORHandle_t xCborData,
                            const char * pcStr,
                            cbor_ssize_t xStringLength );

void CBOR_WriteString( CBORHandle_t xCborData,
                       const void * pvInput )
{
    assert( NULL != xCborData );
    assert( NULL != pvInput );

    const char * pcStr = pvInput;
    cbor_int_t xStringLength = strlen( pcStr );
    CBOR_ValueResize( xCborData, xStringLength + 1 );

    if( CBOR_IsSmallInt( xStringLength ) )
    {
        CBOR_WriteShortString( xCborData, pcStr, xStringLength );
    }
    else if( CBOR_Is8BitInt( xStringLength ) )
    {
        CBOR_WriteInt8String( xCborData, pcStr, xStringLength );
    }
    else if( CBOR_Is16BitInt( xStringLength ) )
    {
        CBOR_WriteInt16String( xCborData, pcStr, xStringLength );
    }
    else
    {
        xCborData->xError = eCborErrUnsupportedWriteOperation;

        return;
    }
}

void CBOR_WriteShortString( CBORHandle_t xCborData,
                            const char * pcStr,
                            cbor_ssize_t xStringLength )
{
    assert( NULL != xCborData );
    assert( NULL != pcStr );
    assert( 0 <= xStringLength );

    cbor_byte_t xCbor_major_type = CBOR_STRING;
    cbor_byte_t xAdditional_detail = xStringLength;
    cbor_byte_t xCbor_type = xCbor_major_type | xAdditional_detail;
    CBOR_AssignAndIncrementCursor( xCborData, xCbor_type );
    CBOR_MemCopy( xCborData, pcStr, xStringLength );
}

void CBOR_WriteInt8String( CBORHandle_t xCborData,
                           const char * pcStr,
                           cbor_ssize_t xStringLength )
{
    assert( NULL != xCborData );
    assert( NULL != pcStr );
    assert( 0 <= xStringLength );

    cbor_byte_t xCbor_major_type = CBOR_STRING;
    cbor_byte_t xAdditional_detail = CBOR_INT8_FOLLOWS;
    cbor_byte_t xCbor_type = xCbor_major_type | xAdditional_detail;
    CBOR_AssignAndIncrementCursor( xCborData, xCbor_type );
    CBOR_AssignAndIncrementCursor( xCborData, ( cbor_byte_t ) xStringLength );
    CBOR_MemCopy( xCborData, pcStr, xStringLength );
}

void CBOR_WriteInt16String( CBORHandle_t xCborData,
                            const char * pcStr,
                            cbor_ssize_t xStringLength )
{
    assert( NULL != xCborData );
    assert( NULL != pcStr );
    assert( 0 <= xStringLength );
    size_t xString_length_u = xStringLength;

    cbor_byte_t xCbor_major_type = CBOR_STRING;
    cbor_byte_t xAdditional_detail = CBOR_INT16_FOLLOWS;
    cbor_byte_t xCbor_type = xCbor_major_type | xAdditional_detail;
    CBOR_AssignAndIncrementCursor( xCborData, xCbor_type );
    CBOR_AssignAndIncrementCursor(
        xCborData, ( cbor_byte_t ) ( xString_length_u >> CBOR_BYTE_WIDTH ) );
    CBOR_AssignAndIncrementCursor( xCborData, ( cbor_byte_t ) xString_length_u );
    CBOR_MemCopy( xCborData, pcStr, xString_length_u );
}

cbor_ssize_t CBOR_StringSize( const cbor_byte_t * pxPtr )
{
    assert( NULL != pxPtr );

    cbor_byte_t xData_head = *pxPtr++;
    cbor_byte_t xMajor_type = xData_head & CBOR_MAJOR_TYPE_MASK;
    assert( xMajor_type == CBOR_STRING || xMajor_type == CBOR_BYTE_STRING );
    cbor_byte_t xAdditional_detail = xData_head & CBOR_ADDITIONAL_DATA_MASK;

    cbor_ssize_t xSize = 0;

    if( CBOR_INT8_FOLLOWS > xAdditional_detail )
    {
        cbor_ssize_t xData_size = xAdditional_detail + 1;
        xSize = xData_size;
    }
    else if( CBOR_INT8_FOLLOWS == xAdditional_detail )
    {
        cbor_ssize_t xData_size =
            *pxPtr + 2; /* 1 byte for the head, 1 byte for cbor_int_t 8 */
        xSize = xData_size;
    }
    else if( CBOR_INT16_FOLLOWS == xAdditional_detail )
    {
        cbor_ssize_t xData_size = *pxPtr++ << CBOR_BYTE_WIDTH;
        xData_size |= *pxPtr++;
        xData_size += 3; /* 1 byte for the head, 2 bytes for cbor_int_t 16 */
        xSize = xData_size;
    }

    return xSize;
}

static cbor_ssize_t CBOR_StringLengthAtPtr( CBORHandle_t xCborData,
                                            cbor_byte_t ** ppxCursor )
{
    assert( NULL != xCborData );
    assert( NULL != ppxCursor );
    assert( NULL != *ppxCursor );

    cbor_byte_t * pxPtr = *ppxCursor;
    cbor_byte_t xAdditional_info = *( pxPtr++ ) & CBOR_ADDITIONAL_DATA_MASK;
    cbor_ssize_t xLength = 0;

    if( CBOR_INT8_FOLLOWS > xAdditional_info )
    {
        xLength = xAdditional_info;
    }
    else if( CBOR_INT8_FOLLOWS == xAdditional_info )
    {
        xLength = *( pxPtr++ );
    }
    else if( CBOR_INT16_FOLLOWS == xAdditional_info )
    {
        xLength = ( *( pxPtr++ ) << CBOR_BYTE_WIDTH );
        xLength |= *( pxPtr++ );
    }

    *ppxCursor = pxPtr;

    return xLength;
}

cbor_ssize_t CBOR_StringLength( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    cbor_byte_t * pxPtr = xCborData->pxCursor;
    cbor_ssize_t xLength = CBOR_StringLengthAtPtr( xCborData, &pxPtr );

    return xLength;
}

char * CBOR_ReadString( CBORHandle_t xCborData )
{
    assert( NULL != xCborData );

    cbor_byte_t * pxPtr = xCborData->pxCursor;
    cbor_ssize_t xLength = CBOR_StringLengthAtPtr( xCborData, &( pxPtr ) );
    char * pcStr = pxCBOR_malloc( xLength + 1 ); /* +1 for the null terminator */
    memcpy( pcStr, pxPtr, xLength );
    pcStr[ xLength ] = 0;

    return pcStr;
}

cbor_int_t CBOR_StringCompare( CBORHandle_t xCborData,
                               const char * pcStr2 )
{
    assert( NULL != xCborData );
    assert( NULL != pcStr2 );

    /* Note: the string in the CBOR buffer is not zero terminated and the
     * string passed in is.  Thus, strcmp can't be used directly.*/
    cbor_byte_t * pxStr1 = xCborData->pxCursor;
    cbor_ssize_t xStr1_len = CBOR_StringLengthAtPtr( xCborData, &pxStr1 );

    cbor_ssize_t xI;

    for( xI = 0; xI < xStr1_len; xI++ )
    {
        cbor_int_t xComparison = *pxStr1++ - *pcStr2++;

        if( 0 != xComparison )
        {
            return xComparison;
        }
    }

    return( -*pcStr2 );
}
