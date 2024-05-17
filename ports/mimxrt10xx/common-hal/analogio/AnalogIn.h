// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_ANALOGIO_ANALOGIN_H
#define MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_ANALOGIO_ANALOGIN_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
} analogio_analogin_obj_t;

#endif // MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_ANALOGIO_ANALOGIN_H
