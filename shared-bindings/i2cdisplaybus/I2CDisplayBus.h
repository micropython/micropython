// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/i2cdisplaybus/I2CDisplayBus.h"

#include "shared-bindings/displayio/__init__.h"
#include "common-hal/microcontroller/Pin.h"

extern const mp_obj_type_t i2cdisplaybus_i2cdisplaybus_type;

void common_hal_i2cdisplaybus_i2cdisplaybus_construct(i2cdisplaybus_i2cdisplaybus_obj_t *self,
    busio_i2c_obj_t *i2c, uint16_t device_address, const mcu_pin_obj_t *reset);

void common_hal_i2cdisplaybus_i2cdisplaybus_deinit(i2cdisplaybus_i2cdisplaybus_obj_t *self);

bool common_hal_i2cdisplaybus_i2cdisplaybus_reset(mp_obj_t self);
bool common_hal_i2cdisplaybus_i2cdisplaybus_bus_free(mp_obj_t self);

bool common_hal_i2cdisplaybus_i2cdisplaybus_begin_transaction(mp_obj_t self);

void common_hal_i2cdisplaybus_i2cdisplaybus_send(mp_obj_t self, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length);

void common_hal_i2cdisplaybus_i2cdisplaybus_end_transaction(mp_obj_t self);

// The I2CDisplayBus object always lives off the MP heap. So, code must collect any pointers
// back to the MP heap manually. Otherwise they'll get freed.
void common_hal_i2cdisplaybus_i2cdisplaybus_collect_ptrs(mp_obj_t obj);
