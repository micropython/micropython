/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_HW_USB_HOST

#include "shared/tinyusb/mp_usbh.h"

// The singleton USB host object, on top of the TinyUSB host stack (see
// shared/tinyusb/mp_usbh.c).

typedef struct _mp_obj_usb_host_t {
    mp_obj_base_t base;
} mp_obj_usb_host_t;

const mp_obj_type_t machine_usb_host_type;

static const mp_obj_usb_host_t usb_host_obj = {{&machine_usb_host_type}};

static mp_obj_t usb_host_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return MP_OBJ_FROM_PTR(&usb_host_obj);
}

static mp_obj_t usb_host_active(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            if (!mp_usbh_init()) {
                mp_raise_OSError(MP_EIO);
            }
        } else {
            mp_usbh_deinit();
        }
    }
    return mp_obj_new_bool(mp_usbh_is_active());
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_host_active_obj, 1, 2, usb_host_active);

// Return a list of (address, vid, pid) tuples for the mounted devices.
static mp_obj_t usb_host_devices(mp_obj_t self_in) {
    (void)self_in;
    mp_obj_t list = mp_obj_new_list(0, NULL);
    if (mp_usbh_is_active()) {
        mp_usbh_task();
        for (uint8_t daddr = 1; daddr <= CFG_TUH_DEVICE_MAX + CFG_TUH_HUB; ++daddr) {
            uint16_t vid, pid;
            if (tuh_mounted(daddr) && tuh_vid_pid_get(daddr, &vid, &pid)) {
                mp_obj_t items[3] = {
                    MP_OBJ_NEW_SMALL_INT(daddr),
                    MP_OBJ_NEW_SMALL_INT(vid),
                    MP_OBJ_NEW_SMALL_INT(pid),
                };
                mp_obj_list_append(list, mp_obj_new_tuple(3, items));
            }
        }
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_devices_obj, usb_host_devices);

/******************************************************************************/
// USBHost

static const mp_rom_map_elem_t usb_host_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&usb_host_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_devices), MP_ROM_PTR(&usb_host_devices_obj) },
};
static MP_DEFINE_CONST_DICT(usb_host_locals_dict, usb_host_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_usb_host_type,
    MP_QSTR_USBHost,
    MP_TYPE_FLAG_NONE,
    make_new, usb_host_make_new,
    locals_dict, &usb_host_locals_dict
    );

#endif // MICROPY_HW_USB_HOST
