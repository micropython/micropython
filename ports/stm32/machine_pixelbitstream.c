/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/mphal.h"
#include "modmachine.h"

/*
Example usage:

import machine

class NeoPixel:
    ORDER = (1, 0, 2, 3)
    TIMING = (350, 800, 700, 600)

    def __init__(self, pin, n, bpp=3):
        self.pin = pin
        self.n = n
        self.bpp = bpp
        self.buf = bytearray(n * bpp)
        self.pin.init(pin.OUT)

    def __setitem__(self, index, val):
        offset = index * self.bpp
        for i in range(self.bpp):
            self.buf[offset + self.ORDER[i]] = val[i]

    def __getitem__(self, index):
        offset = index * self.bpp
        return tuple(self.buf[offset + self.ORDER[i]]
                     for i in range(self.bpp))

    def fill(self, color):
        for i in range(self.n):
            self[i] = color

    def write(self):
        machine.pixelbitstream(self.pin, self.TIMING, self.buf)


np = NeoPixel(machine.Pin('X1', machine.Pin.OUT), 8)

*/

#if 0
// STM32F091 @ 48MHz
#define NS_PRESCALE (6)
#define NS_OVERHEAD (1)
#define NS_OVERHEAD_LOW (2)
#else
// Tested on PYBv1.0
#define NS_PRESCALE (6)
#define NS_OVERHEAD (0)
#define NS_OVERHEAD_LOW (0)
#endif

uint32_t mp_hal_delay_ns_calc(uint32_t ns, uint32_t extra_overhead) {
    uint32_t nclk = SystemCoreClock / 1000000 * ns / 1000 / NS_PRESCALE;
    if (nclk <= NS_OVERHEAD + extra_overhead) {
        nclk = 1;
    } else {
        nclk -= NS_OVERHEAD + extra_overhead;
    }
    return nclk;
}

STATIC mp_obj_t machine_pixelbitstream(mp_obj_t pin_in, mp_obj_t timing_in, mp_obj_t buf_in) {
    // Get the pin to output to
    mp_hal_pin_obj_t pin = mp_hal_get_pin_obj(pin_in);

    // Get timing values (in ns) and convert to machine-dependent loop counters
    uint32_t timing_ns[4];
    mp_obj_t *timing;
    mp_obj_get_array_fixed_n(timing_in, 4, &timing);
    for (size_t i = 0; i < 4; ++i) {
        uint32_t overhead = i == 1 || i == 3 ? NS_OVERHEAD_LOW : 0; // extra overhead for low cycle
        timing_ns[i] = mp_hal_delay_ns_calc(mp_obj_get_int(timing[i]), overhead);
    }

    // Get buffer to write
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    for (size_t i = 0; i < bufinfo.len; ++i) {
        uint8_t b = ((uint8_t*)bufinfo.buf)[i];
        for (size_t j = 0; j < 8; ++j) {
            uint32_t *t = &timing_ns[b >> 6 & 2];
            mp_hal_pin_high(pin);
            uint32_t k = t[0];
            while (--k) {
                __NOP();
            }
            mp_hal_pin_low(pin);
            k = t[1];
            while (--k) {
                __NOP();
            }
            b <<= 1;
        }
    }

    MICROPY_END_ATOMIC_SECTION(atomic_state);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(machine_pixelbitstream_obj, machine_pixelbitstream);
