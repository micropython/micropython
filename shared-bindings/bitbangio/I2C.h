// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/bitbangio/I2C.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t bitbangio_i2c_type;

// Initializes the hardware peripheral.
extern void shared_module_bitbangio_i2c_construct(bitbangio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl,
    const mcu_pin_obj_t *sda,
    uint32_t frequency,
    uint32_t us_timeout);

extern void shared_module_bitbangio_i2c_deinit(bitbangio_i2c_obj_t *self);
extern bool shared_module_bitbangio_i2c_deinited(bitbangio_i2c_obj_t *self);

extern bool shared_module_bitbangio_i2c_try_lock(bitbangio_i2c_obj_t *self);
extern bool shared_module_bitbangio_i2c_has_lock(bitbangio_i2c_obj_t *self);
extern void shared_module_bitbangio_i2c_unlock(bitbangio_i2c_obj_t *self);

// Probe the bus to see if a device acknowledges the given address.
extern bool shared_module_bitbangio_i2c_probe(bitbangio_i2c_obj_t *self, uint8_t addr);

extern uint8_t shared_module_bitbangio_i2c_write(bitbangio_i2c_obj_t *self,
    uint16_t address,
    const uint8_t *data, size_t len,
    bool stop);

// Reads memory of the i2c device picking up where it left off.
extern uint8_t shared_module_bitbangio_i2c_read(bitbangio_i2c_obj_t *self,
    uint16_t address,
    uint8_t *data, size_t len);
