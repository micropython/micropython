/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "shared-bindings/canio/Match.h"

#include "py/objproperty.h"
#include "py/runtime.h"

//| class Match:
//|     """Describe CAN bus messages to match"""
//|
//|
//|     def __init__(self, id: int, *, mask: Optional[int] = None, extended: bool = False) -> None:
//|         """Construct a Match with the given properties.
//|
//|         If mask is not None, then the filter is for any id which matches all
//|         the nonzero bits in mask. Otherwise, it matches exactly the given id.
//|         If extended is true then only extended ids are matched, otherwise
//|         only standard ids are matched."""
//|

STATIC mp_obj_t canio_match_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_id, ARG_mask, ARG_extended, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_mask, MP_ARG_OBJ, {.u_obj = mp_const_none } },
        { MP_QSTR_extended, MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int id_bits = args[ARG_extended].u_bool ? 0x1fffffff : 0x7ff;
    int id = args[ARG_id].u_int;
    int mask = args[ARG_mask].u_obj == mp_const_none ?  id_bits : mp_obj_get_int(args[ARG_mask].u_obj);

    if (id & ~id_bits) {
        mp_raise_ValueError_varg(translate("%q out of range"), MP_QSTR_id);
    }

    if (mask & ~id_bits) {
        mp_raise_ValueError_varg(translate("%q out of range"), MP_QSTR_mask);
    }

    canio_match_obj_t *self = m_new_obj(canio_match_obj_t);
    self->base.type = &canio_match_type;
    common_hal_canio_match_construct(self, id, mask, args[ARG_extended].u_bool);
    return self;
}

//|     id: int
//|     """The id to match"""
//|

STATIC mp_obj_t canio_match_id_get(mp_obj_t self_in) {
    canio_match_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_match_get_id(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_match_id_get_obj, canio_match_id_get);

const mp_obj_property_t canio_match_id_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_match_id_get_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|
//|     mask: int
//|     """The optional mask of ids to match"""
//|

STATIC mp_obj_t canio_match_mask_get(mp_obj_t self_in) {
    canio_match_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_match_get_mask(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_match_mask_get_obj, canio_match_mask_get);

const mp_obj_property_t canio_match_mask_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_match_mask_get_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     extended: bool
//|     """True to match extended ids, False to match standard ides"""
//|

STATIC mp_obj_t canio_match_extended_get(mp_obj_t self_in) {
    canio_match_obj_t *self = self_in;
    return mp_obj_new_bool(common_hal_canio_match_get_extended(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_match_extended_get_obj, canio_match_extended_get);

const mp_obj_property_t canio_match_extended_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_match_extended_get_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t canio_match_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_id), MP_ROM_PTR(&canio_match_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_mask), MP_ROM_PTR(&canio_match_mask_obj) },
    { MP_ROM_QSTR(MP_QSTR_extended), MP_ROM_PTR(&canio_match_extended_obj) },
};
STATIC MP_DEFINE_CONST_DICT(canio_match_locals_dict, canio_match_locals_dict_table);

const mp_obj_type_t canio_match_type = {
    { &mp_type_type },
    .name = MP_QSTR_Match,
    .make_new = canio_match_make_new,
    .locals_dict = (mp_obj_dict_t *)&canio_match_locals_dict,
};
