/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#pragma once

#include "py/obj.h"
#include "shared-bindings/busio/I2C.h"

typedef union {
    struct {
        uint8_t addr, data;
    } a8d8;
    struct {
        uint16_t addr, data; // in little endian
    } a16d16;
    struct {
        uint8_t addr;
        uint16_t data; // in little endian
    } __attribute__((packed)) a8d16;
    uint32_t u32;
} dotclockframebuffer_ioexpander_addr_reg_t;

typedef struct {
    busio_i2c_obj_t *bus;
    uint8_t i2c_device_address;
    uint8_t i2c_write_size;
    dotclockframebuffer_ioexpander_addr_reg_t addr_reg_shadow;
    uint32_t cs_mask;
    uint32_t mosi_mask;
    uint32_t clk_mask;
    uint32_t reset_mask;
} dotclockframebuffer_ioexpander_spi_bus;

void dotclockframebuffer_ioexpander_send_init_sequence(dotclockframebuffer_ioexpander_spi_bus *bus, const mp_buffer_info_t *i2c_bus_init, const mp_buffer_info_t *display_init);
