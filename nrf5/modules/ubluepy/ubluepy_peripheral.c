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

#if MICROPY_PY_UBLUEPY_PERIPHERAL

STATIC const mp_map_elem_t ubluepy_peripheral_locals_dict_table[] = {
#if 0

// #if UBLUEPY_CENTRAL
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
// #if UBLUEPY_PERIPHERAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_advertise),            (mp_obj_t)(&ubluepy_peripheral_advertise_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),           (mp_obj_t)(&ubluepy_peripheral_disconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setService),           (mp_obj_t)(&ubluepy_peripheral_set_service_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setCharacteristic),    (mp_obj_t)(&ubluepy_peripheral_set_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setDescriptor),        (mp_obj_t)(&ubluepy_peripheral_set_desc_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeCharacteristic),  (mp_obj_t)(&ubluepy_peripheral_write_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readCharacteristic),   (mp_obj_t)(&ubluepy_peripheral_read_char_obj) },
// #if UBLUEPY_BROADCASTER
    { MP_OBJ_NEW_QSTR(MP_QSTR_advertise),            (mp_obj_t)(&ubluepy_peripheral_advertise_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_peripheral_locals_dict, ubluepy_peripheral_locals_dict_table);

const mp_obj_type_t ubluepy_peripheral_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peripheral,
#if 0
    .print = ubluepy_peripheral_print,
    .make_new = ubluepy_peripheral_make_new,
    .locals_dict = (mp_obj_t)&ubluepy_peripheral_locals_dict
#endif
};

#endif // MICROPY_PY_UBLUEPY_PERIPHERAL
