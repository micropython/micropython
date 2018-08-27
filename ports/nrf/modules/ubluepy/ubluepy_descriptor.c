/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 - 2018 Glenn Ruben Bakke
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

void desc_data_callback(mp_obj_t self_in, uint16_t length, uint8_t * p_data) {
    ubluepy_descriptor_obj_t * self = MP_OBJ_TO_PTR(self_in);
    self->value_data = mp_obj_new_bytearray(length, p_data);
}

/// \method read()
/// Read Descriptor value.
///
STATIC mp_obj_t desc_read(mp_obj_t self_in) {
    ubluepy_descriptor_obj_t * self = MP_OBJ_TO_PTR(self_in);

#if MICROPY_PY_UBLUEPY_CENTRAL
    ble_drv_attr_c_read(self->p_char->p_service->p_periph->conn_handle,
                        self->handle,
                        self_in,
                        desc_data_callback);

    return self->value_data;
#else
    (void)self;
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_descriptor_read_obj, desc_read);

/// \method write(data)
/// Write Descriptor value.
///
STATIC mp_obj_t desc_write(mp_obj_t self_in, mp_obj_t data) {
#if MICROPY_PY_UBLUEPY_CENTRAL
    ubluepy_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    ble_drv_attr_c_write(self->p_char->p_service->p_periph->conn_handle,
                         self->handle,
                         bufinfo.len,
                         bufinfo.buf,
                         false);
#endif

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_descriptor_write_obj, desc_write);

/// \method uuid()
/// Get UUID instance of the descriptor.
///
STATIC mp_obj_t desc_uuid(mp_obj_t self_in) {
    ubluepy_descriptor_obj_t * self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->p_uuid);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_descriptor_get_uuid_obj, desc_uuid);

/// \method getHandle()
/// Get handle of the characteristic.
///
STATIC mp_obj_t desc_handle(mp_obj_t self_in) {
    ubluepy_descriptor_obj_t * self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->handle);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_descriptor_get_handle_obj, desc_handle);

STATIC const mp_rom_map_elem_t ubluepy_descriptor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read),                MP_ROM_PTR(&ubluepy_descriptor_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),               MP_ROM_PTR(&ubluepy_descriptor_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid),                MP_ROM_PTR(&ubluepy_descriptor_get_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_getHandle),           MP_ROM_PTR(&ubluepy_descriptor_get_handle_obj) },
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
