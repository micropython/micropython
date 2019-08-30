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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/ScanEntry.h"
#include "shared-bindings/_bleio/Scanner.h"

#define INTERVAL_DEFAULT (0.1f)
#define INTERVAL_MIN (0.0025f)
#define INTERVAL_MIN_STRING "0.0025"
#define INTERVAL_MAX (40.959375f)
#define INTERVAL_MAX_STRING "40.959375"
#define WINDOW_DEFAULT (0.1f)

//| .. currentmodule:: _bleio
//|
//| :class:`Scanner` -- scan for nearby BLE devices
//| =========================================================
//|
//| Scan for nearby BLE devices.
//|
//| Usage::
//|
//|    import _bleio
//|    scanner = _bleio.Scanner()
//|    entries = scanner.scan(2.5)  # Scan for 2.5 seconds
//|

//| .. class:: Scanner()
//|
//|   Create a new Scanner object.
//|
STATIC mp_obj_t bleio_scanner_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *all_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 0, 0, false);

    bleio_scanner_obj_t *self = m_new_obj(bleio_scanner_obj_t);
    self->base.type = type;

    common_hal_bleio_scanner_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: scan(timeout, \*, interval=0.1, window=0.1)
//|
//|     Performs a BLE scan.
//|
//|     :param float timeout: the scan timeout in seconds
//|     :param float interval: the interval (in seconds) between the start of two consecutive scan windows
//|        Must be in the range 0.0025 - 40.959375 seconds.
//|     :param float window: the duration (in seconds) to scan a single BLE channel.
//|        window must be <= interval.
//|     :returns: an iterable of `ScanEntry` objects
//|     :rtype: iterable
//|
STATIC mp_obj_t bleio_scanner_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_timeout, ARG_interval, ARG_window };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_window,   MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    bleio_scanner_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_float_t timeout = mp_obj_get_float(args[ARG_timeout].u_obj);

    if (args[ARG_interval].u_obj == MP_OBJ_NULL) {
        args[ARG_interval].u_obj = mp_obj_new_float(INTERVAL_DEFAULT);
    }

    if (args[ARG_window].u_obj == MP_OBJ_NULL) {
        args[ARG_window].u_obj = mp_obj_new_float(WINDOW_DEFAULT);
    }

    const mp_float_t interval = mp_obj_float_get(args[ARG_interval].u_obj);
    if (interval < INTERVAL_MIN || interval > INTERVAL_MAX) {
        mp_raise_ValueError_varg(translate("interval must be in range %s-%s"), INTERVAL_MIN_STRING, INTERVAL_MAX_STRING);
    }

    const mp_float_t window = mp_obj_float_get(args[ARG_window].u_obj);
    if (window > interval) {
        mp_raise_ValueError(translate("window must be <= interval"));
    }

    return common_hal_bleio_scanner_scan(self, timeout, interval, window);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_scanner_scan_obj, 2, bleio_scanner_scan);


STATIC const mp_rom_map_elem_t bleio_scanner_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&bleio_scanner_scan_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_scanner_locals_dict, bleio_scanner_locals_dict_table);

const mp_obj_type_t bleio_scanner_type = {
    { &mp_type_type },
    .name = MP_QSTR_Scanner,
    .make_new = bleio_scanner_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_scanner_locals_dict
};
