// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "nrfx.h"
#include "peripherals/nrf/cache.h"

// Turn off cache and invalidate all data in it.
void nrf_peripherals_disable_and_clear_cache(void) {
    // Memory fence for hardware and compiler reasons. If this routine is inlined, the compiler
    // needs to know that everything written out be stored before this is called.
    // -O2 optimization needed this on SAMD51. Assuming nRF may have the same issue.
    // __sync_synchronize() includes volatile asm(), which tells the compiler not to assume
    // state across this call.
    __sync_synchronize();

    // Disabling cache also invalidates all cache entries.
    NRF_NVMC->ICACHECNF &= ~(1 << NVMC_ICACHECNF_CACHEEN_Pos);

    // Memory fence for hardware and compiler reasons.
    __sync_synchronize();
}

// Enable cache
void nrf_peripherals_enable_cache(void) {
    NRF_NVMC->ICACHECNF |= 1 << NVMC_ICACHECNF_CACHEEN_Pos;
}
