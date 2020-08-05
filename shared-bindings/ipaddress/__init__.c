/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "py/objexcept.h"
#include "py/runtime.h"
#include "shared-bindings/ipaddress/__init__.h"
#include "shared-bindings/ipaddress/IPv4Address.h"

//| """
//| The `ipaddress` module provides types for IP addresses. It is a subset of CPython's ipaddress
//| module.
//| """
//|

//| def ip_address(obj: Union[int]) -> IPv4Address:
//|     """Return a corresponding IP address object or raise ValueError if not possible."""
//|     ...
//|

STATIC mp_obj_t ipaddress_ip_address(mp_obj_t ip_in) {
    mp_int_t value;
    if (!mp_obj_get_int_maybe(ip_in, &value)) {
        mp_raise_ValueError(translate("Only raw int supported for ip."));
    }

    return common_hal_ipaddress_new_ipv4address(value);
}
MP_DEFINE_CONST_FUN_OBJ_1(ipaddress_ip_address_obj, ipaddress_ip_address);

STATIC const mp_rom_map_elem_t ipaddress_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),      MP_ROM_QSTR(MP_QSTR_ipaddress) },
    { MP_ROM_QSTR(MP_QSTR_ip_address),    MP_ROM_PTR(&ipaddress_ip_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_IPv4Address),   MP_ROM_PTR(&ipaddress_ipv4address_type) },
};

STATIC MP_DEFINE_CONST_DICT(ipaddress_module_globals, ipaddress_module_globals_table);


const mp_obj_module_t ipaddress_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&ipaddress_module_globals,
};
