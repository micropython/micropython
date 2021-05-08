/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class BuiltinFont:
//|     """A font built into CircuitPython"""
//|
//|     def __init__(self) -> None:
//|         """Creation not supported. Available fonts are defined when CircuitPython is built. See the
//|         `Adafruit_CircuitPython_Bitmap_Font <https://github.com/adafruit/Adafruit_CircuitPython_Bitmap_Font>`_
//|         library for dynamically loaded fonts."""
//|         ...
//|

//|     bitmap: displayio.Bitmap
//|     """Bitmap containing all font glyphs starting with ASCII and followed by unicode. Use
//|     `get_glyph` in most cases. This is useful for use with `displayio.TileGrid` and
//|     `terminalio.Terminal`."""
//|
STATIC mp_obj_t fontio_builtinfont_obj_get_bitmap(mp_obj_t self_in) {
    fontio_builtinfont_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_fontio_builtinfont_get_bitmap(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(fontio_builtinfont_get_bitmap_obj, fontio_builtinfont_obj_get_bitmap);

const mp_obj_property_t fontio_builtinfont_bitmap_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&fontio_builtinfont_get_bitmap_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def get_bounding_box(self) -> Tuple[int, int]:
//|         """Returns the maximum bounds of all glyphs in the font in a tuple of two values: width, height."""
//|         ...
//|
STATIC mp_obj_t fontio_builtinfont_obj_get_bounding_box(mp_obj_t self_in) {
    fontio_builtinfont_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_fontio_builtinfont_get_bounding_box(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(fontio_builtinfont_get_bounding_box_obj, fontio_builtinfont_obj_get_bounding_box);


//|     def get_glyph(self, codepoint: int) -> Glyph:
//|         """Returns a `fontio.Glyph` for the given codepoint or None if no glyph is available."""
//|         ...
//|
STATIC mp_obj_t fontio_builtinfont_obj_get_glyph(mp_obj_t self_in, mp_obj_t codepoint_obj) {
    fontio_builtinfont_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t codepoint;
    if (!mp_obj_get_int_maybe(codepoint_obj, &codepoint)) {
        mp_raise_ValueError_varg(translate("%q should be an int"), MP_QSTR_codepoint);
    }
    return common_hal_fontio_builtinfont_get_glyph(self, codepoint);
}
MP_DEFINE_CONST_FUN_OBJ_2(fontio_builtinfont_get_glyph_obj, fontio_builtinfont_obj_get_glyph);

STATIC const mp_rom_map_elem_t fontio_builtinfont_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&fontio_builtinfont_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_bounding_box), MP_ROM_PTR(&fontio_builtinfont_get_bounding_box_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_glyph), MP_ROM_PTR(&fontio_builtinfont_get_glyph_obj) },
};
STATIC MP_DEFINE_CONST_DICT(fontio_builtinfont_locals_dict, fontio_builtinfont_locals_dict_table);

const mp_obj_type_t fontio_builtinfont_type = {
    { &mp_type_type },
    .name = MP_QSTR_BuiltinFont,
    .locals_dict = (mp_obj_dict_t *)&fontio_builtinfont_locals_dict,
};
