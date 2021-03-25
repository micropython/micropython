/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries LLC
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
#ifndef MICROPY_INCLUDED_SUPERVISOR_SHARED_EXTERNAL_FLASH_DEVICE_H
#define MICROPY_INCLUDED_SUPERVISOR_SHARED_EXTERNAL_FLASH_DEVICE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t total_size;
    uint16_t start_up_time_us;

    // Three response bytes to 0x9f JEDEC ID command.
    uint8_t manufacturer_id;
    uint8_t memory_type;
    uint8_t capacity;

    // Max clock speed for all operations and the fastest read mode.
    uint8_t max_clock_speed_mhz;

    // Bitmask for Quad Enable bit if present. 0x00 otherwise. This is for the highest byte in the
    // status register.
    uint8_t quad_enable_bit_mask;

    bool has_sector_protection : 1;

    // Supports the 0x0b fast read command with 8 dummy cycles.
    bool supports_fast_read : 1;

    // Supports the fast read, quad output command 0x6b with 8 dummy cycles.
    bool supports_qspi : 1;

    // Supports the quad input page program command 0x32. This is known as 1-1-4 because it only
    // uses all four lines for data.
    bool supports_qspi_writes : 1;

    // Requires a separate command 0x31 to write to the second byte of the status register.
    // Otherwise two byte are written via 0x01.
    bool write_status_register_split : 1;

    // True when the status register is a single byte. This implies the Quad Enable bit is in the
    // first byte and the Read Status Register 2 command (0x35) is unsupported.
    bool single_status_byte : 1;

    // Does not support using a ready bit within the status register
    bool no_ready_bit : 1;

    // Does not support the erase command (0x20)
    bool no_erase_cmd : 1;

    // Device does not have a reset command
    bool no_reset_cmd : 1;
} external_flash_device;

#endif  // MICROPY_INCLUDED_SUPERVISOR_SHARED_EXTERNAL_FLASH_DEVICE_H
