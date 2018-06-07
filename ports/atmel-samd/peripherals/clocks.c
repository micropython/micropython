/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "clocks.h"

#include "hpl_gclk_config.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "py/runtime.h"

// TODO(tannewt): Should we have a way of sharing GCLKs based on their speed? Divisor doesn't
// gaurantee speed because it depends on the source.
uint8_t find_free_gclk(uint16_t divisor) {
    if (divisor > 0xff) {
        if (gclk_enabled(1)) {
            return 0xff;
        }
        return 1;
    }
    uint8_t first_8_bit = 2;
    #ifdef SAMD21
    first_8_bit = 3;
    if (divisor <= (1 << 5) && !gclk_enabled(2)) {
        return 2;
    }
    #endif
    for (uint8_t i = first_8_bit; i < GCLK_GEN_NUM; i++) {
        if (!gclk_enabled(i)) {
            return i;
        }
    }
    return 0xff;
}

static uint8_t last_static_clock = 0;

void init_dynamic_clocks(void) {
    // Find the last statically initialized clock and save it. Everything after will be reset with
    // the VM via reset_gclks.
    for (uint8_t i = 0; i < GCLK_GEN_NUM; i++) {
        if (gclk_enabled(i)) {
            last_static_clock = i;
        }
    }
}

void reset_gclks(void) {
    for (uint8_t i = last_static_clock + 1; i < GCLK_GEN_NUM; i++) {
        disable_gclk(i);
    }
}
