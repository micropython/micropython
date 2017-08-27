/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "asf/sam0/drivers/nvm/nvm.h"

#include <stdint.h>
#include <string.h>

uint32_t common_hal_nvm_bytearray_get_length(nvm_bytearray_obj_t *self) {
    return self->len;
}

bool common_hal_nvm_bytearray_set_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t* values, uint32_t len) {
    uint32_t total_written = 0;
    for (uint32_t i = 0; i < self->len / NVMCTRL_ROW_SIZE; i++) {
        uint32_t row_start = NVMCTRL_ROW_SIZE * i;
        if (row_start + NVMCTRL_ROW_SIZE < start_index || start_index + len < row_start) {
            continue;
        }
        uint8_t temp_row[NVMCTRL_ROW_SIZE];
        memcpy(temp_row,
               self->start_address + row_start,
               NVMCTRL_ROW_SIZE);
        enum status_code error_code;
        do {
            error_code = nvm_erase_row((uint32_t) self->start_address + row_start);
        } while (error_code == STATUS_BUSY);
        if (error_code != STATUS_OK) {
            return false;
        }
        uint32_t data_start = 0;
        if (start_index > row_start) {
            data_start = start_index - row_start;
        }
        uint32_t data_len = len;
        uint32_t data_remaining = data_len - total_written;
        uint32_t row_remaining = NVMCTRL_ROW_SIZE - data_start;
        if (data_remaining > row_remaining) {
            data_len = row_remaining;
        }
        memcpy(temp_row + data_start,
               values + total_written,
               data_len);
        for (int page = 0; page < NVMCTRL_ROW_SIZE / NVMCTRL_PAGE_SIZE; page++) {
            do {
                error_code = nvm_write_buffer((uint32_t) self->start_address + row_start + page * NVMCTRL_PAGE_SIZE,
                                              temp_row + page * NVMCTRL_PAGE_SIZE,
                                              NVMCTRL_PAGE_SIZE);
            } while (error_code == STATUS_BUSY);
            if (error_code != STATUS_OK) {
                return false;
            }
        }
    }
    return true;
}

// NVM memory is memory mapped so reading it is easy.
void common_hal_nvm_bytearray_get_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t* values) {
    memcpy(values, self->start_address + start_index, len);
}
