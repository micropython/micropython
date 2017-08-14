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
#include <string.h>

#include "py/nlr.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "netutils.h"
#include "modnetwork.h"

#if MICROPY_PY_USOCKET

/******************************************************************************/
// socket class

STATIC const mp_obj_type_t socket_type;

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
            if (n_args >= 4) {
                s->u_param.fileno = mp_obj_get_int(args[3]);
            }
        }
    }

    return s;
}

STATIC void socket_select_nic(mod_network_socket_obj_t *self, const byte *ip) {
    if (self->nic == MP_OBJ_NULL) {
        // select NIC based on IP
        self->nic = mod_network_find_nic(ip);
        self->nic_type = (mod_network_nic_type_t*)mp_obj_get_type(self->nic);

        // call the NIC to open the socket
        int _errno;
        if (self->nic_type->socket(self, &_errno) != 0) {
            mp_raise_OSError(_errno);
        }
    }
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

// method socket.bind(address)
STATIC mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    mod_network_socket_obj_t *self = self_in;

    // get address
    uint8_t ip[MOD_NETWORK_IPADDR_BUF_SIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_BIG);

    // check if we need to select a NIC
    socket_select_nic(self, ip);

    // call the NIC to bind the socket
    int _errno;
    if (self->nic_type->bind(self, ip, port, &_errno) != 0) {
        mp_raise_OSError(_errno);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

// method socket.listen(backlog)
STATIC mp_obj_t socket_listen(mp_obj_t self_in, mp_obj_t backlog) {
    mod_network_socket_obj_t *self = self_in;

    if (self->nic == MP_OBJ_NULL) {
        // not connected
        // TODO I think we can listen even if not bound...
        mp_raise_OSError(MP_ENOTCONN);
    }

    int _errno;
    if (self->nic_type->listen(self, mp_obj_get_int(backlog), &_errno) != 0) {
        mp_raise_OSError(_errno);
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
    int _errno;
    if (self->nic_type->accept(self, socket2, ip, &port, &_errno) != 0) {
        mp_raise_OSError(_errno);
    }

    // new socket has valid state, so set the NIC to the same as parent
    socket2->nic = self->nic;
    socket2->nic_type = self->nic_type;

    // make the return value
    mp_obj_tuple_t *client = mp_obj_new_tuple(2, NULL);
    client->items[0] = socket2;
    client->items[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);

    return client;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

// method socket.connect(address)
STATIC mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    mod_network_socket_obj_t *self = self_in;

    // get address
    uint8_t ip[MOD_NETWORK_IPADDR_BUF_SIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_BIG);

    // check if we need to select a NIC
    socket_select_nic(self, ip);

    // call the NIC to connect the socket
    int _errno;
    if (self->nic_type->connect(self, ip, port, &_errno) != 0) {
        mp_raise_OSError(_errno);
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
        mp_raise_OSError(_errno);
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
    mp_uint_t ret = self->nic_type->recv(self, (byte*)vstr.buf, len, &_errno);
    if (ret == -1) {
        mp_raise_OSError(_errno);
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
    uint8_t ip[MOD_NETWORK_IPADDR_BUF_SIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_BIG);

    // check if we need to select a NIC
    socket_select_nic(self, ip);

    // call the NIC to sendto
    int _errno;
    mp_int_t ret = self->nic_type->sendto(self, bufinfo.buf, bufinfo.len, ip, port, &_errno);
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
    byte ip[4];
    mp_uint_t port;
    int _errno;
    mp_int_t ret = self->nic_type->recvfrom(self, (byte*)vstr.buf, vstr.len, ip, &port, &_errno);
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
    tuple[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recvfrom_obj, socket_recvfrom);

// method socket.setsockopt(level, optname, value)
STATIC mp_obj_t socket_setsockopt(mp_uint_t n_args, const mp_obj_t *args) {
    mod_network_socket_obj_t *self = args[0];

    mp_int_t level = mp_obj_get_int(args[1]);
    mp_int_t opt = mp_obj_get_int(args[2]);

    const void *optval;
    mp_uint_t optlen;
    mp_int_t val;
    if (mp_obj_is_integer(args[3])) {
        val = mp_obj_get_int_truncated(args[3]);
        optval = &val;
        optlen = sizeof(val);
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
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendto), (mp_obj_t)&socket_sendto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recvfrom), (mp_obj_t)&socket_recvfrom_obj },
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

// function usocket.getaddrinfo(host, port)
STATIC mp_obj_t mod_usocket_getaddrinfo(mp_obj_t host_in, mp_obj_t port_in) {
    mp_uint_t hlen;
    const char *host = mp_obj_str_get_data(host_in, &hlen);
    mp_int_t port = mp_obj_get_int(port_in);

    // find a NIC that can do a name lookup
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        mod_network_nic_type_t *nic_type = (mod_network_nic_type_t*)mp_obj_get_type(nic);
        if (nic_type->gethostbyname != NULL) {
            uint8_t out_ip[MOD_NETWORK_IPADDR_BUF_SIZE];
            int ret = nic_type->gethostbyname(nic, host, hlen, out_ip);
            if (ret != 0) {
                // TODO CPython raises: socket.gaierror: [Errno -2] Name or service not known
                mp_raise_OSError(ret);
            }
            mp_obj_tuple_t *tuple = mp_obj_new_tuple(5, NULL);
            tuple->items[0] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET);
            tuple->items[1] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM);
            tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
            tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
            tuple->items[4] = netutils_format_inet_addr(out_ip, port, NETUTILS_BIG);
            return mp_obj_new_list(1, (mp_obj_t*)&tuple);
        }
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "no available NIC"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_usocket_getaddrinfo_obj, mod_usocket_getaddrinfo);

STATIC const mp_map_elem_t mp_module_usocket_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_usocket) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&socket_type },
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

STATIC MP_DEFINE_CONST_DICT(mp_module_usocket_globals, mp_module_usocket_globals_table);

const mp_obj_module_t mp_module_usocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_usocket_globals,
};

#endif  // MICROPY_PY_USOCKET
