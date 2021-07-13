/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

// Updated to support Alorium Technology Evo M51
// Author: Bryan Craker
// Date: 2020-05-20

#include "supervisor/board.h"
#include "mpconfigboard.h"

void board_init(void) {
    REG_PORT_DIRSET1 = PORT_PB20; // PB20 as output
    REG_PORT_OUTCLR1 = PORT_PB20; // PB20 cleared
    PORT->Group[1].PINCFG[20].reg |= PORT_PINCFG_PMUXEN;  // Mux enabled on PB20
    PORT->Group[1].PMUX[10].reg = 0x0C;    // PB20 as mux function "M"
    // Gclk[6] is on PB20
    GCLK->GENCTRL[6].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DPLL0) |
        GCLK_GENCTRL_IDC |
        GCLK_GENCTRL_DIV(10) |
        // GCLK_GENCTRL_DIVSEL |
        GCLK_GENCTRL_OE |
        GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL6) {
        // Wait for synchronization
    }
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}
