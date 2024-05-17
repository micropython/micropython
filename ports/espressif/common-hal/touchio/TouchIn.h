// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_TOUCHIO_TOUCHIN_H
#define MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_TOUCHIO_TOUCHIN_H

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint16_t threshold;
} touchio_touchin_obj_t;

#endif // MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_TOUCHIO_TOUCHIN_H
