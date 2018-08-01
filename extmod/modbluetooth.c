/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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
#include "extmod/modbluetooth.h"

#if MICROPY_PY_BLUETOOTH

STATIC const mp_obj_type_t bluetooth_type;

typedef struct _mp_obj_bluetooth_t {
    mp_obj_base_t base;
} mp_obj_bluetooth_t;


// instantiated Bluetooth object
STATIC const mp_obj_bluetooth_t bluetooth_obj = {
    { &bluetooth_type },
};

// Easier (hopefully tail-called) error handling.
STATIC mp_obj_t bluetooth_handle_errno(int errno_) {
    if (errno_ != 0) {
        mp_raise_OSError(errno_);
    }
    return mp_const_none;
}

STATIC mp_obj_t bluetooth_make_new() {
    return MP_OBJ_FROM_PTR(&bluetooth_obj);
}

STATIC mp_obj_t bluetooth_active(size_t n_args, const mp_obj_t *args) {
    if (n_args == 2) { // boolean enable/disable argument supplied
        if (mp_obj_is_true(args[1])) {
            int errno_ = mp_bt_enable();
            if (errno_ != 0) {
                mp_raise_OSError(errno_);
            }
        } else {
            mp_bt_disable();
        }
    }
    return mp_obj_new_bool(mp_bt_is_enabled());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bluetooth_active_obj, 1, 2, bluetooth_active);

STATIC mp_obj_t bluetooth_advertise(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_interval, ARG_name, ARG_connectable };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_interval,    MP_ARG_INT,                  {.u_int = 100} },
        { MP_QSTR_name,        MP_ARG_OBJ,                  {.u_obj = mp_const_none } },
        { MP_QSTR_connectable, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t interval = args[ARG_interval].u_int;
    if (interval == 0) {
        mp_bt_advertise_stop();
        return mp_const_none;
    }
    interval = interval * 8 / 5; // convert from 1ms to 0.625ms units
    if (interval < 0x20 || interval > 0x4000) {
        mp_raise_ValueError("interval out of range");
    }

    mp_bt_adv_type_t adv_type = MP_BT_ADV_TYPE_ADV_IND; // connectable=True
    if (!mp_obj_is_true(args[ARG_connectable].u_obj)) {
        adv_type = MP_BT_ADV_TYPE_ADV_NONCONN_IND; // connectable=False
    }

    size_t name_len;
    const char *name = NULL;
    if (args[ARG_name].u_obj != mp_const_none) {
        name = mp_obj_str_get_data(args[ARG_name].u_obj, &name_len);
    }

    uint8_t adv_data[31];
    size_t adv_data_len = 0;

    if (name != NULL) {
        adv_data[adv_data_len++] = 2; // 1 byte type + 1 byte flags data
        adv_data[adv_data_len++] = MP_BLE_GAP_AD_TYPE_FLAG;
        adv_data[adv_data_len++] = MP_BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

        if (name_len + 3 > sizeof(adv_data) - adv_data_len) {
            mp_raise_ValueError("advertisement packet overflow");
        }
        adv_data[adv_data_len++] = name_len + 1;
        adv_data[adv_data_len++] = MP_BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
        for (size_t i=0; i<name_len; i++) {
            adv_data[adv_data_len++] = name[i];
        }
    }

    int errno_ = mp_bt_advertise_start(adv_type, interval, adv_data_len ? adv_data : NULL, adv_data_len, NULL, 0);
    return bluetooth_handle_errno(errno_);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bluetooth_advertise_obj, 1, bluetooth_advertise);

STATIC mp_obj_t bluetooth_advertise_raw(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_interval, ARG_adv_data, ARG_sr_data, ARG_connectable };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_interval,    MP_ARG_INT,                  {.u_int = 100} },
        { MP_QSTR_adv_data,    MP_ARG_OBJ,                  {.u_obj = mp_const_none } },
        { MP_QSTR_sr_data,     MP_ARG_OBJ,                  {.u_obj = mp_const_none } },
        { MP_QSTR_connectable, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t interval = args[ARG_interval].u_int;
    if (interval == 0) {
        mp_bt_advertise_stop();
        return mp_const_none;
    }
    interval = interval * 8 / 5; // convert from 1ms to 0.625ms units
    if (interval < 0x20 || interval > 0x4000) {
        mp_raise_ValueError("interval out of range");
    }

    mp_bt_adv_type_t adv_type = MP_BT_ADV_TYPE_ADV_IND; // connectable=True
    if (!mp_obj_is_true(args[ARG_connectable].u_obj)) {
        adv_type = MP_BT_ADV_TYPE_ADV_NONCONN_IND; // connectable=False
    }

    size_t adv_data_len;
    const uint8_t *adv_data = NULL;
    if (args[ARG_adv_data].u_obj != mp_const_none) {
        adv_data = (const uint8_t*)mp_obj_str_get_data(args[ARG_adv_data].u_obj, &adv_data_len);
    }

    size_t sr_data_len;
    const uint8_t *sr_data = NULL;
    if (args[ARG_sr_data].u_obj != mp_const_none) {
        sr_data = (const uint8_t*)mp_obj_str_get_data(args[ARG_sr_data].u_obj, &sr_data_len);
    }

    int errno_ = mp_bt_advertise_start(adv_type, interval, adv_data_len ? adv_data : NULL, adv_data_len, sr_data_len ? sr_data : NULL, sr_data_len);
    return bluetooth_handle_errno(errno_);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bluetooth_advertise_raw_obj, 1, bluetooth_advertise_raw);

STATIC const mp_rom_map_elem_t bluetooth_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&bluetooth_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_advertise), MP_ROM_PTR(&bluetooth_advertise_obj) },
    { MP_ROM_QSTR(MP_QSTR_advertise_raw), MP_ROM_PTR(&bluetooth_advertise_raw_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bluetooth_locals_dict, bluetooth_locals_dict_table);

STATIC const mp_obj_type_t bluetooth_type = {
    { &mp_type_type },
    .name = MP_QSTR_Bluetooth,
    .make_new = bluetooth_make_new,
    .locals_dict = (void*)&bluetooth_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_bluetooth_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bluetooth) },
    { MP_ROM_QSTR(MP_QSTR_Bluetooth), MP_ROM_PTR(&bluetooth_type) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_bluetooth_globals, mp_module_bluetooth_globals_table);

const mp_obj_module_t mp_module_bluetooth = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_bluetooth_globals,
};

#endif //MICROPY_PY_BLUETOOTH
