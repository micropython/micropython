// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Nick Moore for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "common-hal/nvm/ByteArray.h"
#include "shared-bindings/nvm/ByteArray.h"

#include <stdio.h>
#include <string.h>

#include "peripherals/nrf/nvm.h"

uint32_t common_hal_nvm_bytearray_get_length(const nvm_bytearray_obj_t *self) {
    return self->len;
}

static bool write_page(uint32_t page_addr, uint32_t offset, uint32_t len, uint8_t *bytes) {
    // Write a whole page to flash, buffering it first and then erasing and rewriting
    // it since we can only clear a whole page at a time.

    if (offset == 0 && len == FLASH_PAGE_SIZE) {
        return nrf_nvm_safe_flash_page_write(page_addr, bytes);
    } else {
        uint8_t buffer[FLASH_PAGE_SIZE];
        memcpy(buffer, (uint8_t *)page_addr, FLASH_PAGE_SIZE);
        memcpy(buffer + offset, bytes, len);
        return nrf_nvm_safe_flash_page_write(page_addr, buffer);
    }
}

bool common_hal_nvm_bytearray_set_bytes(const nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values, uint32_t len) {

    uint32_t address = (uint32_t)self->start_address + start_index;
    uint32_t offset = address % FLASH_PAGE_SIZE;
    uint32_t page_addr = address - offset;

    while (len) {
        uint32_t write_len = MIN(len, FLASH_PAGE_SIZE - offset);
        if (!write_page(page_addr, offset, write_len, values)) {
            return false;
        }
        len -= write_len;
        values += write_len;
        page_addr += FLASH_PAGE_SIZE;
        offset = 0;
    }
    return true;
}

void common_hal_nvm_bytearray_get_bytes(const nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t *values) {
    memcpy(values, self->start_address + start_index, len);
}
