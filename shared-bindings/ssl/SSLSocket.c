/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Lucian Copeland for Adafruit Industries
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

#include "shared-bindings/ssl/SSLSocket.h"

#include <stdio.h>
#include <string.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "lib/netutils/netutils.h"

//| class SSLSocket:
//|     """Implements TLS security on a subset of `socketpool.Socket` functions. Cannot be created
//|        directly. Instead, call `wrap_socket` on an existing socket object.
//|
//|        Provides a subset of CPython's `ssl.SSLSocket` API. It only implements the versions of
//|        recv that do not allocate bytes objects."""
//|

//|     def __enter__(self) -> SSLSocket:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically closes the Socket when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t ssl_sslsocket___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_ssl_sslsocket_close(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ssl_sslsocket___exit___obj, 4, 4, ssl_sslsocket___exit__);

//|     def accept(self) -> Tuple[SSLSocket, Tuple[str, int]]:
//|         """Accept a connection on a listening socket of type SOCK_STREAM,
//|         creating a new socket of type SOCK_STREAM.
//|         Returns a tuple of (new_socket, remote_address)"""
//|
STATIC mp_obj_t ssl_sslsocket_accept(mp_obj_t self_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t ip[4];
    uint32_t port;

    ssl_sslsocket_obj_t *sslsock = common_hal_ssl_sslsocket_accept(self, ip, &port);

    mp_obj_t tuple_contents[2];
    tuple_contents[0] = MP_OBJ_FROM_PTR(sslsock);
    tuple_contents[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);
    return mp_obj_new_tuple(2, tuple_contents);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ssl_sslsocket_accept_obj, ssl_sslsocket_accept);

//|     def bind(self, address: Tuple[str, int]) -> None:
//|         """Bind a socket to an address
//|
//|         :param ~tuple address: tuple of (remote_address, remote_port)"""
//|         ...
//|
STATIC mp_obj_t ssl_sslsocket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);

    size_t hostlen;
    const char *host = mp_obj_str_get_data(addr_items[0], &hostlen);
    mp_int_t port = mp_obj_get_int(addr_items[1]);
    if (port < 0) {
        mp_raise_ValueError(translate("port must be >= 0"));
    }

    bool ok = common_hal_ssl_sslsocket_bind(self, host, hostlen, (uint32_t)port);
    if (!ok) {
        mp_raise_ValueError(translate("Error: Failure to bind"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_bind_obj, ssl_sslsocket_bind);

//|     def close(self) -> None:
//|         """Closes this Socket"""
//|
STATIC mp_obj_t ssl_sslsocket_close(mp_obj_t self_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_ssl_sslsocket_close(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ssl_sslsocket_close_obj, ssl_sslsocket_close);

//|     def connect(self, address: Tuple[str, int]) -> None:
//|         """Connect a socket to a remote address
//|
//|         :param ~tuple address: tuple of (remote_address, remote_port)"""
//|         ...
//|
STATIC mp_obj_t ssl_sslsocket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);

    size_t hostlen;
    const char *host = mp_obj_str_get_data(addr_items[0], &hostlen);
    mp_int_t port = mp_obj_get_int(addr_items[1]);
    if (port < 0) {
        mp_raise_ValueError(translate("port must be >= 0"));
    }

    bool ok = common_hal_ssl_sslsocket_connect(self, host, hostlen, (uint32_t)port);
    if (!ok) {
        mp_raise_OSError(0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_connect_obj, ssl_sslsocket_connect);

//|     def listen(self, backlog: int) -> None:
//|         """Set socket to listen for incoming connections
//|
//|         :param ~int backlog: length of backlog queue for waiting connetions"""
//|         ...
//|
STATIC mp_obj_t ssl_sslsocket_listen(mp_obj_t self_in, mp_obj_t backlog_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int backlog = mp_obj_get_int(backlog_in);

    common_hal_ssl_sslsocket_listen(self, backlog);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_listen_obj, ssl_sslsocket_listen);

//|     def recv_into(self, buffer: WriteableBuffer, bufsize: int) -> int:
//|         """Reads some bytes from the connected remote address, writing
//|         into the provided buffer. If bufsize <= len(buffer) is given,
//|         a maximum of bufsize bytes will be read into the buffer. If no
//|         valid value is given for bufsize, the default is the length of
//|         the given buffer.
//|
//|         Suits sockets of type SOCK_STREAM
//|         Returns an int of number of bytes read.
//|
//|         :param bytearray buffer: buffer to receive into
//|         :param int bufsize: optionally, a maximum number of bytes to read."""
//|         ...
//|
STATIC mp_obj_t ssl_sslsocket_recv_into(size_t n_args, const mp_obj_t *args) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (common_hal_ssl_sslsocket_get_closed(self)) {
        // Bad file number.
        mp_raise_OSError(MP_EBADF);
    }
    // if (!common_hal_ssl_sslsocket_get_connected(self)) {
    //     // not connected
    //     mp_raise_OSError(MP_ENOTCONN);
    // }
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    mp_int_t len = bufinfo.len;
    if (n_args == 3) {
        mp_int_t given_len = mp_obj_get_int(args[2]);
        if (given_len > len) {
            mp_raise_ValueError(translate("buffer too small for requested bytes"));
        }
        if (given_len > 0 && given_len < len) {
            len = given_len;
        }
    }

    if (len == 0) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    mp_int_t ret = common_hal_ssl_sslsocket_recv_into(self, (byte *)bufinfo.buf, len);
    return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ssl_sslsocket_recv_into_obj, 2, 3, ssl_sslsocket_recv_into);

//|     def send(self, bytes: ReadableBuffer) -> int:
//|         """Send some bytes to the connected remote address.
//|         Suits sockets of type SOCK_STREAM
//|
//|         :param ~bytes bytes: some bytes to send"""
//|         ...
//|
STATIC mp_obj_t ssl_sslsocket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_ssl_sslsocket_get_closed(self)) {
        // Bad file number.
        mp_raise_OSError(MP_EBADF);
    }
    if (!common_hal_ssl_sslsocket_get_connected(self)) {
        mp_raise_BrokenPipeError();
    }
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    mp_int_t ret = common_hal_ssl_sslsocket_send(self, bufinfo.buf, bufinfo.len);
    if (ret == -1) {
        mp_raise_BrokenPipeError();
    }
    return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_send_obj, ssl_sslsocket_send);

// //|     def setsockopt(self, level: int, optname: int, value: int) -> None:
// //|         """Sets socket options"""
// //|         ...
// //|
// STATIC mp_obj_t ssl_sslsocket_setsockopt(size_t n_args, const mp_obj_t *args) {
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ssl_sslsocket_setsockopt_obj, 4, 4, ssl_sslsocket_setsockopt);

//|     def settimeout(self, value: int) -> None:
//|         """Set the timeout value for this socket.
//|
//|         :param ~int value: timeout in seconds.  0 means non-blocking.  None means block indefinitely."""
//|         ...
//|
STATIC mp_obj_t ssl_sslsocket_settimeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t timeout_ms;
    if (timeout_in == mp_const_none) {
        timeout_ms = -1;
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT
        timeout_ms = 1000 * mp_obj_get_float(timeout_in);
        #else
        timeout_ms = 1000 * mp_obj_get_int(timeout_in);
        #endif
    }
    common_hal_ssl_sslsocket_settimeout(self, timeout_ms);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_settimeout_obj, ssl_sslsocket_settimeout);

//|     def setblocking(self, flag: bool) -> Optional[int]:
//|         """Set the blocking behaviour of this socket.
//|
//|         :param ~bool flag: False means non-blocking, True means block indefinitely."""
//|         ...
//|
// method socket.setblocking(flag)
STATIC mp_obj_t ssl_sslsocket_setblocking(mp_obj_t self_in, mp_obj_t blocking) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_obj_is_true(blocking)) {
        common_hal_ssl_sslsocket_settimeout(self, -1);
    } else {
        common_hal_ssl_sslsocket_settimeout(self, 0);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_setblocking_obj, ssl_sslsocket_setblocking);

//|     def __hash__(self) -> int:
//|         """Returns a hash for the Socket."""
//|         ...
//|
STATIC mp_obj_t ssl_sslsocket_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    switch (op) {
        case MP_UNARY_OP_HASH: {
            return mp_obj_id(self_in);
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC const mp_rom_map_elem_t ssl_sslsocket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&ssl_sslsocket___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ssl_sslsocket_close_obj) },

    { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&ssl_sslsocket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&ssl_sslsocket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ssl_sslsocket_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&ssl_sslsocket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&ssl_sslsocket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv_into), MP_ROM_PTR(&ssl_sslsocket_recv_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&ssl_sslsocket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&ssl_sslsocket_setblocking_obj) },
    // { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&ssl_sslsocket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout), MP_ROM_PTR(&ssl_sslsocket_settimeout_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ssl_sslsocket_locals_dict, ssl_sslsocket_locals_dict_table);

const mp_obj_type_t ssl_sslsocket_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_SSLSocket,
    .locals_dict = (mp_obj_dict_t *)&ssl_sslsocket_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = ssl_sslsocket_unary_op,
        )
};
