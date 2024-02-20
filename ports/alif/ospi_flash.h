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
#ifndef MICROPY_INCLUDED_ALIF_OSPI_FLASH_H
#define MICROPY_INCLUDED_ALIF_OSPI_FLASH_H

#include <stdbool.h>
#include <stdint.h>

typedef struct _ospi_flash_settings_t {
    uint32_t jedec_id;
    uint32_t freq_mhz;
    bool is_quad : 1;
    bool is_oct : 1;
    bool is_ddr : 1;
    uint8_t read_id_dummy_cycles;
    uint8_t read_dummy_cycles;
    uint8_t read_command;
    uint8_t write_command;
} ospi_flash_settings_t;

// Provided by the board when it enables OSPI1.
extern const ospi_flash_settings_t ospi_flash_settings;

int ospi_flash_init(void);
int ospi_flash_erase_sector(uint32_t addr);
int ospi_flash_read(uint32_t addr, uint32_t len, uint8_t *dest);
int ospi_flash_write(uint32_t addr, uint32_t len, const uint8_t *src);

#endif // MICROPY_INCLUDED_ALIF_OSPI_FLASH_H
