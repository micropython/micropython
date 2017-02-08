/*
 * This file is part of the Micro Python project, http://micropython.org/
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


#if MICROPY_PY_UBLUEPY

typedef struct _ubluepy_peripheral_obj_t {
    mp_obj_base_t base;
    // services
} ubluepy_peripheral_obj_t;

STATIC void ubluepy_peripheral_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_peripheral_obj_t * self = (ubluepy_peripheral_obj_t *)o;
    (void)self;
    mp_printf(print, "Peripheral");
}

STATIC mp_obj_t ubluepy_peripheral_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_NEW_DEVICE_ADDR,
        ARG_NEW_ADDR_TYPE
    };

    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_DEVICE_ADDR, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_ADDR_TYPE,   MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_peripheral_obj_t *s = m_new_obj(ubluepy_peripheral_obj_t);
    s->base.type = type;

    return MP_OBJ_FROM_PTR(s);
}


/// \method advertise()
/// Start advertising.
///
STATIC mp_obj_t peripheral_advertise(mp_obj_t self_in) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_peripheral_advertise_obj, peripheral_advertise);

/// \method disconnect()
/// disconnect connection.
///
STATIC mp_obj_t peripheral_disconnect(mp_obj_t self_in) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_peripheral_disconnect_obj, peripheral_disconnect);

/// \method addService(Service)
/// Add service to the Peripheral.
///
STATIC mp_obj_t peripheral_add_service(mp_obj_t self_in, mp_obj_t uuid) {
    ubluepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_peripheral_add_service_obj, peripheral_add_service);



STATIC const mp_map_elem_t ubluepy_peripheral_locals_dict_table[] = {
#if MICROPY_PY_UBLUEPY_CENTRAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect),              (mp_obj_t)(&ubluepy_peripheral_connect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),           (mp_obj_t)(&ubluepy_peripheral_disconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getServices),          (mp_obj_t)(&ubluepy_peripheral_get_services_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getServiceByUUID),     (mp_obj_t)(&ubluepy_peripheral_get_service_by_uuid_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getCharacteristics),   (mp_obj_t)(&ubluepy_peripheral_get_chars_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getDescriptors),       (mp_obj_t)(&ubluepy_peripheral_get_descs_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_withDelegate),         (mp_obj_t)(&ubluepy_peripheral_with_delegate_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_waitForNotifications), (mp_obj_t)(&ubluepy_peripheral_wait_for_notif_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeCharacteristic),  (mp_obj_t)(&ubluepy_peripheral_write_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readCharacteristic),   (mp_obj_t)(&ubluepy_peripheral_read_char_obj) },
#endif
#if MICROPY_PY_UBLUEPY_PERIPHERAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_advertise),            (mp_obj_t)(&ubluepy_peripheral_advertise_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),           (mp_obj_t)(&ubluepy_peripheral_disconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_addService),           (mp_obj_t)(&ubluepy_peripheral_add_service_obj) },
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_addCharacteristic),    (mp_obj_t)(&ubluepy_peripheral_add_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_addDescriptor),        (mp_obj_t)(&ubluepy_peripheral_add_desc_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeCharacteristic),  (mp_obj_t)(&ubluepy_peripheral_write_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readCharacteristic),   (mp_obj_t)(&ubluepy_peripheral_read_char_obj) },
#endif
#endif
#if MICROPY_PY_UBLUEPY_BROADCASTER
    { MP_OBJ_NEW_QSTR(MP_QSTR_advertise),            (mp_obj_t)(&ubluepy_peripheral_advertise_obj) },
#endif
#if MICROPY_PY_UBLUEPY_OBSERVER
	// Nothing yet.
#endif
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_peripheral_locals_dict, ubluepy_peripheral_locals_dict_table);

const mp_obj_type_t ubluepy_peripheral_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peripheral,
    .print = ubluepy_peripheral_print,
    .make_new = ubluepy_peripheral_make_new,
    .locals_dict = (mp_obj_t)&ubluepy_peripheral_locals_dict
};

#endif // MICROPY_PY_UBLUEPY
