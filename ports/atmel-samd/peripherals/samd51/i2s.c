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

#include "peripherals/i2s.h"

#include "peripherals/clocks.h"

#include "hpl/gclk/hpl_gclk_base.h"

void turn_on_i2s(void) {
    // Make sure the I2S peripheral is running so we can see if the resources we need are free.
    hri_mclk_set_APBDMASK_I2S_bit(MCLK);

    // Connect the clock units to the 2mhz clock by default. They can't reset without it.
    connect_gclk_to_peripheral(5, I2S_GCLK_ID_0);
    connect_gclk_to_peripheral(5, I2S_GCLK_ID_1);
}

void i2s_set_serializer_enable(uint8_t serializer, bool enable) {
    if (serializer == 0) {
        while (I2S->SYNCBUSY.bit.TXEN == 1) {}
        I2S->CTRLA.bit.TXEN = enable;
        while (I2S->SYNCBUSY.bit.TXEN == 1) {}
    } else {
        while (I2S->SYNCBUSY.bit.RXEN == 1) {}
        I2S->CTRLA.bit.RXEN = enable;
        while (I2S->SYNCBUSY.bit.RXEN == 1) {}
    }
}
