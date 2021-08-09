/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jim Mussared
 * Copyright (c) 2021 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/machine_bitstream.h"

#if MICROPY_PY_MACHINE_BITSTREAM

// Timing is a 4-tuple of (high_time_0, low_time_0, high_time_1, low_time_1)
// suitable for driving WS2812.
#define MICROPY_MACHINE_BITSTREAM_TYPE_HIGH_LOW (0)

// machine.bitstream(pin, encoding, (timing), bytes)
STATIC mp_obj_t machine_bitstream_(size_t n_args, const mp_obj_t *args) {
    mp_hal_pin_obj_t pin = mp_hal_get_pin_obj(args[0]);
    int encoding = mp_obj_get_int(args[1]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);

    switch (encoding) {
        case MICROPY_MACHINE_BITSTREAM_TYPE_HIGH_LOW: {
            uint32_t timing_ns[4];
            mp_obj_t *timing;
            mp_obj_get_array_fixed_n(args[2], 4, &timing);
            for (size_t i = 0; i < 4; ++i) {
                timing_ns[i] = mp_obj_get_int(timing[i]);
            }
            machine_bitstream_high_low(pin, timing_ns, bufinfo.buf, bufinfo.len);
            break;
        }
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("encoding"));
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_bitstream_obj, 4, 4, machine_bitstream_);

#endif // MICROPY_PY_MACHINE_BITSTREAM
