/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017, 2018 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/spi_flash_api.h"

#include "supervisor/shared/external_flash/common_commands.h"

void check_quad_enable(const external_flash_device *device) {
    if (device->quad_enable_bit_mask == 0x00) {
        return;
    }

    // Verify that QSPI mode is enabled.
    uint8_t status;
    if (device->single_status_byte) {
        spi_flash_read_command(CMD_READ_STATUS, &status, 1);
    } else {
        spi_flash_read_command(CMD_READ_STATUS2, &status, 1);
    }

    // Check the quad enable bit.
    if ((status & device->quad_enable_bit_mask) == 0) {
        uint8_t full_status[2] = {0x00, device->quad_enable_bit_mask};
        spi_flash_command(CMD_ENABLE_WRITE);
        if (device->write_status_register_split) {
            spi_flash_write_command(CMD_WRITE_STATUS_BYTE2, full_status + 1, 1);
        } else if (device->single_status_byte) {
            spi_flash_write_command(CMD_WRITE_STATUS_BYTE1, full_status + 1, 1);
        } else {
            spi_flash_write_command(CMD_WRITE_STATUS_BYTE1, full_status, 2);
        }
    }
}
