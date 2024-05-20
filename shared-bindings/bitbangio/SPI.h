// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/bitbangio/SPI.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t bitbangio_spi_type;

// Construct an underlying SPI object.
extern void shared_module_bitbangio_spi_construct(bitbangio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso);

extern void shared_module_bitbangio_spi_deinit(bitbangio_spi_obj_t *self);
extern bool shared_module_bitbangio_spi_deinited(bitbangio_spi_obj_t *self);

extern void shared_module_bitbangio_spi_configure(bitbangio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits);

extern bool shared_module_bitbangio_spi_try_lock(bitbangio_spi_obj_t *self);
extern bool shared_module_bitbangio_spi_has_lock(bitbangio_spi_obj_t *self);
extern void shared_module_bitbangio_spi_unlock(bitbangio_spi_obj_t *self);

// Writes out the given data.
extern bool shared_module_bitbangio_spi_write(bitbangio_spi_obj_t *self, const uint8_t *data, size_t len);

// Reads in len bytes while outputting zeroes.
extern bool shared_module_bitbangio_spi_read(bitbangio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value);

// Transfer out len bytes while reading len bytes
extern bool shared_module_bitbangio_spi_transfer(bitbangio_spi_obj_t *self, const uint8_t *dout, uint8_t *din, size_t len);
