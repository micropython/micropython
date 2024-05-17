// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Rose Hooper
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/adafruit_pixelbuf/PixelBuf.h"

typedef struct _pixelmap_pixelmap_obj {
    mp_obj_base_t base;
    mp_obj_t pixelbuf;
    mp_obj_t indices;
    size_t len;
    mp_obj_t *items;
    bool auto_write;
} pixelmap_pixelmap_obj_t;
