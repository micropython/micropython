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
#ifndef AWS_CBOR_H /* Guards against multiple inclusion */
#define AWS_CBOR_H

/**
 * @file
 * @brief Public interface for AWS CBOR
 * @see @glos{cbor}
 * @see @glos{key}
 * @see @glos{value}
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief CBOR object errors
 */
typedef enum
{
    /** No error occurred in the last function call */
    eCborErrNoError = 0,

    /**
     * Unspecified error occurred.  A code path failed to specify a more
     * precise error code.
     */
    eCborErrDefaultError,

    /**
     * The data type in the CBOR buffer does not match the data type of the
     * read function.  E.g. Calling ReadInt while on a string @glos{value}.
     */
    eCborErrReadTypeMismatch,

    /** Performed an unsupported but legal CBOR read operation */
    eCborErrUnsupportedReadOperation,

    /** Performed an unsupported but legal CBOR write operation */
    eCborErrUnsupportedWriteOperation,

    /** Requested @glos{key} was not found in map */
    eCborErrKeyNotFound,

    /** Insufficient space for write operation */
    eCborErrInsufficentSpace,

    /** CBOR data handle is NULL */
    eCborErrNullHandle,

    /** Pointer to @glos{key} string is NULL */
    eCborErrNullKey,

    /** pointer to @glos{value} is NULL */
    eCborErrNullValue,
} cborError_t;

/** @brief Pointer to a CborData_s struct */
typedef struct CborData_s * CBORHandle_t;

/** @brief Pointer to write function for use with assigning values to keys */
typedef void (* write_function_t)( CBORHandle_t,
                                   void const * );

typedef char       * cbor_key_t;
typedef char const * cbor_const_key_t;
typedef char       * cbor_string_t;
typedef char const * cbor_const_string_t;
typedef int        cbor_int_t;
typedef int        cbor_ssize_t;
typedef uint8_t    cbor_byte_t;

/**
 * @brief Checks the error state of the CBOR object.
 * @param   CBORHandle_t  Handle for the CBOR data struct.
 * @return  cborError_t     The current error state of the CBOR object.
 */
cborError_t CBOR_CheckError( CBORHandle_t /*xCborData*/ );

/**
 * @brief Clears the error state from the CBOR object
 * @param CBORHandle_t   Handle for the CBOR data struct.
 */
void CBOR_ClearError( CBORHandle_t /*xCborData*/ );

/**
 * @brief Initializes user provided data buffer with AWS CBOR data structure.
 *
 * Creates a CBOR buffer object and allocates the amount of space the user
 * specifies, or the minimum buffer size, whichever is greater.  The buffer is
 * initialized with an empty map.
 *
 * @param  size size of the new buffer
 * @return A pointer to the AWS CBOR data structure
 * @return Returns NULL if unable to allocate space
 *
 * @see CBORHandle_t
 * @see CBOR_Delete
 */
CBORHandle_t CBOR_New( cbor_ssize_t /*size*/ );

/**
 * @brief Frees memory allocated by CBOR_New(0)
 * Frees memory previosuly allocated and nulls out the CBORHandle_t pointer
 * @param "CBORHandle_t *" handle for CBORHandle_t
 * @see CBOR_New
 */
void CBOR_Delete( CBORHandle_t * /*handle*/ );

/**
 * @brief Returns a pointer to the raw buffer
 *
 * @note The memory pointed to belongs to the CBOR handle.  Do not modify.
 * @note If the CBOR handle is deleted, the pointer should no longer be
 *     dereferenced.
 * @note If the buffer is needed after the buffer handle is destroyed, the
 *     buffer should be copied to memory owned by the caller.
 *
 * @code
 * cbor_byte_t * my_xCborData = CBOR_GetRawBuffer(my_cbor_handle);
 * cbor_ssize_t  my_data_size = CBOR_GerBufferSize(my_cbor_handle);
 * UART_PrintBuffer(my_xCborData, my_data_size);
 * @endcode
 *
 * @param  CBORHandle_t Handle for the CBOR data struct
 * @return               Pointer to the raw CBOR buffer
 * @see CBOR_GerBufferSize
 */
cbor_byte_t const * const CBOR_GetRawBuffer( CBORHandle_t /*handle*/ );

/**
 * @brief Returns the size of the CBOR buffer in bytes
 * @param  CBORHandle_t Handle for the CBOR data struct
 * @return               Size of the CBOR buffer in bytes
 * @see CBOR_GetRawBuffer
 */
cbor_ssize_t const CBOR_GetBufferSize( CBORHandle_t /*handle*/ );

/**
 * @brief Points read/write cursor to @glos{value} at given @glos{key}
 * @return True: Key was found
 * @return False: Key was NOT found
 */
bool CBOR_FindKey( CBORHandle_t /*xCborData*/, cbor_const_key_t /*key*/ );

/**
 * @brief Assigns a @glos{key} with a string @glos{value}
 * @param CBORHandle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param cbor_const_string_t  @glos{value} - zero terminated string
 */
void CBOR_AssignKeyWithString( CBORHandle_t /*xCborData*/,
                               cbor_const_key_t /*key*/, cbor_const_string_t /*value*/ );

/**
 * @brief Appends a @glos{key} with a string @glos{value}
 * @warning AppendKey... does not check for duplicate keys.
 * @param CBORHandle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param cbor_const_string_t  @glos{value} - zero terminated string
 */
void CBOR_AppendKeyWithString( CBORHandle_t /*xCborData*/,
                               cbor_const_key_t /*key*/, cbor_const_string_t /*value*/ );

/**
 * @brief Reads string from CBOR buffer at @glos{key}
 * @note Returns pointer to newly allocated memory
 * @param CBORHandle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @return "char *" @glos{value} - pointer to new zero terminated string
 */
char * CBOR_FromKeyReadString(
    CBORHandle_t /*xCborData*/, cbor_const_key_t /*key*/ );

/**
 * @brief Assigns a @glos{key} with an integer @glos{value}
 * @param CBORHandle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param "cbor_int_t"           @glos{value} - integer
 */
void CBOR_AssignKeyWithInt( CBORHandle_t /*xCborData*/,
                            cbor_const_key_t /*key*/, cbor_int_t /*value*/ );

/**
 * @brief Appends a @glos{key} with an integer @glos{value}
 * @warning AppendKey... does not check for duplicate keys.
 * @param CBORHandle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param "cbor_int_t"           @glos{value} - integer
 */
void CBOR_AppendKeyWithInt( CBORHandle_t /*xCborData*/,
                            cbor_const_key_t /*key*/, cbor_int_t /*value*/ );

/**
 * @brief Reads integer from CBOR buffer at @glos{key}
 * @param CBORHandle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @return "cbor_int_t" @glos{value}
 */
cbor_int_t CBOR_FromKeyReadInt(
    CBORHandle_t /*xCborData*/, cbor_const_key_t /*key*/ );

/**
 * @brief Assigns a @glos{key} with a map.
 *
 * @param CBORHandle_t   Handle for the destination CBOR data struct
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param CBORHandle_t   Handle for the source CBOR data struct
 */
void CBOR_AssignKeyWithMap( CBORHandle_t /*xCborData*/,
                            cbor_const_key_t /*key*/, CBORHandle_t /*value*/ );

/**
 * @brief Appends a @glos{key} with a map.
 * @warning AppendKey... does not check for duplicate keys.
 * @param CBORHandle_t   Handle for the destination CBOR data struct
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param CBORHandle_t   Handle for the source CBOR data struct
 */
void CBOR_AppendKeyWithMap( CBORHandle_t /*xCborData*/,
                            cbor_const_key_t /*key*/, CBORHandle_t /*value*/ );

/**
 * @brief Reads map from CBOR buffer at @glos{key}
 * @note Returns pointer to newly allocated memory
 * @param CBORHandle_t    Handle for the CBOR data struct.
 * @param "const char *"   @glos{key}   - zero terminated string
 * @return "CBORHandle_t" @glos{value} - pointer to new copy of CBOR map
 */
CBORHandle_t CBOR_FromKeyReadMap(
    CBORHandle_t /*xCborData*/, cbor_const_key_t /*key*/ );

/**
 * @brief Concatenates the source map to the destination map
 *
 * @warning AppendMap does not check for duplicate keys.
 *
 * CBOR_AppendMap copies the key value pairs from the source map to the
 * destination map.
 *
 * @param dest CBORHandle_t for destination map
 * @param src CBORHandle_t for source map
 */
void CBOR_AppendMap( CBORHandle_t /*dest*/, CBORHandle_t /*src*/ );

#endif /* ifndef AWS_CBOR_H */
