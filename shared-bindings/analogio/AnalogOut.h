// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/analogio/AnalogOut.h"

extern const mp_obj_type_t analogio_analogout_type;

void common_hal_analogio_analogout_construct(analogio_analogout_obj_t *self, const mcu_pin_obj_t *pin);
void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self);
bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self);
void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self, uint16_t value);
