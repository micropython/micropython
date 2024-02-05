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
#include "ble_drv.h"

STATIC void ubluepy_characteristic_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_characteristic_obj_t * self = (ubluepy_characteristic_obj_t *)o;

    mp_printf(print, "Characteristic(handle: 0x" HEX2_FMT ", conn_handle: " HEX2_FMT ")",
              self->handle, self->p_service->p_periph->conn_handle);
}

STATIC mp_obj_t ubluepy_characteristic_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid,  MP_ARG_REQUIRED| MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_props, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UBLUEPY_PROP_READ | UBLUEPY_PROP_WRITE} },
        { MP_QSTR_attrs, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_characteristic_obj_t *s = mp_obj_malloc(ubluepy_characteristic_obj_t, type);

    mp_obj_t uuid_obj = args[0].u_obj;

    if (uuid_obj == mp_const_none) {
        return MP_OBJ_FROM_PTR(s);
    }

    if (mp_obj_is_type(uuid_obj, &ubluepy_uuid_type)) {
        s->p_uuid = MP_OBJ_TO_PTR(uuid_obj);
        // (void)sd_characterstic_add(s);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid UUID parameter"));
    }

    if (args[1].u_int > 0) {
        s->props = (uint8_t)args[1].u_int;
    }

    if (args[2].u_int > 0) {
        s->attrs = (uint8_t)args[2].u_int;
    }

    // clear pointer to service
    s->p_service = NULL;

    // clear pointer to char value data
    s->value_data = NULL;

    return MP_OBJ_FROM_PTR(s);
}

void char_data_callback(mp_obj_t self_in, uint16_t length, uint8_t * p_data) {
    ubluepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);
    self->value_data = mp_obj_new_bytearray(length, p_data);
}

/// \method read()
/// Read Characteristic value.
///
STATIC mp_obj_t char_read(mp_obj_t self_in) {
    ubluepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);

#if MICROPY_PY_UBLUEPY_CENTRAL
    // TODO: free any previous allocation of value_data

    ble_drv_attr_c_read(self->p_service->p_periph->conn_handle,
                        self->handle,
                        self_in,
                        char_data_callback);

    return self->value_data;
#else
    (void)self;
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_characteristic_read_obj, char_read);

/// \method write(data, [with_response=False])
/// Write Characteristic value.
///
STATIC mp_obj_t char_write(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    ubluepy_characteristic_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_obj_t data                      = pos_args[1];

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_with_response, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    // figure out mode of the Peripheral
    ubluepy_role_type_t role = self->p_service->p_periph->role;

    if (role == UBLUEPY_ROLE_PERIPHERAL) {
        if (self->props & UBLUEPY_PROP_NOTIFY) {
            ble_drv_attr_s_notify(self->p_service->p_periph->conn_handle,
                                  self->handle,
                                  bufinfo.len,
                                  bufinfo.buf);
        } else {
            ble_drv_attr_s_write(self->p_service->p_periph->conn_handle,
                                 self->handle,
                                 bufinfo.len,
                                 bufinfo.buf);
        }
    } else {
#if MICROPY_PY_UBLUEPY_CENTRAL
        bool with_response = args[0].u_bool;

        ble_drv_attr_c_write(self->p_service->p_periph->conn_handle,
                             self->handle,
                             bufinfo.len,
                             bufinfo.buf,
                             with_response);
#endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ubluepy_characteristic_write_obj, 2, char_write);

/// \method properties()
/// Read Characteristic value properties.
///
STATIC mp_obj_t char_properties(mp_obj_t self_in) {
    ubluepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->props);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_characteristic_get_properties_obj, char_properties);

/// \method uuid()
/// Get UUID instance of the characteristic.
///
STATIC mp_obj_t char_uuid(mp_obj_t self_in) {
    ubluepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->p_uuid);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_characteristic_get_uuid_obj, char_uuid);


STATIC const mp_rom_map_elem_t ubluepy_characteristic_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read),                MP_ROM_PTR(&ubluepy_characteristic_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),               MP_ROM_PTR(&ubluepy_characteristic_write_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_supportsRead),        MP_ROM_PTR(&ubluepy_characteristic_supports_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_propertiesToString),  MP_ROM_PTR(&ubluepy_characteristic_properties_to_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_getHandle),           MP_ROM_PTR(&ubluepy_characteristic_get_handle_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_peripheral),          MP_ROM_PTR(&ubluepy_characteristic_get_peripheral_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_uuid),                MP_ROM_PTR(&ubluepy_characteristic_get_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_properties),          MP_ROM_PTR(&ubluepy_characteristic_get_properties_obj) },

    { MP_ROM_QSTR(MP_QSTR_PROP_BROADCAST),      MP_ROM_INT(UBLUEPY_PROP_BROADCAST) },
    { MP_ROM_QSTR(MP_QSTR_PROP_READ),           MP_ROM_INT(UBLUEPY_PROP_READ) },
    { MP_ROM_QSTR(MP_QSTR_PROP_WRITE_WO_RESP),  MP_ROM_INT(UBLUEPY_PROP_WRITE_WO_RESP) },
    { MP_ROM_QSTR(MP_QSTR_PROP_WRITE),          MP_ROM_INT(UBLUEPY_PROP_WRITE) },
    { MP_ROM_QSTR(MP_QSTR_PROP_NOTIFY),         MP_ROM_INT(UBLUEPY_PROP_NOTIFY) },
    { MP_ROM_QSTR(MP_QSTR_PROP_INDICATE),       MP_ROM_INT(UBLUEPY_PROP_INDICATE) },
    { MP_ROM_QSTR(MP_QSTR_PROP_AUTH_SIGNED_WR), MP_ROM_INT(UBLUEPY_PROP_AUTH_SIGNED_WR) },

#if MICROPY_PY_UBLUEPY_PERIPHERAL
    { MP_ROM_QSTR(MP_QSTR_ATTR_CCCD),           MP_ROM_INT(UBLUEPY_ATTR_CCCD) },
#endif

#if MICROPY_PY_UBLUEPY_CENTRAL
    { MP_ROM_QSTR(MP_QSTR_PROP_AUTH_SIGNED_WR), MP_ROM_INT(UBLUEPY_ATTR_SCCD) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_characteristic_locals_dict, ubluepy_characteristic_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    ubluepy_characteristic_type,
    MP_QSTR_Characteristic,
    MP_TYPE_FLAG_NONE,
    make_new, ubluepy_characteristic_make_new,
    print, ubluepy_characteristic_print,
    locals_dict, &ubluepy_characteristic_locals_dict
    );

#endif // MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL
