// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/fourwire/FourWire.h"

#include "shared-bindings/displayio/__init__.h"
#include "shared-bindings/fourwire/__init__.h"
#include "common-hal/microcontroller/Pin.h"


extern const mp_obj_type_t fourwire_fourwire_type;

void common_hal_fourwire_fourwire_construct(fourwire_fourwire_obj_t *self,
    busio_spi_obj_t *spi, const mcu_pin_obj_t *command,
    const mcu_pin_obj_t *chip_select, const mcu_pin_obj_t *reset, uint32_t baudrate,
    uint8_t polarity, uint8_t phase);

void common_hal_fourwire_fourwire_deinit(fourwire_fourwire_obj_t *self);

bool common_hal_fourwire_fourwire_reset(mp_obj_t self);
bool common_hal_fourwire_fourwire_bus_free(mp_obj_t self);

bool common_hal_fourwire_fourwire_begin_transaction(mp_obj_t self);

void common_hal_fourwire_fourwire_send(mp_obj_t self, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length);

void common_hal_fourwire_fourwire_end_transaction(mp_obj_t self);

// The FourWire object always lives off the MP heap. So, code must collect any pointers
// back to the MP heap manually. Otherwise they'll get freed.
void common_hal_fourwire_fourwire_collect_ptrs(mp_obj_t obj);
