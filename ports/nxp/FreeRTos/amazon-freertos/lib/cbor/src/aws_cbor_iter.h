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
#ifndef AWS_CBOR_ITER_H
#define AWS_CBOR_ITER_H

/**
 * @file
 * @brief Provides interface to iterate over CBOR buffer
 */

#include "aws_cbor.h"
#include <stdint.h>

cbor_byte_t * CBOR_NextPtr( const cbor_byte_t * pxPtr );

/**
 * @brief Iterates to the next CBOR data item.
 *
 * @pre The cursor must be pointing to the initial byte of a CBOR data item.
 *
 * @param CBORHandle_t Handle for the CBOR data struct.
 */
void CBOR_Next( CBORHandle_t /*xCborData*/ );

cbor_byte_t * CBOR_NextKeyPtr( const cbor_byte_t * pxPtr );

/**
 * @brief Iterates to the next key in a map.
 *
 * @pre The cursor must be pointing to the initial byte of a key in a map.
 *
 * @param CBORHandle_t Handle for the CBOR data struct.
 */
void CBOR_NextKey( CBORHandle_t /*xCborData*/ );

/**
 * @brief Sets cursor at a specific postion in the CBOR buffer
 *
 * Sets the curso at a positive offset from the beginning of the buffer.
 *
 * @param CBORHandle_t Handle for the CBOR data struct.
 * @param cbor_ssize_t Position to move the cursor to.
 */
void CBOR_SetCursor( CBORHandle_t /*xCborData*/, cbor_ssize_t /*pos*/ );

#endif /* end of include guard: AWS_CBOR_ITER_H */
