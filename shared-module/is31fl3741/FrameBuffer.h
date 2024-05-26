// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Mark Komus
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "lib/protomatter/src/core.h"
#include "shared-module/is31fl3741/IS31FL3741.h"

extern const mp_obj_type_t is31fl3741_framebuffer_type;
typedef struct {
    mp_obj_base_t base;
    is31fl3741_IS31FL3741_obj_t *is31fl3741;
    is31fl3741_IS31FL3741_obj_t inline_is31fl3741;
    mp_obj_t framebuffer;
    mp_buffer_info_t bufinfo;
    uint16_t bufsize, width, height, scale_width, scale_height;
    uint16_t *mapping;
    uint8_t bit_depth;
    bool paused;
    bool scale;
    bool auto_gamma;
} is31fl3741_framebuffer_obj_t;
