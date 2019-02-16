/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Artur Pacholec
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

#include <string.h>
#include <stdio.h>

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/Address.h"
#include "shared-module/bleio/Address.h"

#define ADDRESS_BYTE_LEN 12

STATIC uint8_t xdigit_8b_value(byte nibble1, byte nibble2) {
    return unichar_xdigit_value(nibble1) | (unichar_xdigit_value(nibble2) << 4);
}


//| .. currentmodule:: bleio
//|
//| :class:`Address` -- BLE address
//| =========================================================
//|
//| Encapsulates the address of a BLE device.
//|

//| .. class:: Address(address)
//|
//|   Create a new Address object encapsulating the address value.
//|   The value itself can be one of:
//|
//|   - a `str` value in the format of 'XXXXXXXXXXXX' or 'XX:XX:XX:XX:XX:XX' (12 hex digits)
//|   - a `bytes` or `bytearray` containing 6 bytes
//|   - another Address object
//|
//|   :param address: The address to encapsulate
//|

//|   .. attribute:: type
//|
//|     The address type. One of:
//|
//|     - `bleio.AddressType.PUBLIC`
//|     - `bleio.AddressType.RANDOM_STATIC`
//|     - `bleio.AddressType.RANDOM_PRIVATE_RESOLVABLE`
//|     - `bleio.AddressType.RANDOM_PRIVATE_NON_RESOLVABLE`
//|
STATIC mp_obj_t bleio_address_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bleio_address_obj_t *self = m_new_obj(bleio_address_obj_t);
    self->base.type = &bleio_address_type;
    self->type = ADDRESS_PUBLIC;

    const mp_obj_t address = args[ARG_address].u_obj;

    if (MP_OBJ_IS_STR(address)) {
        GET_STR_DATA_LEN(address, str, str_len);

        size_t value_index = 0;
        int str_index = str_len;
        bool error = false;

        // Loop until fewer than two characters left.
        while (str_index >= 1 && value_index < sizeof(self->value)) {
            if (str[str_index] == ':') {
                // Skip colon separators.
                str_index--;
                continue;
            }

            if (!unichar_isxdigit(str[str_index]) ||
                !unichar_isxdigit(str[str_index-1])) {
                error = true;
                break;
            }

            self->value[value_index] = xdigit_8b_value(str[str_index],
                                                       str[str_index-1]);
            value_index += 1;
            str_index -= 2;
            }
            // Check for correct number of hex digits and no parsing errors.
        if (error || value_index != ADDRESS_BYTE_LEN || str_index != -1) {
            mp_raise_ValueError_varg(translate("Address is not %d bytes long or is in wrong format"),
                                     ADDRESS_BYTE_LEN);
        }
    } else if (MP_OBJ_IS_TYPE(address, &mp_type_bytearray) || MP_OBJ_IS_TYPE(address, &mp_type_bytes)) {
            mp_buffer_info_t buf_info;
            mp_get_buffer_raise(address, &buf_info, MP_BUFFER_READ);
            if (buf_info.len != BLEIO_ADDRESS_BYTES) {
                mp_raise_ValueError_varg(translate("Address must be %d bytes long"), BLEIO_ADDRESS_BYTES);
            }

            for (size_t b = 0; b < BLEIO_ADDRESS_BYTES; ++b) {
                self->value[BLEIO_ADDRESS_BYTES - b - 1] = ((uint8_t*)buf_info.buf)[b];
            }
    } else if (MP_OBJ_IS_TYPE(address, &bleio_address_type)) {
        // deep copy
        bleio_address_obj_t *other = MP_OBJ_TO_PTR(address);
        self->type = other->type;
        memcpy(self->value, other->value, BLEIO_ADDRESS_BYTES);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC void bleio_address_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_address_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "Address('%02x:%02x:%02x:%02x:%02x:%02x')",
        self->value[5], self->value[4], self->value[3],
        self->value[2], self->value[1], self->value[0]);
}

STATIC mp_obj_t bleio_address_get_type(mp_obj_t self_in) {
    bleio_address_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->type == ADDRESS_PUBLIC) {
        return (mp_obj_t)&bleio_addresstype_public_obj;
    } else if (self->type == ADDRESS_RANDOM_STATIC) {
        return (mp_obj_t)&bleio_addresstype_random_static_obj;
    } else if (self->type == ADDRESS_RANDOM_PRIVATE_RESOLVABLE) {
        return (mp_obj_t)&bleio_addresstype_random_private_resolvable_obj;
    } else if (self->type == ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE) {
        return (mp_obj_t)&bleio_addresstype_random_private_non_resolvable_obj;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_address_get_type_obj, bleio_address_get_type);

const mp_obj_property_t bleio_address_type_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_address_get_type_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t bleio_address_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_type), MP_ROM_PTR(&bleio_address_type_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_address_locals_dict, bleio_address_locals_dict_table);

const mp_obj_type_t bleio_address_type = {
    { &mp_type_type },
    .name = MP_QSTR_Address,
    .print = bleio_address_print,
    .make_new = bleio_address_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_address_locals_dict
};
