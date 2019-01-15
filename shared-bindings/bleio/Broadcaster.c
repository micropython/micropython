/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "ble_drv.h"
#include "py/runtime.h"

#include "shared-bindings/bleio/Broadcaster.h"

//| .. currentmodule:: bleio
//|
//| :class:`Broadcaster` -- Broadcast advertising packets.
//| =========================================================
//|
//| Implement a BLE broadcaster which sends data in advertising packets and does not connect.
//| Used for beacons and other one-way data transmission.
//|
//| Usage::
//|
//|    import bleio
//|    import time
//|
//|    # Broadcast once a second.
//|    broadcaster = bleio.Broadcaster(interval=1)
//|    data = 0
//|    # Broadcast a byte of data that's incremented once a minute
//|    while True:
//|        # data is an entire advertising data packet, starting with flags.
//|        broadcaster.start_advertising(data)
//|        time.sleep(60)
//|        data += 1
//|
//| .. class:: Broadcaster(interval=1)
//|
//|   Create a new Broadcaster object.

//|   :param float interval: how often to broadcast
//|

STATIC mp_obj_t bleio_broadcaster_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_interval };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_interval, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t interval = mp_obj_get_float(args[ARG_interval].u_obj);

    bleio_broadcaster_obj_t *self = m_new_obj(bleio_broadcaster_obj_t);
    self->base.type = &bleio_broadcaster_type;
    // Do port-specific initialization. interval will be validated.
    common_hal_bleio_broadcaster_construct(self, interval);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: start_advertising(data)
//|
//|     Start advertising using the given data packet.
//|
//|     :param buf data:  advertising data packet, starting with advertising data flags (0x01)
//|
STATIC mp_obj_t bleio_broadcaster_start_advertising(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_broadcaster_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

    common_hal_bleio_broadcaster_start_advertising(self, &bufinfo);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_broadcaster_start_advertising_obj, 0, bleio_broadcaster_start_advertising);

//|   .. method:: stop_advertising()
//|
//|     Stop sending advertising packets.
STATIC mp_obj_t bleio_broadcaster_stop_advertising(mp_obj_t self_in) {
    bleio_broadcaster_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_broadcaster_stop_advertising(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_broadcaster_stop_advertising_obj, bleio_broadcaster_stop_advertising);

STATIC const mp_rom_map_elem_t bleio_broadcaster_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_start_advertising),  MP_ROM_PTR(&bleio_broadcaster_start_advertising_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_advertising),   MP_ROM_PTR(&bleio_broadcaster_stop_advertising_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_broadcaster_locals_dict, bleio_broadcaster_locals_dict_table);

const mp_obj_type_t bleio_broadcaster_type = {
    { &mp_type_type },
    .name = MP_QSTR_Broadcaster,
    .make_new = bleio_broadcaster_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_broadcaster_locals_dict
};
