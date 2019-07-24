/****************************************************************************
**
** Copyright (C) 2015 Intel Corporation
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
#include "cbor.h"
#include "cborconstants_p.h"
#include "compilersupport_p.h"
#include <stdlib.h>

static inline uint16_t get16(const uint8_t *ptr)
{
    uint16_t result;
    memcpy(&result, ptr, sizeof(result));
    return cbor_ntohs(result);
}

static inline uint32_t get32(const uint8_t *ptr)
{
    uint32_t result;
    memcpy(&result, ptr, sizeof(result));
    return cbor_ntohl(result);
}

static inline uint64_t get64(const uint8_t *ptr)
{
    uint64_t result;
    memcpy(&result, ptr, sizeof(result));
    return cbor_ntohll(result);
}

static CborError extract_number(const uint8_t **ptr, const uint8_t *end, uint64_t *len)
{
    uint8_t additional_information = **ptr & SmallValueMask;
    ++*ptr;
    if (additional_information < Value8Bit) {
        *len = additional_information;
        return CborNoError;
    }
    if (unlikely(additional_information > Value64Bit))
        return CborErrorIllegalNumber;

    size_t bytesNeeded = (size_t)(1 << (additional_information - Value8Bit));
    if (unlikely(bytesNeeded > (size_t)(end - *ptr))) {
        return CborErrorUnexpectedEOF;
    } else if (bytesNeeded == 1) {
        *len = (uint8_t)(*ptr)[0];
    } else if (bytesNeeded == 2) {
        *len = get16(*ptr);
    } else if (bytesNeeded == 4) {
        *len = get32(*ptr);
    } else {
        *len = get64(*ptr);
    }
    *ptr += bytesNeeded;
    return CborNoError;
}
