// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/enum.h"
#include "py/obj.h"
#include "py/objnamedtuple.h"

extern const mp_obj_type_t qrio_pixel_policy_type;

typedef enum {
    QRIO_EVERY_BYTE, QRIO_EVEN_BYTES, QRIO_ODD_BYTES, QRIO_RGB565, QRIO_RGB565_SWAPPED
} qrio_pixel_policy_t;

extern const cp_enum_obj_t qrio_pixel_policy_EVERY_BYTE_obj;
