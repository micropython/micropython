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

#include <string.h>
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/objlist.h"

#if MICROPY_PY_UBLUEPY_CENTRAL

#include "ble_drv.h"

static void adv_event_handler(mp_obj_t self_in, uint16_t event_id, ble_drv_adv_data_t * data) {
    ubluepy_scanner_obj_t *self = MP_OBJ_TO_PTR(self_in);

    ubluepy_scan_entry_obj_t * item = mp_obj_malloc(ubluepy_scan_entry_obj_t, &ubluepy_scan_entry_type);

    vstr_t vstr;
    vstr_init(&vstr, 17);

    vstr_printf(&vstr, ""HEX2_FMT":"HEX2_FMT":"HEX2_FMT":" \
                         HEX2_FMT":"HEX2_FMT":"HEX2_FMT"",
                data->p_peer_addr[5], data->p_peer_addr[4], data->p_peer_addr[3],
                data->p_peer_addr[2], data->p_peer_addr[1], data->p_peer_addr[0]);

    item->addr = mp_obj_new_str(vstr.buf, vstr.len);

    vstr_clear(&vstr);

    item->addr_type = data->addr_type;
    item->rssi      = data->rssi;
    item->data      = mp_obj_new_bytearray(data->data_len, data->p_data);

    mp_obj_list_append(self->adv_reports, item);

    // Continue scanning
    ble_drv_scan_start(true);
}

static void ubluepy_scanner_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_scanner_obj_t * self = (ubluepy_scanner_obj_t *)o;
    (void)self;
    mp_printf(print, "Scanner");
}

static mp_obj_t ubluepy_scanner_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {

    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_scanner_obj_t * s = mp_obj_malloc(ubluepy_scanner_obj_t, type);

    return MP_OBJ_FROM_PTR(s);
}

/// \method scan(timeout)
/// Scan for devices. Timeout is in milliseconds and will set the duration
/// of the scanning.
///
static mp_obj_t scanner_scan(mp_obj_t self_in, mp_obj_t timeout_in) {
    ubluepy_scanner_obj_t * self = MP_OBJ_TO_PTR(self_in);
    mp_int_t timeout = mp_obj_get_int(timeout_in);

    self->adv_reports = mp_obj_new_list(0, NULL);

    ble_drv_adv_report_handler_set(MP_OBJ_FROM_PTR(self), adv_event_handler);

    // start
    ble_drv_scan_start(false);

    // sleep
    mp_hal_delay_ms(timeout);

    // stop
    ble_drv_scan_stop();

    return self->adv_reports;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_scanner_scan_obj, scanner_scan);

static const mp_rom_map_elem_t ubluepy_scanner_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&ubluepy_scanner_scan_obj) },
};

static MP_DEFINE_CONST_DICT(ubluepy_scanner_locals_dict, ubluepy_scanner_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    ubluepy_scanner_type,
    MP_QSTR_Scanner,
    MP_TYPE_FLAG_NONE,
    make_new, ubluepy_scanner_make_new,
    print, ubluepy_scanner_print,
    locals_dict, &ubluepy_scanner_locals_dict
    );

#endif // MICROPY_PY_UBLUEPY_CENTRAL
