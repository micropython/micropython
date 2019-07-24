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
#ifndef AWS_CBOR_STRING_H
#define AWS_CBOR_STRING_H

/**
 * @file
 * @brief String type functions
 */

#include "aws_cbor.h"

/**
 * @brief Writes a string into the CBOR buffer
 *
 * Writes a string into the CBOR buffer at the location of the cursor.  Cursor
 * will then be pointing to the next byte after the string.
 *
 * @param "CBORHandle_t" handle for the CBOR data struct
 * @param "const void *" pointer to a zero terminated string
 */
void CBOR_WriteString( CBORHandle_t /*xCborData*/, const void * /*input*/ );

/**
 * @brief Reads a string from the CBOR buffer
 *
 * Copies the string at the cursor to a newly allocated buffer, and returns a
 * pointer to the buffer.  The user will later need to free the string to avoid
 * memory leaks.
 *
 * @param  Handle for the CBOR data struct
 * @return pointer to a string
 */
char * CBOR_ReadString( CBORHandle_t /*xCborData*/ );

/**
 * @brief Gets the size (in bytes) of a string data item
 * @param  "const cbor_byte_t *" pointer to head of string data item
 * @return cbor_ssize_t size in bytes
 */
cbor_ssize_t CBOR_StringSize( const cbor_byte_t * /*ptr*/ );

/**
 * @brief Gets the length of a string
 *
 * Gets the length of the string that the cursor is pointed at.  Cursor and data
 * remain unchanged.
 *
 * @param  Handle for the CBOR data struct
 * @return Length of the string
 */
cbor_ssize_t CBOR_StringLength( CBORHandle_t /*xCborData*/ );

/**
 * @brief Compares a string to a string in the CBOR buffer
 *
 * Compares the string at the cursor to a string provided by the user.
 *
 * @param  Handle for the CBOR data struct
 * @param  Pointer to a zero terminated string
 *
 * @return 0  - The two strings are equal
 * @return >0 - The first string is greater
 * @return <0 - The second string is greater
 *
 */
cbor_int_t CBOR_StringCompare(
    CBORHandle_t /*xCborData*/, cbor_const_string_t /*str2*/ );

#endif /* end of include guard: AWS_CBOR_STRING_H */
