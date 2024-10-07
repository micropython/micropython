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

#include "py/mphal.h"
#include "ospi_ext.h"
#include "ospi_flash.h"

const ospi_pin_settings_t ospi_pin_settings = {
    .peripheral_number = 1,
    .pin_reset = pin_OSPI1_RESET,
    .pin_cs = pin_OSPI1_CS,
    .pin_clk = pin_OSPI1_SCLK,
    .pin_rwds = pin_OSPI1_RXDS,
    .pin_d0 = pin_OSPI1_D0,
    .pin_d1 = pin_OSPI1_D1,
    .pin_d2 = pin_OSPI1_D2,
    .pin_d3 = pin_OSPI1_D3,
    .pin_d4 = pin_OSPI1_D4,
    .pin_d5 = pin_OSPI1_D5,
    .pin_d6 = pin_OSPI1_D6,
    .pin_d7 = pin_OSPI1_D7,
};

// ISSI IS25WX256 octal flash.
const ospi_flash_settings_t ospi_flash_settings = {
    .jedec_id = 0x1a5b9d, // manuf=0x9d, type=0x5b=1.8V, density=0x1a=512Mb
    .freq_hz = 100000000,
    .octal_switch = ospi_flash_issi_octal_switch,
    .octal_mode = OSPI_FLASH_OCTAL_MODE_SDD,
    .rxds = false,
    .inst_len = OSPI_INST_L_8bit,
    .xip_data_len = OSPI_DATA_L_16bit,
    .read_sr = 0x05,
    .read_sr_dummy_cycles = 8,
    .write_en = 0x06,
    .read_id = 0x9f,
    .read_id_dummy_cycles = 8,
    .read_command = 0xfd, // octal DDR read
    .read_dummy_cycles = 16,
    .write_command = 0xc2, // octal DDR write
    .erase_command = 0x21, // 4kiB sector erase with 32-bit address
};
