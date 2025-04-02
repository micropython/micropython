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
    .pin_clk_p = pin_OSPI1_SCLK,
    .pin_clk_n = NULL,
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

const ospi_flash_settings_t ospi_flash_settings[] = {
    {
        .jedec_id = 0x1a5b9d,
        .freq_hz = 100000000,
        .read_dummy_cycles = 9,
        OSPI_FLASH_SETTINGS_IS25,
    },
};
const size_t ospi_flash_settings_len = 1;
