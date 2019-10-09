/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Glenn Ruben Bakke
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

#include <stdbool.h>

#include "nrf.h"
#include "nrf52840_bitfields.h"

bool uicr_REGOUT0_erased() {
    if (NRF_UICR->REGOUT0 == 0xFFFFFFFFUL) {
        return true;
    }
    return false;
}

void board_modules_init0(void)
{
    if (uicr_REGOUT0_erased()) {

        // Wait for pending NVMC operations to finish.
        while (NRF_NVMC->READY != NVMC_READY_READY_Ready);

        // Enable write mode in NVMC.
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY != NVMC_READY_READY_Ready);

        // Write 3v3 value to UICR->REGOUT0.
        NRF_UICR->REGOUT0 = (UICR_REGOUT0_VOUT_3V3 & UICR_REGOUT0_VOUT_Msk) << UICR_REGOUT0_VOUT_Pos;
        while (NRF_NVMC->READY != NVMC_READY_READY_Ready);

        // Enable read mode in NVMC.
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
        while (NRF_NVMC->READY != NVMC_READY_READY_Ready);

        // Reset to apply the update.
        NVIC_SystemReset();
    }
}
