/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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
#include "py/objstr.h"
#include "py/misc.h"

#if MICROPY_PY_UBLUEPY

#include "modubluepy.h"
#include "ble_drv.h"

STATIC void ubluepy_descriptor_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_descriptor_obj_t * self = (ubluepy_descriptor_obj_t *)o;

    mp_printf(print, "Descriptor(uuid: 0x" HEX2_FMT HEX2_FMT ")",
              self->p_uuid->value[1], self->p_uuid->value[0]);
}

STATIC mp_obj_t ubluepy_descriptor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_NEW_UUID };

    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_UUID, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_descriptor_obj_t * s = m_new_obj(ubluepy_descriptor_obj_t);
    s->base.type = type;

    mp_obj_t uuid_obj = args[ARG_NEW_UUID].u_obj;

    (void)uuid_obj;

    return MP_OBJ_FROM_PTR(s);
}

STATIC const mp_rom_map_elem_t ubluepy_descriptor_locals_dict_table[] = {
#if 0
    { MP_ROM_QSTR(MP_QSTR_binVal), MP_ROM_PTR(&ubluepy_descriptor_bin_val_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_descriptor_locals_dict, ubluepy_descriptor_locals_dict_table);

const mp_obj_type_t ubluepy_descriptor_type = {
    { &mp_type_type },
    .name = MP_QSTR_Descriptor,
    .print = ubluepy_descriptor_print,
    .make_new = ubluepy_descriptor_make_new,
    .locals_dict = (mp_obj_dict_t*)&ubluepy_descriptor_locals_dict
};

#endif // MICROPY_PY_UBLUEPY
