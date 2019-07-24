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

#include "cbor.h"

#ifndef _
#  define _(msg)    msg
#endif

/**
 * \enum CborError
 * \ingroup CborGlobals
 * The CborError enum contains the possible error values used by the CBOR encoder and decoder.
 *
 * TinyCBOR functions report success by returning CborNoError, or one error
 * condition by returning one of the values below. One exception is the
 * out-of-memory condition (CborErrorOutOfMemory), which the functions for \ref
 * CborEncoding may report in bit-wise OR with other conditions.
 *
 * This technique allows code to determine whether the only error condition was
 * a lack of buffer space, which may not be a fatal condition if the buffer can
 * be resized. Additionally, the functions for \ref CborEncoding may continue
 * to be used even after CborErrorOutOfMemory is returned, and instead they
 * will simply calculate the extra space needed.
 *
 * \value CborNoError                   No error occurred
 * \omitvalue CborUnknownError
 * \value CborErrorUnknownLength        Request for the length of an array, map or string whose length is not provided in the CBOR stream
 * \value CborErrorAdvancePastEOF       Not enough data in the stream to decode item (decoding would advance past end of stream)
 * \value CborErrorIO                   An I/O error occurred, probably due to an out-of-memory situation
 * \value CborErrorGarbageAtEnd         Bytes exist past the end of the CBOR stream
 * \value CborErrorUnexpectedEOF        End of stream reached unexpectedly
 * \value CborErrorUnexpectedBreak      A CBOR break byte was found where not expected
 * \value CborErrorUnknownType          An unknown type (future extension to CBOR) was found in the stream
 * \value CborErrorIllegalType          An invalid type was found while parsing a chunked CBOR string
 * \value CborErrorIllegalNumber        An illegal initial byte (encoding unspecified additional information) was found
 * \value CborErrorIllegalSimpleType    An illegal encoding of a CBOR Simple Type of value less than 32 was found
 * \omitvalue CborErrorUnknownSimpleType
 * \omitvalue CborErrorUnknownTag
 * \omitvalue CborErrorInappropriateTagForType
 * \omitvalue CborErrorDuplicateObjectKeys
 * \value CborErrorInvalidUtf8TextString Illegal UTF-8 encoding found while parsing CBOR Text String
 * \value CborErrorTooManyItems         Too many items were added to CBOR map or array of pre-determined length
 * \value CborErrorTooFewItems          Too few items were added to CBOR map or array of pre-determeined length
 * \value CborErrorDataTooLarge         Data item size exceeds TinyCBOR's implementation limits
 * \value CborErrorNestingTooDeep       Data item nesting exceeds TinyCBOR's implementation limits
 * \omitvalue CborErrorUnsupportedType
 * \value CborErrorJsonObjectKeyIsAggregate Conversion to JSON failed because the key in a map is a CBOR map or array
 * \value CborErrorJsonObjectKeyNotString Conversion to JSON failed because the key in a map is not a text string
 * \value CborErrorOutOfMemory          During CBOR encoding, the buffer provided is insufficient for encoding the data item;
 *                                      in other situations, TinyCBOR failed to allocate memory
 * \value CborErrorInternalError        An internal error occurred in TinyCBOR
 */

/**
 * \ingroup CborGlobals
 * Returns the error string corresponding to the CBOR error condition \a error.
 */
const char *cbor_error_string(CborError error)
{
    switch (error) {
    case CborNoError:
        return "";

    case CborUnknownError:
        return _("unknown error");

    case CborErrorOutOfMemory:
        return _("out of memory/need more memory");

    case CborErrorUnknownLength:
        return _("unknown length (attempted to get the length of a map/array/string of indeterminate length");

    case CborErrorAdvancePastEOF:
        return _("attempted to advance past EOF");

    case CborErrorIO:
        return _("I/O error");

    case CborErrorGarbageAtEnd:
        return _("garbage after the end of the content");

    case CborErrorUnexpectedEOF:
        return _("unexpected end of data");

    case CborErrorUnexpectedBreak:
        return _("unexpected 'break' byte");

    case CborErrorUnknownType:
        return _("illegal byte (encodes future extension type)");

    case CborErrorIllegalType:
        return _("mismatched string type in chunked string");

    case CborErrorIllegalNumber:
        return _("illegal initial byte (encodes unspecified additional information)");

    case CborErrorIllegalSimpleType:
        return _("illegal encoding of simple type smaller than 32");

    case CborErrorUnknownSimpleType:
        return _("unknown simple type");

    case CborErrorUnknownTag:
        return _("unknown tag");

    case CborErrorInappropriateTagForType:
        return _("inappropriate tag for type");

    case CborErrorDuplicateObjectKeys:
        return _("duplicate keys in object");

    case CborErrorInvalidUtf8TextString:
        return _("invalid UTF-8 content in string");

    case CborErrorTooManyItems:
        return _("too many items added to encoder");

    case CborErrorTooFewItems:
        return _("too few items added to encoder");

    case CborErrorDataTooLarge:
        return _("internal error: data too large");

    case CborErrorNestingTooDeep:
        return _("internal error: too many nested containers found in recursive function");

    case CborErrorUnsupportedType:
        return _("unsupported type");

    case CborErrorJsonObjectKeyIsAggregate:
        return _("conversion to JSON failed: key in object is an array or map");

    case CborErrorJsonObjectKeyNotString:
        return _("conversion to JSON failed: key in object is not a string");

    case CborErrorJsonNotImplemented:
        return _("conversion to JSON failed: open_memstream unavailable");

    case CborErrorInternalError:
        return _("internal error");
    }
    return cbor_error_string(CborUnknownError);
}
