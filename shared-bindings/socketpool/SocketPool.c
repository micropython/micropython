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

#include <stdio.h>
#include <string.h>

#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "shared-bindings/ipaddress/__init__.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/socketpool/SocketPool.h"

//| class SocketPool:
//|     """A pool of socket resources available for the given radio. Only one
//|     SocketPool can be created for each radio.
//|
//|     SocketPool should be used in place of CPython's socket which provides
//|     a pool of sockets provided by the underlying OS.
//|     """
//|
//|     def __init__(self, radio: wifi.Radio) -> None:
//|         """Create a new SocketPool object for the provided radio
//|
//|         :param wifi.Radio radio: The (connected) network hardware to associate
//|             with this SocketPool; currently, this will always be the object
//|             returned by :py:attr:`wifi.radio`
//|         """
//|         ...
//|
STATIC mp_obj_t socketpool_socketpool_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    socketpool_socketpool_obj_t *s = m_new_obj_with_finaliser(socketpool_socketpool_obj_t);
    s->base.type = &socketpool_socketpool_type;
    mp_obj_t radio = args[0];

    common_hal_socketpool_socketpool_construct(s, radio);

    return MP_OBJ_FROM_PTR(s);
}

//|     class gaierror(OSError):
//|         """Errors raised by getaddrinfo"""
//|
MP_DEFINE_EXCEPTION(gaierror, OSError)

//|
//|     AF_INET: int
//|     AF_INET6: int
//|
//|     SOCK_STREAM: int
//|     SOCK_DGRAM: int
//|     SOCK_RAW: int
//|     EAI_NONAME: int
//|
//|     TCP_NODELAY: int
//|
//|     IPPROTO_TCP: int
//|     IPPROTO_IP: int
//|
//|     IP_MULTICAST_TTL: int
//|
//|     def socket(self, family: int = AF_INET, type: int = SOCK_STREAM) -> socketpool.Socket:
//|         """Create a new socket
//|
//|         :param ~int family: AF_INET or AF_INET6
//|         :param ~int type: SOCK_STREAM, SOCK_DGRAM or SOCK_RAW
//|
//|         The ``proto`` (protocol) and ``fileno`` arguments available in ``socket.socket()``
//|         in CPython are not supported.
//|         """
//|         ...
STATIC mp_obj_t socketpool_socketpool_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_family, ARG_type };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_family, MP_ARG_INT, {.u_int = SOCKETPOOL_AF_INET} },
        { MP_QSTR_type, MP_ARG_INT, {.u_int = SOCKETPOOL_SOCK_STREAM} },
    };
    socketpool_socketpool_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    socketpool_socketpool_addressfamily_t family = args[ARG_family].u_int;
    socketpool_socketpool_sock_t type = args[ARG_type].u_int;

    return common_hal_socketpool_socket(self, family, type);
}
MP_DEFINE_CONST_FUN_OBJ_KW(socketpool_socketpool_socket_obj, 1, socketpool_socketpool_socket);

//|     def getaddrinfo(
//|         self,
//|         host: str,
//|         port: int,
//|         family: int = 0,
//|         type: int = 0,
//|         proto: int = 0,
//|         flags: int = 0,
//|     ) -> Tuple[int, int, int, str, Tuple[str, int]]:
//|         """Gets the address information for a hostname and port
//|
//|         Returns the appropriate family, socket type, socket protocol and
//|         address information to call socket.socket() and socket.connect() with,
//|         as a tuple."""
//|         ...
//|
STATIC mp_obj_t socketpool_socketpool_getaddrinfo(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_host, ARG_port, ARG_family, ARG_type, ARG_proto, ARG_flags };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_host, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_port, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_family, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_type, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_port, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_flags, MP_ARG_INT, {.u_int = 0} },
    };
    socketpool_socketpool_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *host = mp_obj_str_get_str(args[ARG_host].u_obj);
    mp_int_t port = args[ARG_port].u_int;
    mp_obj_t ip_str = mp_const_none;

    if (strlen(host) > 0 && ipaddress_parse_ipv4address(host, strlen(host), NULL)) {
        ip_str = args[ARG_host].u_obj;
    }

    if (ip_str == mp_const_none) {
        ip_str = common_hal_socketpool_socketpool_gethostbyname_raise(self, host);
    }

    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(SOCKETPOOL_AF_INET);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(SOCKETPOOL_SOCK_STREAM);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    mp_obj_tuple_t *sockaddr = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    sockaddr->items[0] = ip_str;
    sockaddr->items[1] = MP_OBJ_NEW_SMALL_INT(port);
    tuple->items[4] = MP_OBJ_FROM_PTR(sockaddr);
    return mp_obj_new_list(1, (mp_obj_t *)&tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(socketpool_socketpool_getaddrinfo_obj, 1, socketpool_socketpool_getaddrinfo);

STATIC const mp_rom_map_elem_t socketpool_socketpool_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&socketpool_socketpool_socket_obj) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&socketpool_socketpool_getaddrinfo_obj) },
    { MP_ROM_QSTR(MP_QSTR_gaierror), MP_ROM_PTR(&mp_type_gaierror) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(SOCKETPOOL_AF_INET) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET6), MP_ROM_INT(SOCKETPOOL_AF_INET6) },

    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(SOCKETPOOL_SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(SOCKETPOOL_SOCK_DGRAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(SOCKETPOOL_SOCK_RAW) },

    { MP_ROM_QSTR(MP_QSTR_TCP_NODELAY), MP_ROM_INT(SOCKETPOOL_TCP_NODELAY) },

    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TCP), MP_ROM_INT(SOCKETPOOL_IPPROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IP), MP_ROM_INT(SOCKETPOOL_IPPROTO_IP) },
    { MP_ROM_QSTR(MP_QSTR_IP_MULTICAST_TTL), MP_ROM_INT(SOCKETPOOL_IP_MULTICAST_TTL) },

    { MP_ROM_QSTR(MP_QSTR_EAI_NONAME), MP_ROM_INT(SOCKETPOOL_EAI_NONAME) },
};

STATIC MP_DEFINE_CONST_DICT(socketpool_socketpool_locals_dict, socketpool_socketpool_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    socketpool_socketpool_type,
    MP_QSTR_SocketPool,
    MP_TYPE_FLAG_NONE,
    make_new, socketpool_socketpool_make_new,
    locals_dict, &socketpool_socketpool_locals_dict
    );

MP_WEAK
mp_obj_t common_hal_socketpool_socketpool_gethostbyname_raise(socketpool_socketpool_obj_t *self, const char *host) {
    mp_obj_t ip_str = common_hal_socketpool_socketpool_gethostbyname(self, host);
    if (ip_str == mp_const_none) {
        common_hal_socketpool_socketpool_raise_gaierror_noname();
    }
    return ip_str;
}

MP_WEAK NORETURN
void common_hal_socketpool_socketpool_raise_gaierror_noname(void) {
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 64, &print);
    mp_printf(&print, "%S", translate("Name or service not known"));

    mp_obj_t exc_args[] = {
        MP_OBJ_NEW_SMALL_INT(SOCKETPOOL_EAI_NONAME),
        mp_obj_new_str_from_vstr(&vstr),
    };
    nlr_raise(mp_obj_new_exception_args(&mp_type_gaierror, MP_ARRAY_SIZE(exc_args), exc_args));
}
