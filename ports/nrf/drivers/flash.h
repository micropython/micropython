/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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

#ifndef __MICROPY_INCLUDED_LIB_FLASH_H__
#define __MICROPY_INCLUDED_LIB_FLASH_H__

#include "nrf_nvmc.h"

#if defined(NRF51)
#define FLASH_PAGESIZE (1024)

#elif defined(NRF52_SERIES)
#define FLASH_PAGESIZE (4096)
#else
#error Unknown chip
#endif

#define FLASH_IS_PAGE_ALIGNED(addr) ((uint32_t)(addr) & (FLASH_PAGESIZE - 1))

#if BLUETOOTH_SD

typedef enum {
    FLASH_STATE_BUSY,
    FLASH_STATE_SUCCESS,
    FLASH_STATE_ERROR,
} flash_state_t;

void flash_page_erase(uint32_t address);
void flash_write_byte(uint32_t address, uint8_t value);
void flash_write_bytes(uint32_t address, const uint8_t *src, uint32_t num_bytes);
void flash_operation_finished(flash_state_t result);

#else

#define flash_page_erase nrf_nvmc_page_erase
#define flash_write_byte nrf_nvmc_write_byte
#define flash_write_bytes nrf_nvmc_write_bytes

#endif

#endif // __MICROPY_INCLUDED_LIB_FLASH_H__
