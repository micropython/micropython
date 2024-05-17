// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016, 2017, 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/spi_flash_api.h"

#include "supervisor/shared/external_flash/common_commands.h"
#include "supervisor/shared/external_flash/qspi_flash.h"

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
