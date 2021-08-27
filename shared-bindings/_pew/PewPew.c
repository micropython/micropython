/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Radomir Dopieralski
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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/gc.h"
#include "py/mpstate.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/util.h"
#include "PewPew.h"
#include "common-hal/_pew/PewPew.h"
#include "supervisor/shared/translate.h"

//| class PewPew:
//|     """This is an internal module to be used by the ``pew.py`` library from
//|     https://github.com/pewpew-game/pew-pewpew-standalone-10.x to handle the
//|     LED matrix display and buttons on the ``pewpew10`` board.
//|
//|     Usage::
//|
//|         This singleton class is instantiated by the ``pew`` library, and
//|         used internally by it. All user-visible interactions are done through
//|         that library."""
//|
//|     def __init__(
//|         self,
//|         buffer: ReadableBuffer,
//|         rows: List[digitalio.DigitalInOut],
//|         cols: List[digitalio.DigitalInOut],
//|         buttons: digitalio.DigitalInOut,
//|     ) -> None:
//|         """Initializes matrix scanning routines.
//|
//|         The ``buffer`` is a 64 byte long ``bytearray`` that stores what should
//|         be displayed on the matrix. ``rows`` and ``cols`` are both lists of
//|         eight ``DigitalInputOutput`` objects that are connected to the matrix
//|         rows and columns. ``buttons`` is a ``DigitalInputOutput`` object that
//|         is connected to the common side of all buttons (the other sides of the
//|         buttons are connected to rows of the matrix)."""
//|         ...
//|
STATIC mp_obj_t pewpew_make_new(const mp_obj_type_t *type, size_t n_args,
    const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 4, 4, true);
    enum { ARG_buffer, ARG_rows, ARG_cols, ARG_buttons };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_rows, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_cols, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_buttons, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
        allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    size_t rows_size = 0;
    mp_obj_t *rows;
    mp_obj_get_array(args[ARG_rows].u_obj, &rows_size, &rows);

    size_t cols_size = 0;
    mp_obj_t *cols;
    mp_obj_get_array(args[ARG_cols].u_obj, &cols_size, &cols);

    if (bufinfo.len != rows_size * cols_size) {
        mp_raise_ValueError(translate("Incorrect buffer size"));
    }

    for (size_t i = 0; i < rows_size; ++i) {
        if (!mp_obj_is_type(rows[i], &digitalio_digitalinout_type)) {
            mp_raise_TypeError(translate("Row entry must be digitalio.DigitalInOut"));
        }
        digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(rows[i]);
        if (common_hal_digitalio_digitalinout_deinited(pin)) {
            raise_deinited_error();
        }
    }

    for (size_t i = 0; i < cols_size; ++i) {
        if (!mp_obj_is_type(cols[i], &digitalio_digitalinout_type)) {
            mp_raise_TypeError(translate("Column entry must be digitalio.DigitalInOut"));
        }
        digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(cols[i]);
        if (common_hal_digitalio_digitalinout_deinited(pin)) {
            raise_deinited_error();
        }
    }

    if (!mp_obj_is_type(args[ARG_buttons].u_obj,
        &digitalio_digitalinout_type)) {
        mp_raise_TypeError(translate("buttons must be digitalio.DigitalInOut"));
    }
    digitalio_digitalinout_obj_t *buttons = MP_OBJ_TO_PTR(
        args[ARG_buttons].u_obj);
    if (common_hal_digitalio_digitalinout_deinited(buttons)) {
        raise_deinited_error();
    }

    pew_obj_t *pew = MP_STATE_VM(pew_singleton);
    if (!pew) {
        pew = m_new_obj(pew_obj_t);
        pew->base.type = &pewpew_type;
        pew = gc_make_long_lived(pew);
        MP_STATE_VM(pew_singleton) = pew;
    }

    pew->buffer = bufinfo.buf;
    pew->rows = rows;
    pew->rows_size = rows_size;
    pew->cols = cols;
    pew->cols_size = cols_size;
    pew->buttons = buttons;
    pew->pressed = 0;
    pew_init();

    return MP_OBJ_FROM_PTR(pew);
}


STATIC const mp_rom_map_elem_t pewpew_locals_dict_table[] = {
};
STATIC MP_DEFINE_CONST_DICT(pewpew_locals_dict, pewpew_locals_dict_table);
const mp_obj_type_t pewpew_type = {
    { &mp_type_type },
    .name = MP_QSTR_PewPew,
    .make_new = pewpew_make_new,
    .locals_dict = (mp_obj_dict_t *)&pewpew_locals_dict,
};
