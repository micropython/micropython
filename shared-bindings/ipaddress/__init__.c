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
#include "py/objstr.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "shared-bindings/ipaddress/__init__.h"
#include "shared-bindings/ipaddress/IPv4Address.h"

//| """
//| The `ipaddress` module provides types for IP addresses. It is a subset of CPython's ipaddress
//| module.
//| """
//|


bool ipaddress_parse_ipv4address(const char *str_data, size_t str_len, uint32_t *ip_out) {
    size_t period_count = 0;
    size_t period_index[4] = {0, 0, 0, str_len};
    for (size_t i = 0; i < str_len; i++) {
        if (str_data[i] == '.') {
            if (period_count < 3) {
                period_index[period_count] = i;
            }
            period_count++;
        }
    }
    if (period_count > 3) {
        return false;
    }

    size_t last_period = 0;
    if (ip_out != NULL) {
        *ip_out = 0;
    }
    for (size_t i = 0; i < 4; i++) {
        // Catch exceptions thrown by mp_parse_num_integer
        nlr_buf_t nlr;
        mp_obj_t octet;
        if (nlr_push(&nlr) == 0) {
            octet = mp_parse_num_integer((const char *)str_data + last_period, period_index[i] - last_period, 10, NULL);
            nlr_pop();
        } else {
            return false;
        }
        last_period = period_index[i] + 1;
        if (ip_out != NULL) {
            mp_int_t int_octet = MP_OBJ_SMALL_INT_VALUE(octet);
            *ip_out |= int_octet << (i * 8);
        }
    }
    return true;
}

//| def ip_address(obj: Union[int, str]) -> IPv4Address:
//|     """Return a corresponding IP address object or raise ValueError if not possible."""
//|     ...
//|

STATIC mp_obj_t ipaddress_ip_address(mp_obj_t ip_in) {
    uint32_t value;
    if (mp_obj_get_int_maybe(ip_in, (mp_int_t *)&value)) {
        // We're done.
    } else if (mp_obj_is_str(ip_in)) {
        GET_STR_DATA_LEN(ip_in, str_data, str_len);
        if (!ipaddress_parse_ipv4address((const char *)str_data, str_len, &value)) {
            mp_raise_ValueError(translate("Not a valid IP string"));
        }
    } else {
        mp_raise_ValueError(translate("Only int or string supported for ip"));
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
    .globals = (mp_obj_dict_t *)&ipaddress_module_globals,
};
