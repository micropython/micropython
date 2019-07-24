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
#include "extract_number_p.h"

#include <assert.h>
#include <string.h>

#include "assert_p.h"       /* Always include last */

#ifndef CBOR_PARSER_MAX_RECURSIONS
#  define CBOR_PARSER_MAX_RECURSIONS 1024
#endif

/**
 * \defgroup CborParsing Parsing CBOR streams
 * \brief Group of functions used to parse CBOR streams.
 *
 * TinyCBOR provides functions for pull-based stream parsing of a CBOR-encoded
 * payload. The main data type for the parsing is a CborValue, which behaves
 * like an iterator and can be used to extract the encoded data. It is first
 * initialized with a call to cbor_parser_init() and is usually used to extract
 * exactly one item, most often an array or map.
 *
 * Nested CborValue objects can be parsed using cbor_value_enter_container().
 * Each call to cbor_value_enter_container() must be matched by a call to
 * cbor_value_leave_container(), with the exact same parameters.
 *
 * The example below initializes a CborParser object, begins the parsing with a
 * CborValue and decodes a single integer:
 *
 * \code
 * int extract_int(const uint8_t *buffer, size_t len)
 * {
 *     CborParser parser;
 *     CborValue value;
 *     int result;
 *     cbor_parser_init(buffer, len, 0, &buffer, &value);
 *     cbor_value_get_int(&value, &result);
 *     return result;
 * }
 * \endcode
 *
 * The code above does no error checking, which means it assumes the data comes
 * from a source trusted to send one properly-encoded integer. The following
 * example does the exact same operation, but includes error parsing and
 * returns 0 on parsing failure:
 *
 * \code
 * int extract_int(const uint8_t *buffer, size_t len)
 * {
 *     CborParser parser;
 *     CborValue value;
 *     int result;
 *     if (cbor_parser_init(buffer, len, 0, &buffer, &value) != CborNoError)
 *         return 0;
 *     if (!cbor_value_is_integer(&value) ||
 *             cbor_value_get_int(&value, &result) != CborNoError)
 *         return 0;
 *     return result;
 * }
 * \endcode
 *
 * Note, in the example above, that one can't distinguish a parsing failure
 * from an encoded value of zero. Reporting a parsing error is left as an
 * exercise to the reader.
 *
 * The code above does not execute a range-check either: it is possible that
 * the value decoded from the CBOR stream encodes a number larger than what can
 * be represented in a variable of type \c{int}. If detecting that case is
 * important, the code should call cbor_value_get_int_checked() instead.
 *
 * <h3 class="groupheader">Memory and parsing constraints</h3>
 *
 * TinyCBOR is designed to run with little memory and with minimal overhead.
 * Except where otherwise noted, the parser functions always run on constant
 * time (O(1)), do not recurse and never allocate memory (thus, stack usage is
 * bounded and is O(1)).
 *
 * <h3 class="groupheader">Error handling and preconditions</h3>
 *
 * All functions operating on a CborValue return a CborError condition, with
 * CborNoError standing for the normal situation in which no parsing error
 * occurred. All functions may return parsing errors in case the stream cannot
 * be decoded properly, be it due to corrupted data or due to reaching the end
 * of the input buffer.
 *
 * Error conditions must not be ignored. All decoder functions have undefined
 * behavior if called after an error has been reported, and may crash.
 *
 * Some functions are also documented to have preconditions, like
 * cbor_value_get_int() requiring that the input be an integral value.
 * Violation of preconditions also results in undefined behavior and the
 * program may crash.
 */

/**
 * \addtogroup CborParsing
 * @{
 */

/**
 * \struct CborValue
 *
 * This type contains one value parsed from the CBOR stream. Each CborValue
 * behaves as an iterator in a StAX-style parser.
 *
 * \if privatedocs
 * Implementation details: the CborValue contains these fields:
 * \list
 *   \li ptr: pointer to the actual data
 *   \li flags: flags from the decoder
 *   \li extra: partially decoded integer value (0, 1 or 2 bytes)
 *   \li remaining: remaining items in this collection after this item or UINT32_MAX if length is unknown
 * \endlist
 * \endif
 */

static CborError extract_length(const CborParser *parser, const uint8_t **ptr, size_t *len)
{
    uint64_t v;
    CborError err = extract_number(ptr, parser->end, &v);
    if (err) {
        *len = 0;
        return err;
    }

    *len = (size_t)v;
    if (v != *len)
        return CborErrorDataTooLarge;
    return CborNoError;
}

static bool is_fixed_type(uint8_t type)
{
    return type != CborTextStringType && type != CborByteStringType && type != CborArrayType &&
           type != CborMapType;
}

static CborError preparse_value(CborValue *it)
{
    const CborParser *parser = it->parser;
    it->type = CborInvalidType;

    /* are we at the end? */
    if (it->ptr == parser->end)
        return CborErrorUnexpectedEOF;

    uint8_t descriptor = *it->ptr;
    uint8_t type = descriptor & MajorTypeMask;
    it->type = type;
    it->flags = 0;
    it->extra = (descriptor &= SmallValueMask);

    if (descriptor > Value64Bit) {
        if (unlikely(descriptor != IndefiniteLength))
            return type == CborSimpleType ? CborErrorUnknownType : CborErrorIllegalNumber;
        if (likely(!is_fixed_type(type))) {
            /* special case */
            it->flags |= CborIteratorFlag_UnknownLength;
            it->type = type;
            return CborNoError;
        }
        return type == CborSimpleType ? CborErrorUnexpectedBreak : CborErrorIllegalNumber;
    }

    size_t bytesNeeded = descriptor < Value8Bit ? 0 : (1 << (descriptor - Value8Bit));
    if (bytesNeeded + 1 > (size_t)(parser->end - it->ptr))
        return CborErrorUnexpectedEOF;

    uint8_t majortype = type >> MajorTypeShift;
    if (majortype == NegativeIntegerType) {
        it->flags |= CborIteratorFlag_NegativeInteger;
        it->type = CborIntegerType;
    } else if (majortype == SimpleTypesType) {
        switch (descriptor) {
        case FalseValue:
            it->extra = false;
            it->type = CborBooleanType;
            break;

        case SinglePrecisionFloat:
        case DoublePrecisionFloat:
            it->flags |= CborIteratorFlag_IntegerValueTooLarge;
            /* fall through */
        case TrueValue:
        case NullValue:
        case UndefinedValue:
        case HalfPrecisionFloat:
            it->type = *it->ptr;
            break;

        case SimpleTypeInNextByte:
            it->extra = (uint8_t)it->ptr[1];
#ifndef CBOR_PARSER_NO_STRICT_CHECKS
            if (unlikely(it->extra < 32)) {
                it->type = CborInvalidType;
                return CborErrorIllegalSimpleType;
            }
#endif
            break;

        case 28:
        case 29:
        case 30:
        case Break:
            assert(false);  /* these conditions can't be reached */
            return CborErrorUnexpectedBreak;
        }
        return CborNoError;
    }

    /* try to decode up to 16 bits */
    if (descriptor < Value8Bit)
        return CborNoError;

    if (descriptor == Value8Bit)
        it->extra = (uint8_t)it->ptr[1];
    else if (descriptor == Value16Bit)
        it->extra = get16(it->ptr + 1);
    else
        it->flags |= CborIteratorFlag_IntegerValueTooLarge;     /* Value32Bit or Value64Bit */
    return CborNoError;
}

static CborError preparse_next_value(CborValue *it)
{
    if (it->remaining != UINT32_MAX) {
        /* don't decrement the item count if the current item is tag: they don't count */
        if (it->type != CborTagType && !--it->remaining) {
            it->type = CborInvalidType;
            return CborNoError;
        }
    } else if (it->remaining == UINT32_MAX && it->ptr != it->parser->end && *it->ptr == (uint8_t)BreakByte) {
        /* end of map or array */
        ++it->ptr;
        it->type = CborInvalidType;
        it->remaining = 0;
        return CborNoError;
    }

    return preparse_value(it);
}

static CborError advance_internal(CborValue *it)
{
    uint64_t length;
    CborError err = extract_number(&it->ptr, it->parser->end, &length);
    assert(err == CborNoError);

    if (it->type == CborByteStringType || it->type == CborTextStringType) {
        assert(length == (size_t)length);
        assert((it->flags & CborIteratorFlag_UnknownLength) == 0);
        it->ptr += length;
    }

    return preparse_next_value(it);
}

/** \internal
 *
 * Decodes the CBOR integer value when it is larger than the 16 bits available
 * in value->extra. This function requires that value->flags have the
 * CborIteratorFlag_IntegerValueTooLarge flag set.
 *
 * This function is also used to extract single- and double-precision floating
 * point values (SinglePrecisionFloat == Value32Bit and DoublePrecisionFloat ==
 * Value64Bit).
 */
uint64_t _cbor_value_decode_int64_internal(const CborValue *value)
{
    assert(value->flags & CborIteratorFlag_IntegerValueTooLarge ||
           value->type == CborFloatType || value->type == CborDoubleType);

    /* since the additional information can only be Value32Bit or Value64Bit,
     * we just need to test for the one bit those two options differ */
    assert((*value->ptr & SmallValueMask) == Value32Bit || (*value->ptr & SmallValueMask) == Value64Bit);
    if ((*value->ptr & 1) == (Value32Bit & 1))
        return get32(value->ptr + 1);

    assert((*value->ptr & SmallValueMask) == Value64Bit);
    return get64(value->ptr + 1);
}

/**
 * Initializes the CBOR parser for parsing \a size bytes beginning at \a
 * buffer. Parsing will use flags set in \a flags. The iterator to the first
 * element is returned in \a it.
 *
 * The \a parser structure needs to remain valid throughout the decoding
 * process. It is not thread-safe to share one CborParser among multiple
 * threads iterating at the same time, but the object can be copied so multiple
 * threads can iterate.
 */
CborError cbor_parser_init(const uint8_t *buffer, size_t size, int flags, CborParser *parser, CborValue *it)
{
    memset(parser, 0, sizeof(*parser));
    parser->end = buffer + size;
    parser->flags = flags;
    it->parser = parser;
    it->ptr = buffer;
    it->remaining = 1;      /* there's one type altogether, usually an array or map */
    return preparse_value(it);
}

/**
 * \fn bool cbor_value_at_end(const CborValue *it)
 *
 * Returns true if \a it has reached the end of the iteration, usually when
 * advancing after the last item in an array or map.
 *
 * In the case of the outermost CborValue object, this function returns true
 * after decoding a single element. A pointer to the first byte of the
 * remaining data (if any) can be obtained with cbor_value_get_next_byte().
 *
 * \sa cbor_value_advance(), cbor_value_is_valid(), cbor_value_get_next_byte()
 */

/**
 * \fn const uint8_t *cbor_value_get_next_byte(const CborValue *it)
 *
 * Returns a pointer to the next byte that would be decoded if this CborValue
 * object were advanced.
 *
 * This function is useful if cbor_value_at_end() returns true for the
 * outermost CborValue: the pointer returned is the first byte of the data
 * remaining in the buffer, if any. Code can decide whether to begin decoding a
 * new CBOR data stream from this point, or parse some other data appended to
 * the same buffer.
 *
 * This function may be used even after a parsing error. If that occurred,
 * then this function returns a pointer to where the parsing error occurred.
 * Note that the error recovery is not precise and the pointer may not indicate
 * the exact byte containing bad data.
 *
 * \sa cbor_value_at_end()
 */

/**
 * \fn bool cbor_value_is_valid(const CborValue *it)
 *
 * Returns true if the iterator \a it contains a valid value. Invalid iterators
 * happen when iteration reaches the end of a container (see \ref
 * cbor_value_at_end()) or when a search function resulted in no matches.
 *
 * \sa cbor_value_advance(), cbor_valie_at_end(), cbor_value_get_type()
 */

/**
 * Advances the CBOR value \a it by one fixed-size position. Fixed-size types
 * are: integers, tags, simple types (including boolean, null and undefined
 * values) and floating point types.
 *
 * If the type is not of fixed size, this function has undefined behavior. Code
 * must be sure that the current type is one of the fixed-size types before
 * calling this function. This function is provided because it can guarantee
 * that runs in constant time (O(1)).
 *
 * If the caller is not able to determine whether the type is fixed or not, code
 * can use the cbor_value_advance() function instead.
 *
 * \sa cbor_value_at_end(), cbor_value_advance(), cbor_value_enter_container(), cbor_value_leave_container()
 */
CborError cbor_value_advance_fixed(CborValue *it)
{
    assert(it->type != CborInvalidType);
    assert(is_fixed_type(it->type));
    if (!it->remaining)
        return CborErrorAdvancePastEOF;
    return advance_internal(it);
}

static CborError advance_recursive(CborValue *it, int nestingLevel)
{
    if (is_fixed_type(it->type))
        return advance_internal(it);

    if (!cbor_value_is_container(it)) {
        size_t len = SIZE_MAX;
        return _cbor_value_copy_string(it, NULL, &len, it);
    }

    /* map or array */
    if (nestingLevel == CBOR_PARSER_MAX_RECURSIONS)
        return CborErrorNestingTooDeep;

    CborError err;
    CborValue recursed;
    err = cbor_value_enter_container(it, &recursed);
    if (err)
        return err;
    while (!cbor_value_at_end(&recursed)) {
        err = advance_recursive(&recursed, nestingLevel + 1);
        if (err)
            return err;
    }
    return cbor_value_leave_container(it, &recursed);
}


/**
 * Advances the CBOR value \a it by one element, skipping over containers.
 * Unlike cbor_value_advance_fixed(), this function can be called on a CBOR
 * value of any type. However, if the type is a container (map or array) or a
 * string with a chunked payload, this function will not run in constant time
 * and will recurse into itself (it will run on O(n) time for the number of
 * elements or chunks and will use O(n) memory for the number of nested
 * containers).
 *
 * \sa cbor_value_at_end(), cbor_value_advance_fixed(), cbor_value_enter_container(), cbor_value_leave_container()
 */
CborError cbor_value_advance(CborValue *it)
{
    assert(it->type != CborInvalidType);
    if (!it->remaining)
        return CborErrorAdvancePastEOF;
    return advance_recursive(it, 0);
}

/**
 * \fn bool cbor_value_is_tag(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR tag.
 *
 * \sa cbor_value_get_tag(), cbor_value_skip_tag()
 */

/**
 * \fn CborError cbor_value_get_tag(const CborValue *value, CborTag *result)
 *
 * Retrieves the CBOR tag value that \a value points to and stores it in \a
 * result. If the iterator \a value does not point to a CBOR tag value, the
 * behavior is undefined, so checking with \ref cbor_value_get_type or with
 * \ref cbor_value_is_tag is recommended.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_tag()
 */

/**
 * Advances the CBOR value \a it until it no longer points to a tag. If \a it is
 * already not pointing to a tag, then this function returns it unchanged.
 *
 * This function does not run in constant time: it will run on O(n) for n being
 * the number of tags. It does use constant memory (O(1) memory requirements).
 *
 * \sa cbor_value_advance_fixed(), cbor_value_advance()
 */
CborError cbor_value_skip_tag(CborValue *it)
{
    while (cbor_value_is_tag(it)) {
        CborError err = cbor_value_advance_fixed(it);
        if (err)
            return err;
    }
    return CborNoError;
}

/**
 * \fn bool cbor_value_is_container(const CborValue *it)
 *
 * Returns true if the \a it value is a container and requires recursion in
 * order to decode (maps and arrays), false otherwise.
 */

/**
 * Creates a CborValue iterator pointing to the first element of the container
 * represented by \a it and saves it in \a recursed. The \a it container object
 * needs to be kept and passed again to cbor_value_leave_container() in order
 * to continue iterating past this container.
 *
 * The \a it CborValue iterator must point to a container.
 *
 * \sa cbor_value_is_container(), cbor_value_leave_container(), cbor_value_advance()
 */
CborError cbor_value_enter_container(const CborValue *it, CborValue *recursed)
{
    CborError err;
    assert(cbor_value_is_container(it));
    *recursed = *it;

    if (it->flags & CborIteratorFlag_UnknownLength) {
        recursed->remaining = UINT32_MAX;
        ++recursed->ptr;
        err = preparse_value(recursed);
        if (err != CborErrorUnexpectedBreak)
            return err;
        /* actually, break was expected here
         * it's just an empty container */
        ++recursed->ptr;
    } else {
        uint64_t len;
        err = extract_number(&recursed->ptr, recursed->parser->end, &len);
        assert(err == CborNoError);

        recursed->remaining = (uint32_t)len;
        if (recursed->remaining != len || len == UINT32_MAX) {
            /* back track the pointer to indicate where the error occurred */
            recursed->ptr = it->ptr;
            return CborErrorDataTooLarge;
        }
        if (recursed->type == CborMapType) {
            /* maps have keys and values, so we need to multiply by 2 */
            if (recursed->remaining > UINT32_MAX / 2) {
                /* back track the pointer to indicate where the error occurred */
                recursed->ptr = it->ptr;
                return CborErrorDataTooLarge;
            }
            recursed->remaining *= 2;
        }
        if (len != 0)
            return preparse_value(recursed);
    }

    /* the case of the empty container */
    recursed->type = CborInvalidType;
    recursed->remaining = 0;
    return CborNoError;
}

/**
 * Updates \a it to point to the next element after the container. The \a
 * recursed object needs to point to the element obtained either by advancing
 * the last element of the container (via cbor_value_advance(),
 * cbor_value_advance_fixed(), a nested cbor_value_leave_container(), or the \c
 * next pointer from cbor_value_copy_string() or cbor_value_dup_string()).
 *
 * The \a it and \a recursed parameters must be the exact same as passed to
 * cbor_value_enter_container().
 *
 * \sa cbor_value_enter_container(), cbor_value_at_end()
 */
CborError cbor_value_leave_container(CborValue *it, const CborValue *recursed)
{
    assert(cbor_value_is_container(it));
    assert(recursed->type == CborInvalidType);
    it->ptr = recursed->ptr;
    return preparse_next_value(it);
}


/**
 * \fn CborType cbor_value_get_type(const CborValue *value)
 *
 * Returns the type of the CBOR value that the iterator \a value points to. If
 * \a value does not point to a valid value, this function returns \ref
 * CborInvalidType.
 *
 * TinyCBOR also provides functions to test directly if a given CborValue object
 * is of a given type, like cbor_value_is_text_string() and cbor_value_is_null().
 *
 * \sa cbor_value_is_valid()
 */

/**
 * \fn bool cbor_value_is_null(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR null type.
 *
 * \sa cbor_value_is_valid(), cbor_value_is_undefined()
 */

/**
 * \fn bool cbor_value_is_undefined(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR undefined type.
 *
 * \sa cbor_value_is_valid(), cbor_value_is_null()
 */

/**
 * \fn bool cbor_value_is_boolean(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR boolean
 * type (true or false).
 *
 * \sa cbor_value_is_valid(), cbor_value_get_boolean()
 */

/**
 * \fn CborError cbor_value_get_boolean(const CborValue *value, bool *result)
 *
 * Retrieves the boolean value that \a value points to and stores it in \a
 * result. If the iterator \a value does not point to a boolean value, the
 * behavior is undefined, so checking with \ref cbor_value_get_type or with
 * \ref cbor_value_is_boolean is recommended.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_boolean()
 */

/**
 * \fn bool cbor_value_is_simple_type(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR Simple Type
 * type (other than true, false, null and undefined).
 *
 * \sa cbor_value_is_valid(), cbor_value_get_simple_type()
 */

/**
 * \fn CborError cbor_value_get_simple_type(const CborValue *value, uint8_t *result)
 *
 * Retrieves the CBOR Simple Type value that \a value points to and stores it
 * in \a result. If the iterator \a value does not point to a simple_type
 * value, the behavior is undefined, so checking with \ref cbor_value_get_type
 * or with \ref cbor_value_is_simple_type is recommended.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_simple_type()
 */

/**
 * \fn bool cbor_value_is_integer(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR integer
 * type.
 *
 * \sa cbor_value_is_valid(), cbor_value_get_int, cbor_value_get_int64, cbor_value_get_uint64, cbor_value_get_raw_integer
 */

/**
 * \fn bool cbor_value_is_unsigned_integer(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR unsigned
 * integer type (positive values or zero).
 *
 * \sa cbor_value_is_valid(), cbor_value_get_uint64()
 */

/**
 * \fn bool cbor_value_is_negative_integer(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR negative
 * integer type.
 *
 * \sa cbor_value_is_valid(), cbor_value_get_int, cbor_value_get_int64, cbor_value_get_raw_integer
 */

/**
 * \fn CborError cbor_value_get_int(const CborValue *value, int *result)
 *
 * Retrieves the CBOR integer value that \a value points to and stores it in \a
 * result. If the iterator \a value does not point to an integer value, the
 * behavior is undefined, so checking with \ref cbor_value_get_type or with
 * \ref cbor_value_is_integer is recommended.
 *
 * Note that this function does not do range-checking: integral values that do
 * not fit in a variable of type \c{int} are silently truncated to fit. Use
 * cbor_value_get_int_checked() that is not acceptable.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_integer()
 */

/**
 * \fn CborError cbor_value_get_int64(const CborValue *value, int64_t *result)
 *
 * Retrieves the CBOR integer value that \a value points to and stores it in \a
 * result. If the iterator \a value does not point to an integer value, the
 * behavior is undefined, so checking with \ref cbor_value_get_type or with
 * \ref cbor_value_is_integer is recommended.
 *
 * Note that this function does not do range-checking: integral values that do
 * not fit in a variable of type \c{int64_t} are silently truncated to fit. Use
 * cbor_value_get_int64_checked() that is not acceptable.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_integer()
 */

/**
 * \fn CborError cbor_value_get_uint64(const CborValue *value, uint64_t *result)
 *
 * Retrieves the CBOR integer value that \a value points to and stores it in \a
 * result. If the iterator \a value does not point to an unsigned integer
 * value, the behavior is undefined, so checking with \ref cbor_value_get_type
 * or with \ref cbor_value_is_unsigned_integer is recommended.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_unsigned_integer()
 */

/**
 * \fn CborError cbor_value_get_raw_integer(const CborValue *value, uint64_t *result)
 *
 * Retrieves the CBOR integer value that \a value points to and stores it in \a
 * result. If the iterator \a value does not point to an integer value, the
 * behavior is undefined, so checking with \ref cbor_value_get_type or with
 * \ref cbor_value_is_integer is recommended.
 *
 * This function is provided because CBOR negative integers can assume values
 * that cannot be represented with normal 64-bit integer variables.
 *
 * If the integer is unsigned (that is, if cbor_value_is_unsigned_integer()
 * returns true), then \a result will contain the actual value. If the integer
 * is negative, then \a result will contain the absolute value of that integer,
 * minus one. That is, \c {actual = -result - 1}. On architectures using two's
 * complement for representation of negative integers, it is equivalent to say
 * that \a result will contain the bitwise negation of the actual value.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_integer()
 */

/**
 * Retrieves the CBOR integer value that \a value points to and stores it in \a
 * result. If the iterator \a value does not point to an integer value, the
 * behavior is undefined, so checking with \ref cbor_value_get_type or with
 * \ref cbor_value_is_integer is recommended.
 *
 * Unlike cbor_value_get_int64(), this function performs a check to see if the
 * stored integer fits in \a result without data loss. If the number is outside
 * the valid range for the data type, this function returns the recoverable
 * error CborErrorDataTooLarge. In that case, use either
 * cbor_value_get_uint64() (if the number is positive) or
 * cbor_value_get_raw_integer().
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_integer(), cbor_value_get_int64()
 */
CborError cbor_value_get_int64_checked(const CborValue *value, int64_t *result)
{
    assert(cbor_value_is_integer(value));
    uint64_t v = _cbor_value_extract_int64_helper(value);

    /* Check before converting, as the standard says (C11 6.3.1.3 paragraph 3):
     * "[if] the new type is signed and the value cannot be represented in it; either the
     *  result is implementation-defined or an implementation-defined signal is raised."
     *
     * The range for int64_t is -2^63 to 2^63-1 (int64_t is required to be
     * two's complement, C11 7.20.1.1 paragraph 3), which in CBOR is
     * represented the same way, differing only on the "sign bit" (the major
     * type).
     */

    if (unlikely(v > (uint64_t)INT64_MAX))
        return CborErrorDataTooLarge;

    *result = v;
    if (value->flags & CborIteratorFlag_NegativeInteger)
        *result = -*result - 1;
    return CborNoError;
}

/**
 * Retrieves the CBOR integer value that \a value points to and stores it in \a
 * result. If the iterator \a value does not point to an integer value, the
 * behavior is undefined, so checking with \ref cbor_value_get_type or with
 * \ref cbor_value_is_integer is recommended.
 *
 * Unlike cbor_value_get_int(), this function performs a check to see if the
 * stored integer fits in \a result without data loss. If the number is outside
 * the valid range for the data type, this function returns the recoverable
 * error CborErrorDataTooLarge. In that case, use one of the other integer
 * functions to obtain the value.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_integer(), cbor_value_get_int64(),
 *     cbor_value_get_uint64(), cbor_value_get_int64_checked(), cbor_value_get_raw_integer()
 */
CborError cbor_value_get_int_checked(const CborValue *value, int *result)
{
    assert(cbor_value_is_integer(value));
    uint64_t v = _cbor_value_extract_int64_helper(value);

    /* Check before converting, as the standard says (C11 6.3.1.3 paragraph 3):
     * "[if] the new type is signed and the value cannot be represented in it; either the
     *  result is implementation-defined or an implementation-defined signal is raised."
     *
     * But we can convert from signed to unsigned without fault (paragraph 2).
     *
     * The range for int is implementation-defined and int is not guaranteed use
     * two's complement representation (int32_t is).
     */

    if (value->flags & CborIteratorFlag_NegativeInteger) {
        if (unlikely(v > (unsigned) -(INT_MIN + 1)))
            return CborErrorDataTooLarge;

        *result = (int)v;
        *result = -*result - 1;
    } else {
        if (unlikely(v > (uint64_t)INT_MAX))
            return CborErrorDataTooLarge;

        *result = (int)v;
    }
    return CborNoError;

}

/**
 * \fn bool cbor_value_is_length_known(const CborValue *value)
 *
 * Returns true if the length of this type is known without calculation. That
 * is, if the length of this CBOR string, map or array is encoded in the data
 * stream, this function returns true. If the length is not encoded, it returns
 * false.
 *
 * If the length is known, code can call cbor_value_get_string_length(),
 * cbor_value_get_array_length() or cbor_value_get_map_length() to obtain the
 * length. If the length is not known but is necessary, code can use the
 * cbor_value_calculate_string_length() function (no equivalent function is
 * provided for maps and arrays).
 */

/**
 * \fn bool cbor_value_is_text_string(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR text
 * string. CBOR text strings are UTF-8 encoded and usually contain
 * human-readable text.
 *
 * \sa cbor_value_is_valid(), cbor_value_get_string_length(), cbor_value_calculate_string_length(),
 *     cbor_value_copy_text_string(), cbor_value_dup_text_string()
 */

/**
 * \fn bool cbor_value_is_byte_string(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR text
 * string. CBOR byte strings are binary data with no specified encoding or
 * format.
 *
 * \sa cbor_value_is_valid(), cbor_value_get_string_length(), cbor_value_calculate_string_length(),
 *     cbor_value_copy_byte_string(), cbor_value_dup_byte_string()
 */

/**
 * \fn CborError cbor_value_get_string_length(const CborValue *value, size_t *length)
 *
 * Extracts the length of the byte or text string that \a value points to and
 * stores it in \a result. If the iterator \a value does not point to a text
 * string or a byte string, the behaviour is undefined, so checking with \ref
 * cbor_value_get_type, with \ref cbor_value_is_text_string or \ref
 * cbor_value_is_byte_string is recommended.
 *
 * If the length of this string is not encoded in the CBOR data stream, this
 * function will return the recoverable error CborErrorUnknownLength. You may
 * also check whether that is the case by using cbor_value_is_length_known().
 *
 * If the length of the string is required but the length was not encoded, use
 * cbor_value_calculate_string_length(), but note that that function does not
 * run in constant time.
 *
 * \note On 32-bit platforms, this function will return error condition of \ref
 * CborErrorDataTooLarge if the stream indicates a length that is too big to
 * fit in 32-bit.
 *
 * \sa cbor_value_is_valid(), cbor_value_is_length_known(), cbor_value_calculate_string_length()
 */

/**
 * Calculates the length of the byte or text string that \a value points to and
 * stores it in \a len. If the iterator \a value does not point to a text
 * string or a byte string, the behaviour is undefined, so checking with \ref
 * cbor_value_get_type, with \ref cbor_value_is_text_string or \ref
 * cbor_value_is_byte_string is recommended.
 *
 * This function is different from cbor_value_get_string_length() in that it
 * calculates the length even for strings sent in chunks. For that reason, this
 * function may not run in constant time (it will run in O(n) time on the
 * number of chunks). It does use constant memory (O(1)).
 *
 * \note On 32-bit platforms, this function will return error condition of \ref
 * CborErrorDataTooLarge if the stream indicates a length that is too big to
 * fit in 32-bit.
 *
 * \sa cbor_value_get_string_length(), cbor_value_copy_string(), cbor_value_is_length_known()
 */
CborError cbor_value_calculate_string_length(const CborValue *value, size_t *len)
{
    *len = SIZE_MAX;
    return _cbor_value_copy_string(value, NULL, len, NULL);
}

/* We return uintptr_t so that we can pass memcpy directly as the iteration
 * function. The choice is to optimize for memcpy, which is used in the base
 * parser API (cbor_value_copy_string), while memcmp is used in convenience API
 * only. */
typedef uintptr_t (*IterateFunction)(char *, const uint8_t *, size_t);

static uintptr_t iterate_noop(char *dest, const uint8_t *src, size_t len)
{
    (void)dest;
    (void)src;
    (void)len;
    return true;
}

static uintptr_t iterate_memcmp(char *s1, const uint8_t *s2, size_t len)
{
    return memcmp(s1, (const char *)s2, len) == 0;
}

static uintptr_t iterate_memcpy(char *dest, const uint8_t *src, size_t len)
{
    return (uintptr_t)memcpy(dest, src, len);
}

static CborError iterate_string_chunks(const CborValue *value, char *buffer, size_t *buflen,
                                       bool *result, CborValue *next, IterateFunction func)
{
    assert(cbor_value_is_byte_string(value) || cbor_value_is_text_string(value));

    size_t total;
    CborError err;
    const uint8_t *ptr = value->ptr;
    if (cbor_value_is_length_known(value)) {
        /* easy case: fixed length */
        err = extract_length(value->parser, &ptr, &total);
        if (err)
            return err;
        if (total > (size_t)(value->parser->end - ptr))
            return CborErrorUnexpectedEOF;
        if (total <= *buflen)
            *result = !!func(buffer, ptr, total);
        else
            *result = false;
        ptr += total;
    } else {
        /* chunked */
        ++ptr;
        total = 0;
        *result = true;
        while (true) {
            size_t chunkLen;
            size_t newTotal;

            if (ptr == value->parser->end)
                return CborErrorUnexpectedEOF;

            if (*ptr == (uint8_t)BreakByte) {
                ++ptr;
                break;
            }

            /* is this the right type? */
            if ((*ptr & MajorTypeMask) != value->type)
                return CborErrorIllegalType;

            err = extract_length(value->parser, &ptr, &chunkLen);
            if (err)
                return err;

            if (unlikely(add_check_overflow(total, chunkLen, &newTotal)))
                return CborErrorDataTooLarge;

            if (chunkLen > (size_t)(value->parser->end - ptr))
                return CborErrorUnexpectedEOF;

            if (*result && *buflen >= newTotal)
                *result = !!func(buffer + total, ptr, chunkLen);
            else
                *result = false;

            ptr += chunkLen;
            total = newTotal;
        }
    }

    /* is there enough room for the ending NUL byte? */
    if (*result && *buflen > total) {
        uint8_t nul[] = { 0 };
        *result = !!func(buffer + total, nul, 1);
    }
    *buflen = total;

    if (next) {
        *next = *value;
        next->ptr = ptr;
        return preparse_next_value(next);
    }
    return CborNoError;
}

/**
 * \fn CborError cbor_value_copy_text_string(const CborValue *value, char *buffer, size_t *buflen, CborValue *next)
 *
 * Copies the string pointed by \a value into the buffer provided at \a buffer
 * of \a buflen bytes. If \a buffer is a NULL pointer, this function will not
 * copy anything and will only update the \a next value.
 *
 * If the iterator \a value does not point to a text string, the behaviour is
 * undefined, so checking with \ref cbor_value_get_type or \ref
 * cbor_value_is_text_string is recommended.
 *
 * If the provided buffer length was too small, this function returns an error
 * condition of \ref CborErrorOutOfMemory. If you need to calculate the length
 * of the string in order to preallocate a buffer, use
 * cbor_value_calculate_string_length().
 *
 * On success, this function sets the number of bytes copied to \c{*buflen}. If
 * the buffer is large enough, this function will insert a null byte after the
 * last copied byte, to facilitate manipulation of text strings. That byte is
 * not included in the returned value of \c{*buflen}.
 *
 * The \a next pointer, if not null, will be updated to point to the next item
 * after this string. If \a value points to the last item, then \a next will be
 * invalid.
 *
 * This function may not run in constant time (it will run in O(n) time on the
 * number of chunks). It requires constant memory (O(1)).
 *
 * \note This function does not perform UTF-8 validation on the incoming text
 * string.
 *
 * \sa cbor_value_dup_text_string(), cbor_value_copy_byte_string(), cbor_value_get_string_length(), cbor_value_calculate_string_length()
 */

/**
 * \fn CborError cbor_value_copy_byte_string(const CborValue *value, uint8_t *buffer, size_t *buflen, CborValue *next)
 *
 * Copies the string pointed by \a value into the buffer provided at \a buffer
 * of \a buflen bytes. If \a buffer is a NULL pointer, this function will not
 * copy anything and will only update the \a next value.
 *
 * If the iterator \a value does not point to a byte string, the behaviour is
 * undefined, so checking with \ref cbor_value_get_type or \ref
 * cbor_value_is_byte_string is recommended.
 *
 * If the provided buffer length was too small, this function returns an error
 * condition of \ref CborErrorOutOfMemory. If you need to calculate the length
 * of the string in order to preallocate a buffer, use
 * cbor_value_calculate_string_length().
 *
 * On success, this function sets the number of bytes copied to \c{*buflen}. If
 * the buffer is large enough, this function will insert a null byte after the
 * last copied byte, to facilitate manipulation of null-terminated strings.
 * That byte is not included in the returned value of \c{*buflen}.
 *
 * The \a next pointer, if not null, will be updated to point to the next item
 * after this string. If \a value points to the last item, then \a next will be
 * invalid.
 *
 * This function may not run in constant time (it will run in O(n) time on the
 * number of chunks). It requires constant memory (O(1)).
 *
 * \sa cbor_value_dup_text_string(), cbor_value_copy_text_string(), cbor_value_get_string_length(), cbor_value_calculate_string_length()
 */

CborError _cbor_value_copy_string(const CborValue *value, void *buffer,
                                 size_t *buflen, CborValue *next)
{
    bool copied_all;
    CborError err = iterate_string_chunks(value, (char*)buffer, buflen, &copied_all, next,
                                          buffer ? iterate_memcpy : iterate_noop);
    return err ? err :
                 copied_all ? CborNoError : CborErrorOutOfMemory;
}

/**
 * Compares the entry \a value with the string \a string and store the result
 * in \a result. If the value is different from \a string \a result will
 * contain \c false.
 *
 * The entry at \a value may be a tagged string. If \a is not a string or a
 * tagged string, the comparison result will be false.
 *
 * CBOR requires text strings to be encoded in UTF-8, but this function does
 * not validate either the strings in the stream or the string \a string to be
 * matched. Moreover, comparison is done on strict codepoint comparison,
 * without any Unicode normalization.
 *
 * This function may not run in constant time (it will run in O(n) time on the
 * number of chunks). It requires constant memory (O(1)).
 *
 * \sa cbor_value_skip_tag(), cbor_value_copy_text_string()
 */
CborError cbor_value_text_string_equals(const CborValue *value, const char *string, bool *result)
{
    CborValue copy = *value;
    CborError err = cbor_value_skip_tag(&copy);
    if (err)
        return err;
    if (!cbor_value_is_text_string(&copy)) {
        *result = false;
        return CborNoError;
    }

    size_t len = strlen(string);
    return iterate_string_chunks(&copy, CONST_CAST(char *, string), &len, result, NULL, iterate_memcmp);
}

/**
 * \fn bool cbor_value_is_array(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR array.
 *
 * \sa cbor_value_is_valid(), cbor_value_is_map()
 */

/**
 * \fn CborError cbor_value_get_array_length(const CborValue *value, size_t *length)
 *
 * Extracts the length of the CBOR array that \a value points to and stores it
 * in \a result. If the iterator \a value does not point to a CBOR array, the
 * behaviour is undefined, so checking with \ref cbor_value_get_type or \ref
 * cbor_value_is_array is recommended.
 *
 * If the length of this array is not encoded in the CBOR data stream, this
 * function will return the recoverable error CborErrorUnknownLength. You may
 * also check whether that is the case by using cbor_value_is_length_known().
 *
 * \note On 32-bit platforms, this function will return error condition of \ref
 * CborErrorDataTooLarge if the stream indicates a length that is too big to
 * fit in 32-bit.
 *
 * \sa cbor_value_is_valid(), cbor_value_is_length_known()
 */

/**
 * \fn bool cbor_value_is_map(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR map.
 *
 * \sa cbor_value_is_valid(), cbor_value_is_array()
 */

/**
 * \fn CborError cbor_value_get_map_length(const CborValue *value, size_t *length)
 *
 * Extracts the length of the CBOR map that \a value points to and stores it in
 * \a result. If the iterator \a value does not point to a CBOR map, the
 * behaviour is undefined, so checking with \ref cbor_value_get_type or \ref
 * cbor_value_is_map is recommended.
 *
 * If the length of this map is not encoded in the CBOR data stream, this
 * function will return the recoverable error CborErrorUnknownLength. You may
 * also check whether that is the case by using cbor_value_is_length_known().
 *
 * \note On 32-bit platforms, this function will return error condition of \ref
 * CborErrorDataTooLarge if the stream indicates a length that is too big to
 * fit in 32-bit.
 *
 * \sa cbor_value_is_valid(), cbor_value_is_length_known()
 */

/**
 * Attempts to find the value in map \a map that corresponds to the text string
 * entry \a string. If the iterator \a value does not point to a CBOR map, the
 * behaviour is undefined, so checking with \ref cbor_value_get_type or \ref
 * cbor_value_is_map is recommended.
 *
 * If the item is found, it is stored in \a result. If no item is found
 * matching the key, then \a result will contain an element of type \ref
 * CborInvalidType. Matching is performed using
 * cbor_value_text_string_equals(), so tagged strings will also match.
 *
 * This function has a time complexity of O(n) where n is the number of
 * elements in the map to be searched. In addition, this function is has O(n)
 * memory requirement based on the number of nested containers (maps or arrays)
 * found as elements of this map.
 *
 * \sa cbor_value_is_valid(), cbor_value_text_string_equals(), cbor_value_advance()
 */
CborError cbor_value_map_find_value(const CborValue *map, const char *string, CborValue *element)
{
    assert(cbor_value_is_map(map));
    size_t len = strlen(string);
    CborError err = cbor_value_enter_container(map, element);
    if (err)
        goto error;

    while (!cbor_value_at_end(element)) {
        /* find the non-tag so we can compare */
        err = cbor_value_skip_tag(element);
        if (err)
            goto error;
        if (cbor_value_is_text_string(element)) {
            bool equals;
            size_t dummyLen = len;
            err = iterate_string_chunks(element, CONST_CAST(char *, string), &dummyLen,
                                        &equals, element, iterate_memcmp);
            if (err)
                goto error;
            if (equals)
                return preparse_value(element);
        } else {
            /* skip this key */
            err = cbor_value_advance(element);
            if (err)
                goto error;
        }

        /* skip this value */
        err = cbor_value_skip_tag(element);
        if (err)
            goto error;
        err = cbor_value_advance(element);
        if (err)
            goto error;
    }

    /* not found */
    element->type = CborInvalidType;
    return CborNoError;

error:
    element->type = CborInvalidType;
    return err;
}

/**
 * \fn bool cbor_value_is_float(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR
 * single-precision floating point (32-bit).
 *
 * \sa cbor_value_is_valid(), cbor_value_is_double(), cbor_value_is_half_float()
 */

/**
 * \fn CborError cbor_value_get_float(const CborValue *value, float *result)
 *
 * Retrieves the CBOR single-precision floating point (32-bit) value that \a
 * value points to and stores it in \a result. If the iterator \a value does
 * not point to a single-precision floating point value, the behavior is
 * undefined, so checking with \ref cbor_value_get_type or with \ref
 * cbor_value_is_float is recommended.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_float(), cbor_value_get_double()
 */

/**
 * \fn bool cbor_value_is_double(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR
 * double-precision floating point (64-bit).
 *
 * \sa cbor_value_is_valid(), cbor_value_is_float(), cbor_value_is_half_float()
 */

/**
 * \fn CborError cbor_value_get_double(const CborValue *value, float *result)
 *
 * Retrieves the CBOR double-precision floating point (64-bit) value that \a
 * value points to and stores it in \a result. If the iterator \a value does
 * not point to a double-precision floating point value, the behavior is
 * undefined, so checking with \ref cbor_value_get_type or with \ref
 * cbor_value_is_double is recommended.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_double(), cbor_value_get_float()
 */

/**
 * \fn bool cbor_value_is_half_float(const CborValue *value)
 *
 * Returns true if the iterator \a value is valid and points to a CBOR
 * single-precision floating point (16-bit).
 *
 * \sa cbor_value_is_valid(), cbor_value_is_double(), cbor_value_is_float()
 */

/**
 * Retrieves the CBOR half-precision floating point (16-bit) value that \a
 * value points to and stores it in \a result. If the iterator \a value does
 * not point to a half-precision floating point value, the behavior is
 * undefined, so checking with \ref cbor_value_get_type or with \ref
 * cbor_value_is_half_float is recommended.
 *
 * Note: since the C language does not have a standard type for half-precision
 * floating point, this function takes a \c{void *} as a parameter for the
 * storage area, which must be at least 16 bits wide.
 *
 * \sa cbor_value_get_type(), cbor_value_is_valid(), cbor_value_is_half_float(), cbor_value_get_float()
 */
CborError cbor_value_get_half_float(const CborValue *value, void *result)
{
    assert(cbor_value_is_half_float(value));

    /* size has been computed already */
    uint16_t v = get16(value->ptr + 1);
    memcpy(result, &v, sizeof(v));
    return CborNoError;
}

/** @} */
