/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>
#include <string.h>

#include "simplelink.h"
#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "lib/netutils/netutils.h"
#include "modnetwork.h"
#include "modusocket.h"
#include "mpexception.h"

/******************************************************************************/
// The following set of macros and functions provide a glue between the CC3100
// simplelink layer and the functions/methods provided by the usocket module.
// They were historically in a separate file because usocket was designed to
// work with multiple NICs, and the wlan_XXX functions just provided one
// particular NIC implementation (that of the CC3100).  But the CC3200 port only
// supports a single NIC (being the CC3100) so it's unnecessary and inefficient
// to provide an intermediate wrapper layer.  Hence the wlan_XXX functions
// are provided below as static functions so they can be inlined directly by
// the corresponding usocket calls.

#define WLAN_MAX_RX_SIZE                16000
#define WLAN_MAX_TX_SIZE                1476

#define MAKE_SOCKADDR(addr, ip, port)       SlSockAddr_t addr; \
                                            addr.sa_family = SL_AF_INET; \
                                            addr.sa_data[0] = port >> 8; \
                                            addr.sa_data[1] = port; \
                                            addr.sa_data[2] = ip[3]; \
                                            addr.sa_data[3] = ip[2]; \
                                            addr.sa_data[4] = ip[1]; \
                                            addr.sa_data[5] = ip[0];

#define UNPACK_SOCKADDR(addr, ip, port)     port = (addr.sa_data[0] << 8) | addr.sa_data[1]; \
                                            ip[0] = addr.sa_data[5]; \
                                            ip[1] = addr.sa_data[4]; \
                                            ip[2] = addr.sa_data[3]; \
                                            ip[3] = addr.sa_data[2];

#define SOCKET_TIMEOUT_QUANTA_MS (20)

STATIC int convert_sl_errno(int sl_errno) {
    return -sl_errno;
}

// This function is left as non-static so it's not inlined.
int check_timedout(mod_network_socket_obj_t *s, int ret, uint32_t *timeout_ms, int *_errno) {
    if (*timeout_ms == 0 || ret != SL_EAGAIN) {
        if (s->sock_base.timeout_ms > 0 && ret == SL_EAGAIN) {
            *_errno = MP_ETIMEDOUT;
        } else {
            *_errno = convert_sl_errno(ret);
        }
        return -1;
    }
    mp_hal_delay_ms(SOCKET_TIMEOUT_QUANTA_MS);
    if (*timeout_ms < SOCKET_TIMEOUT_QUANTA_MS) {
        *timeout_ms = 0;
    } else {
        *timeout_ms -= SOCKET_TIMEOUT_QUANTA_MS;
    }
    return 0;
}

STATIC int wlan_gethostbyname(const char *name, mp_uint_t len, uint8_t *out_ip, uint8_t family) {
    uint32_t ip;
    int result = sl_NetAppDnsGetHostByName((_i8 *)name, (_u16)len, (_u32*)&ip, (_u8)family);
    out_ip[0] = ip;
    out_ip[1] = ip >> 8;
    out_ip[2] = ip >> 16;
    out_ip[3] = ip >> 24;
    return result;
}

STATIC int wlan_socket_socket(mod_network_socket_obj_t *s, int *_errno) {
    int16_t sd = sl_Socket(s->sock_base.u_param.domain, s->sock_base.u_param.type, s->sock_base.u_param.proto);
    if (sd < 0) {
        *_errno = sd;
        return -1;
    }
    s->sock_base.sd = sd;
    return 0;
}

STATIC void wlan_socket_close(mod_network_socket_obj_t *s) {
    // this is to prevent the finalizer to close a socket that failed when being created
    if (s->sock_base.sd >= 0) {
        modusocket_socket_delete(s->sock_base.sd);
        sl_Close(s->sock_base.sd);
        s->sock_base.sd = -1;
    }
}

STATIC int wlan_socket_bind(mod_network_socket_obj_t *s, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_Bind(s->sock_base.sd, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int wlan_socket_listen(mod_network_socket_obj_t *s, mp_int_t backlog, int *_errno) {
    int ret = sl_Listen(s->sock_base.sd, backlog);
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int wlan_socket_accept(mod_network_socket_obj_t *s, mod_network_socket_obj_t *s2, byte *ip, mp_uint_t *port, int *_errno) {
    // accept incoming connection
    int16_t sd;
    SlSockAddr_t addr;
    SlSocklen_t addr_len = sizeof(addr);

    uint32_t timeout_ms = s->sock_base.timeout_ms;
    for (;;) {
        sd = sl_Accept(s->sock_base.sd, &addr, &addr_len);
        if (sd >= 0) {
            // save the socket descriptor
            s2->sock_base.sd = sd;
            // return ip and port
            UNPACK_SOCKADDR(addr, ip, *port);
            return 0;
        }
        if (check_timedout(s, sd, &timeout_ms, _errno)) {
            return -1;
        }
    }
}

STATIC int wlan_socket_connect(mod_network_socket_obj_t *s, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    uint32_t timeout_ms = s->sock_base.timeout_ms;

    // For a non-blocking connect the CC3100 will return SL_EALREADY while the
    // connection is in progress.

    for (;;) {
        int ret = sl_Connect(s->sock_base.sd, &addr, sizeof(addr));
        if (ret == 0) {
            return 0;
        }

        // Check if we are in non-blocking mode and the connection is in progress
        if (s->sock_base.timeout_ms == 0 && ret == SL_EALREADY) {
            // To match BSD we return EINPROGRESS here
            *_errno = MP_EINPROGRESS;
            return -1;
        }

        // We are in blocking mode, so if the connection isn't in progress then error out
        if (ret != SL_EALREADY) {
            *_errno = convert_sl_errno(ret);
            return -1;
        }

        if (check_timedout(s, SL_EAGAIN, &timeout_ms, _errno)) {
            return -1;
        }
    }
}

STATIC int wlan_socket_send(mod_network_socket_obj_t *s, const byte *buf, mp_uint_t len, int *_errno) {
    if (len == 0) {
        return 0;
    }
    uint32_t timeout_ms = s->sock_base.timeout_ms;
    for (;;) {
        int ret = sl_Send(s->sock_base.sd, (const void *)buf, len, 0);
        if (ret > 0) {
            return ret;
        }
        if (check_timedout(s, ret, &timeout_ms, _errno)) {
            return -1;
        }
    }
}

STATIC int wlan_socket_recv(mod_network_socket_obj_t *s, byte *buf, mp_uint_t len, int *_errno) {
    uint32_t timeout_ms = s->sock_base.timeout_ms;
    for (;;) {
        int ret = sl_Recv(s->sock_base.sd, buf, MIN(len, WLAN_MAX_RX_SIZE), 0);
        if (ret >= 0) {
            return ret;
        }
        if (check_timedout(s, ret, &timeout_ms, _errno)) {
            return -1;
        }
    }
}

STATIC int wlan_socket_sendto( mod_network_socket_obj_t *s, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    uint32_t timeout_ms = s->sock_base.timeout_ms;
    for (;;) {
        int ret = sl_SendTo(s->sock_base.sd, (byte*)buf, len, 0, (SlSockAddr_t*)&addr, sizeof(addr));
        if (ret >= 0) {
            return ret;
        }
        if (check_timedout(s, ret, &timeout_ms, _errno)) {
            return -1;
        }
    }
}

STATIC int wlan_socket_recvfrom(mod_network_socket_obj_t *s, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    SlSockAddr_t addr;
    SlSocklen_t addr_len = sizeof(addr);
    uint32_t timeout_ms = s->sock_base.timeout_ms;
    for (;;) {
        int ret = sl_RecvFrom(s->sock_base.sd, buf, MIN(len, WLAN_MAX_RX_SIZE), 0, &addr, &addr_len);
        if (ret >= 0) {
            UNPACK_SOCKADDR(addr, ip, *port);
            return ret;
        }
        if (check_timedout(s, ret, &timeout_ms, _errno)) {
            return -1;
        }
    }
}

STATIC int wlan_socket_setsockopt(mod_network_socket_obj_t *s, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    int ret = sl_SetSockOpt(s->sock_base.sd, level, opt, optval, optlen);
    if (ret < 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int wlan_socket_settimeout(mod_network_socket_obj_t *s, mp_uint_t timeout_s, int *_errno) {
    SlSockNonblocking_t option;
    if (timeout_s == 0 || timeout_s == -1) {
        if (timeout_s == 0) {
            // set non-blocking mode
            option.NonblockingEnabled = 1;
        } else {
            // set blocking mode
            option.NonblockingEnabled = 0;
        }
        timeout_s = 0;
    } else {
        // synthesize timeout via non-blocking behaviour with a loop
        option.NonblockingEnabled = 1;
    }

    int ret = sl_SetSockOpt(s->sock_base.sd, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &option, sizeof(option));
    if (ret != 0) {
        *_errno = convert_sl_errno(ret);
        return -1;
    }

    s->sock_base.timeout_ms = timeout_s * 1000;
    return 0;
}

STATIC int wlan_socket_ioctl (mod_network_socket_obj_t *s, mp_uint_t request, mp_uint_t arg, int *_errno) {
    mp_int_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        int32_t sd = s->sock_base.sd;

        // init fds
        SlFdSet_t rfds, wfds, xfds;
        SL_FD_ZERO(&rfds);
        SL_FD_ZERO(&wfds);
        SL_FD_ZERO(&xfds);

        // set fds if needed
        if (flags & MP_STREAM_POLL_RD) {
            SL_FD_SET(sd, &rfds);
        }
        if (flags & MP_STREAM_POLL_WR) {
            SL_FD_SET(sd, &wfds);
        }
        if (flags & MP_STREAM_POLL_HUP) {
            SL_FD_SET(sd, &xfds);
        }

        // call simplelink's select with minimum timeout
        SlTimeval_t tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1;
        int32_t nfds = sl_Select(sd + 1, &rfds, &wfds, &xfds, &tv);

        // check for errors
        if (nfds == -1) {
            *_errno = nfds;
            return -1;
        }

        // check return of select
        if (SL_FD_ISSET(sd, &rfds)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if (SL_FD_ISSET(sd, &wfds)) {
            ret |= MP_STREAM_POLL_WR;
        }
        if (SL_FD_ISSET(sd, &xfds)) {
            ret |= MP_STREAM_POLL_HUP;
        }
    } else {
        *_errno = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

/******************************************************************************
 DEFINE PRIVATE CONSTANTS
 ******************************************************************************/
#define MOD_NETWORK_MAX_SOCKETS                 10

/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/
typedef struct {
    int16_t sd;
    bool    user;
} modusocket_sock_t;

/******************************************************************************
 DEFINE PRIVATE DATA
 ******************************************************************************/
STATIC const mp_obj_type_t socket_type;
STATIC OsiLockObj_t modusocket_LockObj;
STATIC modusocket_sock_t modusocket_sockets[MOD_NETWORK_MAX_SOCKETS] = {{.sd = -1}, {.sd = -1}, {.sd = -1}, {.sd = -1}, {.sd = -1},
                                                                        {.sd = -1}, {.sd = -1}, {.sd = -1}, {.sd = -1}, {.sd = -1}};

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
__attribute__ ((section (".boot")))
void modusocket_pre_init (void) {
    // create the wlan lock
    ASSERT(OSI_OK == sl_LockObjCreate(&modusocket_LockObj, "SockLock"));
    sl_LockObjUnlock (&modusocket_LockObj);
}

void modusocket_socket_add (int16_t sd, bool user) {
    sl_LockObjLock (&modusocket_LockObj, SL_OS_WAIT_FOREVER);
    for (int i = 0; i < MOD_NETWORK_MAX_SOCKETS; i++) {
        if (modusocket_sockets[i].sd < 0) {
            modusocket_sockets[i].sd = sd;
            modusocket_sockets[i].user = user;
            break;
        }
    }
    sl_LockObjUnlock (&modusocket_LockObj);
}

void modusocket_socket_delete (int16_t sd) {
    sl_LockObjLock (&modusocket_LockObj, SL_OS_WAIT_FOREVER);
    for (int i = 0; i < MOD_NETWORK_MAX_SOCKETS; i++) {
        if (modusocket_sockets[i].sd == sd) {
            modusocket_sockets[i].sd = -1;
            break;
        }
    }
    sl_LockObjUnlock (&modusocket_LockObj);
}

void modusocket_enter_sleep (void) {
    SlFdSet_t socketset;
    int16_t maxfd = 0;

    for (int i = 0; i < MOD_NETWORK_MAX_SOCKETS; i++) {
        int16_t sd;
        if ((sd = modusocket_sockets[i].sd) >= 0) {
            SL_FD_SET(sd, &socketset);
            maxfd = (maxfd > sd) ? maxfd : sd;
        }
    }

    if (maxfd > 0) {
        // wait for any of the sockets to become ready...
        sl_Select(maxfd + 1, &socketset, NULL, NULL, NULL);
    }
}

void modusocket_close_all_user_sockets (void) {
    sl_LockObjLock (&modusocket_LockObj, SL_OS_WAIT_FOREVER);
    for (int i = 0; i < MOD_NETWORK_MAX_SOCKETS; i++) {
        if (modusocket_sockets[i].sd >= 0 && modusocket_sockets[i].user) {
            sl_Close(modusocket_sockets[i].sd);
            modusocket_sockets[i].sd = -1;
        }
    }
    sl_LockObjUnlock (&modusocket_LockObj);
}

/******************************************************************************/
// socket class

// constructor socket(family=AF_INET, type=SOCK_STREAM, proto=IPPROTO_TCP, fileno=None)
STATIC mp_obj_t socket_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, false);

    // create socket object
    mod_network_socket_obj_t *s = m_new_obj_with_finaliser(mod_network_socket_obj_t);
    s->base.type = (mp_obj_t)&socket_type;
    s->sock_base.u_param.domain = SL_AF_INET;
    s->sock_base.u_param.type = SL_SOCK_STREAM;
    s->sock_base.u_param.proto = SL_IPPROTO_TCP;
    s->sock_base.u_param.fileno = -1;
    s->sock_base.timeout_ms = 0;
    s->sock_base.cert_req = false;

    if (n_args > 0) {
        s->sock_base.u_param.domain = mp_obj_get_int(args[0]);
        if (n_args > 1) {
            s->sock_base.u_param.type = mp_obj_get_int(args[1]);
            if (n_args > 2) {
                s->sock_base.u_param.proto = mp_obj_get_int(args[2]);
                if (n_args > 3) {
                    s->sock_base.u_param.fileno = mp_obj_get_int(args[3]);
                }
            }
        }
    }

    // create the socket
    int _errno;
    if (wlan_socket_socket(s, &_errno) != 0) {
        mp_raise_OSError(-_errno);
    }
    // add the socket to the list
    modusocket_socket_add(s->sock_base.sd, true);
    return s;
}

// method socket.close()
STATIC mp_obj_t socket_close(mp_obj_t self_in) {
    mod_network_socket_obj_t *self = self_in;
    wlan_socket_close(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_close_obj, socket_close);

// method socket.bind(address)
STATIC mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    mod_network_socket_obj_t *self = self_in;

    // get address
    uint8_t ip[MOD_NETWORK_IPV4ADDR_BUF_SIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_LITTLE);

    // call the NIC to bind the socket
    int _errno = 0;
    if (wlan_socket_bind(self, ip, port, &_errno) != 0) {
        mp_raise_OSError(-_errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

// method socket.listen([backlog])
STATIC mp_obj_t socket_listen(mp_uint_t n_args, const mp_obj_t *args) {
    mod_network_socket_obj_t *self = args[0];

    int32_t backlog = 0;
    if (n_args > 1) {
        backlog = mp_obj_get_int(args[1]);
        backlog = (backlog < 0) ? 0 : backlog;
    }

    int _errno;
    if (wlan_socket_listen(self, backlog, &_errno) != 0) {
        mp_raise_OSError(-_errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_listen_obj, 1, 2, socket_listen);

// method socket.accept()
STATIC mp_obj_t socket_accept(mp_obj_t self_in) {
    mod_network_socket_obj_t *self = self_in;

    // create new socket object
    mod_network_socket_obj_t *socket2 = m_new_obj_with_finaliser(mod_network_socket_obj_t);
    // the new socket inherits all properties from its parent
    memcpy (socket2, self, sizeof(mod_network_socket_obj_t));

    // accept the incoming connection
    uint8_t ip[MOD_NETWORK_IPV4ADDR_BUF_SIZE];
    mp_uint_t port = 0;
    int _errno = 0;
    if (wlan_socket_accept(self, socket2, ip, &port, &_errno) != 0) {
        mp_raise_OSError(_errno);
    }

    // add the socket to the list
    modusocket_socket_add(socket2->sock_base.sd, true);

    // make the return value
    mp_obj_tuple_t *client = mp_obj_new_tuple(2, NULL);
    client->items[0] = socket2;
    client->items[1] = netutils_format_inet_addr(ip, port, NETUTILS_LITTLE);
    return client;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

// method socket.connect(address)
STATIC mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    mod_network_socket_obj_t *self = self_in;

    // get address
    uint8_t ip[MOD_NETWORK_IPV4ADDR_BUF_SIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_LITTLE);

    // connect the socket
    int _errno;
    if (wlan_socket_connect(self, ip, port, &_errno) != 0) {
        if (!self->sock_base.cert_req && _errno == SL_ESECSNOVERIFY) {
            return mp_const_none;
        }
        mp_raise_OSError(_errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

// method socket.send(bytes)
STATIC mp_obj_t socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    mod_network_socket_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    int _errno;
    mp_int_t ret = wlan_socket_send(self, bufinfo.buf, bufinfo.len, &_errno);
    if (ret < 0) {
        mp_raise_OSError(_errno);
    }
    return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, socket_send);

// method socket.recv(bufsize)
STATIC mp_obj_t socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    mod_network_socket_obj_t *self = self_in;
    mp_int_t len = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, len);
    int _errno;
    mp_int_t ret = wlan_socket_recv(self, (byte*)vstr.buf, len, &_errno);
    if (ret < 0) {
        mp_raise_OSError(_errno);
    }
    if (ret == 0) {
        return mp_const_empty_bytes;
    }
    vstr.len = ret;
    vstr.buf[vstr.len] = '\0';
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
    uint8_t ip[MOD_NETWORK_IPV4ADDR_BUF_SIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_LITTLE);

    // call the nic to sendto
    int _errno = 0;
    mp_int_t ret = wlan_socket_sendto(self, bufinfo.buf, bufinfo.len, ip, port, &_errno);
    if (ret < 0) {
        mp_raise_OSError(_errno);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(socket_sendto_obj, socket_sendto);

// method socket.recvfrom(bufsize)
STATIC mp_obj_t socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    mod_network_socket_obj_t *self = self_in;
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(len_in));
    byte ip[4];
    mp_uint_t port = 0;
    int _errno = 0;
    mp_int_t ret = wlan_socket_recvfrom(self, (byte*)vstr.buf, vstr.len, ip, &port, &_errno);
    if (ret < 0) {
        mp_raise_OSError(_errno);
    }
    mp_obj_t tuple[2];
    if (ret == 0) {
        tuple[0] = mp_const_empty_bytes;
    } else {
        vstr.len = ret;
        vstr.buf[vstr.len] = '\0';
        tuple[0] = mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
    tuple[1] = netutils_format_inet_addr(ip, port, NETUTILS_LITTLE);
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
    if (wlan_socket_setsockopt(self, level, opt, optval, optlen, &_errno) != 0) {
        mp_raise_OSError(-_errno);
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
    mp_uint_t timeout;
    if (timeout_in == mp_const_none) {
        timeout = -1;
    } else {
        timeout = mp_obj_get_int(timeout_in);
    }
    int _errno = 0;
    if (wlan_socket_settimeout(self, timeout, &_errno) != 0) {
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

STATIC mp_obj_t socket_makefile(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_makefile_obj, 1, 6, socket_makefile);

STATIC const mp_rom_map_elem_t socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__),         MP_ROM_PTR(&socket_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_close),           MP_ROM_PTR(&socket_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind),            MP_ROM_PTR(&socket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen),          MP_ROM_PTR(&socket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_accept),          MP_ROM_PTR(&socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),         MP_ROM_PTR(&socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_send),            MP_ROM_PTR(&socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendall),         MP_ROM_PTR(&socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv),            MP_ROM_PTR(&socket_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendto),          MP_ROM_PTR(&socket_sendto_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom),        MP_ROM_PTR(&socket_recvfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_setsockopt),      MP_ROM_PTR(&socket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout),      MP_ROM_PTR(&socket_settimeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking),     MP_ROM_PTR(&socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_makefile),        MP_ROM_PTR(&socket_makefile_obj) },

    // stream methods
    { MP_ROM_QSTR(MP_QSTR_read),            MP_ROM_PTR(&mp_stream_read1_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline),        MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&mp_stream_write_obj) },
};

MP_DEFINE_CONST_DICT(socket_locals_dict, socket_locals_dict_table);

STATIC mp_uint_t socket_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    mod_network_socket_obj_t *self = self_in;
    mp_int_t ret = wlan_socket_recv(self, buf, size, errcode);
    if (ret < 0) {
        // we need to ignore the socket closed error here because a read() without params
        // only returns when the socket is closed by the other end
        if (*errcode != -SL_ESECCLOSED) {
            ret = MP_STREAM_ERROR;
        } else {
            ret = 0;
        }
    }
    return ret;
}

STATIC mp_uint_t socket_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    mod_network_socket_obj_t *self = self_in;
    mp_int_t ret = wlan_socket_send(self, buf, size, errcode);
    if (ret < 0) {
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC mp_uint_t socket_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    mod_network_socket_obj_t *self = self_in;
    return wlan_socket_ioctl(self, request, arg, errcode);
}

const mp_stream_p_t socket_stream_p = {
    .read = socket_read,
    .write = socket_write,
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
/// \function getaddrinfo(host, port)
STATIC mp_obj_t mod_usocket_getaddrinfo(mp_obj_t host_in, mp_obj_t port_in) {
    size_t hlen;
    const char *host = mp_obj_str_get_data(host_in, &hlen);
    mp_int_t port = mp_obj_get_int(port_in);

    // ipv4 only
    uint8_t out_ip[MOD_NETWORK_IPV4ADDR_BUF_SIZE];
    int32_t result = wlan_gethostbyname(host, hlen, out_ip, SL_AF_INET);
    if (result < 0) {
        mp_raise_OSError(-result);
    }
    mp_obj_tuple_t *tuple = mp_obj_new_tuple(5, NULL);
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(SL_AF_INET);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(SL_SOCK_STREAM);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    tuple->items[4] = netutils_format_inet_addr(out_ip, port, NETUTILS_LITTLE);
    return mp_obj_new_list(1, (mp_obj_t*)&tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_usocket_getaddrinfo_obj, mod_usocket_getaddrinfo);

STATIC const mp_rom_map_elem_t mp_module_usocket_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_ROM_QSTR(MP_QSTR_usocket) },

    { MP_ROM_QSTR(MP_QSTR_socket),          MP_ROM_PTR(&socket_type) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo),     MP_ROM_PTR(&mod_usocket_getaddrinfo_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_AF_INET),         MP_ROM_INT(SL_AF_INET) },

    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM),     MP_ROM_INT(SL_SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM),      MP_ROM_INT(SL_SOCK_DGRAM) },

    { MP_ROM_QSTR(MP_QSTR_IPPROTO_SEC),     MP_ROM_INT(SL_SEC_SOCKET) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TCP),     MP_ROM_INT(SL_IPPROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_UDP),     MP_ROM_INT(SL_IPPROTO_UDP) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_usocket_globals, mp_module_usocket_globals_table);

const mp_obj_module_t mp_module_usocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_usocket_globals,
};
