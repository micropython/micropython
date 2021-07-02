/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SUPERVISOR_SHARED_BLUETOOTH_FILE_TRANSFER_PROTOCOL_H
#define MICROPY_INCLUDED_SUPERVISOR_SHARED_BLUETOOTH_FILE_TRANSFER_PROTOCOL_H

#include <stdint.h>

// See https://github.com/adafruit/Adafruit_CircuitPython_BLE_File_Transfer
// for full protocol documentation and a Python client API.

// 0x00 - 0x0f are never used by the protocol as a command
#define READ 0x10
struct read_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint32_t chunk_offset;
    uint32_t chunk_size;
    uint8_t path[];
};

#define READ_DATA 0x11
struct read_data {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t chunk_offset;
    uint32_t total_length;
    uint32_t data_size;
    uint8_t data[];
};

#define READ_PACING 0x12
struct read_pacing {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t chunk_offset;
    uint32_t chunk_size;
};

#define WRITE 0x20
struct write_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint32_t offset;
    uint32_t total_length;
    uint8_t path[];
};

#define WRITE_PACING 0x21
struct write_pacing {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t offset;
    uint32_t free_space;
};

#define WRITE_DATA 0x22
struct write_data {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t offset;
    uint32_t data_size;
    uint8_t data[];
};

#define DELETE 0x30
struct delete_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint8_t path[];
};

#define DELETE_STATUS 0x31
struct delete_status {
    uint8_t command;
    uint8_t status;
};

#define MKDIR 0x40
struct mkdir_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint8_t path[];
};

#define MKDIR_STATUS 0x41
struct mkdir_status {
    uint8_t command;
    uint8_t status;
};

#define LISTDIR 0x50
struct listdir_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint8_t path[];
};

#define LISTDIR_ENTRY 0x51
struct listdir_entry {
    uint8_t command;
    uint8_t status;
    uint16_t path_length;
    uint32_t entry_number;
    uint32_t entry_count;
    uint32_t flags;
    uint32_t file_size;
    uint8_t path[];
};

#define STATUS_OK 0x01
#define STATUS_ERROR 0x02
#define STATUS_ERROR_NO_FILE 0x03
#define STATUS_ERROR_PROTOCOL 0x04


#endif // MICROPY_INCLUDED_SUPERVISOR_SHARED_BLUETOOTH_FILE_TRANSFER_PROTOCOL_H
