// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_BROADCOM_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H
#define MICROPY_INCLUDED_BROADCOM_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // Stores no state currently.
} mcu_processor_obj_t;

#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH 8

#endif // MICROPY_INCLUDED_BROADCOM_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H
