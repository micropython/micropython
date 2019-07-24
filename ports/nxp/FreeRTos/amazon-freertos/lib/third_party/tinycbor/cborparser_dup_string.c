/****************************************************************************
**
** Copyright (C) 2016 Intel Corporation
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
**
****************************************************************************/

#define _BSD_SOURCE 1
#define _DEFAULT_SOURCE 1
#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS 1
#endif

#include "cbor.h"
#include <stdlib.h>

/**
 * \fn CborError cbor_value_dup_text_string(const CborValue *value, char **buffer, size_t *buflen, CborValue *next)
 *
 * Allocates memory for the string pointed by \a value and copies it into this
 * buffer. The pointer to the buffer is stored in \a buffer and the number of
 * bytes copied is stored in \a len (those variables must not be NULL).
 *
 * If the iterator \a value does not point to a text string, the behaviour is
 * undefined, so checking with \ref cbor_value_get_type or \ref
 * cbor_value_is_text_string is recommended.
 *
 * If \c malloc returns a NULL pointer, this function will return error
 * condition \ref CborErrorOutOfMemory.
 *
 * On success, \c{*buffer} will contain a valid pointer that must be freed by
 * calling \c{free()}. This is the case even for zero-length strings.
 *
 * The \a next pointer, if not null, will be updated to point to the next item
 * after this string. If \a value points to the last item, then \a next will be
 * invalid.
 *
 * This function may not run in constant time (it will run in O(n) time on the
 * number of chunks). It requires constant memory (O(1)) in addition to the
 * malloc'ed block.
 *
 * \note This function does not perform UTF-8 validation on the incoming text
 * string.
 *
 * \sa cbor_value_copy_text_string(), cbor_value_dup_byte_string()
 */

/**
 * \fn CborError cbor_value_dup_byte_string(const CborValue *value, uint8_t **buffer, size_t *buflen, CborValue *next)
 *
 * Allocates memory for the string pointed by \a value and copies it into this
 * buffer. The pointer to the buffer is stored in \a buffer and the number of
 * bytes copied is stored in \a len (those variables must not be NULL).
 *
 * If the iterator \a value does not point to a byte string, the behaviour is
 * undefined, so checking with \ref cbor_value_get_type or \ref
 * cbor_value_is_byte_string is recommended.
 *
 * If \c malloc returns a NULL pointer, this function will return error
 * condition \ref CborErrorOutOfMemory.
 *
 * On success, \c{*buffer} will contain a valid pointer that must be freed by
 * calling \c{free()}. This is the case even for zero-length strings.
 *
 * The \a next pointer, if not null, will be updated to point to the next item
 * after this string. If \a value points to the last item, then \a next will be
 * invalid.
 *
 * This function may not run in constant time (it will run in O(n) time on the
 * number of chunks). It requires constant memory (O(1)) in addition to the
 * malloc'ed block.
 *
 * \sa cbor_value_copy_byte_string(), cbor_value_dup_text_string()
 */
CborError _cbor_value_dup_string(const CborValue *value, void **buffer, size_t *buflen, CborValue *next)
{
    assert(buffer);
    assert(buflen);
    *buflen = SIZE_MAX;
    CborError err = _cbor_value_copy_string(value, NULL, buflen, NULL);
    if (err)
        return err;

    ++*buflen;
    *buffer = malloc(*buflen);
    if (!*buffer) {
        /* out of memory */
        return CborErrorOutOfMemory;
    }
    err = _cbor_value_copy_string(value, *buffer, buflen, next);
    if (err) {
        free(*buffer);
        return err;
    }
    return CborNoError;
}
