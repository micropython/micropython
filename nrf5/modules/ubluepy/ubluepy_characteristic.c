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

#if MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL

STATIC const mp_map_elem_t ubluepy_characteristic_locals_dict_table[] = {
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),               (mp_obj_t)(&ubluepy_characteristic_read_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),              (mp_obj_t)(&ubluepy_characteristic_write_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_supportsRead),       (mp_obj_t)(&ubluepy_characteristic_supports_read_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_propertiesToString), (mp_obj_t)(&ubluepy_characteristic_properties_to_str_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getHandle),          (mp_obj_t)(&ubluepy_characteristic_get_handle_obj) },

    // Properties
    { MP_OBJ_NEW_QSTR(MP_QSTR_peripheral), (mp_obj_t)(&ubluepy_characteristic_get_peripheral_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_uuid),       (mp_obj_t)(&ubluepy_characteristic_get_uuid_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_properties), (mp_obj_t)(&ubluepy_characteristic_get_properties_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_characteristic_locals_dict, ubluepy_characteristic_locals_dict_table);

const mp_obj_type_t ubluepy_characteristic_type = {
    { &mp_type_type },
    .name = MP_QSTR_Characteristic,
#if 0
    .print = ubluepy_characteristic_print,
    .make_new = ubluepy_characteristic_make_new,
    .locals_dict = (mp_obj_t)&ubluepy_characteristic_locals_dict
#endif
};

#endif // MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL
