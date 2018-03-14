/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
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

#include "hpl/gclk/hpl_gclk_base.h"
#include "hpl/pm/hpl_pm_base.h"

// The clock initializer values are rather random, so we need to put them in
// tables for lookup. We can't compute them.

static const uint8_t SERCOMx_GCLK_ID_CORE[] = {
    SERCOM0_GCLK_ID_CORE,
    SERCOM1_GCLK_ID_CORE,
    SERCOM2_GCLK_ID_CORE,
    SERCOM3_GCLK_ID_CORE,
#ifdef SERCOM4
    SERCOM4_GCLK_ID_CORE,
#endif
#ifdef SERCOM5
    SERCOM5_GCLK_ID_CORE,
#endif
};

static const uint8_t SERCOMx_GCLK_ID_SLOW[] = {
    SERCOM0_GCLK_ID_SLOW,
    SERCOM1_GCLK_ID_SLOW,
    SERCOM2_GCLK_ID_SLOW,
    SERCOM3_GCLK_ID_SLOW,
#ifdef SERCOM4
    SERCOM4_GCLK_ID_SLOW,
#endif
#ifdef SERCOM5
    SERCOM5_GCLK_ID_SLOW,
#endif
};

Sercom* sercom_insts[SERCOM_INST_NUM] = SERCOM_INSTS;

// Clock initialization as done in Atmel START.
void samd_peripherals_sercom_clock_init(Sercom* sercom, uint8_t sercom_index) {
    _pm_enable_bus_clock(PM_BUS_APBC, sercom);
    _gclk_enable_channel(SERCOMx_GCLK_ID_CORE[sercom_index], GCLK_CLKCTRL_GEN_GCLK0_Val);
    _gclk_enable_channel(SERCOMx_GCLK_ID_SLOW[sercom_index], GCLK_CLKCTRL_GEN_GCLK3_Val);
}

// Figure out the DOPO value given the chosen clock pad and mosi pad.
// Return an out-of-range value (255) if the combination is not permitted.
// <0x0=>PAD[0,1]_DO_SCK
// <0x1=>PAD[2,3]_DO_SCK
// <0x2=>PAD[3,1]_DO_SCK
// <0x3=>PAD[0,3]_DO_SCK
uint8_t samd_peripherals_get_spi_dopo(uint8_t clock_pad, uint8_t mosi_pad) {
    if (clock_pad == 1) {
        if (mosi_pad == 0) {
            return 0;
        } else if (mosi_pad == 3) {
            return 2;
        }
    } else if (clock_pad == 3) {
        if (mosi_pad == 0) {
            return 3;
        } else if (mosi_pad == 2) {
            return 1;
        }
    }
    return 255;
}

bool samd_peripherals_valid_spi_clock_pad(uint8_t clock_pad) {
    return clock_pad == 1 || clock_pad == 3;
}
