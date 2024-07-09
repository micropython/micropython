// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#if CIRCUITPY_TOUCHIO_USE_NATIVE
#include "common-hal/touchio/TouchIn.h"
#else
#include "shared-module/touchio/TouchIn.h"
#endif

extern const mp_obj_type_t touchio_touchin_type;

void common_hal_touchio_touchin_construct(touchio_touchin_obj_t *self, const mcu_pin_obj_t *pin);
void common_hal_touchio_touchin_deinit(touchio_touchin_obj_t *self);
bool common_hal_touchio_touchin_deinited(touchio_touchin_obj_t *self);
bool common_hal_touchio_touchin_get_value(touchio_touchin_obj_t *self);
uint16_t common_hal_touchio_touchin_get_raw_value(touchio_touchin_obj_t *self);
uint16_t common_hal_touchio_touchin_get_threshold(touchio_touchin_obj_t *self);
void common_hal_touchio_touchin_set_threshold(touchio_touchin_obj_t *self, uint16_t new_threshold);
