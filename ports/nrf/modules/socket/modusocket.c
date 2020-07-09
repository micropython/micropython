/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 * Copyright (c) 2019 Glenn Ruben Bakke
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

#include "py/nlr.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/stream.h"

#if MICROPY_PY_USOCKET

#include "modnetwork.h"
#include "nrf_socket.h"
#include "nrf_errno.h"

/******************************************************************************/
// socket class

STATIC const mp_obj_type_t socket_type;

static void raise_os_errno(int errno_val) {
    switch (errno_val) {
        case NRF_EPERM:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EPERM: 1"));
            break;
        case NRF_ENOENT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENOENT: 2"));
            break;
        case NRF_EIO:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EIO: 5"));
            break;
        case NRF_ENOEXEC:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENOEXEC: 8"));
            break;
        case NRF_EBADF:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EBADF: 9"));
            break;
        case NRF_ENOMEM:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENOMEM: 12"));
            break;
        case NRF_EACCES:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EACCES: 13"));
            break;
        case NRF_EFAULT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EFAULT: 14"));
            break;
        case NRF_EINVAL:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EINVAL: 22"));
            break;
        case NRF_EMFILE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EMFILE: 24"));
            break;
        case NRF_ENOSPC:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENOSPC: 28"));
            break;
        case NRF_EAGAIN:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EAGAIN: 35"));
            break;
        case NRF_EMSGSIZE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EBADF: 40"));
            break;
        case NRF_EPROTOTYPE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EPROTOTYPE: 41"));
            break;
        case NRF_ENOPROTOOPT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENOPROTOOPT: 42"));
            break;
        case NRF_EPROTONOSUPPORT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EPROTONOSUPPORT: 43"));
            break;
        case NRF_ESOCKTNOSUPPORT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EBADF: 44"));
            break;
        case NRF_EOPNOTSUPP:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EOPNOTSUPP: 45"));
            break;
        case NRF_EAFNOSUPPORT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EAFNOSUPPORT: 47"));
            break;
        case NRF_EADDRINUSE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EADDRINUSE: 48"));
            break;
        case NRF_ENETDOWN:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENETDOWN: 50"));
            break;
        case NRF_ENETUNREACH:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENETUNREACH: 51"));
            break;
        case NRF_ENETRESET:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENETRESET: 52"));
            break;
        case NRF_ECONNRESET:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ECONNRESET: 54"));
            break;
        case NRF_EISCONN:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EISCONN: 56"));
            break;
        case NRF_ENOTCONN:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENOTCONN: 57"));
            break;
        case NRF_ETIMEDOUT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ETIMEDOUT: 60"));
            break;
        case NRF_ENOBUFS:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENOBUFS: 105"));
            break;
        case NRF_EHOSTDOWN:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EHOSTDOWN: 112"));
            break;
        case NRF_EALREADY:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EALREADY: 114"));
            break;
        case NRF_EINPROGRESS:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EINPROGRESS: 115"));
            break;
        case NRF_ECANCELED:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ECANCELED: 125"));
            break;
        case NRF_ENOKEY:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_ENOKEY: 126"));
            break;
        case NRF_EKEYEXPIRED:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EKEYEXPIRED: 127"));
            break;
        case NRF_EKEYREVOKED:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EKEYREVOKED: 128"));
            break;
        case NRF_EKEYREJECTED:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("NRF_EKEYREJECTED: 129"));
            break;
        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("NRF_ERROR UNKNOWN (%d)"), errno_val);
            break;
    }
}

STATIC void socket_select_nic(mod_network_socket_obj_t *self, const byte *ip) {
    if (self->nic == MP_OBJ_NULL) {
        // select NIC based on IP
        self->nic = mod_network_find_nic(ip);
        self->nic_type = (mod_network_nic_type_t *)mp_obj_get_type(self->nic);

        // call the NIC to open the socket
        int _errno;
        if (self->nic_type->socket(self, &_errno) != 0) {
            mp_raise_OSError(_errno);
        }
    }
}

// constructor socket(family=AF_INET, type=SOCK_STREAM, proto=0, fileno=None)
STATIC mp_obj_t socket_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, false);

    // create socket object (not bound to any NIC yet)
    mod_network_socket_obj_t *s = m_new_obj_with_finaliser(mod_network_socket_obj_t);
    s->base.type = (mp_obj_t)&socket_type;
    s->nic = MP_OBJ_NULL;
    s->nic_type = NULL;
    s->u_param.domain = MOD_NETWORK_AF_INET;
    s->u_param.type = MOD_NETWORK_SOCK_STREAM;
    s->u_param.fileno = -1;
    if (n_args >= 1) {
        s->u_param.domain = mp_obj_get_int(args[0]);
        if (n_args >= 2) {
            s->u_param.type = mp_obj_get_int(args[1]);
            if (n_args >= 3) {
                s->u_param.proto = mp_obj_get_int(args[2]);
                if (n_args >= 4) {
                    s->u_param.fileno = mp_obj_get_int(args[3]);
                }
            }
        }
    }

    if (s->u_param.fileno == -1) {
        // check if we need to select a NIC
        socket_select_nic(s, NULL);
    }

    return s;
}

// method socket.close()
STATIC mp_obj_t socket_close(mp_obj_t self_in) {
    mod_network_socket_obj_t *self = self_in;
    if (self->nic != MP_OBJ_NULL) {
        self->nic_type->close(self);
        self->nic = MP_OBJ_NULL;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_close_obj, socket_close);

static mp_uint_t parse_ip_addr(mp_uint_t family_in, mp_obj_t addr_in, void *addr_out) {

    mp_obj_t *items;
    mp_obj_get_array_fixed_n(addr_in, 2, &items);

    mp_uint_t addr_len;
    const char *addr_str = mp_obj_str_get_data(items[0], &addr_len);

    nrf_inet_pton(family_in, addr_str, addr_out);

    return mp_obj_get_int(items[1]);
}

static mp_obj_t format_ip_addr(mp_uint_t family_in, void *addr_in, mp_uint_t port_in) {
    char addr_str[NRF_INET6_ADDRSTRLEN];
    const char *result = nrf_inet_ntop(family_in, addr_in, addr_str, sizeof(addr_str));

    mp_obj_t tuple[2] = {
        tuple[0] = mp_obj_new_str(result, strlen(result)),
        tuple[1] = mp_obj_new_int(port_in),
    };

    return mp_obj_new_tuple(2, tuple);
}

// method socket.bind(address)
STATIC mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    mod_network_socket_obj_t *self = self_in;

    byte addr[NRF_INET6_ADDRSTRLEN];
    mp_uint_t port = parse_ip_addr(self->u_param.domain, addr_in, addr);

    // call the NIC to bind the socket
    int _errno;
    if (self->nic_type->bind(self, (uint8_t *)&addr, port, &_errno) != 0) {
        raise_os_errno(_errno);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

// method socket.listen(backlog)
STATIC mp_obj_t socket_listen(mp_obj_t self_in, mp_obj_t backlog) {
    mod_network_socket_obj_t *self = self_in;

    if (self->nic == MP_OBJ_NULL) {
        // not connected
        // TODO Coult we listen even if not bound...
        // mp_raise_OSError(MP_ENOTCONN);
    }

    int _errno;
    if (self->nic_type->listen(self, mp_obj_get_int(backlog), &_errno) != 0) {
        raise_os_errno(_errno);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_listen_obj, socket_listen);

// method socket.accept()
STATIC mp_obj_t socket_accept(mp_obj_t self_in) {
    mod_network_socket_obj_t *self = self_in;

    // create new socket object
    // starts with empty NIC so that finaliser doesn't run close() method if accept() fails
    mod_network_socket_obj_t *socket2 = m_new_obj_with_finaliser(mod_network_socket_obj_t);
    socket2->base.type = (mp_obj_t)&socket_type;
    socket2->nic = MP_OBJ_NULL;
    socket2->nic_type = NULL;

    // accept incoming connection
    uint8_t ip[MOD_NETWORK_IPADDR_BUF_SIZE];
    mp_uint_t port;

    int _errno = 0;
    if (self->nic_type->accept(self, socket2, ip, &port, &_errno) < 0) {
        raise_os_errno(_errno);
    }

    // new socket has valid state, so set the NIC to the same as parent
    socket2->nic = self->nic;
    socket2->nic_type = self->nic_type;

    // make the return value
    mp_obj_tuple_t *client = mp_obj_new_tuple(2, NULL);
    client->items[0] = socket2;
    client->items[1] = format_ip_addr(self->u_param.domain, ip, port);

    return client;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

// method socket.connect(address)
STATIC mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    mod_network_socket_obj_t *self = self_in;
    byte *p_address = NULL;
    byte addr[MOD_NETWORK_IPADDR_BUF_SIZE];

    mp_uint_t port_or_len = 0;
    // if string, its a character array.
    if (mp_obj_is_str(addr_in)) {
        GET_STR_DATA_LEN(addr_in, str_data, str_len);

        p_address = (byte *)str_data;
        port_or_len = str_len;
    } else {
        // else if tupple, its an address.

        // get address
        port_or_len = parse_ip_addr(self->u_param.domain, addr_in, addr);
        p_address = addr;
    }

    // call the NIC to connect the socket
    int _errno;
    if (self->nic_type->connect(self, p_address, port_or_len, &_errno) != 0) {
        raise_os_errno(_errno);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

// method socket.send(bytes)
STATIC mp_obj_t socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    mod_network_socket_obj_t *self = self_in;
    if (self->nic == MP_OBJ_NULL) {
        // not connected
        mp_raise_OSError(MP_EPIPE);
    }
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    int _errno;
    mp_uint_t ret = self->nic_type->send(self, bufinfo.buf, bufinfo.len, &_errno);
    if (ret == -1) {
        raise_os_errno(_errno);
    }
    return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, socket_send);

// method socket.recv(bufsize)
STATIC mp_obj_t socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    mod_network_socket_obj_t *self = self_in;
    if (self->nic == MP_OBJ_NULL) {
        // not connected
        mp_raise_OSError(MP_ENOTCONN);
    }
    mp_int_t len = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, len);
    int _errno;
    mp_uint_t ret = self->nic_type->recv(self, (byte *)vstr.buf, len, &_errno);
    if (ret == -1) {
        raise_os_errno(_errno);
    }
    if (ret == 0) {
        return mp_const_empty_bytes;
    }
    vstr.len = ret;
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recv_obj, socket_recv);

// method socket.sendto(bytes, address)
STATIC mp_obj_t socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    mod_network_socket_obj_t *self = self_in;

    // get the data
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    // get address
    byte addr[MOD_NETWORK_IPADDR_BUF_SIZE];
    mp_uint_t port = parse_ip_addr(self->u_param.domain, addr_in, addr);

    // check if we need to select a NIC
    socket_select_nic(self, addr);

    // call the NIC to sendto
    int _errno;
    mp_int_t ret = self->nic_type->sendto(self, bufinfo.buf, bufinfo.len, addr, port, &_errno);
    if (ret == -1) {
        mp_raise_OSError(_errno);
    }

    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(socket_sendto_obj, socket_sendto);

// method socket.recvfrom(bufsize)
STATIC mp_obj_t socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    mod_network_socket_obj_t *self = self_in;
    if (self->nic == MP_OBJ_NULL) {
        // not connected
        mp_raise_OSError(MP_ENOTCONN);
    }
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(len_in));
    byte addr[MOD_NETWORK_IPADDR_BUF_SIZE];
    mp_uint_t port;
    int _errno;
    mp_int_t ret = self->nic_type->recvfrom(self, (byte *)vstr.buf, vstr.len, addr, &port, &_errno);
    if (ret == -1) {
        mp_raise_OSError(_errno);
    }
    mp_obj_t tuple[2];
    if (ret == 0) {
        tuple[0] = mp_const_empty_bytes;
    } else {
        vstr.len = ret;
        tuple[0] = mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }

    tuple[1] = format_ip_addr(self->u_param.domain, addr, port);

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recvfrom_obj, socket_recvfrom);

// method socket.getsockopt(level, optname, value)
STATIC mp_obj_t socket_getsockopt(mp_uint_t n_args, const mp_obj_t *args) {
    mod_network_socket_obj_t *self = args[0];
    if (self->nic == MP_OBJ_NULL) {
        // not connected
        mp_raise_OSError(MP_ENOTCONN);
    }

    mp_int_t level = mp_obj_get_int(args[1]);
    mp_int_t opt = mp_obj_get_int(args[2]);
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(args[3]));

    int _errno;
    // TODO: figure out if &vstr.len can be passed like this ...
    if (self->nic_type->getsockopt(self, level, opt, vstr.buf, &vstr.len, &_errno) != 0) {
        mp_raise_OSError(_errno);
    }

    if (vstr.len == 0) {
        return mp_const_none;
    }

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_getsockopt_obj, 4, 4, socket_getsockopt);

// method socket.setsockopt(level, optname, value)
STATIC mp_obj_t socket_setsockopt(mp_uint_t n_args, const mp_obj_t *args) {
    mod_network_socket_obj_t *self = args[0];
    if (self->nic == MP_OBJ_NULL) {
        // not connected
        mp_raise_OSError(MP_ENOTCONN);
    }

    mp_int_t level = mp_obj_get_int(args[1]);
    mp_int_t opt = mp_obj_get_int(args[2]);

    const void *optval;
    mp_uint_t optlen;
    mp_int_t val;
    if (mp_obj_is_integer(args[3])) {
        val = mp_obj_get_int_truncated(args[3]);
        optval = &val;
        optlen = sizeof(val);
    } else if (args[3] == mp_const_none) {
        optval = NULL;
        optlen = 0;
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
        optval = bufinfo.buf;
        optlen = bufinfo.len;
    }

    int _errno;
    if (self->nic_type->setsockopt(self, level, opt, optval, optlen, &_errno) != 0) {
        mp_raise_OSError(_errno);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_setsockopt_obj, 4, 4, socket_setsockopt);

// method socket.settimeout(value)
// timeout=0 means non-blocking
// timeout=None means blocking
// otherwise, timeout is in seconds
STATIC mp_obj_t socket_settimeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    mod_network_socket_obj_t *self = self_in;
    if (self->nic == MP_OBJ_NULL) {
        // not connected
        mp_raise_OSError(MP_ENOTCONN);
    }
    mp_uint_t timeout;
    if (timeout_in == mp_const_none) {
        timeout = -1;
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT
        timeout = 1000 * mp_obj_get_float(timeout_in);
        #else
        timeout = 1000 * mp_obj_get_int(timeout_in);
        #endif
    }
    int _errno;
    if (self->nic_type->settimeout(self, timeout, &_errno) != 0) {
        mp_raise_OSError(_errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_settimeout_obj, socket_settimeout);

// method socket.setblocking(flag)
STATIC mp_obj_t socket_setblocking(mp_obj_t self_in, mp_obj_t blocking) {
    if (mp_obj_is_true(blocking)) {
        return socket_settimeout(self_in, mp_const_none);
    } else {
        return socket_settimeout(self_in, MP_OBJ_NEW_SMALL_INT(0));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_setblocking_obj, socket_setblocking);

STATIC const mp_map_elem_t socket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind), (mp_obj_t)&socket_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listen), (mp_obj_t)&socket_listen_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accept), (mp_obj_t)&socket_accept_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&socket_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&socket_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&socket_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&socket_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&socket_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendto), (mp_obj_t)&socket_sendto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recvfrom), (mp_obj_t)&socket_recvfrom_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getsockopt), (mp_obj_t)&socket_getsockopt_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setsockopt), (mp_obj_t)&socket_setsockopt_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_settimeout), (mp_obj_t)&socket_settimeout_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setblocking), (mp_obj_t)&socket_setblocking_obj },
};

STATIC MP_DEFINE_CONST_DICT(socket_locals_dict, socket_locals_dict_table);

mp_uint_t socket_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    mod_network_socket_obj_t *self = self_in;
    return self->nic_type->ioctl(self, request, arg, errcode);
}

STATIC const mp_stream_p_t socket_stream_p = {
    .ioctl = socket_ioctl,
    .is_text = false,
};

STATIC const mp_obj_type_t socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .make_new = socket_make_new,
    .protocol = &socket_stream_p,
    .locals_dict = (mp_obj_t)&socket_locals_dict,
};

/******************************************************************************/
// usocket module

// function usocket.getaddrinfo(host, port, [interface_name])
STATIC mp_obj_t mod_usocket_getaddrinfo(mp_uint_t n_args, const mp_obj_t *args) {
    mp_uint_t hlen;
    const char *host = mp_obj_str_get_data(args[0], &hlen);
    mp_int_t port = mp_obj_get_int(args[1]);

    mp_uint_t interface_len = 0;
    const char *interface = NULL;
    if ((n_args == 3) && mp_obj_is_str(args[2])) {
        interface = mp_obj_str_get_data(args[2], &interface_len);
    }
    // find a NIC that can do a name lookup
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        mod_network_nic_type_t *nic_type = (mod_network_nic_type_t *)mp_obj_get_type(nic);
        if (nic_type->gethostbyname != NULL) {
            byte ipv4_out_ip[MOD_NETWORK_IPADDR_BUF_SIZE];
            uint8_t ipv4_out_family = MOD_NETWORK_AF_INET;
            uint8_t ipv4_out_proto = 0;
            int result_ipv4 = -1;

            mp_obj_t ret_list = mp_obj_new_list(0, NULL);

            if ((result_ipv4 = nic_type->gethostbyname(nic, host, hlen, interface, interface_len, ipv4_out_ip, &ipv4_out_family, &ipv4_out_proto)) == 0) {
                mp_obj_tuple_t *tuple = mp_obj_new_tuple(5, NULL);
                tuple->items[0] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET);
                tuple->items[1] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM);
                tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
                tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
                tuple->items[4] = format_ip_addr(ipv4_out_family, ipv4_out_ip, port);
                mp_obj_list_append(ret_list, tuple);
            }

            byte ipv6_out_ip[MOD_NETWORK_IPADDR_BUF_SIZE];
            uint8_t ipv6_out_family = MOD_NETWORK_AF_INET6;
            uint8_t ipv6_out_proto = 0;
            int result_ipv6 = -1;
            if ((result_ipv6 = nic_type->gethostbyname(nic, host, hlen, interface, interface_len, ipv6_out_ip, &ipv6_out_family, &ipv6_out_proto)) == 0) {
                mp_obj_tuple_t *tuple = mp_obj_new_tuple(5, NULL);
                tuple->items[0] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET6);
                tuple->items[1] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM);
                tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
                tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
                tuple->items[4] = format_ip_addr(ipv6_out_family, ipv6_out_ip, port);
                mp_obj_list_append(ret_list, tuple);
            }

            if ((result_ipv4 != 0) && (result_ipv6 != 0)) {
                // TODO CPython raises: socket.gaierror: [Errno -2] Name or service not known
                mp_raise_OSError(98); // EADDRNOTAVAIL
            }

            return ret_list;
        }
    }

    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("no available NIC"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_usocket_getaddrinfo_obj, 2, 3, mod_usocket_getaddrinfo);

STATIC const mp_map_elem_t mp_module_usocket_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_usocket) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&socket_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getaddrinfo), (mp_obj_t)&mod_usocket_getaddrinfo_obj },

    // class constants

    // Socket levels.
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_LOCAL), MP_OBJ_NEW_SMALL_INT(NRF_AF_LOCAL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_PACKET), MP_OBJ_NEW_SMALL_INT(NRF_AF_PACKET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET6), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_LTE), MP_OBJ_NEW_SMALL_INT(NRF_AF_LTE) },

    // Socket types.
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_STREAM), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_DGRAM), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_DGRAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_RAW), MP_OBJ_NEW_SMALL_INT(NRF_SOCK_RAW) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_MGMT), MP_OBJ_NEW_SMALL_INT(NRF_SOCK_MGMT) },

    // Socket protocols.
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_TCP), MP_OBJ_NEW_SMALL_INT(NRF_IPPROTO_TCP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_UDP), MP_OBJ_NEW_SMALL_INT(NRF_IPPROTO_UDP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PROTO_AT), MP_OBJ_NEW_SMALL_INT(NRF_PROTO_AT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PROTO_PDN), MP_OBJ_NEW_SMALL_INT(NRF_PROTO_PDN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PROTO_DFU), MP_OBJ_NEW_SMALL_INT(NRF_PROTO_DFU) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PROTO_GNSS), MP_OBJ_NEW_SMALL_INT(NRF_PROTO_GNSS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPROTO_TLS1v2), MP_OBJ_NEW_SMALL_INT(NRF_SPROTO_TLS1v2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPROTO_TLS1v3), MP_OBJ_NEW_SMALL_INT(NRF_SPROTO_TLS1v3) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPROTO_DTLS1v2), MP_OBJ_NEW_SMALL_INT(NRF_SPROTO_DTLS1v2) },

    // Socket options levels.
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOL_SOCKET), MP_OBJ_NEW_SMALL_INT(NRF_SOL_SOCKET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOL_SECURE), MP_OBJ_NEW_SMALL_INT(NRF_SOL_SECURE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOL_PDN), MP_OBJ_NEW_SMALL_INT(NRF_SOL_PDN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOL_DFU), MP_OBJ_NEW_SMALL_INT(NRF_SOL_DFU) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOL_GNSS), MP_OBJ_NEW_SMALL_INT(NRF_SOL_GNSS) },

    // Socket options.

    // SOL_SOCKET.
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_ERROR), MP_OBJ_NEW_SMALL_INT(NRF_SO_ERROR) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_RCVTIMEO), MP_OBJ_NEW_SMALL_INT(NRF_SO_RCVTIMEO) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SNDTIMEO), MP_OBJ_NEW_SMALL_INT(NRF_SO_SNDTIMEO) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_BINDTODEVICE), MP_OBJ_NEW_SMALL_INT(NRF_SO_BINDTODEVICE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SILENCE_ALL), MP_OBJ_NEW_SMALL_INT(NRF_SO_SILENCE_ALL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SILENCE_IP_ECHO_REPLY), MP_OBJ_NEW_SMALL_INT(NRF_SO_SILENCE_IP_ECHO_REPLY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SILENCE_IPV6_ECHO_REPLY), MP_OBJ_NEW_SMALL_INT(NRF_SO_SILENCE_IPV6_ECHO_REPLY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_REUSEADDR), MP_OBJ_NEW_SMALL_INT(NRF_SO_REUSEADDR) },

    // SOL_SECURE.
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_ROLE), MP_OBJ_NEW_SMALL_INT(NRF_SO_SEC_ROLE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_TAG_LIST), MP_OBJ_NEW_SMALL_INT(NRF_SO_SEC_TAG_LIST) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_SESSION_CACHE), MP_OBJ_NEW_SMALL_INT(NRF_SO_SEC_SESSION_CACHE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_PEER_VERIFY), MP_OBJ_NEW_SMALL_INT(NRF_SO_SEC_PEER_VERIFY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_HOSTNAME), MP_OBJ_NEW_SMALL_INT(NRF_SO_HOSTNAME) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_CIPHERSUITE_LIST), MP_OBJ_NEW_SMALL_INT(NRF_SO_CIPHERSUITE_LIST) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_SEC_CIPHER_IN_USE), MP_OBJ_NEW_SMALL_INT(NRF_SO_CIPHER_IN_USE) },

    // SOL_PDN
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_PDN_AF), MP_OBJ_NEW_SMALL_INT(NRF_SO_PDN_AF) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_PDN_CONTEXT_ID), MP_OBJ_NEW_SMALL_INT(NRF_SO_PDN_CONTEXT_ID) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_PDN_STATE), MP_OBJ_NEW_SMALL_INT(NRF_SO_PDN_STATE) },

    // SOL_DFU
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_DFU_FW_VERSION), MP_OBJ_NEW_SMALL_INT(NRF_SO_DFU_FW_VERSION) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_DFU_RESOURCES), MP_OBJ_NEW_SMALL_INT(NRF_SO_DFU_RESOURCES) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_DFU_TIMEO), MP_OBJ_NEW_SMALL_INT(NRF_SO_DFU_TIMEO) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_DFU_APPLY), MP_OBJ_NEW_SMALL_INT(NRF_SO_DFU_APPLY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_DFU_REVERT), MP_OBJ_NEW_SMALL_INT(NRF_SO_DFU_REVERT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_DFU_BACKUP_DELETE), MP_OBJ_NEW_SMALL_INT(NRF_SO_DFU_BACKUP_DELETE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_DFU_OFFSET), MP_OBJ_NEW_SMALL_INT(NRF_SO_DFU_OFFSET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_DFU_ERROR), MP_OBJ_NEW_SMALL_INT(NRF_SO_DFU_ERROR) },

    // SOL_GNSS
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_FIX_INTERVAL), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_FIX_INTERVAL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_FIX_RETRY), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_FIX_RETRY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_SYSTEM), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_SYSTEM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_NMEA_MASK), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_NMEA_MASK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_ELEVATION_MASK), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_ELEVATION_MASK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_USE_CASE), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_USE_CASE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_START), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_START) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_STOP), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_STOP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_PSM), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_POWER_SAVE_MODE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_ENABLE_PRIORITY), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_ENABLE_PRIORITY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_GNSS_DISABLE_PRIORITY), MP_OBJ_NEW_SMALL_INT(NRF_SO_GNSS_DISABLE_PRIORITY) },

    // GNSS option value flags.
    { MP_OBJ_NEW_QSTR(MP_QSTR_GNSS_NMEA_CONFIG_GGA_FLAG), MP_OBJ_NEW_SMALL_INT(NRF_GNSS_NMEA_GGA_MASK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GNSS_NMEA_CONFIG_GLL_FLAG), MP_OBJ_NEW_SMALL_INT(NRF_GNSS_NMEA_GLL_MASK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GNSS_NMEA_CONFIG_GSA_FLAG), MP_OBJ_NEW_SMALL_INT(NRF_GNSS_NMEA_GSA_MASK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GNSS_NMEA_CONFIG_GSV_FLAG), MP_OBJ_NEW_SMALL_INT(NRF_GNSS_NMEA_GSV_MASK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GNSS_NMEA_CONFIG_RMC_FLAG), MP_OBJ_NEW_SMALL_INT(NRF_GNSS_NMEA_RMC_MASK) },

    // GNSS Power Saving Mode (PSM).
    { MP_OBJ_NEW_QSTR(MP_QSTR_GNSS_PSM_DISABLED), MP_OBJ_NEW_SMALL_INT(NRF_GNSS_PSM_DISABLED) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GNSS_PSM_DUTY_CYCLE_PERF), MP_OBJ_NEW_SMALL_INT(NRF_GNSS_PSM_DUTY_CYCLING_PERFORMANCE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GNSS_PSM_DUTY_CYCLE_POWER), MP_OBJ_NEW_SMALL_INT(NRF_GNSS_PSM_DUTY_CYCLING_POWER) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_usocket_globals, mp_module_usocket_globals_table);

const mp_obj_module_t mp_module_usocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_usocket_globals,
};

#endif  // MICROPY_PY_USOCKET
