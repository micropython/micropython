// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/busio/SPI.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t busio_spi_type;

// Construct an underlying SPI object.
extern void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex);

extern void common_hal_busio_spi_deinit(busio_spi_obj_t *self);
extern bool common_hal_busio_spi_deinited(busio_spi_obj_t *self);

extern bool common_hal_busio_spi_configure(busio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits);

extern bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self);
extern bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self);
extern void common_hal_busio_spi_unlock(busio_spi_obj_t *self);

// Writes out the given data.
extern bool common_hal_busio_spi_write(busio_spi_obj_t *self, const uint8_t *data, size_t len);

// Reads in len bytes while outputting the byte write_value.
extern bool common_hal_busio_spi_read(busio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value);

// Reads and write len bytes simultaneously.
extern bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len);

// Return actual SPI bus frequency.
uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self);

// Return SPI bus phase.
uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self);

// Return SPI bus polarity.
uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self);

// This is used by the supervisor to claim SPI devices indefinitely.
extern void common_hal_busio_spi_never_reset(busio_spi_obj_t *self);

extern busio_spi_obj_t *validate_obj_is_spi_bus(mp_obj_t obj_in, qstr arg_name);
