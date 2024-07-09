// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Junji Sakai
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_float_t monotonic_time;      // values compatible with time.monotonic_time()
} alarm_time_timealarm_obj_t;

extern volatile int rtc_woke_up_counter;
extern void port_disable_interrupt_after_ticks_ch(uint32_t channel);
extern void port_interrupt_after_ticks_ch(uint32_t channel, uint32_t ticks);

mp_obj_t alarm_time_timealarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms);
mp_obj_t alarm_time_timealarm_record_wake_alarm(void);

bool alarm_time_timealarm_woke_this_cycle(void);
void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms);
void alarm_time_timealarm_reset(void);

extern void alarm_time_timealarm_prepare_for_deep_sleep(void);
extern int64_t alarm_time_timealarm_get_wakeup_timediff_ms(void);
extern void alarm_time_timealarm_clear_wakeup_time(void);
extern void dbg_dump_RTCreg(void);
extern void tick_set_prescaler(uint32_t prescaler_val);
