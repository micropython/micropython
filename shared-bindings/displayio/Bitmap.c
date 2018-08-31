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

#include "shared-bindings/displayio/Bitmap.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| .. currentmodule:: displayio
//|
//| :class:`Bitmap` -- Stores values in a 2D array
//| ==========================================================================
//|
//| Stores values of a certain size in a 2D array
//|
//| .. warning:: This will likely be changed before 4.0.0. Consider it very experimental.
//|
//| .. class:: Bitmap(width, height, value_count)
//|
//|   Create a Bitmap object with the given fixed size. Each pixel stores a value that is used to
//|   index into a corresponding palette. This enables differently colored sprites to share the
//|   underlying Bitmap. value_count is used to minimize the memory used to store the Bitmap.
//|
//|   :param int width: The number of values wide
//|   :param int height: The number of values high
//|   :param int value_count: The number of possible pixel values.
//|
STATIC mp_obj_t displayio_bitmap_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 3, 3, false);
    uint32_t width = mp_obj_get_int(pos_args[0]);
    uint32_t height = mp_obj_get_int(pos_args[1]);
    uint32_t value_count = mp_obj_get_int(pos_args[2]);
    uint32_t power_of_two = 1;
    while (value_count > (1U << power_of_two)) {
        power_of_two <<= 1;
    }

    displayio_bitmap_t *self = m_new_obj(displayio_bitmap_t);
    self->base.type = &displayio_bitmap_type;
    common_hal_displayio_bitmap_construct(self, width, height, power_of_two);

    return MP_OBJ_FROM_PTR(self);
}


//|   .. method:: _load_row(y, data)
//|
//|     Loads pre-packed data into the given row.
//|
STATIC mp_obj_t displayio_bitmap_obj__load_row(mp_obj_t self_in, mp_obj_t y_in, mp_obj_t data_in) {
    displayio_bitmap_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t y;
    if (!mp_obj_get_int_maybe(y_in, &y)) {
        mp_raise_ValueError(translate("y should be an int"));
    }
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(data_in, &bufinfo, MP_BUFFER_READ)) {
        if (bufinfo.typecode != 'b' && bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
            mp_raise_ValueError(translate("row buffer must be a bytearray or array of type 'b' or 'B'"));
        }
        uint8_t* buf = bufinfo.buf;
        common_hal_displayio_bitmap_load_row(self, y, buf, bufinfo.len);
    } else {
        mp_raise_TypeError(translate("row data must be a buffer"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(displayio_bitmap__load_row_obj, displayio_bitmap_obj__load_row);

STATIC const mp_rom_map_elem_t displayio_bitmap_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR__load_row), MP_ROM_PTR(&displayio_bitmap__load_row_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_bitmap_locals_dict, displayio_bitmap_locals_dict_table);

const mp_obj_type_t displayio_bitmap_type = {
    { &mp_type_type },
    .name = MP_QSTR_Bitmap,
    .make_new = displayio_bitmap_make_new,
    // TODO(tannewt): Implement subscr after slices support start, stop and step tuples.
    // .subscr = displayio_bitmap_subscr,
    .locals_dict = (mp_obj_dict_t*)&displayio_bitmap_locals_dict,
};
