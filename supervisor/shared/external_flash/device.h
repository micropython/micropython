// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT
#pragma once

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
