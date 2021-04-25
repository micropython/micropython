/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014 Damien P. George
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/objlist.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "shared-module/network/__init__.h"

//| """Support for WizNet hardware, including the WizNet 5500 Ethernet adaptor.
//|
//|
//| .. warning:: This module is disabled in 6.x and will removed in 7.x. Please use networking
//|              libraries instead.
//| """
//|

extern const mod_network_nic_type_t mod_network_nic_type_wiznet5k;

STATIC const mp_rom_map_elem_t mp_module_wiznet_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_wiznet) },
    #ifdef MICROPY_PY_WIZNET5K
    { MP_ROM_QSTR(MP_QSTR_WIZNET5K), MP_ROM_PTR(&mod_network_nic_type_wiznet5k) },
    #endif // MICROPY_PY_WIZNET5K
};
STATIC MP_DEFINE_CONST_DICT(mp_module_wiznet_globals, mp_module_wiznet_globals_table);

const mp_obj_module_t wiznet_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_wiznet_globals,
};
