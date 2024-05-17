// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/runtime.h"

#include "bindings/espulp/ULP.h"

typedef struct {
    mp_obj_base_t base;
    espulp_ulp_obj_t *ulp;
} espulp_ulpalarm_obj_t;

mp_obj_t espulp_ulpalarm_find_triggered_alarm(const size_t n_alarms, const mp_obj_t *alarms);
mp_obj_t espulp_ulpalarm_record_wake_alarm(void);

void espulp_ulpalarm_prepare_for_deep_sleep(void);
void espulp_ulpalarm_reset(void);
void espulp_ulpalarm_set_alarm(const bool deep_sleep, const size_t n_alarms, const mp_obj_t *alarms);
bool espulp_ulpalarm_woke_this_cycle(void);
