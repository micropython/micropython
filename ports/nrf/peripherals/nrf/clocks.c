
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
#include "mpconfigboard.h" // for BOARD_HAS_CRYSTAL

static inline bool board_has_crystal(void) {
#ifdef BOARD_HAS_CRYSTAL
    return BOARD_HAS_CRYSTAL == 1;
#else
    return false;
#endif
}

void nrf_peripherals_clocks_init(void) {
    // Set low-frequency clock source to be either an external 32.768kHz crystal if one exists on the board
    // or an internal 32.768 kHz RC oscillator otherwise
    uint32_t clock_src = board_has_crystal() ? CLOCK_LFCLKSRC_SRC_Xtal : CLOCK_LFCLKSRC_SRC_RC;
    NRF_CLOCK->LFCLKSRC = (uint32_t)((clock_src << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
    NRF_CLOCK->TASKS_LFCLKSTART = 1UL;

    // Wait for clocks to start.
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}
}
