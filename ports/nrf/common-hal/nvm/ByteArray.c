/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
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

#include <stdio.h>
#include <string.h>

#include "nrf_nvmc.h"

uint32_t common_hal_nvm_bytearray_get_length(nvm_bytearray_obj_t *self) {
    return self->len;
}

bool common_hal_nvm_bytearray_set_bytes(nvm_bytearray_obj_t *self,
        uint32_t start_index, uint8_t* values, uint32_t len) {
    printf("%ld %ld\n", start_index, len);
    nrf_nvmc_page_erase(self->start_address);
    nrf_nvmc_write_bytes(self->start_address + start_index, values, len);    
    return true;
}

void common_hal_nvm_bytearray_get_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t* values) {
    memcpy(values, (uint8_t *)(self->start_address + start_index), len);
}
