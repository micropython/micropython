// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint32_t *framebuffer;
    mp_uint_t width;
    mp_uint_t height;
    uint32_t pitch;
    bool doublebuffer;
} videocore_framebuffer_obj_t;
