// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/rotaryio/IncrementalEncoder.h"

extern const mp_obj_type_t rotaryio_incrementalencoder_type;

extern void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t *self,
    const mcu_pin_obj_t *pin_a, const mcu_pin_obj_t *pin_b);
extern void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t *self);
extern bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t *self);
extern mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t *self);
extern void common_hal_rotaryio_incrementalencoder_set_position(rotaryio_incrementalencoder_obj_t *self,
    mp_int_t new_position);
extern mp_int_t common_hal_rotaryio_incrementalencoder_get_divisor(rotaryio_incrementalencoder_obj_t *self);
extern void common_hal_rotaryio_incrementalencoder_set_divisor(rotaryio_incrementalencoder_obj_t *self,
    mp_int_t new_divisor);
