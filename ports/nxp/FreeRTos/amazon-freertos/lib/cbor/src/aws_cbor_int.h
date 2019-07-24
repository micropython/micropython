/*
 * Amazon FreeRTOS
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
#ifndef AWS_CBOR_INT_H /* Guards against multiple inclusion */
#define AWS_CBOR_INT_H

/**
 * @file
 * @brief Integer type functions
 * @see @glos{small_int}
 */

#include "aws_cbor.h"
#include <stdint.h>

/**
 * @brief Gets the size (in bytes) of an integer data item
 */
cbor_ssize_t CBOR_IntSize( const cbor_byte_t * pxPtr );

/**
 * @brief Writes an integer to the CBOR buffer.
 * @param CBORHandle_t  Handle for the CBOR data struct.
 * @param "const void *" Pointer to integer to write.
 */
void CBOR_WriteInt( CBORHandle_t /*xCborData*/, const void * /*input*/ );

/**
 * @brief Writes a 32-bit integer to the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @param int32_t        Integer to write
 */
void CBOR_WriteInt32( CBORHandle_t /*xCborData*/, uint32_t /*num*/ );

/**
 * @brief Writes a 16-bit integer to the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @param int16_t        Integer to write
 */
void CBOR_WriteInt16( CBORHandle_t /*xCborData*/, uint16_t /*num*/ );

/**
 * @brief Writes an 8-bit integer to the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @param cbor_byte_t        Integer to write
 */
void CBOR_WriteInt8( CBORHandle_t /*xCborData*/, cbor_byte_t /*num*/ );

/**
 * @brief Writes a small integer (less than or equal to 24) to the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @param cbor_byte_t        Integer to write
 */
void CBOR_WriteSmallInt( CBORHandle_t /*xCborData*/, cbor_byte_t /*num*/ );

/**
 * @brief Reads an integer from the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @return Integer from the cursor location of the CBOR buffer.
 */
cbor_int_t CBOR_ReadInt( CBORHandle_t /*xCborData*/ );

/**
 * @brief Reads a 32-bit integer from the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @return 32-bit integer from the cursor location of the CBOR buffer.
 */
uint32_t CBOR_ReadPositiveInt32( CBORHandle_t /*xCborData*/ );

/**
 * @brief Reads a 16-bit integer from the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @return 16-bit integer from the cursor location of the CBOR buffer.
 */
uint16_t CBOR_ReadPositiveInt16( CBORHandle_t /*xCborData*/ );

/**
 * @brief Reads an 8-bit integer from the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @return 8-bit integer from the cursor location of the CBOR buffer.
 */
cbor_byte_t CBOR_ReadPositiveInt8( CBORHandle_t /*xCborData*/ );

/**
 * @brief Reads a small integer from the CBOR buffer.
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @return              An integer in the range of 0-23 (inclusive)
 */
cbor_byte_t CBOR_ReadSmallPositiveInt( CBORHandle_t /*xCborData*/ );

#endif /* ifndef AWS_CBOR_INT_H */
