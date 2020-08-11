/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014 Damien P. George
 *               2018 Nick Moore for Adafruit Industries
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
#include <string.h>

#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/socketpool/SocketPool.h"

//| class SocketPool:
//|

STATIC mp_obj_t socketpool_socketpool_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 0, 4, false);

    socketpool_socketpool_obj_t *s = m_new_obj_with_finaliser(socketpool_socketpool_obj_t);
    s->base.type = &socketpool_socketpool_type;

    return MP_OBJ_FROM_PTR(s);
}


//|     def socket(self, family: int, type: int, proto: int) -> None:
//|         """Create a new socket
//|
//|         :param ~int family: AF_INET or AF_INET6
//|         :param ~int type: SOCK_STREAM, SOCK_DGRAM or SOCK_RAW
//|         :param ~int proto: IPPROTO_TCP, IPPROTO_UDP or IPPROTO_RAW (ignored)"""
//|         ...
//|
STATIC mp_obj_t socketpool_socketpool_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 0, 4, false);

    // create socket object (not bound to any NIC yet)
    socketpool_socket_obj_t *s = m_new_obj_with_finaliser(socketpool_socket_obj_t);
    s->base.type = &socketpool_socket_type;
    // s->nic = MP_OBJ_NULL;
    // s->nic_type = NULL;
    // s->u_param.domain = MOD_NETWORK_AF_INET;
    // s->u_param.type = MOD_NETWORK_SOCK_STREAM;
    // s->u_param.fileno = -1;
    // if (n_args >= 1) {
    //     s->u_param.domain = mp_obj_get_int(args[0]);
    //     if (n_args >= 2) {
    //         s->u_param.type = mp_obj_get_int(args[1]);
    //         if (n_args >= 4) {
    //             s->u_param.fileno = mp_obj_get_int(args[3]);
    //         }
    //     }
    // }

    return MP_OBJ_FROM_PTR(s);
}
MP_DEFINE_CONST_FUN_OBJ_KW(socketpool_socketpool_socket_obj, 1, socketpool_socketpool_socket);

//| def getaddrinfo(host: str, port: int) -> tuple:
//|     """Gets the address information for a hostname and port
//|
//|     Returns the appropriate family, socket type, socket protocol and
//|     address information to call socket.socket() and socket.connect() with,
//|     as a tuple."""
//|     ...
//|

STATIC mp_obj_t socketpool_socketpool_getaddrinfo(mp_obj_t host_in, mp_obj_t port_in) {
    // size_t hlen;
    // const char *host = mp_obj_str_get_data(host_in, &hlen);
    // mp_int_t port = mp_obj_get_int(port_in);
    // uint8_t out_ip[MOD_NETWORK_IPADDR_BUF_SIZE];
    // bool have_ip = false;

    // if (hlen > 0) {
    //     // check if host is already in IP form
    //     nlr_buf_t nlr;
    //     if (nlr_push(&nlr) == 0) {
    //         netutils_parse_ipv4_addr(host_in, out_ip, NETUTILS_BIG);
    //         have_ip = true;
    //         nlr_pop();
    //     } else {
    //         // swallow exception: host was not in IP form so need to do DNS lookup
    //     }
    // }

    // if (!have_ip) {
    //     // find a NIC that can do a name lookup
    //     for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
    //         mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
    //         mod_network_nic_type_t *nic_type = (mod_network_nic_type_t*)mp_obj_get_type(nic);
    //         if (nic_type->gethostbyname != NULL) {
    //             int ret = nic_type->gethostbyname(nic, host, hlen, out_ip);
    //             if (ret != 0) {
    //                 mp_raise_OSError(ret);
    //             }
    //             have_ip = true;
    //             break;
    //         }
    //     }
    // }

    // if (!have_ip) {
    //     nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, translate("no available NIC")));
    // }

    // mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
    // tuple->items[0] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET);
    // tuple->items[1] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM);
    // tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    // tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    // tuple->items[4] = netutils_format_inet_addr(out_ip, port, NETUTILS_BIG);
    // return mp_obj_new_list(1, (mp_obj_t*)&tuple);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpool_socketpool_getaddrinfo_obj, socketpool_socketpool_getaddrinfo);

STATIC const mp_rom_map_elem_t socketpool_socketpool_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&socketpool_socketpool_socket_obj) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&socketpool_socketpool_getaddrinfo_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(SOCKETPOOL_AF_INET) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET6), MP_ROM_INT(SOCKETPOOL_AF_INET6) },

    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(SOCKETPOOL_SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(SOCKETPOOL_SOCK_DGRAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(SOCKETPOOL_SOCK_RAW) },
};

STATIC MP_DEFINE_CONST_DICT(socketpool_socketpool_locals_dict, socketpool_socketpool_locals_dict_table);

const mp_obj_type_t socketpool_socketpool_type = {
    { &mp_type_type },
    .name = MP_QSTR_SocketPool,
    .make_new = socketpool_socketpool_make_new,
    .locals_dict = (mp_obj_dict_t*)&socketpool_socketpool_locals_dict,
};
