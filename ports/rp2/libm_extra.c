/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

#include <math.h>
#include <stdint.h>

#define INF_POS (0x7f800000)
#define INF_NEG (0xff800000)

union float_int_t {
    float f;
    uint32_t i;
};

float __wrap___aeabi_fadd(float x, float y);

// The pico-sdk wraps __aeabi_fadd() with __wrap___aeabi_fadd() in order to use the ROM
// floating-point functions.  But __wrap___aeabi_fadd() does not handle inf+(-inf) or
// (-inf)+inf correctly.  To fix this we provide our own __aeabi_fadd() that fixes the
// inf calculation, and do not use the "--wrap=" linker command.  The compiler then
// picks our __aeabi_fadd() instead of its built-in one.  And the pico-sdk function
// still exists for us to call.
float __aeabi_fadd(float x, float y) {
    // Handle addition of inf/-inf.  This is optimised to reduce C stack usage, and
    // only have one comparison/jump in the common case of x != inf.
    union float_int_t xu = {.f = x};
    union float_int_t yu = {.f = y};
    if ((xu.i << 1) == (INF_POS << 1)) {
        if (xu.i == INF_POS) {
            if (yu.i == INF_POS) {
                return INFINITY;
            } else if (yu.i == INF_NEG) {
                return NAN;
            }
        } else {
            if (yu.i == INF_POS) {
                return NAN;
            } else if (yu.i == INF_NEG) {
                return -INFINITY;
            }
        }
    }

    // Use the pico-sdk function for all other calculations.
    return __wrap___aeabi_fadd(x, y);
}
