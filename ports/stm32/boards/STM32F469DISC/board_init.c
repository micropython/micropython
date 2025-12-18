/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Mike Tolkachev <contact@miketolkachev.dev>
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
#include "boardctrl.h"
#include "qspi.h"

// Micron N25Q128A13EF840F of original STM32F469I-DISCO board
static const mp_spiflash_chip_params_t chip_params_n25q128a13ef840f = {
    .jedec_id = 0, // Not used for detection
    .memory_size_bytes_log2 = MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2,
    .qspi_prescaler = MICROPY_HW_QSPI_PRESCALER,
    .qread_num_dummy = MICROPY_HW_QSPIFLASH_DUMMY_CYCLES
};

// Early board initialization hook called before file system is mounted
void STM32F469DISC_board_early_init(void) {
    // Initialize QSPI flash device parameters
    MICROPY_HW_BDEV_SPIFLASH->spiflash.chip_params = &chip_params_n25q128a13ef840f;
}
