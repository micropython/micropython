/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

#ifndef MICROPY_INCLUDED_MIMXRT_MBOOT_UPGRADE_H
#define MICROPY_INCLUDED_MIMXRT_MBOOT_UPGRADE_H

#include <stdint.h>

typedef enum _mboot_upgrade_status_t {
    BU_STATUS_OK = 1,
    BU_STATUS_FLASH_ERASE_ERROR = 10,
    BU_STATUS_FLASH_WRITE_ERROR = 11,
    BU_STATUS_RECEIVED_IMAGE_HEADER_INVALID = 20,
    BU_STATUS_FIRMWARE_INVALID = 30,
    BU_STATUS_UNKNOWN_ERROR = 99,
} mboot_upgrade_status_t;

// --------------------------------------------------------------------+
// Global Function Declarations
// --------------------------------------------------------------------+

/**
 * Initializes firmware upgrade session base don selected configuration.
 */
mboot_upgrade_status_t mboot_upgrade_init();

/**
 * De-initializes firmware upgrade session.
 */
mboot_upgrade_status_t mboot_upgrade_deinit();

/**
 * Write block of data to memory.
 *  Note that data is not immediately written but rather buffered
 *  and flushed when buffer is full.
 **/
mboot_upgrade_status_t mboot_upgrade_write_data(uint16_t block_num, const uint8_t *data, uint16_t block_length);

/**
 * Finalize firmware upgrade by:
 *  - flushing remaining data from buffer and write to memory
 **/
mboot_upgrade_status_t mboot_upgrade_finalize();

/**
 * Check and validate firmware upgrade:
 *  - check written data
 *  - valdiate firmware image if check ok
 **/
mboot_upgrade_status_t mboot_upgrade_validate();

#endif // MICROPY_INCLUDED_MIMXRT_MBOOT_UPGRADE_H
