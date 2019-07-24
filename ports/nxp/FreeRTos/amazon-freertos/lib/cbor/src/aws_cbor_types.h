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
#ifndef AWS_CBOR_TYPES_H /* Guards against multiple inclusion */
#define AWS_CBOR_TYPES_H

/**
 * @file
 * @brief Internal CBOR C structures and types
 */

#include "aws_cbor.h"
#include <stdint.h>

/**
 * @brief Pointer to a CBOR Data Struct
 */
struct CborData_s
{
    /** Start of the CBOR data buffer */
    cbor_byte_t * pxBufferStart;
    /** End of the allocated space for the CBOR buffer */
    cbor_byte_t * pxBufferEnd;

    /**
     * End of the space used by the CBOR buffer
     * map_end - pxBufferStart + 1 = buffer_length
     * pxBufferEnd - map_end = space remaining */
    cbor_byte_t * pxMapEnd;
    /** Current position of read/write cursor */
    cbor_byte_t * pxCursor;
    /** Current error code status */
    cborError_t xError;
};

#endif /* ifndef AWS_CBOR_TYPES_H */

/*
 * End of File
 */
