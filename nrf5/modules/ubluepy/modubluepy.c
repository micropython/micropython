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

#if MICROPY_PY_UBLUEPY

extern const mp_obj_type_t ubluepy_peripheral_type;
extern const mp_obj_type_t ubluepy_service_type;
extern const mp_obj_type_t ubluepy_uuid_type;
extern const mp_obj_type_t ubluepy_characteristic_type;
extern const mp_obj_type_t ubluepy_delegate_type;
extern const mp_obj_type_t ubluepy_constants_type;

STATIC const mp_map_elem_t mp_module_ubluepy_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_ubluepy) },
#if MICROPY_PY_UBLUEPY_PERIPHERAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_Peripheral),      (mp_obj_t)&ubluepy_peripheral_type },
#endif
#if MICROPY_PY_UBLUEPY_CENTRAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_Central),         (mp_obj_t)&ubluepy_central_type },
#endif
#if MICROPY_PY_UBLUEPY_SCANNER
    { MP_OBJ_NEW_QSTR(MP_QSTR_Scanner),         (mp_obj_t)&ubluepy_scanner_type },
#endif
#if MICROPY_PY_UBLUEPY_CENTRAL
    { MP_OBJ_NEW_QSTR(MP_QSTR_ScanEntry),       (mp_obj_t)&ubluepy_scan_entry_type },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_DefaultDelegate), (mp_obj_t)&ubluepy_delegate_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_UUID),            (mp_obj_t)&ubluepy_uuid_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Service),         (mp_obj_t)&ubluepy_service_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Characteristic),  (mp_obj_t)&ubluepy_characteristic_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_constants),       (mp_obj_t)&ubluepy_constants_type },
#if MICROPY_PY_UBLUEPY_DESCRIPTOR
    { MP_OBJ_NEW_QSTR(MP_QSTR_Descriptor),      (mp_obj_t)&ubluepy_descriptor_type },
#endif
};


STATIC MP_DEFINE_CONST_DICT(mp_module_ubluepy_globals, mp_module_ubluepy_globals_table);

const mp_obj_module_t mp_module_ubluepy = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ubluepy_globals,
};

#endif // MICROPY_PY_UBLUEPY
