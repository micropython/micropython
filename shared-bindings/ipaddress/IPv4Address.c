/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#include "shared-bindings/ipaddress/IPv4Address.h"

#include <string.h>
#include <stdio.h>

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/ipaddress/__init__.h"

//| class IPv4Address:
//|     """Encapsulates an IPv4 address."""
//|

//|     def __init__(self, address: Union[int, str, bytes]) -> None:
//|         """Create a new IPv4Address object encapsulating the address value.
//|
//|            The value itself can either be bytes or a string formatted address."""
//|         ...
//|
STATIC mp_obj_t ipaddress_ipv4address_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t address = args[ARG_address].u_obj;

    uint32_t value;
    uint8_t *buf = NULL;
    if (mp_obj_get_int_maybe(address, (mp_int_t *)&value)) {
        // We're done.
        buf = (uint8_t *)&value;
    } else if (mp_obj_is_str(address)) {
        GET_STR_DATA_LEN(address, str_data, str_len);
        if (!ipaddress_parse_ipv4address((const char *)str_data, str_len, &value)) {
            mp_raise_ValueError(translate("Not a valid IP string"));
        }
        buf = (uint8_t *)&value;
    } else {
        mp_buffer_info_t buf_info;
        if (mp_get_buffer(address, &buf_info, MP_BUFFER_READ)) {
            if (buf_info.len != 4) {
                mp_raise_ValueError_varg(translate("Address must be %d bytes long"), 4);
            }
            buf = buf_info.buf;
        }
    }


    ipaddress_ipv4address_obj_t *self = m_new_obj(ipaddress_ipv4address_obj_t);
    self->base.type = &ipaddress_ipv4address_type;

    common_hal_ipaddress_ipv4address_construct(self, buf, 4);

    return MP_OBJ_FROM_PTR(self);
}

//|     packed: bytes
//|     """The bytes that make up the address (read-only)."""
//|
STATIC mp_obj_t ipaddress_ipv4address_get_packed(mp_obj_t self_in) {
    ipaddress_ipv4address_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_ipaddress_ipv4address_get_packed(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(ipaddress_ipv4address_get_packed_obj, ipaddress_ipv4address_get_packed);

const mp_obj_property_t ipaddress_ipv4address_packed_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ipaddress_ipv4address_get_packed_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     version: int
//|     """4 for IPv4, 6 for IPv6"""
//|
STATIC mp_obj_t ipaddress_ipv4address_get_version(mp_obj_t self_in) {
    ipaddress_ipv4address_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf_info;
    mp_obj_t address_bytes = common_hal_ipaddress_ipv4address_get_packed(self);
    mp_get_buffer_raise(address_bytes, &buf_info, MP_BUFFER_READ);
    mp_int_t version = 6;
    if (buf_info.len == 4) {
        version = 4;
    }

    return MP_OBJ_NEW_SMALL_INT(version);
}
MP_DEFINE_CONST_FUN_OBJ_1(ipaddress_ipv4address_get_version_obj, ipaddress_ipv4address_get_version);

const mp_obj_property_t ipaddress_ipv4address_version_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&ipaddress_ipv4address_get_version_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def __eq__(self, other: object) -> bool:
//|         """Two Address objects are equal if their addresses and address types are equal."""
//|         ...
//|
STATIC mp_obj_t ipaddress_ipv4address_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    switch (op) {
        // Two Addresses are equal if their address bytes and address_type are equal
        case MP_BINARY_OP_EQUAL:
            if (mp_obj_is_type(rhs_in, &ipaddress_ipv4address_type)) {
                ipaddress_ipv4address_obj_t *lhs = MP_OBJ_TO_PTR(lhs_in);
                ipaddress_ipv4address_obj_t *rhs = MP_OBJ_TO_PTR(rhs_in);
                return mp_obj_new_bool(
                    mp_obj_equal(common_hal_ipaddress_ipv4address_get_packed(lhs),
                        common_hal_ipaddress_ipv4address_get_packed(rhs)));

            } else {
                return mp_const_false;
            }

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

//|     def __hash__(self) -> int:
//|         """Returns a hash for the IPv4Address data."""
//|         ...
//|
STATIC mp_obj_t ipaddress_ipv4address_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    switch (op) {
        // Two Addresses are equal if their address bytes and address_type are equal
        case MP_UNARY_OP_HASH: {
            mp_obj_t bytes = common_hal_ipaddress_ipv4address_get_packed(MP_OBJ_TO_PTR(self_in));
            GET_STR_HASH(bytes, h);
            if (h == 0) {
                GET_STR_DATA_LEN(bytes, data, len);
                h = qstr_compute_hash(data, len);
            }
            return MP_OBJ_NEW_SMALL_INT(h);
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC void ipaddress_ipv4address_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    ipaddress_ipv4address_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf_info;
    mp_obj_t address_bytes = common_hal_ipaddress_ipv4address_get_packed(self);
    mp_get_buffer_raise(address_bytes, &buf_info, MP_BUFFER_READ);

    const uint8_t *buf = (uint8_t *)buf_info.buf;
    mp_printf(print, "%d.%d.%d.%d", buf[0], buf[1], buf[2], buf[3]);
}

STATIC const mp_rom_map_elem_t ipaddress_ipv4address_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_packed),                 MP_ROM_PTR(&ipaddress_ipv4address_packed_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ipaddress_ipv4address_locals_dict, ipaddress_ipv4address_locals_dict_table);

const mp_obj_type_t ipaddress_ipv4address_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_Address,
    .make_new = ipaddress_ipv4address_make_new,
    .locals_dict = (mp_obj_dict_t *)&ipaddress_ipv4address_locals_dict,
    .print = ipaddress_ipv4address_print,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = ipaddress_ipv4address_unary_op,
        .binary_op = ipaddress_ipv4address_binary_op,
        )
};
