/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/mphal.h"
#include "py/runtime.h"
#include "modalif.h"
#include "se_services.h"

static mp_obj_t alif_info(void) {
    se_services_dump_device_data();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(alif_info_obj, alif_info);

static const mp_rom_map_elem_t alif_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_alif) },
    #if MICROPY_HW_ENABLE_OSPI
    { MP_ROM_QSTR(MP_QSTR_Flash),               MP_ROM_PTR(&alif_flash_type) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_info),                MP_ROM_PTR(&alif_info_obj) },
    #if MICROPY_HW_USB_MSC
    // Attribute to indicate USB MSC is enabled.
    { MP_ROM_QSTR(MP_QSTR_usb_msc),             MP_ROM_TRUE },
    #endif
};
static MP_DEFINE_CONST_DICT(alif_module_globals, alif_module_globals_table);

const mp_obj_module_t mp_module_alif = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&alif_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_alif, mp_module_alif);
