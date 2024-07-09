// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/espulp/ULP.h"

extern const mp_obj_type_t espulp_ulp_type;

void common_hal_espulp_ulp_construct(espulp_ulp_obj_t *self, espulp_architecture_t arch);
bool common_hal_espulp_ulp_deinited(espulp_ulp_obj_t *self);
void common_hal_espulp_ulp_deinit(espulp_ulp_obj_t *self);

void common_hal_espulp_ulp_set_wakeup_period(espulp_ulp_obj_t *self, size_t period_index, uint32_t period_us);
void common_hal_espulp_ulp_run(espulp_ulp_obj_t *self, uint32_t *program, size_t length, uint32_t entry_point, uint32_t pin_mask);
void common_hal_espulp_ulp_halt(espulp_ulp_obj_t *self);
