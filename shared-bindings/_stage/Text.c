/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Radomir Dopieralski
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

#include <py/runtime.h>

#include "__init__.h"
#include "Text.h"
#include "supervisor/shared/translate.h"

//| class Text:
//|     """Keep information about a single grid of text"""
//|
//|     def __init__(self, width: int, height: int, font: ReadableBuffer, palette: ReadableBuffer, chars: ReadableBuffer) -> None:
//|         """Keep internal information about a grid of text
//|         in a format suitable for fast rendering
//|         with the ``render()`` function.
//|
//|         :param int width: The width of the grid in tiles, or 1 for sprites.
//|         :param int height: The height of the grid in tiles, or 1 for sprites.
//|         :param ~_typing.ReadableBuffer font: The font data of the characters.
//|         :param ~_typing.ReadableBuffer palette: The color palette to be used.
//|         :param ~_typing.ReadableBuffer chars: The contents of the character grid.
//|
//|         This class is intended for internal use in the ``stage`` library and
//|         it shouldn't be used on its own."""
//|         ...
//|
STATIC mp_obj_t text_make_new(const mp_obj_type_t *type, size_t n_args,
    const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 5, 5, false);

    text_obj_t *self = m_new_obj(text_obj_t);
    self->base.type = type;

    self->width = mp_obj_get_int(args[0]);
    self->height = mp_obj_get_int(args[1]);
    self->x = 0;
    self->y = 0;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    self->font = bufinfo.buf;
    if (bufinfo.len != 2048) {
        mp_raise_ValueError(translate("font must be 2048 bytes long"));
    }

    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
    self->palette = bufinfo.buf;
    if (bufinfo.len != 32) {
        mp_raise_ValueError(translate("palette must be 32 bytes long"));
    }

    mp_get_buffer_raise(args[4], &bufinfo, MP_BUFFER_READ);
    self->chars = bufinfo.buf;
    if (bufinfo.len < self->width * self->height) {
        mp_raise_ValueError(translate("chars buffer too small"));
    }

    return MP_OBJ_FROM_PTR(self);
}

//|     def move(self, x: int, y: int) -> None:
//|         """Set the offset of the text to the specified values."""
//|         ...
//|
STATIC mp_obj_t text_move(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    text_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->x = mp_obj_get_int(x_in);
    self->y = mp_obj_get_int(y_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(text_move_obj, text_move);


STATIC const mp_rom_map_elem_t text_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_move), MP_ROM_PTR(&text_move_obj) },
};
STATIC MP_DEFINE_CONST_DICT(text_locals_dict, text_locals_dict_table);

const mp_obj_type_t mp_type_text = {
    { &mp_type_type },
    .name = MP_QSTR_Text,
    .make_new = text_make_new,
    .locals_dict = (mp_obj_dict_t *)&text_locals_dict,
};
