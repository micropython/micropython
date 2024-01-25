/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#pragma once

#include "shared-module/sdcardio/SDCard.h"

extern const mp_obj_type_t sdcardio_SDCard_type;

void common_hal_sdcardio_sdcard_construct(sdcardio_sdcard_obj_t *self, busio_spi_obj_t *spi, const mcu_pin_obj_t *cs, int baudrate);
void common_hal_sdcardio_sdcard_deinit(sdcardio_sdcard_obj_t *self);
void common_hal_sdcardio_sdcard_check_for_deinit(sdcardio_sdcard_obj_t *self);
int common_hal_sdcardio_sdcard_get_blockcount(sdcardio_sdcard_obj_t *self);
int common_hal_sdcardio_sdcard_readblocks(sdcardio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *buf);
int common_hal_sdcardio_sdcard_sync(sdcardio_sdcard_obj_t *self);
int common_hal_sdcardio_sdcard_writeblocks(sdcardio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *buf);

// Used by native vfs blockdev.
mp_uint_t sdcardio_sdcard_readblocks(mp_obj_t self_in, uint8_t *buf, uint32_t start_block, uint32_t buflen);
mp_uint_t sdcardio_sdcard_writeblocks(mp_obj_t self_in, uint8_t *buf, uint32_t start_block, uint32_t buflen);
bool sdcardio_sdcard_ioctl(mp_obj_t self_in, size_t cmd, size_t arg, mp_int_t *out_value);
