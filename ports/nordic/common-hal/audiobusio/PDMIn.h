// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_AUDIOBUSIO_AUDIOOUT_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_AUDIOBUSIO_AUDIOOUT_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t clock_pin_number, data_pin_number;
    bool mono;
} audiobusio_pdmin_obj_t;

#endif
