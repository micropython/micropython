// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/ssl/SSLSocket.h"

#include <stdio.h>
#include <string.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/mperrno.h"
#include "py/objlist.h"
#include "py/objtuple.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared/netutils/netutils.h"

//| class SSLSocket:
//|     """Implements TLS security on a subset of `socketpool.Socket` functions. Cannot be created
//|     directly. Instead, call `wrap_socket` on an existing socket object.
//|
//|     Provides a subset of CPython's `ssl.SSLSocket` API. It only implements the versions of
//|     recv that do not allocate bytes objects."""
//|

//|     def __hash__(self) -> int:
//|         """Returns a hash for the Socket."""
//|         ...
// Provided by automatic inclusion of hash()
// https://github.com/micropython/micropython/pull/10348

//|     def __enter__(self) -> SSLSocket:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically closes the Socket when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
static mp_obj_t ssl_sslsocket___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_ssl_sslsocket_close(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ssl_sslsocket___exit___obj, 4, 4, ssl_sslsocket___exit__);

//|     def accept(self) -> Tuple[SSLSocket, Tuple[str, int]]:
//|         """Accept a connection on a listening socket of type SOCK_STREAM,
//|         creating a new socket of type SOCK_STREAM.
//|         Returns a tuple of (new_socket, remote_address)"""
static mp_obj_t ssl_sslsocket_accept(mp_obj_t self_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_ssl_sslsocket_accept(self);
}
static MP_DEFINE_CONST_FUN_OBJ_1(ssl_sslsocket_accept_obj, ssl_sslsocket_accept);

//|     def bind(self, address: Tuple[str, int]) -> None:
//|         """Bind a socket to an address
//|
//|         :param ~tuple address: tuple of (remote_address, remote_port)"""
//|         ...
static mp_obj_t ssl_sslsocket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);

    common_hal_ssl_sslsocket_bind(self, addr_in);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_bind_obj, ssl_sslsocket_bind);

//|     def close(self) -> None:
//|         """Closes this Socket"""
static mp_obj_t ssl_sslsocket_close(mp_obj_t self_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_ssl_sslsocket_close(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(ssl_sslsocket_close_obj, ssl_sslsocket_close);

//|     def connect(self, address: Tuple[str, int]) -> None:
//|         """Connect a socket to a remote address
//|
//|         :param ~tuple address: tuple of (remote_address, remote_port)"""
//|         ...
static mp_obj_t ssl_sslsocket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_ssl_sslsocket_connect(self, addr_in);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_connect_obj, ssl_sslsocket_connect);

//|     def listen(self, backlog: int) -> None:
//|         """Set socket to listen for incoming connections
//|
//|         :param ~int backlog: length of backlog queue for waiting connetions"""
//|         ...
static mp_obj_t ssl_sslsocket_listen(mp_obj_t self_in, mp_obj_t backlog_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int backlog = mp_obj_get_int(backlog_in);

    common_hal_ssl_sslsocket_listen(self, backlog);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_listen_obj, ssl_sslsocket_listen);

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
static mp_obj_t ssl_sslsocket_recv_into(size_t n_args, const mp_obj_t *args) {
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
            mp_raise_ValueError(MP_ERROR_TEXT("buffer too small for requested bytes"));
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
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ssl_sslsocket_recv_into_obj, 2, 3, ssl_sslsocket_recv_into);

//|     def send(self, bytes: ReadableBuffer) -> int:
//|         """Send some bytes to the connected remote address.
//|         Suits sockets of type SOCK_STREAM
//|
//|         :param ~bytes bytes: some bytes to send"""
//|         ...
static mp_obj_t ssl_sslsocket_send(mp_obj_t self_in, mp_obj_t buf_in) {
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
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_send_obj, ssl_sslsocket_send);

// //|     def setsockopt(self, level: int, optname: int, value: int | ReadableBuffer) -> None:
// //|         """Sets socket options"""
// //|         ...
// //|
static mp_obj_t ssl_sslsocket_setsockopt(size_t n_args, const mp_obj_t *args) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t level = args[1];
    mp_obj_t optname = args[2];
    mp_obj_t optval = args[3];

    // throws on error
    common_hal_ssl_sslsocket_setsockopt(self, level, optname, optval);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ssl_sslsocket_setsockopt_obj, 4, 4, ssl_sslsocket_setsockopt);

//|     def settimeout(self, value: int) -> None:
//|         """Set the timeout value for this socket.
//|
//|         :param ~int value: timeout in seconds.  0 means non-blocking.  None means block indefinitely.
//|         """
//|         ...
static mp_obj_t ssl_sslsocket_settimeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_ssl_sslsocket_settimeout(self, timeout_in);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_settimeout_obj, ssl_sslsocket_settimeout);

//|     def setblocking(self, flag: bool) -> Optional[int]:
//|         """Set the blocking behaviour of this socket.
//|
//|         :param ~bool flag: False means non-blocking, True means block indefinitely."""
//|         ...
//|
// method socket.setblocking(flag)
static mp_obj_t ssl_sslsocket_setblocking(mp_obj_t self_in, mp_obj_t blocking) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_obj_is_true(blocking)) {
        common_hal_ssl_sslsocket_settimeout(self, mp_const_none);
    } else {
        common_hal_ssl_sslsocket_settimeout(self, MP_OBJ_NEW_SMALL_INT(0));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslsocket_setblocking_obj, ssl_sslsocket_setblocking);

static const mp_rom_map_elem_t ssl_sslsocket_locals_dict_table[] = {
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
    { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&ssl_sslsocket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout), MP_ROM_PTR(&ssl_sslsocket_settimeout_obj) },
};

static MP_DEFINE_CONST_DICT(ssl_sslsocket_locals_dict, ssl_sslsocket_locals_dict_table);

typedef mp_uint_t (*readwrite_func)(ssl_sslsocket_obj_t *, const uint8_t *, mp_uint_t);

static mp_int_t readwrite_common(mp_obj_t self_in, readwrite_func fn, const uint8_t *buf, size_t size, int *errorcode) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t ret = -EIO;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        ret = fn(self, buf, size);
        nlr_pop();
    } else {
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);
        if (nlr_push(&nlr) == 0) {
            ret = -mp_obj_get_int(mp_load_attr(exc, MP_QSTR_errno));
            nlr_pop();
        }
    }
    if (ret < 0) {
        *errorcode = -ret;
        return MP_STREAM_ERROR;
    }
    return ret;
}

static mp_uint_t sslsocket_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errorcode) {
    return readwrite_common(self_in, (readwrite_func)common_hal_ssl_sslsocket_recv_into, buf, size, errorcode);
}

static mp_uint_t sslsocket_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errorcode) {
    return readwrite_common(self_in, common_hal_ssl_sslsocket_send, buf, size, errorcode);
}

static mp_uint_t sslsocket_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    ssl_sslsocket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && common_hal_ssl_sslsocket_readable(self) > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && common_hal_ssl_sslsocket_writable(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}


static const mp_stream_p_t sslsocket_stream_p = {
    .read = sslsocket_read,
    .write = sslsocket_write,
    .ioctl = sslsocket_ioctl,
    .is_text = false,
};


MP_DEFINE_CONST_OBJ_TYPE(
    ssl_sslsocket_type,
    MP_QSTR_SSLSocket,
    MP_TYPE_FLAG_NONE,
    locals_dict, &ssl_sslsocket_locals_dict,
    protocol, &sslsocket_stream_p
    );
