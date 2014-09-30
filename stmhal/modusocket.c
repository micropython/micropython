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

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "objtuple.h"
#include "objlist.h"
#include "runtime.h"
#include "modnetwork.h"

/// \module usocket

/// \method socket(family=AF_INET, type=SOCK_STREAM, fileno=-1)
/// Create a socket.
STATIC mp_obj_t mod_usocket_socket(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_family, MP_ARG_INT,                  {.u_int = MOD_NETWORK_AF_INET} },
        { MP_QSTR_type,   MP_ARG_INT,                  {.u_int = MOD_NETWORK_SOCK_STREAM} },
        { MP_QSTR_fileno, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // find a NIC that can create a socket and call it
    for (mp_uint_t i = 0; i < mod_network_nic_list.len; i++) {
        mp_obj_t nic = mod_network_nic_list.items[i];
        mod_network_nic_type_t *nic_type = (mod_network_nic_type_t*)mp_obj_get_type(nic);
        if (nic_type->socket != NULL) {
            int _errno;
            mp_obj_t obj = nic_type->socket(nic, args[0].u_int, args[1].u_int, args[2].u_int, &_errno);
            if (obj == MP_OBJ_NULL) {
                nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
            } else {
                return obj;
            }
        }
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "no available NIC"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mod_usocket_socket_obj, 0, mod_usocket_socket);

/// \method getaddrinfo(host, port)
STATIC mp_obj_t mod_usocket_getaddrinfo(mp_obj_t host_in, mp_obj_t port_in) {
    mp_uint_t hlen;
    const char *host = mp_obj_str_get_data(host_in, &hlen);
    mp_int_t port = mp_obj_get_int(port_in);

    // find a NIC that can do a name lookup
    for (mp_uint_t i = 0; i < mod_network_nic_list.len; i++) {
        mp_obj_t nic = mod_network_nic_list.items[i];
        mod_network_nic_type_t *nic_type = (mod_network_nic_type_t*)mp_obj_get_type(nic);
        if (nic_type->gethostbyname != NULL) {
            uint8_t out_ip[MOD_NETWORK_IPADDR_BUF_SIZE];
            int ret = nic_type->gethostbyname(nic, host, hlen, out_ip);
            if (ret != 0) {
                // TODO CPython raises: socket.gaierror: [Errno -2] Name or service not known
                nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ret)));
            }
            mp_obj_tuple_t *tuple = mp_obj_new_tuple(5, NULL);
            tuple->items[0] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET);
            tuple->items[1] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM);
            tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
            tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
            tuple->items[4] = mod_network_format_inet_addr(out_ip, port);
            return mp_obj_new_list(1, (mp_obj_t*)&tuple);
        }
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "no available NIC"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_usocket_getaddrinfo_obj, mod_usocket_getaddrinfo);

STATIC const mp_map_elem_t mp_module_usocket_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_usocket) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&mod_usocket_socket_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getaddrinfo), (mp_obj_t)&mod_usocket_getaddrinfo_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET6), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET6) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_STREAM), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_DGRAM), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_DGRAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_RAW), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_RAW) },

    /*
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_IP), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_IPPROTO_IP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_ICMP), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_IPPROTO_ICMP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_IPV4), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_IPPROTO_IPV4) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_TCP), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_IPPROTO_TCP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_UDP), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_IPPROTO_UDP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_IPV6), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_IPPROTO_IPV6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_RAW), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_IPPROTO_RAW) },
    */
};

STATIC const mp_obj_dict_t mp_module_usocket_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(mp_module_usocket_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_usocket_globals_table),
        .table = (mp_map_elem_t*)mp_module_usocket_globals_table,
    },
};

const mp_obj_module_t mp_module_usocket = {
    .base = { &mp_type_module },
    .name = MP_QSTR_usocket,
    .globals = (mp_obj_dict_t*)&mp_module_usocket_globals,
};
