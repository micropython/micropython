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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/ScanEntry.h"
#include "shared-bindings/bleio/Scanner.h"

#define DEFAULT_INTERVAL 100
#define DEFAULT_WINDOW 100

// Work-in-progress: orphaned for now.
//| :orphan:
//|
//| .. currentmodule:: bleio
//|
//| :class:`Scanner` -- scan for nearby BLE devices
//| =========================================================
//|
//| Allows scanning for nearby BLE devices.
//|
//| Usage::
//|
//|    import bleio
//|    scanner = bleio.Scanner()
//|    entries = scanner.scan(2500)
//|    print(entries)
//|

//| .. class:: Scanner()
//|
//|   Create a new Scanner object.
//|

//|   .. attribute:: interval
//|
//|     The interval (in ms) between the start of two consecutive scan windows.
//|     Allowed values are between 10ms and 10.24 sec.
//|

//|   .. attribute:: window
//|
//|     The duration (in ms) in which a single BLE channel is scanned.
//|     Allowed values are between 10ms and 10.24 sec.
//|

//|   .. method:: scan(timeout)
//|
//|     Performs a BLE scan.
//|
//|     :param int timeout: the scan timeout in ms
//|     :returns: advertising packets found
//|     :rtype: list of :py:class:`bleio.ScanEntry`
//|
STATIC void bleio_scanner_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_scanner_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "Scanner(interval: %d window: %d)", self->interval, self->window);
}

STATIC mp_obj_t bleio_scanner_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *all_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 0, 0, false);

    bleio_scanner_obj_t *self = m_new_obj(bleio_scanner_obj_t);
    self->base.type = type;

    self->interval = DEFAULT_INTERVAL;
    self->window = DEFAULT_WINDOW;

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t bleio_scanner_get_interval(mp_obj_t self_in) {
    bleio_scanner_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_int(self->interval);
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_scanner_get_interval_obj, bleio_scanner_get_interval);

static mp_obj_t bleio_scanner_set_interval(mp_obj_t self_in, mp_obj_t value) {
    bleio_scanner_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->interval = mp_obj_get_int(value);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_scanner_set_interval_obj, bleio_scanner_set_interval);

const mp_obj_property_t bleio_scanner_interval_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_scanner_get_interval_obj,
               (mp_obj_t)&bleio_scanner_set_interval_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t scanner_scan(mp_obj_t self_in, mp_obj_t timeout_in) {
    bleio_scanner_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const mp_int_t timeout = mp_obj_get_int(timeout_in);

    self->adv_reports = mp_obj_new_list(0, NULL);

    common_hal_bleio_scanner_scan(self, timeout);

    return self->adv_reports;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_scanner_scan_obj, scanner_scan);

STATIC mp_obj_t bleio_scanner_get_window(mp_obj_t self_in) {
    bleio_scanner_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_int(self->window);
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_scanner_get_window_obj, bleio_scanner_get_window);

static mp_obj_t bleio_scanner_set_window(mp_obj_t self_in, mp_obj_t value) {
    bleio_scanner_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->window = mp_obj_get_int(value);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_scanner_set_window_obj, bleio_scanner_set_window);

const mp_obj_property_t bleio_scanner_window_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_scanner_get_window_obj,
               (mp_obj_t)&bleio_scanner_set_window_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC const mp_rom_map_elem_t bleio_scanner_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_interval), MP_ROM_PTR(&bleio_scanner_interval_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&bleio_scanner_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_window), MP_ROM_PTR(&bleio_scanner_window_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_scanner_locals_dict, bleio_scanner_locals_dict_table);

const mp_obj_type_t bleio_scanner_type = {
    { &mp_type_type },
    .name = MP_QSTR_Scanner,
    .print = bleio_scanner_print,
    .make_new = bleio_scanner_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_scanner_locals_dict
};
