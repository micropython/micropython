/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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


/**
 * @file buffer_pool.h
 * @brief Buffer Pool Interface.
 *
 * The central pool of buffers.
 */

#ifndef _AWS_BUFFER_POOL_H_
#define _AWS_BUFFER_POOL_H_

#include <stdint.h>
#include "aws_lib_init.h"

/**
 * @brief Initializes the central buffer pool.
 *
 * This function must be called exactly once before calling any
 * other buffer pool functions.
 */
lib_initDECLARE_LIB_INIT( BUFFERPOOL_Init );

/**
 * @brief Gets a free buffer from the central buffer pool.
 *
 * It tries to get a free buffer of the given length from the
 * buffer pool. If a free buffer of the requested length or more
 * is available, it is returned and pulBufferLength is updated to
 * the actual length of the buffer. Otherwise NULL is returned to
 * indicate failure.
 *
 * @param[in, out] pulBufferLength The caller should set it to the
 * desired length of the buffer. The returned buffer can be larger
 * than the requested length and pulBufferLength is updated to the
 * actual length of the buffer.
 *
 * @return The pointer to the buffer if one is available, NULL otherwise.
 */
uint8_t * BUFFERPOOL_GetFreeBuffer( uint32_t * pulBufferLength );

/**
 * @brief Returns the buffer back to the central buffer pool.
 *
 * @param[in] pucBuffer The buffer to return to the buffer pool.
 */
void BUFFERPOOL_ReturnBuffer( uint8_t * const pucBuffer );

#endif /* _AWS_BUFFER_POOL_H_ */
