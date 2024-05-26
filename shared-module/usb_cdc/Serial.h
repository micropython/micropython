// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_float_t timeout;       // if negative, wait forever.
    mp_float_t write_timeout; // if negative, wait forever.
    uint8_t idx;              // which CDC device?
} usb_cdc_serial_obj_t;
