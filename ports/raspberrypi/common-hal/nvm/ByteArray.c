/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "common-hal/nvm/ByteArray.h"

#include <string.h>

#include "py/runtime.h"
#include "src/rp2_common/hardware_flash/include/hardware/flash.h"

extern uint32_t __flash_binary_start;
static const uint32_t flash_binary_start = (uint32_t)&__flash_binary_start;

#define RMV_OFFSET(addr) addr - flash_binary_start

uint32_t common_hal_nvm_bytearray_get_length(nvm_bytearray_obj_t *self) {
    return self->len;
}

static void write_page(uint32_t page_addr, uint32_t offset, uint32_t len, uint8_t *bytes) {
    // Write a whole page to flash, buffering it first and then erasing and rewriting it
    // since we can only write a whole page at a time.
    if (offset == 0 && len == FLASH_PAGE_SIZE) {
        flash_range_program(RMV_OFFSET(page_addr), bytes, FLASH_PAGE_SIZE);
    } else {
        uint8_t buffer[FLASH_PAGE_SIZE];
        memcpy(buffer, (uint8_t *)page_addr, FLASH_PAGE_SIZE);
        memcpy(buffer + offset, bytes, len);
        flash_range_program(RMV_OFFSET(page_addr), buffer, FLASH_PAGE_SIZE);
    }
}

static void erase_and_write_sector(uint32_t address, uint32_t len, uint8_t *bytes) {
    // Write a whole sector to flash, buffering it first and then erasing and rewriting it
    // since we can only erase a whole sector at a time.
    uint8_t buffer[FLASH_SECTOR_SIZE];
    memcpy(buffer, (uint8_t *)CIRCUITPY_INTERNAL_NVM_START_ADDR, FLASH_SECTOR_SIZE);
    memcpy(buffer + address, bytes, len);
    flash_range_erase(RMV_OFFSET(CIRCUITPY_INTERNAL_NVM_START_ADDR), FLASH_SECTOR_SIZE);
    flash_range_program(RMV_OFFSET(CIRCUITPY_INTERNAL_NVM_START_ADDR), buffer, FLASH_SECTOR_SIZE);
}

void common_hal_nvm_bytearray_get_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t *values) {
    memcpy(values, self->start_address + start_index, len);
}

bool common_hal_nvm_bytearray_set_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values, uint32_t len) {
    uint8_t values_in[len];
    common_hal_nvm_bytearray_get_bytes(self, start_index, len, values_in);

    bool all_ones = true;
    for (uint32_t i = 0; i < len; i++) {
        if (values_in[i] != UINT8_MAX) {
            all_ones = false;
            break;
        }
    }

    if (all_ones) {
        uint32_t address = (uint32_t)self->start_address + start_index;
        uint32_t offset = address % FLASH_PAGE_SIZE;
        uint32_t page_addr = address - offset;

        while (len) {
            uint32_t write_len = MIN(len, FLASH_PAGE_SIZE - offset);
            write_page(page_addr, offset, write_len, values);
            len -= write_len;
            values += write_len;
            page_addr += FLASH_PAGE_SIZE;
            offset = 0;
        }
    } else {
        erase_and_write_sector(start_index, len, values);
    }

    return true;
}
