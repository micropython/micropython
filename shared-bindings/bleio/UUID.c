/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/UUID.h"

// Including hyphens.
#define UUID128_STR_LEN 36
// Number of bytes
#define UUID128_BYTE_LEN 32

STATIC uint8_t xdigit_8b_value(byte nibble1, byte nibble2) {
    return unichar_xdigit_value(nibble1) | (unichar_xdigit_value(nibble2) << 4);
}


//| .. currentmodule:: bleio
//|
//| :class:`UUID16` -- BLE UUID16
//| =========================================================
//|
//| A 16-bit or 128-bit UUID. Can be used for services, characteristics, descriptors and more.
//|

//| .. class:: UUID(uuid)
//|
//|   Create a new UUID or UUID object encapsulating the uuid value.
//|   The value can be one of:
//|
//|   - an `int` value in range 0 to 0xFFFF (Bluetooth SIG 16-bit UUID)
//|   - a `str` value in the format 'xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx', where the X's are hex digits.
//|     (128 bit UUID)
//|
//|   :param int/str uuid: The uuid to encapsulate
//|

STATIC mp_obj_t bleio_uuid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);

    bleio_uuid_obj_t *self = m_new_obj(bleio_uuid_obj_t);
    self->base.type = type;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);

    enum { ARG_uuid };
    static const mp_arg_t allowed_args[] = {
        { ARG_uuid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t uuid = args[ARG_uuid].u_obj;

    if (MP_OBJ_IS_INT(uuid)) {
        mp_int_t uuid16 = mp_obj_get_int(uuid);
        if (uuid16 < 0 || uuid16 > 0xffff) {
            mp_raise_ValueError(translate("Integer UUID not in range 0 to 0xffff"));
        }

        // This is a 16-bit Bluetooth SIG UUID. NULL means no 128-bit value.
        common_hal_bleio_uuid_construct(self, uuid16, NULL);

    } else if (MP_OBJ_IS_STR(uuid)) {
        uint8_t uuid128[UUID128_BYTE_LEN];
        GET_STR_DATA_LEN(uuid, str, str_len);
        if (str_len == UUID128_STR_LEN &&
            str[8] == '-' && str[13] == '-' && str[18] == '-' && str[23] == '-') {
            size_t str_index = UUID128_STR_LEN - 1;
            size_t uuid128_index = 0;
            bool error = false;

            // Loop until fewer than two characters left.
            while (str_index >= 1 && uuid128_index < UUID128_BYTE_LEN) {
                if (str[str_index] == '-') {
                    // Skip hyphen separators.
                    str_index--;
                    continue;
                }

                if (!unichar_isxdigit(str[str_index]) ||
                    !unichar_isxdigit(str[str_index-1])) {
                    error = true;
                    break;
                }

                uuid128[uuid128_index] = xdigit_8b_value(str[str_index],
                                                         str[str_index-1]);
                uuid128_index += 1;
                str_index -= 2;
            }
            // Check for correct number of hex digits and no parsing errors.
            if (!error && uuid128_index == UUID128_BYTE_LEN && str_index == -1) {
                uint32_t uuid16 = (uuid128[13] << 8) | uuid128[12];
                uuid128[12] = 0;
                uuid128[13] = 0;
                common_hal_bleio_uuid_construct(self, uuid16, uuid128);
                return MP_OBJ_FROM_PTR(self);
            }
        }
        mp_raise_ValueError(translate("UUID string must be of the form xxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"));
    } else {
        mp_raise_ValueError(translate("UUID value is not int or string"));
    }
    // Unreachable.
    return mp_const_none;
}

STATIC void bleio_uuid_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_uuid_print(self, print);
}

//|   .. attribute:: uuid16
//|
//|     The 16-bit part of the UUID. (read-only)
//|
STATIC mp_obj_t bleio_uuid_get_uuid16(mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_uuid_get_uuid16(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(bleio_uuid_get_uuid16_obj, bleio_uuid_get_uuid16);

const mp_obj_property_t bleio_uuid_uuid16_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_uuid_get_uuid16_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: uuid128_handle
//|
//|     An opaque handle representing the 128-bit UUID. (read-only)
//|     Returns None if this is a 16-bit UUID.
//|
STATIC mp_obj_t bleio_uuid_get_uuid128_handle(mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t handle = common_hal_bleio_uuid_get_uuid128_handle(self);
    return handle == 0 ? mp_const_none : MP_OBJ_NEW_SMALL_INT(handle);
}

MP_DEFINE_CONST_FUN_OBJ_1(bleio_uuid_get_uuid128_handle_obj, bleio_uuid_get_uuid128_handle);

const mp_obj_property_t bleio_uuid_uuid128_handle_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_uuid_get_uuid128_handle_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: size
//|
//|     Returns 128 if this UUID represents a 128-bit vendor-specific UUID.
//|     Returns 16 if this UUID represents a 16-bit Bluetooth SIG assigned UUID. (read-only)
//|     32-bit UUIDs are not currently supported.
//|
STATIC mp_obj_t bleio_uuid_get_size(mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_uuid_get_size(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(bleio_uuid_get_size_obj, bleio_uuid_get_size);

const mp_obj_property_t bleio_uuid_size_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_uuid_get_size_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t bleio_uuid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_uuid16), MP_ROM_PTR(&bleio_uuid_uuid16_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid128_handle), MP_ROM_PTR(&bleio_uuid_uuid128_handle_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&bleio_uuid_size_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_uuid_locals_dict, bleio_uuid_locals_dict_table);

const mp_obj_type_t bleio_uuid_type = {
    { &mp_type_type },
    .name = MP_QSTR_UUID,
    .print = bleio_uuid_print,
    .make_new = bleio_uuid_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_uuid_locals_dict,
};
