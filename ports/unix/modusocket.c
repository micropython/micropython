/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Paul Sokolovsky
 * Copyright (c) 2014-2019 Damien P. George
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
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <math.h>

#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/builtin.h"
#include "py/mphal.h"

/*
  The idea of this module is to implement reasonable minimum of
  socket-related functions to write typical clients and servers.
  The module named "usocket" on purpose, to allow to make
  Python-level module more (or fully) compatible with CPython
  "socket", e.g.:
  ---- socket.py ----
  from usocket import *
  from socket_more_funcs import *
  from socket_more_funcs2 import *
  -------------------
  I.e. this module should stay lean, and more functions (if needed)
  should be add to separate modules (C or Python level).
 */

// This type must "inherit" from mp_obj_fdfile_t, i.e. matching subset of
// fields should have the same layout.
typedef struct _mp_obj_socket_t {
    mp_obj_base_t base;
    int fd;
    bool blocking;
} mp_obj_socket_t;

const mp_obj_type_t mp_type_socket;

// Helper functions
static inline mp_obj_t mp_obj_from_sockaddr(const struct sockaddr *addr, socklen_t len) {
    return mp_obj_new_bytes((const byte *)addr, len);
}

STATIC mp_obj_socket_t *socket_new(int fd) {
    mp_obj_socket_t *o = m_new_obj(mp_obj_socket_t);
    o->base.type = &mp_type_socket;
    o->fd = fd;
    o->blocking = true;
    return o;
}


STATIC void socket_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<_socket %d>", self->fd);
}

STATIC mp_uint_t socket_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_socket_t *o = MP_OBJ_TO_PTR(o_in);
    mp_int_t r = read(o->fd, buf, size);
    if (r == -1) {
        int err = errno;
        // On blocking socket, we get EAGAIN in case SO_RCVTIMEO/SO_SNDTIMEO
        // timed out, and need to convert that to ETIMEDOUT.
        if (err == EAGAIN && o->blocking) {
            err = MP_ETIMEDOUT;
        }

        *errcode = err;
        return MP_STREAM_ERROR;
    }
    return r;
}

STATIC mp_uint_t socket_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_socket_t *o = MP_OBJ_TO_PTR(o_in);
    mp_int_t r = write(o->fd, buf, size);
    if (r == -1) {
        int err = errno;
        // On blocking socket, we get EAGAIN in case SO_RCVTIMEO/SO_SNDTIMEO
        // timed out, and need to convert that to ETIMEDOUT.
        if (err == EAGAIN && o->blocking) {
            err = MP_ETIMEDOUT;
        }

        *errcode = err;
        return MP_STREAM_ERROR;
    }
    return r;
}

STATIC mp_uint_t socket_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(o_in);
    (void)arg;
    switch (request) {
        case MP_STREAM_CLOSE:
            // There's a POSIX drama regarding return value of close in general,
            // and EINTR error in particular. See e.g.
            // http://lwn.net/Articles/576478/
            // http://austingroupbugs.net/view.php?id=529
            // The rationale MicroPython follows is that close() just releases
            // file descriptor. If you're interested to catch I/O errors before
            // closing fd, fsync() it.
            close(self->fd);
            return 0;

        case MP_STREAM_GET_FILENO:
            return self->fd;

        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

STATIC mp_obj_t socket_fileno(mp_obj_t self_in) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_fileno_obj, socket_fileno);

STATIC mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(addr_in, &bufinfo, MP_BUFFER_READ);
    int r = connect(self->fd, (const struct sockaddr *)bufinfo.buf, bufinfo.len);
    int err = errno;
    if (r == -1 && self->blocking && err == EINPROGRESS) {
        // EINPROGRESS on a blocking socket means the operation timed out
        err = MP_ETIMEDOUT;
    }
    RAISE_ERRNO(r, err);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

STATIC mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(addr_in, &bufinfo, MP_BUFFER_READ);
    int r = bind(self->fd, (const struct sockaddr *)bufinfo.buf, bufinfo.len);
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

STATIC mp_obj_t socket_listen(mp_obj_t self_in, mp_obj_t backlog_in) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(self_in);
    int r = listen(self->fd, MP_OBJ_SMALL_INT_VALUE(backlog_in));
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_listen_obj, socket_listen);

STATIC mp_obj_t socket_accept(mp_obj_t self_in) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(self_in);
    // sockaddr_storage isn't stack-friendly (129 bytes or so)
    //struct sockaddr_storage addr;
    byte addr[32];
    socklen_t addr_len = sizeof(addr);
    int fd = accept(self->fd, (struct sockaddr*)&addr, &addr_len);
    int err = errno;
    if (fd == -1 && self->blocking && err == EAGAIN) {
        // EAGAIN on a blocking socket means the operation timed out
        err = MP_ETIMEDOUT;
    }
    RAISE_ERRNO(fd, err);

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    t->items[0] = MP_OBJ_FROM_PTR(socket_new(fd));
    t->items[1] = mp_obj_new_bytearray(addr_len, &addr);

    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

// Note: besides flag param, this differs from read() in that
// this does not swallow blocking errors (EAGAIN, EWOULDBLOCK) -
// these would be thrown as exceptions.
STATIC mp_obj_t socket_recv(size_t n_args, const mp_obj_t *args) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(args[0]);
    int sz = MP_OBJ_SMALL_INT_VALUE(args[1]);
    int flags = 0;

    if (n_args > 2) {
        flags = MP_OBJ_SMALL_INT_VALUE(args[2]);
    }

    byte *buf = m_new(byte, sz);
    int out_sz = recv(self->fd, buf, sz, flags);
    RAISE_ERRNO(out_sz, errno);

    mp_obj_t ret = mp_obj_new_str_of_type(&mp_type_bytes, buf, out_sz);
    m_del(char, buf, sz);
    return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_recv_obj, 2, 3, socket_recv);

STATIC mp_obj_t socket_recvfrom(size_t n_args, const mp_obj_t *args) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(args[0]);
    int sz = MP_OBJ_SMALL_INT_VALUE(args[1]);
    int flags = 0;

    if (n_args > 2) {
        flags = MP_OBJ_SMALL_INT_VALUE(args[2]);
    }

    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    byte *buf = m_new(byte, sz);
    int out_sz = recvfrom(self->fd, buf, sz, flags, (struct sockaddr*)&addr, &addr_len);
    RAISE_ERRNO(out_sz, errno);

    mp_obj_t buf_o = mp_obj_new_str_of_type(&mp_type_bytes, buf, out_sz);
    m_del(char, buf, sz);

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    t->items[0] = buf_o;
    t->items[1] = mp_obj_from_sockaddr((struct sockaddr*)&addr, addr_len);

    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_recvfrom_obj, 2, 3, socket_recvfrom);

// Note: besides flag param, this differs from write() in that
// this does not swallow blocking errors (EAGAIN, EWOULDBLOCK) -
// these would be thrown as exceptions.
STATIC mp_obj_t socket_send(size_t n_args, const mp_obj_t *args) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(args[0]);
    int flags = 0;

    if (n_args > 2) {
        flags = MP_OBJ_SMALL_INT_VALUE(args[2]);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    int out_sz = send(self->fd, bufinfo.buf, bufinfo.len, flags);
    RAISE_ERRNO(out_sz, errno);

    return MP_OBJ_NEW_SMALL_INT(out_sz);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_send_obj, 2, 3, socket_send);

STATIC mp_obj_t socket_sendto(size_t n_args, const mp_obj_t *args) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(args[0]);
    int flags = 0;

    mp_obj_t dst_addr = args[2];
    if (n_args > 3) {
        flags = MP_OBJ_SMALL_INT_VALUE(args[2]);
        dst_addr = args[3];
    }

    mp_buffer_info_t bufinfo, addr_bi;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    mp_get_buffer_raise(dst_addr, &addr_bi, MP_BUFFER_READ);
    int out_sz = sendto(self->fd, bufinfo.buf, bufinfo.len, flags,
                        (struct sockaddr *)addr_bi.buf, addr_bi.len);
    RAISE_ERRNO(out_sz, errno);

    return MP_OBJ_NEW_SMALL_INT(out_sz);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_sendto_obj, 3, 4, socket_sendto);

STATIC mp_obj_t socket_setsockopt(size_t n_args, const mp_obj_t *args) {
    (void)n_args; // always 4
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(args[0]);
    int level = MP_OBJ_SMALL_INT_VALUE(args[1]);
    int option = mp_obj_get_int(args[2]);

    const void *optval;
    socklen_t optlen;
    int val;
    if (mp_obj_is_int(args[3])) {
        val = mp_obj_int_get_truncated(args[3]);
        optval = &val;
        optlen = sizeof(val);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
        optval = bufinfo.buf;
        optlen = bufinfo.len;
    }
    int r = setsockopt(self->fd, level, option, optval, optlen);
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_setsockopt_obj, 4, 4, socket_setsockopt);

STATIC mp_obj_t socket_setblocking(mp_obj_t self_in, mp_obj_t flag_in) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(self_in);
    int val = mp_obj_is_true(flag_in);
    int flags = fcntl(self->fd, F_GETFL, 0);
    RAISE_ERRNO(flags, errno);
    if (val) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    flags = fcntl(self->fd, F_SETFL, flags);
    RAISE_ERRNO(flags, errno);
    self->blocking = val;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_setblocking_obj, socket_setblocking);

STATIC mp_obj_t socket_settimeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(self_in);
    struct timeval tv = {0,};
    bool new_blocking = true;

    // Timeout of None means no timeout, which in POSIX is signified with 0 timeout,
    // and that's how 'tv' is initialized above
    if (timeout_in != mp_const_none) {
        #if MICROPY_PY_BUILTINS_FLOAT
        mp_float_t val = mp_obj_get_float(timeout_in);
        double ipart;
        tv.tv_usec = round(modf(val, &ipart) * 1000000);
        tv.tv_sec = ipart;
        #else
        tv.tv_sec = mp_obj_get_int(timeout_in);
        #endif

        // For SO_RCVTIMEO/SO_SNDTIMEO, zero timeout means infinity, but
        // for Python API it means non-blocking.
        if (tv.tv_sec == 0 && tv.tv_usec == 0) {
            new_blocking = false;
        }
    }

    if (new_blocking) {
        int r;
        r = setsockopt(self->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
        RAISE_ERRNO(r, errno);
        r = setsockopt(self->fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));
        RAISE_ERRNO(r, errno);
    }

    if (self->blocking != new_blocking) {
        socket_setblocking(self_in, mp_obj_new_bool(new_blocking));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_settimeout_obj, socket_settimeout);

STATIC mp_obj_t socket_makefile(size_t n_args, const mp_obj_t *args) {
    // TODO: CPython explicitly says that closing returned object doesn't close
    // the original socket (Python2 at all says that fd is dup()ed). But we
    // save on the bloat.
    mp_obj_socket_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t *new_args = alloca(n_args * sizeof(mp_obj_t));
    memcpy(new_args + 1, args + 1, (n_args - 1) * sizeof(mp_obj_t));
    new_args[0] = MP_OBJ_NEW_SMALL_INT(self->fd);
    return mp_builtin_open(n_args, new_args, (mp_map_t*)&mp_const_empty_map);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_makefile_obj, 1, 3, socket_makefile);

STATIC mp_obj_t socket_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    (void)n_kw;

    int family = AF_INET;
    int type = SOCK_STREAM;
    int proto = 0;

    if (n_args > 0) {
        assert(mp_obj_is_small_int(args[0]));
        family = MP_OBJ_SMALL_INT_VALUE(args[0]);
        if (n_args > 1) {
            assert(mp_obj_is_small_int(args[1]));
            type = MP_OBJ_SMALL_INT_VALUE(args[1]);
            if (n_args > 2) {
                assert(mp_obj_is_small_int(args[2]));
                proto = MP_OBJ_SMALL_INT_VALUE(args[2]);
            }
        }
    }

    int fd = socket(family, type, proto);
    RAISE_ERRNO(fd, errno);
    return MP_OBJ_FROM_PTR(socket_new(fd));
}

STATIC const mp_rom_map_elem_t usocket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&socket_fileno_obj) },
    { MP_ROM_QSTR(MP_QSTR_makefile), MP_ROM_PTR(&socket_makefile_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&socket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&socket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&socket_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom), MP_ROM_PTR(&socket_recvfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendto), MP_ROM_PTR(&socket_sendto_obj) },
    { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&socket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout), MP_ROM_PTR(&socket_settimeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
};

STATIC MP_DEFINE_CONST_DICT(usocket_locals_dict, usocket_locals_dict_table);

STATIC const mp_stream_p_t usocket_stream_p = {
    .read = socket_read,
    .write = socket_write,
    .ioctl = socket_ioctl,
};

const mp_obj_type_t mp_type_socket = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .print = socket_print,
    .make_new = socket_make_new,
    .getiter = NULL,
    .iternext = NULL,
    .protocol = &usocket_stream_p,
    .locals_dict = (mp_obj_dict_t*)&usocket_locals_dict,
};

#define BINADDR_MAX_LEN sizeof(struct in6_addr)
STATIC mp_obj_t mod_socket_inet_pton(mp_obj_t family_in, mp_obj_t addr_in) {
    int family = mp_obj_get_int(family_in);
    byte binaddr[BINADDR_MAX_LEN];
    int r = inet_pton(family, mp_obj_str_get_str(addr_in), binaddr);
    RAISE_ERRNO(r, errno);
    if (r == 0) {
        mp_raise_OSError(MP_EINVAL);
    }
    int binaddr_len = 0;
    switch (family) {
        case AF_INET:
            binaddr_len = sizeof(struct in_addr);
            break;
        case AF_INET6:
            binaddr_len = sizeof(struct in6_addr);
            break;
    }
    return mp_obj_new_bytes(binaddr, binaddr_len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_socket_inet_pton_obj, mod_socket_inet_pton);

STATIC mp_obj_t mod_socket_inet_ntop(mp_obj_t family_in, mp_obj_t binaddr_in) {
    int family = mp_obj_get_int(family_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(binaddr_in, &bufinfo, MP_BUFFER_READ);
    vstr_t vstr;
    vstr_init_len(&vstr, family == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN);
    if (inet_ntop(family, bufinfo.buf, vstr.buf, vstr.len) == NULL) {
        mp_raise_OSError(errno);
    }
    vstr.len = strlen(vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_socket_inet_ntop_obj, mod_socket_inet_ntop);

STATIC mp_obj_t mod_socket_getaddrinfo(size_t n_args, const mp_obj_t *args) {
    // TODO: Implement 5th and 6th args

    const char *host = mp_obj_str_get_str(args[0]);
    const char *serv = NULL;
    struct addrinfo hints;
    char buf[6];
    memset(&hints, 0, sizeof(hints));
    // getaddrinfo accepts port in string notation, so however
    // it may seem stupid, we need to convert int to str
    if (mp_obj_is_small_int(args[1])) {
        unsigned port = (unsigned short)MP_OBJ_SMALL_INT_VALUE(args[1]);
        snprintf(buf, sizeof(buf), "%u", port);
        serv = buf;
        hints.ai_flags = AI_NUMERICSERV;
#ifdef __UCLIBC_MAJOR__
#if __UCLIBC_MAJOR__ == 0 && (__UCLIBC_MINOR__ < 9 || (__UCLIBC_MINOR__ == 9 && __UCLIBC_SUBLEVEL__ <= 32))
// "warning" requires -Wno-cpp which is a relatively new gcc option, so we choose not to use it.
//#warning Working around uClibc bug with numeric service name
        // Older versions og uClibc have bugs when numeric ports in service
        // arg require also hints.ai_socktype (or hints.ai_protocol) != 0
        // This actually was fixed in 0.9.32.1, but uClibc doesn't allow to
        // test for that.
        // http://git.uclibc.org/uClibc/commit/libc/inet/getaddrinfo.c?id=bc3be18145e4d5
        // Note that this is crude workaround, precluding UDP socket addresses
        // to be returned. TODO: set only if not set by Python args.
        hints.ai_socktype = SOCK_STREAM;
#endif
#endif
    } else {
        serv = mp_obj_str_get_str(args[1]);
    }

    if (n_args > 2) {
        hints.ai_family = MP_OBJ_SMALL_INT_VALUE(args[2]);
        if (n_args > 3) {
            hints.ai_socktype = MP_OBJ_SMALL_INT_VALUE(args[3]);
        }
    }

    struct addrinfo *addr_list;
    int res = getaddrinfo(host, serv, &hints, &addr_list);

    if (res != 0) {
        // CPython: socket.gaierror
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[addrinfo error %d]", res));
    }
    assert(addr_list);

    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (struct addrinfo *addr = addr_list; addr; addr = addr->ai_next) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
        t->items[0] = MP_OBJ_NEW_SMALL_INT(addr->ai_family);
        t->items[1] = MP_OBJ_NEW_SMALL_INT(addr->ai_socktype);
        t->items[2] = MP_OBJ_NEW_SMALL_INT(addr->ai_protocol);
        // "canonname will be a string representing the canonical name of the host
        // if AI_CANONNAME is part of the flags argument; else canonname will be empty." ??
        if (addr->ai_canonname) {
            t->items[3] = MP_OBJ_NEW_QSTR(qstr_from_str(addr->ai_canonname));
        } else {
            t->items[3] = mp_const_none;
        }
        t->items[4] = mp_obj_new_bytearray(addr->ai_addrlen, addr->ai_addr);
        mp_obj_list_append(list, MP_OBJ_FROM_PTR(t));
    }
    freeaddrinfo(addr_list);
    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_socket_getaddrinfo_obj, 2, 4, mod_socket_getaddrinfo);

STATIC mp_obj_t mod_socket_sockaddr(mp_obj_t sockaddr_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(sockaddr_in, &bufinfo, MP_BUFFER_READ);
    switch (((struct sockaddr*)bufinfo.buf)->sa_family) {
        case AF_INET: {
            struct sockaddr_in *sa = (struct sockaddr_in*)bufinfo.buf;
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
            t->items[0] = MP_OBJ_NEW_SMALL_INT(AF_INET);
            t->items[1] = mp_obj_new_bytes((byte*)&sa->sin_addr, sizeof(sa->sin_addr));
            t->items[2] = MP_OBJ_NEW_SMALL_INT(ntohs(sa->sin_port));
            return MP_OBJ_FROM_PTR(t);
        }
        case AF_INET6: {
            struct sockaddr_in6 *sa = (struct sockaddr_in6*)bufinfo.buf;
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
            t->items[0] = MP_OBJ_NEW_SMALL_INT(AF_INET6);
            t->items[1] = mp_obj_new_bytes((byte*)&sa->sin6_addr, sizeof(sa->sin6_addr));
            t->items[2] = MP_OBJ_NEW_SMALL_INT(ntohs(sa->sin6_port));
            t->items[3] = MP_OBJ_NEW_SMALL_INT(ntohl(sa->sin6_flowinfo));
            t->items[4] = MP_OBJ_NEW_SMALL_INT(ntohl(sa->sin6_scope_id));
            return MP_OBJ_FROM_PTR(t);
        }
        default: {
            struct sockaddr *sa = (struct sockaddr*)bufinfo.buf;
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
            t->items[0] = MP_OBJ_NEW_SMALL_INT(sa->sa_family);
            t->items[1] = mp_obj_new_bytes((byte*)sa->sa_data, bufinfo.len - offsetof(struct sockaddr, sa_data));
            return MP_OBJ_FROM_PTR(t);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_socket_sockaddr_obj, mod_socket_sockaddr);

STATIC const mp_rom_map_elem_t mp_module_socket_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usocket) },
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&mp_type_socket) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&mod_socket_getaddrinfo_obj) },
    { MP_ROM_QSTR(MP_QSTR_inet_pton), MP_ROM_PTR(&mod_socket_inet_pton_obj) },
    { MP_ROM_QSTR(MP_QSTR_inet_ntop), MP_ROM_PTR(&mod_socket_inet_ntop_obj) },
    { MP_ROM_QSTR(MP_QSTR_sockaddr), MP_ROM_PTR(&mod_socket_sockaddr_obj) },

#define C(name) { MP_ROM_QSTR(MP_QSTR_ ## name), MP_ROM_INT(name) }
    C(AF_UNIX),
    C(AF_INET),
    C(AF_INET6),
    C(SOCK_STREAM),
    C(SOCK_DGRAM),
    C(SOCK_RAW),

    C(MSG_DONTROUTE),
    C(MSG_DONTWAIT),

    C(SOL_SOCKET),
    C(SO_BROADCAST),
    C(SO_ERROR),
    C(SO_KEEPALIVE),
    C(SO_LINGER),
    C(SO_REUSEADDR),
#undef C
};

STATIC MP_DEFINE_CONST_DICT(mp_module_socket_globals, mp_module_socket_globals_table);

const mp_obj_module_t mp_module_socket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_socket_globals,
};
