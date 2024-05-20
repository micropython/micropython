// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan  Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH 12

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // Stores no state currently.
} mcu_processor_obj_t;
