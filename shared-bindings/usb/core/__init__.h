// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "py/obj.h"

extern const mp_obj_module_t usb_core_module;

void usb_core_exception_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);

#define MP_DEFINE_USB_CORE_EXCEPTION(exc_name, base_name) \
    MP_DEFINE_CONST_OBJ_TYPE(mp_type_usb_core_##exc_name, MP_QSTR_##exc_name, MP_TYPE_FLAG_NONE, \
    make_new, mp_obj_exception_make_new, \
    print, usb_core_exception_print, \
    attr, mp_obj_exception_attr, \
    parent, &mp_type_##base_name \
    );

extern const mp_obj_type_t mp_type_usb_core_USBError;
extern const mp_obj_type_t mp_type_usb_core_USBTimeoutError;

NORETURN void mp_raise_usb_core_USBError(mp_rom_error_text_t fmt, ...);
NORETURN void mp_raise_usb_core_USBTimeoutError(void);

// Find is all Python object oriented so we don't need a separate common-hal API
// for it. It uses the device common-hal instead.
