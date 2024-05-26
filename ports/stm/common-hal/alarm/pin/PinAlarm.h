// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objtuple.h"

#include "shared-bindings/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    bool value;
    bool pull;
} alarm_pin_pinalarm_obj_t;

mp_obj_t alarm_pin_pinalarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms);
mp_obj_t alarm_pin_pinalarm_record_wake_alarm(void);

void alarm_pin_pinalarm_reset(void);
void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms);
void alarm_pin_pinalarm_prepare_for_deep_sleep(void);
bool alarm_pin_pinalarm_woke_this_cycle(void);
