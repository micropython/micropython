// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/fontio/BuiltinFont.h"

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| from typing_extensions import Protocol  # for compat with python < 3.8
//|
//| class FontProtocol(Protocol):
//|     """A protocol shared by `BuiltinFont` and classes in ``adafruit_bitmap_font``"""
//|
//|     def get_bounding_box(self) -> Union[Tuple[int, int], Tuple[int, int, int, int]]:
//|         """Retrieve the maximum bounding box of any glyph in the font.
//|
//|         The four element version is ``(width, height, x_offset, y_offset)``.
//|         The two element version is ``(width, height)``, in which
//|         ``x_offset`` and ``y_offset`` are assumed to be zero."""
//|         pass
//|
//|     def get_glyph(self, codepoint: int) -> Optional[Glyph]:
//|         """Retrieve the Glyph for a given code point
//|
//|         If the code point is not present in the font, `None` is returned."""
//|         pass
//|

//| class BuiltinFont:
//|     """A font built into CircuitPython"""
//|
//|     def __init__(self) -> None:
//|         """Creation not supported. Available fonts are defined when CircuitPython is built. See the
//|         `Adafruit_CircuitPython_Bitmap_Font <https://github.com/adafruit/Adafruit_CircuitPython_Bitmap_Font>`_
//|         library for dynamically loaded fonts."""
//|         ...

//|     bitmap: displayio.Bitmap
//|     """Bitmap containing all font glyphs starting with ASCII and followed by unicode. Use
//|     `get_glyph` in most cases. This is useful for use with `displayio.TileGrid` and
//|     `terminalio.Terminal`."""
static mp_obj_t fontio_builtinfont_obj_get_bitmap(mp_obj_t self_in) {
    fontio_builtinfont_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_fontio_builtinfont_get_bitmap(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(fontio_builtinfont_get_bitmap_obj, fontio_builtinfont_obj_get_bitmap);

MP_PROPERTY_GETTER(fontio_builtinfont_bitmap_obj,
    (mp_obj_t)&fontio_builtinfont_get_bitmap_obj);

//|     def get_bounding_box(self) -> Tuple[int, int]:
//|         """Returns the maximum bounds of all glyphs in the font in a tuple of two values: width, height."""
//|         ...
static mp_obj_t fontio_builtinfont_obj_get_bounding_box(mp_obj_t self_in) {
    fontio_builtinfont_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_fontio_builtinfont_get_bounding_box(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(fontio_builtinfont_get_bounding_box_obj, fontio_builtinfont_obj_get_bounding_box);


//|     def get_glyph(self, codepoint: int) -> Glyph:
//|         """Returns a `fontio.Glyph` for the given codepoint or None if no glyph is available."""
//|         ...
//|
static mp_obj_t fontio_builtinfont_obj_get_glyph(mp_obj_t self_in, mp_obj_t codepoint_obj) {
    fontio_builtinfont_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t codepoint = mp_arg_validate_type_int(codepoint_obj, MP_QSTR_codepoint);
    return common_hal_fontio_builtinfont_get_glyph(self, codepoint);
}
MP_DEFINE_CONST_FUN_OBJ_2(fontio_builtinfont_get_glyph_obj, fontio_builtinfont_obj_get_glyph);

static const mp_rom_map_elem_t fontio_builtinfont_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&fontio_builtinfont_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_bounding_box), MP_ROM_PTR(&fontio_builtinfont_get_bounding_box_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_glyph), MP_ROM_PTR(&fontio_builtinfont_get_glyph_obj) },
};
static MP_DEFINE_CONST_DICT(fontio_builtinfont_locals_dict, fontio_builtinfont_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    fontio_builtinfont_type,
    MP_QSTR_BuiltinFont,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &fontio_builtinfont_locals_dict
    );
