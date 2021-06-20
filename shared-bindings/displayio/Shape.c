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

#include "shared-bindings/displayio/Shape.h"

#include <stdint.h>

#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class Shape:
//|     """Represents a shape made by defining boundaries that may be mirrored."""
//|
//|     def __init__(self, width: int, height: int, *, mirror_x: bool = False, mirror_y: bool = False) -> None:
//|         """Create a Shape object with the given fixed size. Each pixel is one bit and is stored by the
//|         column boundaries of the shape on each row. Each row's boundary defaults to the full row.
//|
//|         :param int width: The number of pixels wide
//|         :param int height: The number of pixels high
//|         :param bool mirror_x: When true the left boundary is mirrored to the right.
//|         :param bool mirror_y: When true the top boundary is mirrored to the bottom."""
//|         ...
//|
STATIC mp_obj_t displayio_shape_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_width, ARG_height, ARG_mirror_x, ARG_mirror_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_mirror_x, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_mirror_y, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t width = args[ARG_width].u_int;
    if (width < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_width);
    }
    mp_int_t height = args[ARG_height].u_int;
    if (height < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_height);
    }

    displayio_shape_t *self = m_new_obj(displayio_shape_t);
    self->base.type = &displayio_shape_type;
    common_hal_displayio_shape_construct(self,
        width,
        height,
        args[ARG_mirror_x].u_bool,
        args[ARG_mirror_y].u_bool);

    return MP_OBJ_FROM_PTR(self);
}


//|     def set_boundary(self, y: int, start_x: int, end_x: int) -> None:
//|         """Loads pre-packed data into the given row."""
//|         ...
//|
STATIC mp_obj_t displayio_shape_obj_set_boundary(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    displayio_shape_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t y;
    if (!mp_obj_get_int_maybe(args[1], &y)) {
        mp_raise_ValueError(translate("y should be an int"));
    }
    mp_int_t start_x;
    if (!mp_obj_get_int_maybe(args[2], &start_x)) {
        mp_raise_ValueError(translate("start_x should be an int"));
    }
    mp_int_t end_x;
    if (!mp_obj_get_int_maybe(args[3], &end_x)) {
        mp_raise_ValueError(translate("end_x should be an int"));
    }
    common_hal_displayio_shape_set_boundary(self, y, start_x, end_x);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(displayio_shape_set_boundary_obj, 4, 4, displayio_shape_obj_set_boundary);

STATIC const mp_rom_map_elem_t displayio_shape_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_boundary), MP_ROM_PTR(&displayio_shape_set_boundary_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_shape_locals_dict, displayio_shape_locals_dict_table);

const mp_obj_type_t displayio_shape_type = {
    { &mp_type_type },
    .name = MP_QSTR_Shape,
    .make_new = displayio_shape_make_new,
    .locals_dict = (mp_obj_dict_t *)&displayio_shape_locals_dict,
};
