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
#include "hal/nrf_nvmc.h"

void nrf_peripherals_power_init(void) {
    // Set GPIO reference voltage to 3.3V if it isn't already. REGOUT0 will get reset to 0xfffffff
    // if flash is erased, which sets the default to 1.8V
    // This matters only when "high voltage mode" is enabled, which is true on the PCA10059,
    // and might be true on other boards.
    if (NRF_UICR->REGOUT0 == 0xffffffff && NRF_POWER->MAINREGSTATUS & 1) {
        // Expand what nrf_nvmc_word_write() did.
        // It's missing from nrfx V2.0.0, and nrfx_nvmc_word_write() does bounds
        // checking which prevents writes to UICR.
        // Reported: https://devzone.nordicsemi.com/f/nordic-q-a/57243/nrfx_nvmc-h-api-cannot-write-to-uicr
        NRF_NVMC->CONFIG = NRF_NVMC_MODE_WRITE;
        while (!(NRF_NVMC->READY & NVMC_READY_READY_Msk)) {
        }
        NRF_UICR->REGOUT0 = UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos;
        __DMB();
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
        }
        NRF_NVMC->CONFIG = NRF_NVMC_MODE_READONLY;

        // Must reset to enable change.
        NVIC_SystemReset();
    }
}
