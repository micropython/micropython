// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
//|     """A pool of socket resources available for the given network interface. More than one
//|     SocketPool can be created for a network interface. The sockets of every SocketPool
//|     count toward the same limit on open sockets for the given network interface.
//|
//|     SocketPool should be used in place of CPython's socket which provides
//|     a pool of sockets provided by the underlying OS.
//|     """
//|
//|     def __init__(self, network_interface: Union[wifi.Radio, mii.Ethernet, hostnetwork.HostNetwork]) -> None:
//|         """Create a new SocketPool object for the provided network interface
//|
//|         The network interface does not need to be connected when the SocketPool is created.
//|         It must be connected, or :py:attr:`wifi.radio` must be running an access point, before
//|         the SocketPool can communicate over the network. The same SocketPool can be used again
//|         after the network interface disconnects and reconnects.
//|
//|         :param network_interface: The network interface to associate with this
//|             SocketPool, such as :py:attr:`wifi.radio`, an `mii.Ethernet`, or :py:attr:`board.NETWORK`.
//|         """
//|         ...
//|
static mp_obj_t socketpool_socketpool_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t network_interface = args[0];

    socketpool_socketpool_obj_t *s = mp_obj_malloc_with_finaliser(socketpool_socketpool_obj_t, &socketpool_socketpool_type);
    common_hal_socketpool_socketpool_construct(s, network_interface);

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
//|     SOL_SOCKET: int
//|
//|     SO_REUSEADDR: int
//|
//|     TCP_NODELAY: int
//|
//|     IPPROTO_IP: int
//|     IPPROTO_ICMP: int
//|     IPPROTO_TCP: int
//|     IPPROTO_UDP: int
//|     IPPROTO_IPV6: int
//|     IPPROTO_RAW: int
//|
//|     IP_MULTICAST_TTL: int
//|
//|     def socket(
//|         self, family: int = AF_INET, type: int = SOCK_STREAM, proto: int = IPPROTO_IP
//|     ) -> socketpool.Socket:
//|         """Create a new socket
//|
//|         :param ~int family: AF_INET or AF_INET6
//|         :param ~int type: SOCK_STREAM, SOCK_DGRAM or SOCK_RAW
//|         :param ~int proto: IPPROTO_IP, IPPROTO_ICMP, IPPROTO_TCP, IPPROTO_UDP, IPPROTO_IPV6or IPPROTO_RAW. Only works with SOCK_RAW
//|
//|         The ``fileno`` argument available in ``socket.socket()``
//|         in CPython is not supported.
//|         """
//|         ...
//|
static mp_obj_t socketpool_socketpool_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_family, ARG_type, ARG_proto };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_family, MP_ARG_INT, {.u_int = SOCKETPOOL_AF_INET} },
        { MP_QSTR_type, MP_ARG_INT, {.u_int = SOCKETPOOL_SOCK_STREAM} },
        { MP_QSTR_proto, MP_ARG_INT, {.u_int = SOCKETPOOL_IPPROTO_IP} },
    };
    socketpool_socketpool_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    socketpool_socketpool_addressfamily_t family = args[ARG_family].u_int;
    socketpool_socketpool_sock_t type = args[ARG_type].u_int;
    socketpool_socketpool_ipproto_t proto = args[ARG_proto].u_int;

    return common_hal_socketpool_socket(self, family, type, proto);
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
//|
static mp_obj_t socketpool_socketpool_getaddrinfo(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    return common_hal_socketpool_getaddrinfo_raise(
        self,
        mp_obj_str_get_str(args[ARG_host].u_obj),
        args[ARG_port].u_int,
        args[ARG_family].u_int,
        args[ARG_type].u_int,
        args[ARG_proto].u_int,
        args[ARG_flags].u_int);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(socketpool_socketpool_getaddrinfo_obj, 1, socketpool_socketpool_getaddrinfo);

static const mp_rom_map_elem_t socketpool_socketpool_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&socketpool_socketpool_socket_obj) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&socketpool_socketpool_getaddrinfo_obj) },
    { MP_ROM_QSTR(MP_QSTR_gaierror), MP_ROM_PTR(&mp_type_gaierror) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(SOCKETPOOL_AF_INET) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET6), MP_ROM_INT(SOCKETPOOL_AF_INET6) },

    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(SOCKETPOOL_SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(SOCKETPOOL_SOCK_DGRAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(SOCKETPOOL_SOCK_RAW) },

    { MP_ROM_QSTR(MP_QSTR_SOL_SOCKET), MP_ROM_INT(SOCKETPOOL_SOL_SOCKET) },

    { MP_ROM_QSTR(MP_QSTR_SO_REUSEADDR), MP_ROM_INT(SOCKETPOOL_SO_REUSEADDR) },

    { MP_ROM_QSTR(MP_QSTR_TCP_NODELAY), MP_ROM_INT(SOCKETPOOL_TCP_NODELAY) },

    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IP), MP_ROM_INT(SOCKETPOOL_IPPROTO_IP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_ICMP), MP_ROM_INT(SOCKETPOOL_IPPROTO_ICMP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TCP), MP_ROM_INT(SOCKETPOOL_IPPROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_UDP), MP_ROM_INT(SOCKETPOOL_IPPROTO_UDP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IPV6), MP_ROM_INT(SOCKETPOOL_IPPROTO_IPV6) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_RAW), MP_ROM_INT(SOCKETPOOL_IPPROTO_RAW) },
    { MP_ROM_QSTR(MP_QSTR_IP_MULTICAST_TTL), MP_ROM_INT(SOCKETPOOL_IP_MULTICAST_TTL) },

    { MP_ROM_QSTR(MP_QSTR_EAI_NONAME), MP_ROM_INT(SOCKETPOOL_EAI_NONAME) },
};

static MP_DEFINE_CONST_DICT(socketpool_socketpool_locals_dict, socketpool_socketpool_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    socketpool_socketpool_type,
    MP_QSTR_SocketPool,
    MP_TYPE_FLAG_NONE,
    make_new, socketpool_socketpool_make_new,
    locals_dict, &socketpool_socketpool_locals_dict
    );

MP_WEAK MP_NORETURN
void common_hal_socketpool_socketpool_raise_gaierror(int err) {
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 64, &print);
    // Only EAI_NONAME means the name was not found. Other codes cover
    // unrelated failures, such as an unsupported family, so use the generic
    // message for those rather than one that would be misleading.
    if (err == SOCKETPOOL_EAI_NONAME) {
        mp_printf(&print, "%S", MP_ERROR_TEXT("Name or service not known"));
    } else {
        mp_cprintf(&print, MP_ERROR_TEXT("%q failure: %d"), MP_QSTR_getaddrinfo, err);
    }

    mp_obj_t exc_args[] = {
        MP_OBJ_NEW_SMALL_INT(err),
        mp_obj_new_str_from_vstr(&vstr),
    };
    nlr_raise(mp_obj_new_exception_args(&mp_type_gaierror, MP_ARRAY_SIZE(exc_args), exc_args));
}

MP_WEAK MP_NORETURN
void common_hal_socketpool_socketpool_raise_gaierror_noname(void) {
    common_hal_socketpool_socketpool_raise_gaierror(SOCKETPOOL_EAI_NONAME);
}
