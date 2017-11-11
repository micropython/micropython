/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Ayke van Laethem
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

#ifndef HAL_NVMC_H__
#define HAL_NVMC_H__

#include <stdint.h>

#include "nrf.h"

// Erase a single page. The pageaddr is an address within the first page.
bool hal_nvmc_erase_page(uint32_t pageaddr);

// Write an array of 32-bit words to flash. The len parameter is the
// number of words, not the number of bytes. Dest and buf must be aligned.
bool hal_nvmc_write_words(uint32_t *dest, const uint32_t *buf, size_t len);

// Write a byte to flash. May have any alignment.
bool hal_nvmc_write_byte(byte *dest, byte b);

// Write an (unaligned) byte buffer to flash.
bool hal_nvmc_write_buffer(void *dest_in, const void *buf_in, size_t len);

// Call for ble_drv.c: notify (from an interrupt) that the current flash
// operation has finished.
void hal_nvmc_operation_finished(uint8_t result);

enum {
    HAL_NVMC_BUSY,
    HAL_NVMC_SUCCESS,
    HAL_NVMC_ERROR,
};

#if defined(NRF51)
#define HAL_NVMC_PAGESIZE (1024)

#elif defined(NRF52)
#define HAL_NVMC_PAGESIZE (4096)
#error NRF52 not yet implemented

#else
#error Unknown chip
#endif

#define HAL_NVMC_IS_PAGE_ALIGNED(addr) ((uint32_t)(addr) & (HAL_NVMC_PAGESIZE - 1))

#endif // HAL_NVMC_H__
