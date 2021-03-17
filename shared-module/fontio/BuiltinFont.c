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

#include "shared-bindings/fontio/BuiltinFont.h"


#include "shared-bindings/fontio/Glyph.h"

#include "py/objnamedtuple.h"

mp_obj_t common_hal_fontio_builtinfont_get_bitmap(const fontio_builtinfont_t *self) {
    return MP_OBJ_FROM_PTR(self->bitmap);
}

mp_obj_t common_hal_fontio_builtinfont_get_bounding_box(const fontio_builtinfont_t *self) {
    mp_obj_t *items = m_new(mp_obj_t, 2);
    items[0] = MP_OBJ_NEW_SMALL_INT(self->width);
    items[1] = MP_OBJ_NEW_SMALL_INT(self->height);
    return mp_obj_new_tuple(2, items);
}

uint8_t fontio_builtinfont_get_glyph_index(const fontio_builtinfont_t *self, mp_uint_t codepoint) {
    if (codepoint >= 0x20 && codepoint <= 0x7e) {
        return codepoint - 0x20;
    }
    // Do a linear search of the mapping for unicode.
    const byte *j = self->unicode_characters;
    uint8_t k = 0;
    while (j < self->unicode_characters + self->unicode_characters_len) {
        unichar potential_c = utf8_get_char(j);
        j = utf8_next_char(j);
        if (codepoint == potential_c) {
            return 0x7f - 0x20 + k;
        }
        k++;
    }
    return 0xff;
}

mp_obj_t common_hal_fontio_builtinfont_get_glyph(const fontio_builtinfont_t *self, mp_uint_t codepoint) {
    uint8_t glyph_index = fontio_builtinfont_get_glyph_index(self, codepoint);
    if (glyph_index == 0xff) {
        return mp_const_none;
    }
    mp_obj_t field_values[8] = {
        MP_OBJ_FROM_PTR(self->bitmap),
        MP_OBJ_NEW_SMALL_INT(glyph_index),
        MP_OBJ_NEW_SMALL_INT(self->width),
        MP_OBJ_NEW_SMALL_INT(self->height),
        MP_OBJ_NEW_SMALL_INT(0),
        MP_OBJ_NEW_SMALL_INT(0),
        MP_OBJ_NEW_SMALL_INT(self->width),
        MP_OBJ_NEW_SMALL_INT(0)
    };
    return namedtuple_make_new((const mp_obj_type_t *)&fontio_glyph_type, 8, field_values, NULL);
}
