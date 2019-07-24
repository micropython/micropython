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
#ifndef AWS_CBOR_MEM_H /* Guards against multiple inclusion */
#define AWS_CBOR_MEM_H

/**
 * @file
 * @brief Low level memory management and navigations interface
 */

#include "aws_cbor.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Assigns writes to the cursor's location and increments the cursor.
 * @param Handle for the CBOR data struct.
 * @param Byte to write to cursor location.
 */
void CBOR_AssignAndIncrementCursor(
    CBORHandle_t /*xCborData*/, cbor_byte_t /*input*/ );

/**
 * @brief Assigns writes to the cursor's location and decrements the cursor.
 * @param Handle for the CBOR data struct.
 * @param Byte to write to cursor location.
 */
void CBOR_AssignAndDecrementCursor(
    CBORHandle_t /*xCborData*/, cbor_byte_t /*input*/ );

/**
 * @brief Copies data to the read/write cursor.
 *
 * Copies data from the pointer provided to the read/write cursor.  As this is a
 * write function, the cursor will point to the next position after the write
 * location.  The user of this function can provide a pointer to a location in
 * the CBOR buffer, to include overlapping buffers.
 *
 * @param CBORHandle_t  Handle for the CBOR data struct.
 * @param "const void *" Pointer to a memory buffer.
 * @param cbor_ssize_t         Length (in bytes) of the memory buffer.
 */
void CBOR_MemCopy( CBORHandle_t /*xCborData*/, const void * /*input*/,
                   cbor_ssize_t /*length*/ );

/**
 * @brief Gets size of the CBOR data item that the cursor points to.
 *
 * @param  CBORHandle_t Handle for the CBOR data struct.
 * @return cbor_ssize_t        Size of the CBOR data item
 */
cbor_ssize_t xCborDataItemSize( CBORHandle_t /*xCborData*/ );

/**
 * @brief Gets size of the CBOR data item at the pointer
 *
 * @param "const cbor_byte_t *" Pointer - Points to the head of a CBOR data
 * item.
 * @return cbor_ssize_t        Size of the CBOR data item
 */
cbor_ssize_t xCborDataItemSizePtr( const cbor_byte_t * /*ptr*/ );

/**
 * @brief Resizes a value to allow writing a larger or smaller value in place.
 *
 * Resizes a value in a CBOR buffer by copying the data to the left (higher
 * indexes) left to grow the value, or to the right to shrink the value.  If the
 * new size is the same as the old size, nothing happens.
 *
 * @note This can become expensive in large CBOR buffers
 *
 * @param Handle for the CBOR data struct.
 * @param The new size for the value.
 */
void CBOR_ValueResize( CBORHandle_t /*xCborData*/, cbor_ssize_t /*new_size*/ );

#endif /* ifndef AWS_CBOR_MEM_H */
