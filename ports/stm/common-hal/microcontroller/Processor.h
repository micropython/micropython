// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan  Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_STM32_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H
#define MICROPY_INCLUDED_STM32_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H

#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH 12

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // Stores no state currently.
} mcu_processor_obj_t;

#endif // MICROPY_INCLUDED_STM32_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H
