/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 Damien P. George
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
#include "boardctrl.h"

#define OMV_BOOT_MAGIC_ADDR     (0x3401FFFCU)
#define OMV_BOOT_MAGIC_VALUE    (0xB00710ADU)

void board_enter_bootloader(unsigned int n_args, const void *args) {
    // Support both OpenMV bootloader and mboot.
    *((uint32_t *)OMV_BOOT_MAGIC_ADDR) = OMV_BOOT_MAGIC_VALUE;
    SCB_CleanDCache();
    boardctrl_maybe_enter_mboot(n_args, args);
}

void board_early_init(void) {
    // TODO: if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY ) != HAL_OK)
    //xspi_flash_init();

    LL_PWR_EnableWakeUpPin(LL_PWR_WAKEUP_PIN3 | LL_PWR_WAKEUP_PIN2);
    LL_PWR_SetWakeUpPinPolarityLow(LL_PWR_WAKEUP_PIN3 | LL_PWR_WAKEUP_PIN2);
}
