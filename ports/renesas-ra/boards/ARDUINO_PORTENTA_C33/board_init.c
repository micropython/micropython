/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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

#include "py/mphal.h"
#include "r_sce.h"

// Key code for writing PRCR register.
#define BSP_PRV_PRCR_KEY            (0xA500U)
#define BSP_PRV_PRCR_PRC1_UNLOCK    ((BSP_PRV_PRCR_KEY) | 0x2U)
#define BSP_PRV_PRCR_LOCK               ((BSP_PRV_PRCR_KEY) | 0x0U)
#define BOOT_DOUBLE_TAP_DATA        (0x07738135)
#define BOOT_DOUBLE_TAP_ADDR        (*((volatile uint32_t *)&R_SYSTEM->VBTBKR[0]))

void PORTENTA_C33_board_startup() {
    // Enable USB_BASE
    R_SYSTEM->PRCR = (uint16_t)BSP_PRV_PRCR_PRC1_UNLOCK;
    R_MSTP->MSTPCRB &= ~(1U << 11U);
    R_MSTP->MSTPCRB &= ~(1U << 12U);
    R_SYSTEM->PRCR = (uint16_t)BSP_PRV_PRCR_LOCK;

    // Re-enable IRQs after jump from the bootloader.
    __enable_irq();
}

void PORTENTA_C33_board_enter_bootloader(void) {
    R_SYSTEM->PRCR = (uint16_t)BSP_PRV_PRCR_PRC1_UNLOCK;
    BOOT_DOUBLE_TAP_ADDR = BOOT_DOUBLE_TAP_DATA;
    R_SYSTEM->PRCR = (uint16_t)BSP_PRV_PRCR_LOCK;
    NVIC_SystemReset();
}
