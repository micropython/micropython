/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>

#include "mpconfigboard.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "gpio.h"
#include "antenna.h"


#if MICROPY_HW_ANTENNA_DIVERSITY

/******************************************************************************
DEFINE CONSTANTS
******************************************************************************/
#define REG_PAD_CONFIG_26           (0x4402E108)
#define REG_PAD_CONFIG_27           (0x4402E10C)

/******************************************************************************
DEFINE PRIVATE DATA
******************************************************************************/
static antenna_type_t antenna_type_selected = ANTENNA_TYPE_INTERNAL;

/******************************************************************************
DEFINE PUBLIC FUNCTIONS
******************************************************************************/
void antenna_init0(void) {
    // enable the peripheral clock and set the gpio direction for
    // both antenna 1 and antenna 2 pins
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    MAP_GPIODirModeSet(GPIOA3_BASE, 0x0C, GPIO_DIR_MODE_OUT);

    // configure antenna 1 pin type and strength
    HWREG(REG_PAD_CONFIG_26) = ((HWREG(REG_PAD_CONFIG_26) & ~(PAD_STRENGTH_MASK | PAD_TYPE_MASK)) | (0x00000020 | 0x00000000));
    // set the mode
    HWREG(REG_PAD_CONFIG_26) = ((HWREG(REG_PAD_CONFIG_26) & ~PAD_MODE_MASK) | 0x00000000) & ~(3 << 10);
    // set the direction
    HWREG(REG_PAD_CONFIG_26) = ((HWREG(REG_PAD_CONFIG_26) & ~0xC00) | 0x00000800);

    // configure antenna 2 pin type and strength
    HWREG(REG_PAD_CONFIG_27) = ((HWREG(REG_PAD_CONFIG_27) & ~(PAD_STRENGTH_MASK | PAD_TYPE_MASK)) | (0x00000020 | 0x00000000));
    // set the mode
    HWREG(REG_PAD_CONFIG_27) = ((HWREG(REG_PAD_CONFIG_27) & ~PAD_MODE_MASK) | 0x00000000) & ~(3 << 10);
    // set the direction
    HWREG(REG_PAD_CONFIG_27) = ((HWREG(REG_PAD_CONFIG_27) & ~0xC00) | 0x00000800);

    // select the currently active antenna
    antenna_select(antenna_type_selected);
}

void antenna_select (antenna_type_t _antenna) {
    if (_antenna == ANTENNA_TYPE_INTERNAL) {
        MAP_GPIOPinWrite(GPIOA3_BASE, 0x0C, 0x04);
        // also configure the pull-up and pull-down accordingly
        HWREG(REG_PAD_CONFIG_26) = ((HWREG(REG_PAD_CONFIG_26) & ~PAD_TYPE_MASK)) | PIN_TYPE_STD_PU;
        HWREG(REG_PAD_CONFIG_27) = ((HWREG(REG_PAD_CONFIG_27) & ~PAD_TYPE_MASK)) | PIN_TYPE_STD_PD;
    } else {
        MAP_GPIOPinWrite(GPIOA3_BASE, 0x0C, 0x08);
        // also configure the pull-up and pull-down accordingly
        HWREG(REG_PAD_CONFIG_26) = ((HWREG(REG_PAD_CONFIG_26) & ~PAD_TYPE_MASK)) | PIN_TYPE_STD_PD;
        HWREG(REG_PAD_CONFIG_27) = ((HWREG(REG_PAD_CONFIG_27) & ~PAD_TYPE_MASK)) | PIN_TYPE_STD_PU;
    }
    antenna_type_selected = _antenna;
}

#endif

