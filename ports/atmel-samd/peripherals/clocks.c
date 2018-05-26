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

void reset_gclks(void) {
    // Never reset GCLK0 because its used for the core
    #if CONF_GCLK_GEN_1_GENEN == 0
    disable_gclk(1);
    #endif
    #if CONF_GCLK_GEN_2_GENEN == 0
    disable_gclk(2);
    #endif
    #if CONF_GCLK_GEN_3_GENEN == 0
    disable_gclk(3);
    #endif
    #if CONF_GCLK_GEN_4_GENEN == 0
    disable_gclk(4);
    #endif
    #if CONF_GCLK_GEN_5_GENEN == 0
    disable_gclk(5);
    #endif
    #if CONF_GCLK_GEN_6_GENEN == 0
    disable_gclk(6);
    #endif
    #if CONF_GCLK_GEN_7_GENEN == 0
    disable_gclk(7);
    #endif
    #ifdef SAMD51
    #if CONF_GCLK_GEN_8_GENEN == 0
    disable_gclk(8);
    #endif
    #if CONF_GCLK_GEN_9_GENEN == 0
    disable_gclk(9);
    #endif
    #if CONF_GCLK_GEN_10_GENEN == 0
    disable_gclk(10);
    #endif
    #if CONF_GCLK_GEN_11_GENEN == 0
    disable_gclk(11);
    #endif
    #endif
}
