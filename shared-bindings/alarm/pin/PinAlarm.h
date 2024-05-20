// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

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
