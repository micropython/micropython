// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2018 Nick Moore for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_MODULE_TOUCHIO_TOUCHIN_H
#define MICROPY_INCLUDED_SHARED_MODULE_TOUCHIO_TOUCHIN_H

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    digitalio_digitalinout_obj_t *digitalinout;
    uint16_t threshold;
} touchio_touchin_obj_t;

void touchin_reset(void);

#endif // MICROPY_INCLUDED_SHARED_MODULE_TOUCHIO_TOUCHIN_H
