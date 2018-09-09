/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#include "nrfx.h"
#include "nrf_nvmc.h"

void nrf_peripherals_power_init(void) {
    // Set GPIO reference voltage to 3.3V if it isn't already. REGOUT0 will get reset to 0xfffffff
    // if flash is erased, which sets the default to 1.8V
    // This matters only when "high voltage mode" is enabled, which is true on the PCA10059,
    // and might be true on other boards.
    if (NRF_UICR->REGOUT0 == 0xffffffff) {
        nrf_nvmc_write_word((uint32_t) &NRF_UICR->REGOUT0, UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos);
        // Must reset to make enable change.
        NVIC_SystemReset();
    }
}
