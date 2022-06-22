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

#if MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL

#include "modubluepy.h"

STATIC void ubluepy_delegate_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_delegate_obj_t * self = (ubluepy_delegate_obj_t *)o;
    (void)self;
    mp_printf(print, "DefaultDelegate()");
}

STATIC mp_obj_t ubluepy_delegate_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    ubluepy_delegate_obj_t *s = mp_obj_malloc(ubluepy_delegate_obj_t, type);

    return MP_OBJ_FROM_PTR(s);
}

/// \method handleConnection()
/// Handle connection events.
///
STATIC mp_obj_t delegate_handle_conn(mp_obj_t self_in) {
    ubluepy_delegate_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_delegate_handle_conn_obj, delegate_handle_conn);

/// \method handleNotification()
/// Handle notification events.
///
STATIC mp_obj_t delegate_handle_notif(mp_obj_t self_in) {
    ubluepy_delegate_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_delegate_handle_notif_obj, delegate_handle_notif);

STATIC const mp_rom_map_elem_t ubluepy_delegate_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_handleConnection),   MP_ROM_PTR(&ubluepy_delegate_handle_conn_obj) },
    { MP_ROM_QSTR(MP_QSTR_handleNotification), MP_ROM_PTR(&ubluepy_delegate_handle_notif_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_handleDiscovery),    MP_ROM_PTR(&ubluepy_delegate_handle_disc_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_delegate_locals_dict, ubluepy_delegate_locals_dict_table);

const mp_obj_type_t ubluepy_delegate_type = {
    { &mp_type_type },
    .name = MP_QSTR_DefaultDelegate,
    .print = ubluepy_delegate_print,
    .make_new = ubluepy_delegate_make_new,
    .locals_dict = (mp_obj_dict_t*)&ubluepy_delegate_locals_dict
};

#endif // MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL
