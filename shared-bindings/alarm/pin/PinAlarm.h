/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_PIN_PINALARM_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_PIN_PINALARM_H

#include "py/obj.h"
#include "py/objtuple.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/alarm/pin/PinAlarm.h"

extern const mp_obj_type_t alarm_pin_pinalarm_type;

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, const mcu_pin_obj_t *pin, bool value, bool edge, bool pull);
extern const mcu_pin_obj_t *common_hal_alarm_pin_pinalarm_get_pin(alarm_pin_pinalarm_obj_t *self);
extern bool common_hal_alarm_pin_pinalarm_get_value(alarm_pin_pinalarm_obj_t *self);
extern bool common_hal_alarm_pin_pinalarm_get_edge(alarm_pin_pinalarm_obj_t *self);
extern bool common_hal_alarm_pin_pinalarm_get_pull(alarm_pin_pinalarm_obj_t *self);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_ALARM_PIN_PINALARM_H
