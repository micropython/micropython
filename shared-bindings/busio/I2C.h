// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/busio/I2C.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t busio_i2c_type;

// Initializes the hardware peripheral.
extern void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl,
    const mcu_pin_obj_t *sda,
    uint32_t frequency,
    uint32_t timeout);

extern void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self);
extern bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self);

extern bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self);
extern bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self);
extern void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self);

// Probe the bus to see if a device acknowledges the given address.
extern bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr);

// Write to the device and return 0 on success or an appropriate error code from mperrno.h
extern uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t address,
    const uint8_t *data, size_t len);

// Reads memory of the i2c device picking up where it left off and return 0 on
// success or an appropriate error code from mperrno.h
extern uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t address,
    uint8_t *data, size_t len);

// Do a write and then a read in the same I2C transaction.
uint8_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t address,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len);

// This is used by the supervisor to claim I2C devices indefinitely.
extern void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self);
