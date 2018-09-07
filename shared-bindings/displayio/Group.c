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

#include "shared-bindings/displayio/Group.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| .. currentmodule:: displayio
//|
//| :class:`Group` -- Group together sprites and subgroups
//| ==========================================================================
//|
//| Manage a group of sprites and groups and how they are inter-related.
//|
//| .. warning:: This will be changed before 4.0.0. Consider it very experimental.
//|
//| .. class:: Group(*, max_size=4)
//|
//|   Create a Group of a given size.
//|
//|   :param int max_size: The maximum group size.
//|
STATIC mp_obj_t displayio_group_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_max_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_max_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 4} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t max_size = args[ARG_max_size].u_int;
    if (max_size < 1) {
        mp_raise_ValueError(translate("Group must have size at least 1"));
    }

    displayio_group_t *self = m_new_obj(displayio_group_t);
    self->base.type = &displayio_group_type;
    common_hal_displayio_group_construct(self, max_size);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: append(layer)
//|
//|     Append a layer to the group. It will be drawn above other layers.
//|
STATIC mp_obj_t displayio_group_obj_append(mp_obj_t self_in, mp_obj_t layer) {
    displayio_group_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_displayio_group_append(self, layer);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_group_append_obj, displayio_group_obj_append);

STATIC const mp_rom_map_elem_t displayio_group_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_append), MP_ROM_PTR(&displayio_group_append_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_group_locals_dict, displayio_group_locals_dict_table);

const mp_obj_type_t displayio_group_type = {
    { &mp_type_type },
    .name = MP_QSTR_Group,
    .make_new = displayio_group_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_group_locals_dict,
};
