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
    bool edge;
} alarm_pin_pinalarm_obj_t;

mp_obj_t alarm_pin_pinalarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms);
mp_obj_t alarm_pin_pinalarm_record_wake_alarm(void);

void alarm_pin_pinalarm_reset(void);
void alarm_pin_pinalarm_light_reset(void);
void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms);
bool alarm_pin_pinalarm_woke_this_cycle(void);
void alarm_pin_pinalarm_entering_deep_sleep(void);
#if PICO_RP2040
// Saves the lowest-numbered triggered pin in a watchdog scratch register, or
// saves "no pin" if no pin alarm went off. Call this just before the reset that
// ends a deep sleep. alarm_pin_pinalarm_record_wake_alarm() reads it after the reset.
void alarm_pin_pinalarm_save_wake_pin(void);
#endif
#if PICO_RP2350
// Arms the powman GPIO wakeups for the pin alarms given to
// alarm_pin_pinalarm_set_alarms() with deep_sleep true.
void alarm_pin_pinalarm_enable_powman_wakeups(void);
// Returns false and leaves the outputs alone when no pin alarm was set for deep sleep.
// Otherwise fills in the pin alarm that uses powman wakeup slot 0 and returns true.
bool alarm_pin_pinalarm_first_powman_wakeup(uint *pin_number, bool *edge, bool *value);
#endif
