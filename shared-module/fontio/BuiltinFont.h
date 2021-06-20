/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_SHARED_MODULE_FONTIO_BUILTINFONT_H
#define MICROPY_INCLUDED_SHARED_MODULE_FONTIO_BUILTINFONT_H

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

#endif // MICROPY_INCLUDED_SHARED_MODULE_FONTIO_BUILTINFONT_H
