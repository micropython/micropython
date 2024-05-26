// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Updated to support Alorium Technology Evo M51
// Author: Bryan Craker
// Date: 2020-05-20

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "common-hal/microcontroller/Pin.h"

void board_init(void) {
    never_reset_pin_number(PIN_PB20);
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

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
