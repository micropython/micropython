/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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

#include "cgu_ext.h"
#include "peripheral_types.h"

#if CORE_M55_HP
#define CLK_BIT_OFFSET (0)
#elif CORE_M55_HE
#define CLK_BIT_OFFSET (4)
#endif

// Get RTSS_Hx_CLK in kHz.
uint32_t cgu_get_rtss_hx_clk_khz(void) {
    uint32_t es = (CGU->PLL_CLK_SEL >> (16 + CLK_BIT_OFFSET)) & 1;
    if (es == 0) {
        // Clock routed via ESx_OSC.
        uint32_t es_osc = (CGU->ESCLK_SEL >> (8 + CLK_BIT_OFFSET)) & 3;
        if (es_osc == 0 || es_osc == 2) {
            // 76.8MHz ring-oscillator, or 76.8MHz crystal oscillator.
            return 76800;
        } else {
            // 38.4MHz ring-oscillator, or 38.4MHz crystal oscillator.
            return 38400;
        }
    } else {
        // Clock routed via ESx_PLL.
        uint32_t es_pll = (CGU->ESCLK_SEL >> (0 + CLK_BIT_OFFSET)) & 3;
        #if CORE_M55_HP
        if (es_pll == 0) {
            return 100000;
        } else if (es_pll == 1) {
            return 200000;
        } else {
            return 400000;
        }
        #elif CORE_M55_HE
        if (es_pll == 0) {
            return 60000;
        } else if (es_pll == 1) {
            return 80000;
        } else if (es_pll == 2) {
            return 120000;
        } else {
            return 160000;
        }
        #endif
    }
}
