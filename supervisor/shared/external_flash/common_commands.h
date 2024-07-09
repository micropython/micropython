// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT
#pragma once

#define CMD_READ_JEDEC_ID 0x9f
#define CMD_READ_DATA 0x03
#define CMD_FAST_READ_DATA 0x0B
#define CMD_SECTOR_ERASE 0x20
// #define CMD_SECTOR_ERASE CMD_READ_JEDEC_ID
#define CMD_DISABLE_WRITE 0x04
#define CMD_ENABLE_WRITE 0x06
#define CMD_PAGE_PROGRAM 0x02
// #define CMD_PAGE_PROGRAM CMD_READ_JEDEC_ID
#define CMD_READ_STATUS 0x05
#define CMD_READ_STATUS2 0x35
#define CMD_WRITE_STATUS_BYTE1 0x01
#define CMD_WRITE_STATUS_BYTE2 0x31
#define CMD_DUAL_READ 0x3b
#define CMD_QUAD_READ 0x6b
#define CMD_ENABLE_RESET 0x66
#define CMD_RESET 0x99
#define CMD_WAKE 0xab
