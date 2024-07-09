// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/analogio/AnalogIn.h"

extern const mp_obj_type_t analogio_analogin_type;

const mcu_pin_obj_t *common_hal_analogio_analogin_validate_pin(mp_obj_t obj);
void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self, const mcu_pin_obj_t *pin);
void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self);
bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self);
uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self);
float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self);
