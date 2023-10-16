/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#include <stdarg.h>
#include <string.h>

#include "py/obj.h"
#include "py/objexcept.h"
#include "py/misc.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/usb/core/__init__.h"
#include "shared-bindings/usb/core/Device.h"

//| """USB Core
//|
//| This is a subset of the PyUSB core module.
//| """
//|

//| class USBError(OSError):
//|     """Catchall exception for USB related errors."""
//|
//|     ...
//|
MP_DEFINE_USB_CORE_EXCEPTION(USBError, OSError)
NORETURN void mp_raise_usb_core_USBError(const compressed_string_t *fmt, ...) {
    mp_obj_t exception;
    if (fmt == NULL) {
        exception = mp_obj_new_exception(&mp_type_usb_core_USBError);
    } else {
        va_list argptr;
        va_start(argptr, fmt);
        exception = mp_obj_new_exception_msg_vlist(&mp_type_usb_core_USBError, fmt, argptr);
        va_end(argptr);
    }
    nlr_raise(exception);
}

//| class USBTimeoutError(USBError):
//|     """Raised when a USB transfer times out."""
//|
//|     ...
//|
MP_DEFINE_USB_CORE_EXCEPTION(USBTimeoutError, usb_core_USBError)
NORETURN void mp_raise_usb_core_USBTimeoutError(void) {
    mp_raise_type(&mp_type_usb_core_USBTimeoutError);
}


//| def find(
//|     find_all: bool = False, *, idVendor: Optional[int] = None, idProduct: Optional[int] = None
//| ) -> Device:
//|     """Find the first device that matches the given requirements or, if
//|     find_all is True, return a generator of all matching devices.
//|
//|     Returns None if no device matches.
//|     """
//|
typedef struct {
    mp_obj_base_t base;
    mp_int_t vid;
    mp_int_t pid;
    mp_int_t next_index;
} usb_core_devices_obj_t;

// This is an internal iterator type to use with find.
STATIC mp_obj_t _next_device(usb_core_devices_obj_t *iter) {
    // Brute force check all possible device numbers for one that matches.
    usb_core_device_obj_t temp_device;
    for (size_t i = iter->next_index; i < 256; i++) {
        if (!common_hal_usb_core_device_construct(&temp_device, i)) {
            continue;
        }
        if (iter->vid < 0x10000 && iter->vid != common_hal_usb_core_device_get_idVendor(&temp_device)) {
            continue;
        }
        if (iter->pid < 0x10000 && iter->pid != common_hal_usb_core_device_get_idProduct(&temp_device)) {
            continue;
        }

        // We passed the filters. Now make a properly allocated object to
        // return to the user.
        usb_core_device_obj_t *self = mp_obj_malloc(usb_core_device_obj_t, &usb_core_device_type);

        common_hal_usb_core_device_construct(self, i);
        iter->next_index = i + 1;
        return MP_OBJ_FROM_PTR(self);
    }
    // Iter is done.
    iter->next_index = 256;
    return mp_const_none;
}

STATIC mp_obj_t usb_core_devices_iternext(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &usb_core_devices_type));
    usb_core_devices_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t device = _next_device(self);
    if (device != mp_const_none) {
        return device;
    }
    return MP_OBJ_STOP_ITERATION;
}

MP_DEFINE_CONST_OBJ_TYPE(
    usb_core_devices_type,
    MP_QSTR_USBDevices,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, usb_core_devices_iternext
    );

STATIC mp_obj_t usb_core_find(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_find_all, ARG_idVendor, ARG_idProduct };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_find_all, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_idVendor, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_idProduct, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const bool find_all = args[ARG_find_all].u_bool;
    usb_core_devices_obj_t temp_iter;
    temp_iter.base.type = &usb_core_devices_type;
    temp_iter.next_index = 1;
    if (!mp_obj_get_int_maybe(args[ARG_idVendor].u_obj, &temp_iter.vid)) {
        temp_iter.vid = 0x10000;
    }
    if (!mp_obj_get_int_maybe(args[ARG_idProduct].u_obj, &temp_iter.pid)) {
        temp_iter.pid = 0x10000;
    }
    if (find_all) {
        // Copy the temp iter contents to a heap object before we return it.
        // We could do this up front but GCC falsely detects that we may return
        // the stack copy.
        usb_core_devices_obj_t *iter = m_new_obj(usb_core_devices_obj_t);
        memcpy(iter, &temp_iter, sizeof(usb_core_devices_obj_t));
        return MP_OBJ_FROM_PTR(iter);
    }
    return _next_device(&temp_iter);
}
MP_DEFINE_CONST_FUN_OBJ_KW(usb_core_find_obj, 0, usb_core_find);


STATIC mp_rom_map_elem_t usb_core_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_usb_dot_core) },
    // Functions
    { MP_ROM_QSTR(MP_QSTR_find),            MP_OBJ_FROM_PTR(&usb_core_find_obj) },

    // Classes
    { MP_ROM_QSTR(MP_QSTR_Device),          MP_OBJ_FROM_PTR(&usb_core_device_type) },

    // Errors
    { MP_ROM_QSTR(MP_QSTR_USBError),        MP_OBJ_FROM_PTR(&mp_type_usb_core_USBError) },
    { MP_ROM_QSTR(MP_QSTR_USBTimeoutError), MP_OBJ_FROM_PTR(&mp_type_usb_core_USBTimeoutError) },
};

STATIC MP_DEFINE_CONST_DICT(usb_core_module_globals, usb_core_module_globals_table);

void usb_core_exception_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_print_kind_t k = kind & ~PRINT_EXC_SUBCLASS;
    bool is_subclass = kind & PRINT_EXC_SUBCLASS;
    if (!is_subclass && (k == PRINT_EXC)) {
        mp_print_str(print, qstr_str(MP_OBJ_QSTR_VALUE(usb_core_module_globals_table[0].value)));
        mp_print_str(print, ".");
    }
    mp_obj_exception_print(print, o_in, kind);
}

const mp_obj_module_t usb_core_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_core_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_dot_core, usb_core_module);
