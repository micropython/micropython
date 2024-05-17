// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT


#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_COUNTIO_COUNTER_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_COUNTIO_COUNTER_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t pin;
    mp_int_t count;
} countio_counter_obj_t;

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_COUNTIO_COUNT_H
