// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan  Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH 16

#include "py/obj.h"
#include "clocks.h"

typedef struct {
    mp_obj_base_t base;
    uint32_t frequency;
} mcu_processor_obj_t;
