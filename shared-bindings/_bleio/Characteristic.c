/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"

//| class Characteristic:
//|     """Stores information about a BLE service characteristic and allows reading
//|        and writing of the characteristic's value."""
//|
//|     def __init__(self) -> None:
//|         """There is no regular constructor for a Characteristic. A new local Characteristic can be created
//|         and attached to a Service by calling `add_to_service()`.
//|         Remote Characteristic objects are created by `Connection.discover_remote_services()`
//|         as part of remote Services."""
//|         ...
//|

//|     def add_to_service(self, service: Service, uuid: UUID, *, properties: int = 0,
//|                        read_perm: int = Attribute.OPEN, write_perm: int = Attribute.OPEN,
//|                        max_length: int = 20, fixed_length: bool = False,
//|                        initial_value: Optional[ReadableBuffer] = None,
//|                        user_description: Optional[str] = None) -> Characteristic:
//|         """Create a new Characteristic object, and add it to this Service.
//|
//|         :param Service service: The service that will provide this characteristic
//|         :param UUID uuid: The uuid of the characteristic
//|         :param int properties: The properties of the characteristic,
//|            specified as a bitmask of these values bitwise-or'd together:
//|            `BROADCAST`, `INDICATE`, `NOTIFY`, `READ`, `WRITE`, `WRITE_NO_RESPONSE`.
//|         :param int read_perm: Specifies whether the characteristic can be read by a client, and if so, which
//|            security mode is required. Must be one of the integer values `Attribute.NO_ACCESS`, `Attribute.OPEN`,
//|            `Attribute.ENCRYPT_NO_MITM`, `Attribute.ENCRYPT_WITH_MITM`, `Attribute.LESC_ENCRYPT_WITH_MITM`,
//|            `Attribute.SIGNED_NO_MITM`, or `Attribute.SIGNED_WITH_MITM`.
//|         :param int write_perm: Specifies whether the characteristic can be written by a client, and if so, which
//|            security mode is required. Values allowed are the same as ``read_perm``.
//|         :param int max_length: Maximum length in bytes of the characteristic value. The maximum allowed is
//|          is 512, or possibly 510 if ``fixed_length`` is False. The default, 20, is the maximum
//|          number of data bytes that fit in a single BLE 4.x ATT packet.
//|         :param bool fixed_length: True if the characteristic value is of fixed length.
//|         :param ~_typing.ReadableBuffer initial_value: The initial value for this characteristic. If not given, will be
//|          filled with zeros.
//|         :param str user_description: User friendly description of the characteristic
//|
//|         :return: the new Characteristic."""
//|         ...
//|
STATIC mp_obj_t bleio_characteristic_add_to_service(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // class is arg[0], which we can ignore.

    enum { ARG_service, ARG_uuid, ARG_properties, ARG_read_perm, ARG_write_perm,
           ARG_max_length, ARG_fixed_length, ARG_initial_value, ARG_user_description };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_service,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_uuid,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_properties, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_read_perm, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SECURITY_MODE_OPEN} },
        { MP_QSTR_write_perm, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SECURITY_MODE_OPEN} },
        { MP_QSTR_max_length, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 20} },
        { MP_QSTR_fixed_length, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_initial_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_user_description, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t service_obj = args[ARG_service].u_obj;
    if (!mp_obj_is_type(service_obj, &bleio_service_type)) {
        mp_raise_TypeError(translate("Expected a Service"));
    }

    const mp_obj_t uuid_obj = args[ARG_uuid].u_obj;
    if (!mp_obj_is_type(uuid_obj, &bleio_uuid_type)) {
        mp_raise_TypeError(translate("Expected a UUID"));
    }

    const bleio_characteristic_properties_t properties = args[ARG_properties].u_int;
    if (properties & ~CHAR_PROP_ALL) {
        mp_raise_ValueError(translate("Invalid properties"));
    }

    const bleio_attribute_security_mode_t read_perm = args[ARG_read_perm].u_int;
    common_hal_bleio_attribute_security_mode_check_valid(read_perm);

    const bleio_attribute_security_mode_t write_perm = args[ARG_write_perm].u_int;
    common_hal_bleio_attribute_security_mode_check_valid(write_perm);

    const mp_int_t max_length_int = args[ARG_max_length].u_int;
    if (max_length_int < 0) {
        mp_raise_ValueError(translate("max_length must be >= 0"));
    }
    const size_t max_length = (size_t)max_length_int;
    const bool fixed_length = args[ARG_fixed_length].u_bool;
    mp_obj_t initial_value = args[ARG_initial_value].u_obj;

    mp_buffer_info_t initial_value_bufinfo;
    if (initial_value == mp_const_none) {
        if (fixed_length && max_length > 0) {
            initial_value = mp_obj_new_bytes_of_zeros(max_length);
        } else {
            initial_value = mp_const_empty_bytes;
        }
    }

    mp_get_buffer_raise(initial_value, &initial_value_bufinfo, MP_BUFFER_READ);
    if (initial_value_bufinfo.len > max_length ||
        (fixed_length && initial_value_bufinfo.len != max_length)) {
        mp_raise_ValueError(translate("initial_value length is wrong"));
    }

    const char *user_description = NULL;
    if (args[ARG_user_description].u_obj != mp_const_none) {
        user_description = mp_obj_str_get_str(args[ARG_user_description].u_obj);
    }

    bleio_characteristic_obj_t *characteristic = m_new_obj(bleio_characteristic_obj_t);
    characteristic->base.type = &bleio_characteristic_type;

    // Range checking on max_length arg is done by the common_hal layer, because
    // it may vary depending on underlying BLE implementation.
    common_hal_bleio_characteristic_construct(
        characteristic, MP_OBJ_TO_PTR(service_obj), 0, MP_OBJ_TO_PTR(uuid_obj),
        properties, read_perm, write_perm,
        max_length, fixed_length, &initial_value_bufinfo,
        user_description);

    return MP_OBJ_FROM_PTR(characteristic);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_characteristic_add_to_service_fun_obj, 3, bleio_characteristic_add_to_service);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(bleio_characteristic_add_to_service_obj, MP_ROM_PTR(&bleio_characteristic_add_to_service_fun_obj));



//|     properties: int
//|     """An int bitmask representing which properties are set, specified as bitwise or'ing of
//|     of these possible values.
//|     `BROADCAST`, `INDICATE`, `NOTIFY`, `READ`, `WRITE`, `WRITE_NO_RESPONSE`."""
//|
STATIC mp_obj_t bleio_characteristic_get_properties(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_characteristic_get_properties(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_properties_obj, bleio_characteristic_get_properties);

const mp_obj_property_t bleio_characteristic_properties_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_properties_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     uuid: Optional[UUID]
//|     """The UUID of this characteristic. (read-only)
//|
//|     Will be ``None`` if the 128-bit UUID for this characteristic is not known."""
//|
STATIC mp_obj_t bleio_characteristic_get_uuid(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_uuid_obj_t *uuid = common_hal_bleio_characteristic_get_uuid(self);
    return uuid ? MP_OBJ_FROM_PTR(uuid) : mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_uuid_obj, bleio_characteristic_get_uuid);

const mp_obj_property_t bleio_characteristic_uuid_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_uuid_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     value: bytearray
//|     """The value of this characteristic."""
//|
STATIC mp_obj_t bleio_characteristic_get_value(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint8_t temp[512];
    size_t actual_len = common_hal_bleio_characteristic_get_value(self, temp, sizeof(temp));
    return mp_obj_new_bytearray(actual_len, temp);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_value_obj, bleio_characteristic_get_value);

STATIC mp_obj_t bleio_characteristic_set_value(mp_obj_t self_in, mp_obj_t value_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(value_in, &bufinfo, MP_BUFFER_READ);

    common_hal_bleio_characteristic_set_value(self, &bufinfo);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_characteristic_set_value_obj, bleio_characteristic_set_value);

const mp_obj_property_t bleio_characteristic_value_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_value_obj,
               (mp_obj_t)&bleio_characteristic_set_value_obj,
               MP_ROM_NONE },
};

//|     max_length: int
//|     """The max length of this characteristic."""
//|
STATIC mp_obj_t bleio_characteristic_get_max_length(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_characteristic_get_max_length(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_max_length_obj, bleio_characteristic_get_max_length);

const mp_obj_property_t bleio_characteristic_max_length_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_max_length_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     descriptors: Descriptor
//|     """A tuple of :py:class:`Descriptor` objects related to this characteristic. (read-only)"""
//|
STATIC mp_obj_t bleio_characteristic_get_descriptors(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Return list as a tuple so user won't be able to change it.
    return MP_OBJ_FROM_PTR(common_hal_bleio_characteristic_get_descriptors(self));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_descriptors_obj, bleio_characteristic_get_descriptors);

const mp_obj_property_t bleio_characteristic_descriptors_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_descriptors_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     service: Service
//|     """The Service this Characteristic is a part of."""
//|
STATIC mp_obj_t bleio_characteristic_get_service(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_bleio_characteristic_get_service(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_service_obj, bleio_characteristic_get_service);

const mp_obj_property_t bleio_characteristic_service_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_service_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     def set_cccd(self, *, notify: bool = False, indicate: bool = False) -> None:
//|         """Set the remote characteristic's CCCD to enable or disable notification and indication.
//|
//|         :param bool notify: True if Characteristic should receive notifications of remote writes
//|         :param float indicate: True if Characteristic should receive indications of remote writes"""
//|         ...
//|
STATIC mp_obj_t bleio_characteristic_set_cccd(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_notify, ARG_indicate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_notify, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_indicate, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_bleio_characteristic_set_cccd(self, args[ARG_notify].u_bool, args[ARG_indicate].u_bool);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_characteristic_set_cccd_obj, 1, bleio_characteristic_set_cccd);

STATIC const mp_rom_map_elem_t bleio_characteristic_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_add_to_service), MP_ROM_PTR(&bleio_characteristic_add_to_service_obj) },
    { MP_ROM_QSTR(MP_QSTR_properties),     MP_ROM_PTR(&bleio_characteristic_properties_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid),           MP_ROM_PTR(&bleio_characteristic_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),          MP_ROM_PTR(&bleio_characteristic_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_cccd),       MP_ROM_PTR(&bleio_characteristic_set_cccd_obj) },

    // Bitmask constants to represent properties
//|     BROADCAST: int
//|     """property: allowed in advertising packets"""
//|
//|     INDICATE: int
//|     """property: server will indicate to the client when the value is set and wait for a response"""
//|
//|     NOTIFY: int
//|     """property: server will notify the client when the value is set"""
//|
//|     READ: int
//|     """property: clients may read this characteristic"""
//|
//|     WRITE: int
//|     """property: clients may write this characteristic; a response will be sent back"""
//|
//|     WRITE_NO_RESPONSE: int
//|     """property: clients may write this characteristic; no response will be sent back"""
//|
    { MP_ROM_QSTR(MP_QSTR_BROADCAST),         MP_ROM_INT(CHAR_PROP_BROADCAST) },
    { MP_ROM_QSTR(MP_QSTR_INDICATE),          MP_ROM_INT(CHAR_PROP_INDICATE) },
    { MP_ROM_QSTR(MP_QSTR_NOTIFY),            MP_ROM_INT(CHAR_PROP_NOTIFY) },
    { MP_ROM_QSTR(MP_QSTR_READ),              MP_ROM_INT(CHAR_PROP_READ) },
    { MP_ROM_QSTR(MP_QSTR_WRITE),             MP_ROM_INT(CHAR_PROP_WRITE) },
    { MP_ROM_QSTR(MP_QSTR_WRITE_NO_RESPONSE), MP_ROM_INT(CHAR_PROP_WRITE_NO_RESPONSE) },

};
STATIC MP_DEFINE_CONST_DICT(bleio_characteristic_locals_dict, bleio_characteristic_locals_dict_table);

STATIC void bleio_characteristic_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->uuid) {
        mp_printf(print, "Characteristic(");
        bleio_uuid_print(print, MP_OBJ_FROM_PTR(self->uuid), kind);
        mp_printf(print, ")");
    } else {
        mp_printf(print, "<Characteristic with Unregistered UUID>");
    }
}

const mp_obj_type_t bleio_characteristic_type = {
    { &mp_type_type },
    .name = MP_QSTR_Characteristic,
    .print = bleio_characteristic_print,
    .locals_dict = (mp_obj_dict_t *)&bleio_characteristic_locals_dict,
};
