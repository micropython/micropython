/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#ifndef __OSPI_FLASH_SETTINGS_H__
#define __OSPI_FLASH_SETTINGS_H__
#include <stdint.h>
#include "ospi_flash.h"

// Macronix MX25
#define OSPI_FLASH_SETTINGS_MX25 \
    .flash_init = ospi_flash_mx_init, \
    .octal_mode = OSPI_FLASH_OCTAL_MODE_DDD, \
    .rxds = true, \
    .bswap16 = true, \
    .inst_len = OSPI_INST_L_16bit, \
    .xip_data_len = OSPI_DATA_L_16bit, \
    .read_sr = 0x05fa, \
    .read_sr_dummy_cycles = 4, \
    .write_en = 0x06f9, \
    .read_id = 0x9f60, \
    .read_id_dummy_cycles = 4, \
    .read_command = 0xee11, \
    .write_command = 0x12ed, \
    .erase_command = 0x21de

// Everspin EM.
#define OSPI_FLASH_SETTINGS_EM \
    .flash_init = ospi_flash_issi_init, \
    .octal_mode = OSPI_FLASH_OCTAL_MODE_DDD, \
    .rxds = false, \
    .bswap16 = false, \
    .inst_len = OSPI_INST_L_8bit, \
    .xip_data_len = OSPI_DATA_L_8bit, \
    .read_sr = 0x05, \
    .read_sr_dummy_cycles = 8, \
    .write_en = 0x06, \
    .read_id = 0x9f, \
    .read_id_dummy_cycles = 8, \
    .read_command = 0xfd, \
    .write_command = 0xc2, \
    .erase_command = 0x21

// ISSI IS25.
#define OSPI_FLASH_SETTINGS_IS25 \
    .flash_init = ospi_flash_issi_init, \
    .octal_mode = OSPI_FLASH_OCTAL_MODE_DDD, \
    .rxds = false, \
    .bswap16 = false, \
    .inst_len = OSPI_INST_L_8bit, \
    .xip_data_len = OSPI_DATA_L_8bit, \
    .read_sr = 0x05, \
    .read_sr_dummy_cycles = 8, \
    .write_en = 0x06, \
    .read_id = 0x9f, \
    .read_id_dummy_cycles = 8, \
    .read_command = 0xfd, \
    .write_command = 0xc2, \
    .erase_command = 0x21
#endif  // __OSPI_FLASH_SETTINGS_H__
