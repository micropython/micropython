/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "py/mperrno.h"
#include "py/runtime.h"
#include "modmimxrt.h"

#if MICROPY_HW_USB_MSC

#define USB_MODE_VCP        "vcp"
#define USB_MODE_VCP_MSC    "vcp+msc"

extern void set_msc_enabled(bool state);

STATIC mp_obj_t mimxrt_usb_mode(mp_obj_t mode_in) {
    size_t slen;
    const char *s = mp_obj_str_get_data(mode_in, &slen);
    if (strncmp(s, USB_MODE_VCP, slen) == 0) {
        set_msc_enabled(false);
    } else if (strncmp(s, USB_MODE_VCP_MSC, slen) == 0) {
        set_msc_enabled(true);
    } else {
        mp_raise_ValueError("Invalid usb_mode");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mimxrt_usb_mode_obj, mimxrt_usb_mode);
#endif

STATIC const mp_rom_map_elem_t mimxrt_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_mimxrt) },
    { MP_ROM_QSTR(MP_QSTR_Flash),               MP_ROM_PTR(&mimxrt_flash_type) },
    #if MICROPY_HW_USB_MSC
    { MP_ROM_QSTR(MP_QSTR_usb_mode),            MP_ROM_PTR(&mimxrt_usb_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_MSC),                 MP_ROM_INT(1) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(mimxrt_module_globals, mimxrt_module_globals_table);

const mp_obj_module_t mp_module_mimxrt = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mimxrt_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mimxrt, mp_module_mimxrt);
