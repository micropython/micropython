// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "nrfx.h"
#include "hal/nrf_nvmc.h"
#include "peripherals/nrf/power.h"

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
