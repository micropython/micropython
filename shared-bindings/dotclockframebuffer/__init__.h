// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
