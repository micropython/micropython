/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#include <errno.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "modnetwork.h"

/// \module network - network configuration
///
/// This module provides network drivers and routing configuration.

void mod_network_init(void) {
    mp_obj_list_init(&MP_STATE_PORT(mod_network_nic_list), 0);
}

void mod_network_register_nic(mp_obj_t nic) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        if (MP_STATE_PORT(mod_network_nic_list).items[i] == nic) {
            // nic already registered
            return;
        }
    }
    // nic not registered so add to list
    mp_obj_list_append(&MP_STATE_PORT(mod_network_nic_list), nic);
}

mp_obj_t mod_network_find_nic(const uint8_t *ip) {
    // find a NIC that is suited to given IP address
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        // TODO check IP suitability here
        //mod_network_nic_type_t *nic_type = (mod_network_nic_type_t*)mp_obj_get_type(nic);
        return nic;
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "no available NIC"));
}

STATIC mp_obj_t network_route(void) {
    return &MP_STATE_PORT(mod_network_nic_list);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(network_route_obj, network_route);

STATIC const mp_map_elem_t mp_module_network_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_network) },

    #if MICROPY_PY_WIZNET5K
    { MP_OBJ_NEW_QSTR(MP_QSTR_WIZNET5K), (mp_obj_t)&mod_network_nic_type_wiznet5k },
    #endif
    #if MICROPY_PY_CC3K
    { MP_OBJ_NEW_QSTR(MP_QSTR_CC3K), (mp_obj_t)&mod_network_nic_type_cc3k },
    #endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_route), (mp_obj_t)&network_route_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .name = MP_QSTR_network,
    .globals = (mp_obj_dict_t*)&mp_module_network_globals,
};

/******************************************************************************/
// Miscellaneous helpers

void mod_network_convert_ipv4_endianness(uint8_t *ip) {
    uint8_t ip0 = ip[0]; ip[0] = ip[3]; ip[3] = ip0;
    uint8_t ip1 = ip[1]; ip[1] = ip[2]; ip[2] = ip1;
}

// Takes an address of the form '192.168.0.1' and converts it to network format
// in out_ip (big endian, so the 192 is the first byte).
void mod_network_parse_ipv4_addr(mp_obj_t addr_in, uint8_t *out_ip) {
    mp_uint_t addr_len;
    const char *addr_str = mp_obj_str_get_data(addr_in, &addr_len);
    if (addr_len == 0) {
        // special case of no address given
        memset(out_ip, 0, MOD_NETWORK_IPADDR_BUF_SIZE);
        return;
    }
    const char *s = addr_str;
    const char *s_top = addr_str + addr_len;
    for (mp_uint_t i = 0;; i++) {
        mp_uint_t val = 0;
        for (; s < s_top && *s != '.'; s++) {
            val = val * 10 + *s - '0';
        }
        out_ip[i] = val;
        if (i == 3 && s == s_top) {
            return;
        } else if (i < 3 && s < s_top && *s == '.') {
            s++;
        } else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid IP address"));
        }
    }
}

// Takes an address of the form ('192.168.0.1', 8080), returns the port and
// puts IP in out_ip (which must take at least IPADDR_BUF_SIZE bytes).
mp_uint_t mod_network_parse_inet_addr(mp_obj_t addr_in, uint8_t *out_ip) {
    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
    mod_network_parse_ipv4_addr(addr_items[0], out_ip);
    return mp_obj_get_int(addr_items[1]);
}

// Takes an array with a raw IPv4 address and returns something like '192.168.0.1'.
mp_obj_t mod_network_format_ipv4_addr(uint8_t *ip) {
    char ip_str[16];
    mp_uint_t ip_len = snprintf(ip_str, 16, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    return mp_obj_new_str(ip_str, ip_len, false);
}

// Takes an array with a raw IP address, and a port, and returns a net-address
// tuple such as ('192.168.0.1', 8080).
mp_obj_t mod_network_format_inet_addr(uint8_t *ip, mp_uint_t port) {
    mp_obj_t tuple[2] = {
        tuple[0] = mod_network_format_ipv4_addr(ip),
        tuple[1] = mp_obj_new_int(port),
    };
    return mp_obj_new_tuple(2, tuple);
}
