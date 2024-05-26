// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/espulp/ULPAlarm.h"

extern const mp_obj_type_t espulp_ulpalarm_type;

void common_hal_espulp_ulpalarm_construct(espulp_ulpalarm_obj_t *self, espulp_ulp_obj_t *ulp);
