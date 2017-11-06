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

    
// Clock initialization as done in Atmel START.
void sercom_clock_init(Sercom* sercom, uint8_t sercom_index) {
    _pm_enable_bus_clock(PM_BUS_APBC, sercom);
    _gclk_enable_channel(SERCOMx_GCLK_ID_CORE[sercom_index], GCLK_CLKCTRL_GEN_GCLK0_Val);
    _gclk_enable_channel(SERCOMx_GCLK_ID_SLOW[sercom_index], GCLK_CLKCTRL_GEN_GCLK3_Val);
}
