/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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
