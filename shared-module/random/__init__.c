/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/os/__init__.h"
#include "shared-bindings/time/__init__.h"

// Yasmarang random number generator
// by Ilya Levin
// http://www.literatecode.com/yasmarang
// Public Domain

STATIC uint32_t yasmarang_pad = 0xeda4baba, yasmarang_n = 69, yasmarang_d = 233;
STATIC uint8_t yasmarang_dat = 0;

STATIC uint32_t yasmarang(void) {
    if (yasmarang_pad == 0xeda4baba) {
        if (!common_hal_os_urandom((uint8_t *)&yasmarang_pad, sizeof(uint32_t))) {
            yasmarang_pad = common_hal_time_monotonic_ms() & 0xffffffff;
        }
    }
    yasmarang_pad += yasmarang_dat + yasmarang_d * yasmarang_n;
    yasmarang_pad = (yasmarang_pad << 3) + (yasmarang_pad >> 29);
    yasmarang_n = yasmarang_pad | 2;
    yasmarang_d ^= (yasmarang_pad << 31) + (yasmarang_pad >> 1);
    yasmarang_dat ^= (char)yasmarang_pad ^ (yasmarang_d >> 8) ^ 1;

    return yasmarang_pad ^ (yasmarang_d << 5) ^ (yasmarang_pad >> 18) ^ (yasmarang_dat << 1);
}  /* yasmarang */

// End of Yasmarang

// returns an unsigned integer below the given argument
// n must not be zero
STATIC uint32_t yasmarang_randbelow(uint32_t n) {
    uint32_t mask = 1;
    while ((n & mask) < n) {
        mask = (mask << 1) | 1;
    }
    uint32_t r;
    do {
        r = yasmarang() & mask;
    } while (r >= n);
    return r;
}

void shared_modules_random_seed(mp_uint_t seed) {
    yasmarang_pad = seed;
    yasmarang_n = 69;
    yasmarang_d = 233;
    yasmarang_dat = 0;
}

mp_uint_t shared_modules_random_getrandbits(uint8_t n) {
    uint32_t mask = ~0;
    // Beware of C undefined behavior when shifting by >= than bit size
    mask >>= (32 - n);
    return yasmarang() & mask;
}

mp_int_t shared_modules_random_randrange(mp_int_t start, mp_int_t stop, mp_int_t step) {
    mp_int_t n;
    if (step > 0) {
        n = (stop - start + step - 1) / step;
    } else {
        n = (stop - start + step + 1) / step;
    }
    return start + step * yasmarang_randbelow(n);
}

// returns a number in the range [0..1) using Yasmarang to fill in the fraction bits
STATIC mp_float_t yasmarang_float(void) {
    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
    typedef uint64_t mp_float_int_t;
    #elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    typedef uint32_t mp_float_int_t;
    #endif
    union {
        mp_float_t f;
        #if MP_ENDIANNESS_LITTLE
        struct { mp_float_int_t frc : MP_FLOAT_FRAC_BITS, exp : MP_FLOAT_EXP_BITS, sgn : 1;
        } p;
        #else
        struct { mp_float_int_t sgn : 1, exp : MP_FLOAT_EXP_BITS, frc : MP_FLOAT_FRAC_BITS;
        } p;
        #endif
    } u;
    u.p.sgn = 0;
    u.p.exp = (1 << (MP_FLOAT_EXP_BITS - 1)) - 1;
    if (MP_FLOAT_FRAC_BITS <= 32) {
        u.p.frc = yasmarang();
    } else {
        u.p.frc = ((uint64_t)yasmarang() << 32) | (uint64_t)yasmarang();
    }
    return u.f - 1;
}

mp_float_t shared_modules_random_random(void) {
    return yasmarang_float();
}

mp_float_t shared_modules_random_uniform(mp_float_t a, mp_float_t b) {
    return a + (b - a) * yasmarang_float();
}
