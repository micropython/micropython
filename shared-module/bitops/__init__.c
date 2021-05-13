/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/bitops/__init__.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "py/mpconfig.h"

// adapted from "Hacker's Delight" - Figure 7-2 Transposing an 8x8-bit matrix
// basic idea is:
// > First, treat the 8x8-bit matrix as 16 2x2-bit matrices, and transpose each
// > of the 16 2x2-bit matrices. Second, treat the matrix as four 2x2 submatrices
// > whose elements are 2x2-bit matrices and transpose each of the four 2x2
// > submatrices. Finally, treat the matrix as a 2x2 matrix whose elements are
// > 4x4-bit matrices, and transpose the 2x2 matrix. These transformations are
// > illustrated below.
// We want a different definition of bit/byte order, deal with strides differently, etc.
// so the code is heavily re-worked compared to the original.
static void transpose_var(uint32_t *result, const uint8_t *src, int src_stride, int num_strands) {
    uint32_t x = 0, y = 0, t;

    src += (num_strands - 1) * src_stride;

    switch (num_strands) {
        case 7:
            x |= *src << 16;
            src -= src_stride;
            MP_FALLTHROUGH;
        case 6:
            x |= *src << 8;
            src -= src_stride;
            MP_FALLTHROUGH;
        case 5:
            x |= *src;
            src -= src_stride;
            MP_FALLTHROUGH;
        case 4:
            y |= *src << 24;
            src -= src_stride;
            MP_FALLTHROUGH;
        case 3:
            y |= *src << 16;
            src -= src_stride;
            MP_FALLTHROUGH;
        case 2:
            y |= *src << 8;
            src -= src_stride;
            y |= *src;
    }

    t = (x ^ (x >> 7)) & 0x00AA00AA;
    x = x ^ t ^ (t << 7);
    t = (y ^ (y >> 7)) & 0x00AA00AA;
    y = y ^ t ^ (t << 7);

    t = (x ^ (x >> 14)) & 0x0000CCCC;
    x = x ^ t ^ (t << 14);
    t = (y ^ (y >> 14)) & 0x0000CCCC;
    y = y ^ t ^ (t << 14);

    t = (x & 0xF0F0F0F0) | ((y >> 4) & 0x0F0F0F0F);
    y = ((x << 4) & 0xF0F0F0F0) | (y & 0x0F0F0F0F);
    x = t;

    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    x = __builtin_bswap32(x);
    y = __builtin_bswap32(y);
    #endif
    result[0] = x;
    result[1] = y;
}

static void transpose_8(uint32_t *result, const uint8_t *src, int src_stride) {
    uint32_t x, y, t;

    y = *src;
    src += src_stride;
    y |= (*src << 8);
    src += src_stride;
    y |= (*src << 16);
    src += src_stride;
    y |= (*src << 24);
    src += src_stride;
    x = *src;
    src += src_stride;
    x |= (*src << 8);
    src += src_stride;
    x |= (*src << 16);
    src += src_stride;
    x |= (*src << 24);
    src += src_stride;

    t = (x ^ (x >> 7)) & 0x00AA00AA;
    x = x ^ t ^ (t << 7);
    t = (y ^ (y >> 7)) & 0x00AA00AA;
    y = y ^ t ^ (t << 7);

    t = (x ^ (x >> 14)) & 0x0000CCCC;
    x = x ^ t ^ (t << 14);
    t = (y ^ (y >> 14)) & 0x0000CCCC;
    y = y ^ t ^ (t << 14);

    t = (x & 0xF0F0F0F0) | ((y >> 4) & 0x0F0F0F0F);
    y = ((x << 4) & 0xF0F0F0F0) | (y & 0x0F0F0F0F);
    x = t;

    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    x = __builtin_bswap32(x);
    y = __builtin_bswap32(y);
    #endif
    result[0] = x;
    result[1] = y;
}

static void bit_transpose_8(uint32_t *result, const uint8_t *src, size_t src_stride, size_t n) {
    for (size_t i = 0; i < n; i++) {
        transpose_8(result, src, src_stride);
        result += 2;
        src += 1;
    }
}

static void bit_transpose_var(uint32_t *result, const uint8_t *src, size_t src_stride, size_t n, int num_strands) {
    for (size_t i = 0; i < n; i++) {
        transpose_var(result, src, src_stride, num_strands);
        result += 2;
        src += 1;
    }
}

void common_hal_bitops_bit_transpose(uint8_t *result, const uint8_t *src, size_t inlen, size_t num_strands) {
    if (num_strands == 8) {
        bit_transpose_8((uint32_t *)(void *)result, src, inlen / 8, inlen / 8);
    } else {
        bit_transpose_var((uint32_t *)(void *)result, src, inlen / num_strands, inlen / num_strands, num_strands);
    }
}
