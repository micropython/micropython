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

#include "py/mphal.h"
#include "ospi_flash_settings.h"

// Format of command, address and data phases.
enum {
    OSPI_FLASH_OCTAL_MODE_SSS,
    OSPI_FLASH_OCTAL_MODE_SDD,
    OSPI_FLASH_OCTAL_MODE_DDD,
};

struct _ospi_flash_t;

typedef struct _ospi_pin_settings_t {
    uint32_t peripheral_number;
    const mp_hal_pin_obj_t pin_reset;
    const mp_hal_pin_obj_t pin_cs;
    const mp_hal_pin_obj_t pin_clk_p;
    const mp_hal_pin_obj_t pin_clk_n;
    const mp_hal_pin_obj_t pin_rwds;
    const mp_hal_pin_obj_t pin_d0;
    const mp_hal_pin_obj_t pin_d1;
    const mp_hal_pin_obj_t pin_d2;
    const mp_hal_pin_obj_t pin_d3;
    const mp_hal_pin_obj_t pin_d4;
    const mp_hal_pin_obj_t pin_d5;
    const mp_hal_pin_obj_t pin_d6;
    const mp_hal_pin_obj_t pin_d7;
} ospi_pin_settings_t;

typedef struct _ospi_flash_settings_t {
    uint32_t jedec_id;
    uint32_t freq_hz;
    int (*flash_init)(struct _ospi_flash_t *);
    uint8_t octal_mode;
    bool rxds;
    bool bswap16;
    uint8_t inst_len;
    uint8_t xip_data_len;
    uint16_t read_sr;
    uint8_t read_sr_dummy_cycles;
    uint16_t read_id;
    uint8_t read_id_dummy_cycles;
    uint16_t write_en;
    uint16_t read_command;
    uint8_t read_dummy_cycles;
    uint16_t write_command;
    uint16_t erase_command;
} ospi_flash_settings_t;

// Provided by the board when it enables OSPI.
extern const ospi_pin_settings_t ospi_pin_settings;
extern const ospi_flash_settings_t ospi_flash_settings[];
extern const size_t ospi_flash_settings_len;

// Functions specific to ISSI flash chips.
int ospi_flash_issi_init(struct _ospi_flash_t *self);

// Functions specific to MX flash chips.
int ospi_flash_mx_init(struct _ospi_flash_t *self);
uint8_t ospi_flash_mx_read_cr(struct _ospi_flash_t *self);
uint8_t ospi_flash_mx_read_cr2(struct _ospi_flash_t *self, uint32_t addr);
int ospi_flash_mx_write_cr(struct _ospi_flash_t *self, uint8_t value);
int ospi_flash_mx_write_cr2(struct _ospi_flash_t *self, uint32_t addr, uint8_t value);

// XIP control
int ospi_flash_xip_enter(struct _ospi_flash_t *self);
int ospi_flash_xip_exit(struct _ospi_flash_t *self);

// SPI flash interface.
int ospi_flash_init(void);
uintptr_t ospi_flash_get_xip_base(void);
int ospi_flash_erase_sector(uint32_t addr);
int ospi_flash_read(uint32_t addr, uint32_t len, uint8_t *dest);
int ospi_flash_write(uint32_t addr, uint32_t len, const uint8_t *src);

#endif // MICROPY_INCLUDED_ALIF_OSPI_FLASH_H
