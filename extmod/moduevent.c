/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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

#include "py/runtime.h"
#include "extmod/moduevent.h"

#if MICROPY_PY_UEVENT != MICROPY_PY_UEVENT_IMPL_NONE

STATIC const mp_obj_fun_builtin_var_t poll_unregister_obj;

STATIC void mp_uevent_poll_entry_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_poll_entry_base_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute.
        if (attr == MP_QSTR_data) {
            dest[0] = self->user_data;
        } else if (attr == MP_QSTR_flags) {
            dest[0] = MP_OBJ_NEW_SMALL_INT(self->user_flags);
        } else if (attr == MP_QSTR_unregister) {
            dest[0] = MP_OBJ_FROM_PTR(&poll_unregister_obj);
            dest[1] = MP_OBJ_FROM_PTR(self);
        } else if (attr == self->user_method_name) {
            dest[0] = self->user_method_obj;
            dest[1] = self_in;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store attribute.
        if (attr == MP_QSTR_data) {
            self->user_data = dest[1];
            dest[0] = MP_OBJ_NULL; // success
        } else if (attr != MP_QSTR_flags) {
            self->user_method_name = attr;
            self->user_method_obj = dest[1];
            dest[0] = MP_OBJ_NULL; // success
        }
    }
}

const mp_obj_type_t mp_type_poll_entry = {
    { &mp_type_type },
    .name = MP_QSTR_poll_entry,
    .attr = mp_uevent_poll_entry_attr,
};

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_register_obj, 2, 3, mp_uevent_poll_register);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_unregister_obj, 1, 2, mp_uevent_poll_unregister);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_poll_ms_obj, 1, 2, mp_uevent_poll_poll_ms);

STATIC const mp_rom_map_elem_t poll_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&poll_register_obj) },
    { MP_ROM_QSTR(MP_QSTR_unregister), MP_ROM_PTR(&poll_unregister_obj) },
    { MP_ROM_QSTR(MP_QSTR_poll_ms), MP_ROM_PTR(&poll_poll_ms_obj) },
};
STATIC MP_DEFINE_CONST_DICT(poll_locals_dict, poll_locals_dict_table);

const mp_obj_type_t mp_type_poll = {
    { &mp_type_type },
    .name = MP_QSTR_poll,
    .getiter = mp_identity_getiter,
    .iternext = mp_uevent_poll_iternext,
    .locals_dict = (mp_obj_dict_t *)&poll_locals_dict,
};

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_uevent_poll_obj, mp_uevent_poll);

STATIC const mp_rom_map_elem_t mp_module_uevent_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uevent) },
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&mp_uevent_poll_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_uevent_globals, mp_module_uevent_globals_table);

const mp_obj_module_t mp_module_uevent = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_uevent_globals,
};

#endif // MICROPY_PY_UEVENT != MICROPY_PY_UEVENT_IMPL_NONE
