// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-bindings/displayio/Bitmap.h"

typedef struct {
    mp_obj_base_t base;
    const displayio_bitmap_t *bitmap;
    uint8_t width;
    uint8_t height;
    const byte *unicode_characters;
    uint16_t unicode_characters_len;
} fontio_builtinfont_t;

uint8_t fontio_builtinfont_get_glyph_index(const fontio_builtinfont_t *self, mp_uint_t codepoint);
