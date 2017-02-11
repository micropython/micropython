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
#include "modubluepy.h"
#include "softdevice.h"

#if MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL

STATIC void ubluepy_service_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_service_obj_t * self = (ubluepy_service_obj_t *)o;

    mp_printf(print, "Service(handle: 0x" HEX2_FMT ")", self->handle);
}

STATIC mp_obj_t ubluepy_service_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_NEW_UUID, ARG_NEW_TYPE };

    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_UUID, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_TYPE, MP_ARG_INT, {.u_int = UBLUEPY_SERVICE_PRIMARY} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_service_obj_t *s = m_new_obj(ubluepy_service_obj_t);
    s->base.type = type;

    mp_obj_t uuid_obj = args[ARG_NEW_UUID].u_obj;

    if (uuid_obj == MP_OBJ_NULL) {
        return MP_OBJ_FROM_PTR(s);
    }

    if (MP_OBJ_IS_TYPE(uuid_obj, &ubluepy_uuid_type)) {
        s->p_uuid = MP_OBJ_TO_PTR(uuid_obj);

        uint8_t type = args[ARG_NEW_TYPE].u_int;
        if (type > 0 &&  type <= UBLUEPY_SERVICE_PRIMARY) {
            s->type = type;
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                      "Invalid Service type"));
        }

        (void)sd_service_add(s);

    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "Invalid UUID parameter"));
    }

    return MP_OBJ_FROM_PTR(s);
}


STATIC const mp_map_elem_t ubluepy_service_locals_dict_table[] = {
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_getCharacteristic), (mp_obj_t)(&ubluepy_service_get_char_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setCharacteristic), (mp_obj_t)(&ubluepy_service_set_char_obj) },
    // Properties
    { MP_OBJ_NEW_QSTR(MP_QSTR_peripheral), (mp_obj_t)(&ubluepy_service_get_peripheral_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_uuid),       (mp_obj_t)(&ubluepy_service_get_uuid_obj) },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_PRIMARY),   MP_OBJ_NEW_SMALL_INT(UBLUEPY_SERVICE_PRIMARY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECONDARY), MP_OBJ_NEW_SMALL_INT(UBLUEPY_SERVICE_SECONDARY) },
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_service_locals_dict, ubluepy_service_locals_dict_table);

const mp_obj_type_t ubluepy_service_type = {
    { &mp_type_type },
    .name = MP_QSTR_Service,
    .print = ubluepy_service_print,
    .make_new = ubluepy_service_make_new,
    .locals_dict = (mp_obj_t)&ubluepy_service_locals_dict
};

#endif // MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL
