/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/mphal.h"
#include "flash.h"

typedef struct {
    uint32_t base_address;
    uint32_t sector_size;
    uint32_t sector_count;
} flash_layout_t;

uint32_t flash_get_sector_info(uint32_t addr, uint32_t *start_addr, uint32_t *size) {
    *start_addr = sector_start(addr);
    *size = sector_size(addr);
    return sector_index(addr);
}

bool flash_erase(uint32_t flash_dest, uint32_t num_word32) {
    // check there is something to write
    if (num_word32 == 0) {
        return true;
    }
    bool ret = internal_flash_eraseblock((unsigned char *)flash_dest);
    return ret;
}

bool flash_write(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32) {
    bool ret;
    ret = internal_flash_write((unsigned char *)flash_dest, num_word32, (uint8_t *)src, false);
    return ret;
}
