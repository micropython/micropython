// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "nrfx.h"
#include "mpconfigport.h"

void nrf_peripherals_clocks_init(void) {

    #if BOARD_HAS_32KHZ_XTAL
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
    #else
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
    #endif
    NRF_CLOCK->TASKS_LFCLKSTART = 1UL;

    // Wait for clocks to start.
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
    }
}
