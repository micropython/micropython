// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_FOMU_COMMON_HAL_DIGITALIO_DIGITALINOUT_H
#define MICROPY_INCLUDED_FOMU_COMMON_HAL_DIGITALIO_DIGITALINOUT_H

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
} digitalio_digitalinout_obj_t;

#endif // MICROPY_INCLUDED_FOMU_COMMON_HAL_DIGITALIO_DIGITALINOUT_H
