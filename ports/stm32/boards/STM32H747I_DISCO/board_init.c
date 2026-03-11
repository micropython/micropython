/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Andrew Leech
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
#include "storage.h"
#include "sdram.h"
#include "qspi.h"

// Micron MT25QL flash on STM32H747I-DISCO (single bank, BK1)
static const mp_spiflash_chip_params_t chip_params_mt25ql = {
    .jedec_id = 0,
    .memory_size_bytes_log2 = MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 - 3,
    .qspi_prescaler = MICROPY_HW_QSPI_PRESCALER,
    .qread_num_dummy = MICROPY_HW_QSPIFLASH_DUMMY_CYCLES,
};

void DISCO_board_early_init(void) {
    HAL_InitTick(0);

    MICROPY_HW_BDEV_SPIFLASH->spiflash.chip_params = &chip_params_mt25ql;
}

void DISCO_board_low_power(int mode) {
    switch (mode) {
        case 0:     // Leave stop mode.
            sdram_leave_low_power();
            break;
        case 1:     // Enter stop mode.
            sdram_enter_low_power();
            break;
        case 2:     // Enter standby mode.
            sdram_enter_power_down();
            break;
    }

    // Enable QSPI deepsleep for modes 1 and 2
    mp_spiflash_deepsleep(&spi_bdev.spiflash, (mode != 0));

    #if defined(M4_APP_ADDR)
    // Signal Cortex-M4 to go to Standby mode.
    if (mode == 2) {
        __HAL_RCC_HSEM_CLK_ENABLE();
        HAL_HSEM_FastTake(0);
        HAL_HSEM_Release(0, 0);
        __HAL_RCC_HSEM_CLK_DISABLE();
        HAL_Delay(100);
    }
    #endif
}
