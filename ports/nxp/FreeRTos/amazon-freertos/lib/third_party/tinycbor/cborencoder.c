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
#include "cborconstants_p.h"
#include "compilersupport_p.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "assert_p.h"       /* Always include last */

/**
 * \defgroup CborEncoding Encoding to CBOR
 * \brief Group of functions used to encode data to CBOR.
 *
 * CborEncoder is used to encode data into a CBOR stream. The outermost
 * CborEncoder is initialized by calling cbor_encoder_init(), with the buffer
 * where the CBOR stream will be stored. The outermost CborEncoder is usually
 * used to encode exactly one item, most often an array or map. It is possible
 * to encode more than one item, but care must then be taken on the decoder
 * side to ensure the state is reset after each item was decoded.
 *
 * Nested CborEncoder objects are created using cbor_encoder_create_array() and
 * cbor_encoder_create_map(), later closed with cbor_encoder_close_container()
 * or cbor_encoder_close_container_checked(). The pairs of creation and closing
 * must be exactly matched and their parameters are always the same.
 *
 * CborEncoder writes directly to the user-supplied buffer, without extra
 * buffering. CborEncoder does not allocate memory and CborEncoder objects are
 * usually created on the stack of the encoding functions.
 *
 * The example below initializes a CborEncoder object with a buffer and encodes
 * a single integer.
 *
 * \code
 *      uint8_t buf[16];
 *      CborEncoder encoder;
 *      cbor_encoder_init(&encoder, &buf, sizeof(buf), 0);
 *      cbor_encode_int(&encoder, some_value);
 * \endcode
 *
 * As explained before, usually the outermost CborEncoder object is used to add
 * one array or map, which in turn contains multiple elements. The example
 * below creates a CBOR map with one element: a key "foo" and a boolean value.
 *
 * \code
 *      uint8_t buf[16];
 *      CborEncoder encoder, mapEncoder;
 *      cbor_encoder_init(&encoder, &buf, sizeof(buf), 0);
 *      cbor_encoder_create_map(&encoder, &mapEncoder, 1);
 *      cbor_encode_text_stringz(&mapEncoder, "foo");
 *      cbor_encode_boolean(&mapEncoder, some_value);
 *      cbor_encoder_close_container(&encoder, &mapEncoder);
 * \endcode
 *
 * <h3 class="groupheader">Error checking and buffer size</h2>
 *
 * All functions operating on CborEncoder return a condition of type CborError.
 * If the encoding was successful, they return CborNoError. Some functions do
 * extra checking on the input provided and may return some other error
 * conditions (for example, cbor_encode_simple_value() checks that the type is
 * of the correct type).
 *
 * In addition, all functions check whether the buffer has enough bytes to
 * encode the item being appended. If that is not possible, they return
 * CborErrorOutOfMemory.
 *
 * It is possible to continue with the encoding of data past the first function
 * that returns CborErrorOutOfMemory. CborEncoder functions will not overrun
 * the buffer, but will instead count how many more bytes are needed to
 * complete the encoding. At the end, you can obtain that count by calling
 * cbor_encoder_get_extra_bytes_needed().
 *
 * \section1 Finalizing the encoding
 *
 * Once all items have been appended and the containers have all been properly
 * closed, the user-supplied buffer will contain the CBOR stream and may be
 * immediately used. To obtain the size of the buffer, call
 * cbor_encoder_get_buffer_size() with the original buffer pointer.
 *
 * The example below illustrates how one can encode an item with error checking
 * and then pass on the buffer for network sending.
 *
 * \code
 *      uint8_t buf[16];
 *      CborError err;
 *      CborEncoder encoder, mapEncoder;
 *      cbor_encoder_init(&encoder, &buf, sizeof(buf), 0);
 *      err = cbor_encoder_create_map(&encoder, &mapEncoder, 1);
 *      if (!err)
 *          return err;
 *      err = cbor_encode_text_stringz(&mapEncoder, "foo");
 *      if (!err)
 *          return err;
 *      err = cbor_encode_boolean(&mapEncoder, some_value);
 *      if (!err)
 *          return err;
 *      err = cbor_encoder_close_container_checked(&encoder, &mapEncoder);
 *      if (!err)
 *          return err;
 *
 *      size_t len = cbor_encoder_get_buffer_size(&encoder, buf);
 *      send_payload(buf, len);
 *      return CborNoError;
 * \endcode
 *
 * Finally, the example below illustrates expands on the one above and also
 * deals with dynamically growing the buffer if the initial allocation wasn't
 * big enough. Note the two places where the error checking was replaced with
 * an assertion, showing where the author assumes no error can occur.
 *
 * \code
 * uint8_t *encode_string_array(const char **strings, int n, size_t *bufsize)
 * {
 *     CborError err;
 *     CborEncoder encoder, arrayEncoder;
 *     size_t size = 256;
 *     uint8_t *buf = NULL;
 *
 *     while (1) {
 *         int i;
 *         size_t more_bytes;
 *         uint8_t *nbuf = realloc(buf, size);
 *         if (nbuf == NULL)
 *             goto error;
 *         buf = nbuf;
 *
 *         cbor_encoder_init(&encoder, &buf, size, 0);
 *         err = cbor_encoder_create_array(&encoder, &arrayEncoder, n);
 *         assert(err);         // can't fail, the buffer is always big enough
 *
 *         for (i = 0; i < n; ++i) {
 *             err = cbor_encode_text_stringz(&arrayEncoder, strings[i]);
 *             if (err && err != CborErrorOutOfMemory)
 *                 goto error;
 *         }
 *
 *         err = cbor_encoder_close_container_checked(&encoder, &arrayEncoder);
 *         assert(err);         // shouldn't fail!
 *
 *         more_bytes = cbor_encoder_get_extra_bytes_needed(encoder);
 *         if (more_size) {
 *             // buffer wasn't big enough, try again
 *             size += more_bytes;
 *             continue;
 *         }
 *
 *         *bufsize = cbor_encoder_get_buffer_size(encoder, buf);
 *         return buf;
 *     }
 *  error:
 *     free(buf);
 *     return NULL;
 *  }
 * \endcode
 */

/**
 * \addtogroup CborEncoding
 * @{
 */

/**
 * \struct CborEncoder
 * Structure used to encode to CBOR.
 */

/**
 * Initializes a CborEncoder structure \a encoder by pointing it to buffer \a
 * buffer of size \a size. The \a flags field is currently unused and must be
 * zero.
 */
void cbor_encoder_init(CborEncoder *encoder, uint8_t *buffer, size_t size, int flags)
{
    encoder->data.ptr = buffer;
    encoder->end = buffer + size;
    encoder->added = 0;
    encoder->flags = flags;
}

static inline void put16(void *where, uint16_t v)
{
    v = cbor_htons(v);
    memcpy(where, &v, sizeof(v));
}

/* Note: Since this is currently only used in situations where OOM is the only
 * valid error, we KNOW this to be true.  Thus, this function now returns just 'true',
 * but if in the future, any function starts returning a non-OOM error, this will need
 * to be changed to the test.  At the moment, this is done to prevent more branches
 * being created in the tinycbor output */
static inline bool isOomError(CborError err)
{
    (void) err;
    return true;
}

static inline void put32(void *where, uint32_t v)
{
    v = cbor_htonl(v);
    memcpy(where, &v, sizeof(v));
}

static inline void put64(void *where, uint64_t v)
{
    v = cbor_htonll(v);
    memcpy(where, &v, sizeof(v));
}

static inline bool would_overflow(CborEncoder *encoder, size_t len)
{
    ptrdiff_t remaining = (ptrdiff_t)encoder->end;
    remaining -= remaining ? (ptrdiff_t)encoder->data.ptr : encoder->data.bytes_needed;
    remaining -= (ptrdiff_t)len;
    return unlikely(remaining < 0);
}

static inline void advance_ptr(CborEncoder *encoder, size_t n)
{
    if (encoder->end)
        encoder->data.ptr += n;
    else
        encoder->data.bytes_needed += n;
}

static inline CborError append_to_buffer(CborEncoder *encoder, const void *data, size_t len)
{
    if (would_overflow(encoder, len)) {
        if (encoder->end != NULL) {
            len -= encoder->end - encoder->data.ptr;
            encoder->end = NULL;
            encoder->data.bytes_needed = 0;
        }

        advance_ptr(encoder, len);
        return CborErrorOutOfMemory;
    }

    memcpy(encoder->data.ptr, data, len);
    encoder->data.ptr += len;
    return CborNoError;
}

static inline CborError append_byte_to_buffer(CborEncoder *encoder, uint8_t byte)
{
    return append_to_buffer(encoder, &byte, 1);
}

static inline CborError encode_number_no_update(CborEncoder *encoder, uint64_t ui, uint8_t shiftedMajorType)
{
    /* Little-endian would have been so much more convenient here:
     * We could just write at the beginning of buf but append_to_buffer
     * only the necessary bytes.
     * Since it has to be big endian, do it the other way around:
     * write from the end. */
    uint64_t buf[2];
    uint8_t *const bufend = (uint8_t *)buf + sizeof(buf);
    uint8_t *bufstart = bufend - 1;
    put64(buf + 1, ui);     /* we probably have a bunch of zeros in the beginning */

    if (ui < Value8Bit) {
        *bufstart += shiftedMajorType;
    } else {
        uint8_t more = 0;
        if (ui > 0xffU)
            ++more;
        if (ui > 0xffffU)
            ++more;
        if (ui > 0xffffffffU)
            ++more;
        bufstart -= (size_t)1 << more;
        *bufstart = shiftedMajorType + Value8Bit + more;
    }

    return append_to_buffer(encoder, bufstart, bufend - bufstart);
}

static inline CborError encode_number(CborEncoder *encoder, uint64_t ui, uint8_t shiftedMajorType)
{
    ++encoder->added;
    return encode_number_no_update(encoder, ui, shiftedMajorType);
}

/**
 * Appends the unsigned 64-bit integer \a value to the CBOR stream provided by
 * \a encoder.
 *
 * \sa cbor_encode_negative_int, cbor_encode_int
 */
CborError cbor_encode_uint(CborEncoder *encoder, uint64_t value)
{
    return encode_number(encoder, value, UnsignedIntegerType << MajorTypeShift);
}

/**
 * Appends the negative 64-bit integer whose absolute value is \a
 * absolute_value to the CBOR stream provided by \a encoder.
 *
 * \sa cbor_encode_uint, cbor_encode_int
 */
CborError cbor_encode_negative_int(CborEncoder *encoder, uint64_t absolute_value)
{
    return encode_number(encoder, absolute_value, NegativeIntegerType << MajorTypeShift);
}

/**
 * Appends the signed 64-bit integer \a value to the CBOR stream provided by
 * \a encoder.
 *
 * \sa cbor_encode_negative_int, cbor_encode_uint
 */
CborError cbor_encode_int(CborEncoder *encoder, int64_t value)
{
    /* adapted from code in RFC 7049 appendix C (pseudocode) */
    uint64_t ui = value >> 63;              /* extend sign to whole length */
    uint8_t majorType = ui & 0x20;          /* extract major type */
    ui ^= value;                            /* complement negatives */
    return encode_number(encoder, ui, majorType);
}

/**
 * Appends the CBOR Simple Type of value \a value to the CBOR stream provided by
 * \a encoder.
 *
 * This function may return error CborErrorIllegalSimpleType if the \a value
 * variable contains a number that is not a valid simple type.
 */
CborError cbor_encode_simple_value(CborEncoder *encoder, uint8_t value)
{
#ifndef CBOR_ENCODER_NO_CHECK_USER
    /* check if this is a valid simple type */
    if (value >= HalfPrecisionFloat && value <= Break)
        return CborErrorIllegalSimpleType;
#endif
    return encode_number(encoder, value, SimpleTypesType << MajorTypeShift);
}

/**
 * Appends the floating-point value of type \a fpType and pointed to by \a
 * value to the CBOR stream provided by \a encoder. The value of \a fpType must
 * be one of CborHalfFloatType, CborFloatType or CborDoubleType, otherwise the
 * behavior of this function is undefined.
 *
 * This function is useful for code that needs to pass through floating point
 * values but does not wish to have the actual floating-point code.
 *
 * \sa cbor_encode_half_float, cbor_encode_float, cbor_encode_double
 */
CborError cbor_encode_floating_point(CborEncoder *encoder, CborType fpType, const void *value)
{
    uint8_t buf[1 + sizeof(uint64_t)];
    assert(fpType == CborHalfFloatType || fpType == CborFloatType || fpType == CborDoubleType);
    buf[0] = fpType;

    unsigned size = 2U << (fpType - CborHalfFloatType);
    if (size == 8)
        put64(buf + 1, *(const uint64_t*)value);
    else if (size == 4)
        put32(buf + 1, *(const uint32_t*)value);
    else
        put16(buf + 1, *(const uint16_t*)value);
    ++encoder->added;
    return append_to_buffer(encoder, buf, size + 1);
}

/**
 * Appends the CBOR tag \a tag to the CBOR stream provided by \a encoder.
 *
 * \sa CborTag
 */
CborError cbor_encode_tag(CborEncoder *encoder, CborTag tag)
{
    /* tags don't count towards the number of elements in an array or map */
    return encode_number_no_update(encoder, tag, TagType << MajorTypeShift);
}

static CborError encode_string(CborEncoder *encoder, size_t length, uint8_t shiftedMajorType, const void *string)
{
    CborError err = encode_number(encoder, length, shiftedMajorType);
    if (err && !isOomError(err))
        return err;
    return append_to_buffer(encoder, string, length);
}

/**
 * \fn CborError cbor_encode_text_stringz(CborEncoder *encoder, const char *string)
 *
 * Appends the null-terminated text string \a string to the CBOR stream
 * provided by \a encoder. CBOR requires that \a string be valid UTF-8, but
 * TinyCBOR makes no verification of correctness. The terminating null is not
 * included in the stream.
 *
 * \sa cbor_encode_text_string, cbor_encode_byte_string
 */

/**
 * Appends the text string \a string of length \a length to the CBOR stream
 * provided by \a encoder. CBOR requires that \a string be valid UTF-8, but
 * TinyCBOR makes no verification of correctness.
 *
 * \sa CborError cbor_encode_text_stringz, cbor_encode_byte_string
 */
CborError cbor_encode_byte_string(CborEncoder *encoder, const uint8_t *string, size_t length)
{
    return encode_string(encoder, length, ByteStringType << MajorTypeShift, string);
}

/**
 * Appends the byte string \a string of length \a length to the CBOR stream
 * provided by \a encoder. CBOR byte strings are arbitrary raw data.
 *
 * \sa cbor_encode_text_stringz, cbor_encode_text_string
 */
CborError cbor_encode_text_string(CborEncoder *encoder, const char *string, size_t length)
{
    return encode_string(encoder, length, TextStringType << MajorTypeShift, string);
}

#ifdef __GNUC__
__attribute__((noinline))
#endif
static CborError create_container(CborEncoder *encoder, CborEncoder *container, size_t length, uint8_t shiftedMajorType)
{
    CborError err;
    container->data.ptr = encoder->data.ptr;
    container->end = encoder->end;
    ++encoder->added;
    container->added = 0;

    cbor_static_assert(((MapType << MajorTypeShift) & CborIteratorFlag_ContainerIsMap) == CborIteratorFlag_ContainerIsMap);
    cbor_static_assert(((ArrayType << MajorTypeShift) & CborIteratorFlag_ContainerIsMap) == 0);
    container->flags = shiftedMajorType & CborIteratorFlag_ContainerIsMap;

    if (length == CborIndefiniteLength) {
        container->flags |= CborIteratorFlag_UnknownLength;
        err = append_byte_to_buffer(container, shiftedMajorType + IndefiniteLength);
    } else {
        err = encode_number_no_update(container, length, shiftedMajorType);
    }
    return err;
}

/**
 * Creates a CBOR array in the CBOR stream provided by \a encoder and
 * initializes \a arrayEncoder so that items can be added to the array using
 * the CborEncoder functions. The array must be terminated by calling either
 * cbor_encoder_close_container() or cbor_encoder_close_container_checked()
 * with the same \a encoder and \a arrayEncoder parameters.
 *
 * The number of items inserted into the array must be exactly \a length items,
 * otherwise the stream is invalid. If the number of items is not known when
 * creating the array, the constant \ref CborIndefiniteLength may be passed as
 * length instead.
 *
 * \sa cbor_encoder_create_map
 */
CborError cbor_encoder_create_array(CborEncoder *encoder, CborEncoder *arrayEncoder, size_t length)
{
    return create_container(encoder, arrayEncoder, length, ArrayType << MajorTypeShift);
}

/**
 * Creates a CBOR map in the CBOR stream provided by \a encoder and
 * initializes \a mapEncoder so that items can be added to the map using
 * the CborEncoder functions. The map must be terminated by calling either
 * cbor_encoder_close_container() or cbor_encoder_close_container_checked()
 * with the same \a encoder and \a mapEncoder parameters.
 *
 * The number of pair of items inserted into the map must be exactly \a length
 * items, otherwise the stream is invalid. If the number of items is not known
 * when creating the map, the constant \ref CborIndefiniteLength may be passed as
 * length instead.
 *
 * \b{Implementation limitation:} TinyCBOR cannot encode more than SIZE_MAX/2
 * key-value pairs in the stream. If the length \a length is larger than this
 * value, this function returns error CborErrorDataTooLarge.
 *
 * \sa cbor_encoder_create_array
 */
CborError cbor_encoder_create_map(CborEncoder *encoder, CborEncoder *mapEncoder, size_t length)
{
    if (length != CborIndefiniteLength && length > SIZE_MAX / 2)
        return CborErrorDataTooLarge;
    return create_container(encoder, mapEncoder, length, MapType << MajorTypeShift);
}

/**
 * Closes the CBOR container (array or map) provided by \a containerEncoder and
 * updates the CBOR stream provided by \a encoder. Both parameters must be the
 * same as were passed to cbor_encoder_create_array() or
 * cbor_encoder_create_map().
 *
 * This function does not verify that the number of items (or pair of items, in
 * the case of a map) was correct. To execute that verification, call
 * cbor_encoder_close_container_checked() instead.
 *
 * \sa cbor_encoder_create_array(), cbor_encoder_create_map()
 */
CborError cbor_encoder_close_container(CborEncoder *encoder, const CborEncoder *containerEncoder)
{
    if (encoder->end)
        encoder->data.ptr = containerEncoder->data.ptr;
    else
        encoder->data.bytes_needed = containerEncoder->data.bytes_needed;
    encoder->end = containerEncoder->end;
    if (containerEncoder->flags & CborIteratorFlag_UnknownLength)
        return append_byte_to_buffer(encoder, BreakByte);
    return CborNoError;
}

/**
 * \fn CborError cbor_encode_boolean(CborEncoder *encoder, bool value)
 *
 * Appends the boolean value \a value to the CBOR stream provided by \a encoder.
 */

/**
 * \fn CborError cbor_encode_null(CborEncoder *encoder)
 *
 * Appends the CBOR type representing a null value to the CBOR stream provided
 * by \a encoder.
 *
 * \sa cbor_encode_undefined()
 */

/**
 * \fn CborError cbor_encode_undefined(CborEncoder *encoder)
 *
 * Appends the CBOR type representing an undefined value to the CBOR stream
 * provided by \a encoder.
 *
 * \sa cbor_encode_null()
 */

/**
 * \fn CborError cbor_encode_half_float(CborEncoder *encoder, const void *value)
 *
 * Appends the IEEE 754 half-precision (16-bit) floating point value pointed to
 * by \a value to the CBOR stream provided by \a encoder.
 *
 * \sa cbor_encode_floating_point(), cbor_encode_float(), cbor_encode_double()
 */

/**
 * \fn CborError cbor_encode_float(CborEncoder *encoder, float value)
 *
 * Appends the IEEE 754 single-precision (32-bit) floating point value \a value
 * to the CBOR stream provided by \a encoder.
 *
 * \sa cbor_encode_floating_point(), cbor_encode_half_float(), cbor_encode_double()
 */

/**
 * \fn CborError cbor_encode_double(CborEncoder *encoder, double value)
 *
 * Appends the IEEE 754 double-precision (64-bit) floating point value \a value
 * to the CBOR stream provided by \a encoder.
 *
 * \sa cbor_encode_floating_point(), cbor_encode_half_float(), cbor_encode_float()
 */

/**
 * \fn size_t cbor_encoder_get_buffer_size(const CborEncoder *encoder, const uint8_t *buffer)
 *
 * Returns the total size of the buffer starting at \a buffer after the
 * encoding finished without errors. The \a encoder and \a buffer arguments
 * must be the same as supplied to cbor_encoder_init().
 *
 * If the encoding process had errors, the return value of this function is
 * meaningless. If the only errors were CborErrorOutOfMemory, instead use
 * cbor_encoder_get_extra_bytes_needed() to find out by how much to grow the
 * buffer before encoding again.
 *
 * See \ref CborEncoding for an example of using this function.
 *
 * \sa cbor_encoder_init(), cbor_encoder_get_extra_bytes_needed(), CborEncoding
 */

/**
 * \fn size_t cbor_encoder_get_extra_bytes_needed(const CborEncoder *encoder)
 *
 * Returns how many more bytes the original buffer supplied to
 * cbor_encoder_init() needs to be extended by so that no CborErrorOutOfMemory
 * condition will happen for the encoding. If the buffer was big enough, this
 * function returns 0. The \a encoder must be the original argument as passed
 * to cbor_encoder_init().
 *
 * This function is usually called after an encoding sequence ended with one or
 * more CborErrorOutOfMemory errors, but no other error. If any other error
 * happened, the return value of this function is meaningless.
 *
 * See \ref CborEncoding for an example of using this function.
 *
 * \sa cbor_encoder_init(), cbor_encoder_get_buffer_size(), CborEncoding
 */

/** @} */
