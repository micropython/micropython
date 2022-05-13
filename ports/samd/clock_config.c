/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * This file provides functions for configuring the clocks.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Robert Hammelrath
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

#include <stdint.h>

#include "py/runtime.h"
#include "sam.h"
#include "utils.h"

#if defined(MCU_SAMD21)
int sercom_gclk_id[] = {
    GCLK_CLKCTRL_ID_SERCOM0_CORE, GCLK_CLKCTRL_ID_SERCOM1_CORE,
    GCLK_CLKCTRL_ID_SERCOM2_CORE, GCLK_CLKCTRL_ID_SERCOM3_CORE,
    GCLK_CLKCTRL_ID_SERCOM4_CORE, GCLK_CLKCTRL_ID_SERCOM5_CORE
};
#elif defined(MCU_SAMD51)
int sercom_gclk_id[] = {
    SERCOM0_GCLK_ID_CORE, SERCOM1_GCLK_ID_CORE,
    SERCOM2_GCLK_ID_CORE, SERCOM3_GCLK_ID_CORE,
    SERCOM4_GCLK_ID_CORE, SERCOM5_GCLK_ID_CORE,
    #if defined(SERCOM7_GCLK_ID_CORE)
    SERCOM6_GCLK_ID_CORE, SERCOM7_GCLK_ID_CORE,
    #endif
};
#endif

void enable_sercom_clock(int id) {
    // Next: Set up the clocks
    #if defined(MCU_SAMD21)
    // Enable synchronous clock. The bits are nicely arranged
    PM->APBCMASK.reg |= 0x04 << id;
    // Select multiplexer generic clock source and enable.
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | sercom_gclk_id[id];
    // Wait while it updates synchronously.
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    #elif defined(MCU_SAMD51)
    GCLK->PCHCTRL[sercom_gclk_id[id]].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK2;
    // no easy way to set the clocks, except enabling all of them
    switch (id) {
        case 0:
            MCLK->APBAMASK.bit.SERCOM0_ = 1;
            break;
        case 1:
            MCLK->APBAMASK.bit.SERCOM1_ = 1;
            break;
        case 2:
            MCLK->APBBMASK.bit.SERCOM2_ = 1;
            break;
        case 3:
            MCLK->APBBMASK.bit.SERCOM3_ = 1;
            break;
        case 4:
            MCLK->APBDMASK.bit.SERCOM4_ = 1;
            break;
        case 5:
            MCLK->APBDMASK.bit.SERCOM5_ = 1;
            break;
        #ifdef SERCOM7_GCLK_ID_CORE
        case 6:
            MCLK->APBDMASK.bit.SERCOM6_ = 1;
            break;
        case 7:
            MCLK->APBDMASK.bit.SERCOM7_ = 1;
            break;
        #endif
    }
    #endif
}
