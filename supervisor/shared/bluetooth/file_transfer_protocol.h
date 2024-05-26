// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

// See https://github.com/adafruit/Adafruit_CircuitPython_BLE_File_Transfer
// for full protocol documentation and a Python client API.

// Each struct is packed so that no padding is added by the compiler. (structs
// may having padding at the end in order to align a particular element when in
// an array of the struct.) So, be careful that types added are aligned. Otherwise,
// the compiler may generate more code than necessary.

// 0x00 - 0x0f are never used by the protocol as a command
#define READ 0x10
struct read_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint32_t chunk_offset;
    uint32_t chunk_size;
    uint8_t path[];
} __attribute__((packed));

#define READ_DATA 0x11
struct read_data {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t chunk_offset;
    uint32_t total_length;
    uint32_t data_size;
    uint8_t data[];
} __attribute__((packed));

#define READ_PACING 0x12
struct read_pacing {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t chunk_offset;
    uint32_t chunk_size;
} __attribute__((packed));

#define WRITE 0x20
struct write_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint32_t offset;
    uint64_t modification_time;
    uint32_t total_length;
    uint8_t path[];
} __attribute__((packed));

#define WRITE_PACING 0x21
struct write_pacing {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t offset;
    uint64_t truncated_time;
    uint32_t free_space;
} __attribute__((packed));

#define WRITE_DATA 0x22
struct write_data {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t offset;
    uint32_t data_size;
    uint8_t data[];
} __attribute__((packed));

#define DELETE 0x30
struct delete_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint8_t path[];
} __attribute__((packed));

#define DELETE_STATUS 0x31
struct delete_status {
    uint8_t command;
    uint8_t status;
} __attribute__((packed));

#define MKDIR 0x40
struct mkdir_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint32_t reserved2;
    uint64_t modification_time;
    uint8_t path[];
} __attribute__((packed));

#define MKDIR_STATUS 0x41
struct mkdir_status {
    uint8_t command;
    uint8_t status;
    uint16_t reserved;
    uint32_t reserved2;
    uint64_t truncated_time;
} __attribute__((packed));

#define LISTDIR 0x50
struct listdir_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t path_length;
    uint8_t path[];
} __attribute__((packed));

#define LISTDIR_ENTRY 0x51
struct listdir_entry {
    uint8_t command;
    uint8_t status;
    uint16_t path_length;
    uint32_t entry_number;
    uint32_t entry_count;
    uint32_t flags;
    uint64_t truncated_time;
    uint32_t file_size;
    uint8_t path[];
} __attribute__((packed));

#define MOVE 0x60
struct move_command {
    uint8_t command;
    uint8_t reserved;
    uint16_t old_path_length;
    uint16_t new_path_length;
    // paths is two strings. The first is old_path and then a reserved byte.
    // The last path is new_path.
    uint8_t paths[];
} __attribute__((packed));

#define MOVE_STATUS 0x61
struct move_status {
    uint8_t command;
    uint8_t status;
} __attribute__((packed));

#define STATUS_OK 0x01
#define STATUS_ERROR 0x02
#define STATUS_ERROR_NO_FILE 0x03
#define STATUS_ERROR_PROTOCOL 0x04
#define STATUS_ERROR_READONLY 0x05
