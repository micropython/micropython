/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
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

#include "py/mpconfig.h"

#if MICROPY_PY_SOCKET

BUILD_ASSERT(IS_ENABLED(CONFIG_NET_SOCKETS),
    "CONFIG_NET_SOCKETS must be enabled to use MICROPY_PY_SOCKET");

#include <zephyr/kernel.h>
#include <zephyr/net/quic.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/socketcan.h>
#include <zephyr/posix/fcntl.h>

#include "py/objstr.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "modsocket.h"

#define DNS_RETRY_AGAIN 3

const mp_obj_type_t zephyr_socket_type;

// Helper functions

socket_obj_t *socket_new(socket_obj_t *from) {
    socket_obj_t *socket = mp_obj_malloc_with_finaliser(socket_obj_t, &zephyr_socket_type);
    socket->state = SOCK_STATE_NEW;
    socket->bound = false;
    socket->timeout = -1;
    if (from != NULL) {
        socket->family = from->family;
        socket->proto = from->proto;
        socket->type = from->type;
    }
    return socket;
}

static void socket_check_closed(socket_obj_t *socket) {
    if (socket->ctx < 0) {
        // already closed
        mp_raise_ValueError(MP_ERROR_TEXT("socket is closed"));
    }
}

typedef struct _socket_proto_t {
    const char *str;
    bool secure;
} socket_proto_t;

static const socket_proto_t socket_proto_get(const int id) {
    switch (id) {
        case NET_IPPROTO_IP:
            return (const socket_proto_t) { .str = "IPPROTO_IP", .secure = false, };
        case NET_IPPROTO_ICMP:
            return (const socket_proto_t) { .str = "IPPROTO_ICMP", .secure = false, };
        case NET_IPPROTO_IGMP:
            return (const socket_proto_t) { .str = "IPPROTO_IGMP", .secure = false, };
        case NET_IPPROTO_ETH_P_ALL:
            return (const socket_proto_t) { .str = "IPPROTO_ETH_P_ALL", .secure = false, };
        case NET_IPPROTO_IPIP:
            return (const socket_proto_t) { .str = "IPPROTO_IPIP", .secure = false, };
        case NET_IPPROTO_TCP:
            return (const socket_proto_t) { .str = "IPPROTO_TCP", .secure = false, };
        case NET_IPPROTO_UDP:
            return (const socket_proto_t) { .str = "IPPROTO_UDP", .secure = false, };
        case NET_IPPROTO_IPV6:
            return (const socket_proto_t) { .str = "IPPROTO_IPV6", .secure = false, };
        case NET_IPPROTO_ICMPV6:
            return (const socket_proto_t) { .str = "IPPROTO_ICMPV6", .secure = false, };
        case NET_IPPROTO_RAW:
            return (const socket_proto_t) { .str = "IPPROTO_RAW", .secure = false, };
        case NET_IPPROTO_TLS_1_0:
            return (const socket_proto_t) { .str = "IPPROTO_TLS_1_0", .secure = true, };
        case NET_IPPROTO_TLS_1_1:
            return (const socket_proto_t) { .str = "IPPROTO_TLS_1_1", .secure = true, };
        case NET_IPPROTO_TLS_1_2:
            return (const socket_proto_t) { .str = "IPPROTO_TLS_1_2", .secure = true, };
        case NET_IPPROTO_TLS_1_3:
            return (const socket_proto_t) { .str = "IPPROTO_TLS_1_3", .secure = true, };
        case NET_IPPROTO_DTLS_1_0:
            return (const socket_proto_t) { .str = "IPPROTO_DTLS_1_0", .secure = true, };
        case NET_IPPROTO_DTLS_1_2:
            return (const socket_proto_t) { .str = "IPPROTO_DTLS_1_2", .secure = true, };
        case NET_IPPROTO_QUIC:
            return (const socket_proto_t) { .str = "IPPROTO_QUIC", .secure = true, };
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%d is not a socket protocol"), id);
    return (const socket_proto_t) { 0 };
}

static const char *socket_type_get_str(const enum net_sock_type type) {
    switch (type) {
        case NET_SOCK_STREAM:
            return "SOCK_STREAM";
        case NET_SOCK_DGRAM:
            return "SOCK_DGRAM";
        case NET_SOCK_RAW:
            return "SOCK_RAW";
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%d is not a socket type"), type);
    return NULL;
}

static const char *socket_family_get_str(const net_sa_family_t family) {
    switch (family) {
        case NET_AF_UNSPEC:
            return "AF_UNSPEC";
        case NET_AF_INET:
            return "AF_INET";
        case NET_AF_INET6:
            return "AF_INET6";
        case NET_AF_PACKET:
            return "AF_PACKET";
        case NET_AF_CAN:
            return "AF_CAN";
        case NET_AF_NET_MGMT:
            return "AF_NET_MGMT";
        case NET_AF_LOCAL: /* NET_AF_UNIX*/
            return "AF_LOCAL";
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%d is not a socket address family"), family);
    return NULL;
}

static int socket_parse_addr(socket_obj_t *socket, mp_obj_t addr_in, struct net_sockaddr *saddr_in) {
    saddr_in->sa_family = socket->family;

    /* Single address string */
    #if defined(CONFIG_NET_IPV4) || defined(CONFIG_NET_IPV6)
    if (mp_obj_is_str(addr_in)) {
        mp_warning(MP_WARN_CAT(ValueWarning), "use address tuple");

        GET_STR_DATA_LEN(addr_in, addr_in_str, addr_in_len);
        if (!net_ipaddr_parse(addr_in_str, addr_in_len, saddr_in)) {
            mp_raise_ValueError(MP_ERROR_TEXT("could not parse socket address"));
            return -EINVAL;
        }

        return 0;
    }
    #endif

    switch (socket->family) {
        #if defined(CONFIG_NET_IPV4)
        case NET_AF_INET: {
            struct net_sockaddr_in *addr_sin = (struct net_sockaddr_in *)saddr_in;
            mp_obj_t *addr_items;
            mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
            int ret;

            ret = net_addr_pton(socket->family, mp_obj_str_get_str(addr_items[0]), &addr_sin->sin_addr);
            if (ret < 0) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("failed to parse socket address: %d"), ret);
            }

            addr_sin->sin_port = htons(mp_obj_get_int(addr_items[1]));

            return 0;
        }
        #endif
        #if defined(CONFIG_NET_IPV6)
        case NET_AF_INET6: {
            struct net_sockaddr_in6 *addr_sin = (struct net_sockaddr_in6 *)saddr_in;
            mp_obj_t *addr_items;
            mp_obj_get_array_fixed_n(addr_in, 4, &addr_items);
            int ret;

            ret = net_addr_pton(socket->family, mp_obj_str_get_str(addr_items[0]), &addr_sin->sin6_addr);
            if (ret < 0) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("failed to parse socket address: %d"), ret);
            }

            addr_sin->sin6_port = htons(mp_obj_get_int(addr_items[1]));

            if (mp_obj_get_int(addr_items[2]) != 0) {
                mp_raise_ValueError(MP_ERROR_TEXT("flowinfo (addr[2]) must be 0"));
            }

            addr_sin->sin6_scope_id = mp_obj_get_uint(addr_items[3]);

            return 0;
        }
        #endif
        #if defined(NET_SOCKETS_CAN)
        case NET_AF_CAN: {
            struct net_sockaddr_can *addr_sin = (struct net_sockaddr_can *)saddr_in;
            if (mp_obj_is_int(addr_in)) {
                addr_sin->can_ifindex = mp_obj_get_int(addr_in);
            } else {
                mp_obj_t *addr_items;
                mp_obj_get_array_fixed_n(addr_in, 1, &addr_items);
                addr_sin->can_ifindex = mp_obj_get_int(addr_items[0]);
            }
            return 0;
        }
        #endif
        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%s is not supported"), socket_family_get_str(socket->family));
            return -ENOTSUP;
    }

    return -EINVAL;
}

static void socket_format_addr_correct_port(const struct net_sockaddr *saddr_in, unsigned int port) {
    switch (saddr_in->sa_family) {
        #if defined(CONFIG_NET_IPV4)
        case NET_AF_INET: {
            struct net_sockaddr_in *addr_sin = (struct net_sockaddr_in *)saddr_in;
            addr_sin->sin_port = port;
            return;
        }
        #endif
        #if defined(CONFIG_NET_IPV6)
        case NET_AF_INET6: {
            struct net_sockaddr_in6 *addr_sin = (struct net_sockaddr_in6 *)saddr_in;
            addr_sin->sin6_port = port;
        }
        #endif
    }
    return;
}

static mp_obj_t socket_format_addr(const struct net_sockaddr *saddr_in) {

    switch (saddr_in->sa_family) {
        #if defined(CONFIG_NET_IPV4)
        case NET_AF_INET: {
            struct net_sockaddr_in *addr_sin = (struct net_sockaddr_in *)saddr_in;
            char buf[NET_IPV4_ADDR_LEN];
            if (net_addr_ntop(saddr_in->sa_family, &addr_sin->sin_addr, buf, NET_IPV4_ADDR_LEN) == NULL) {
                mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to convert ip to string"));
            }
            mp_obj_tuple_t *tuple = mp_obj_new_tuple(2, NULL);
            tuple->items[0] = mp_obj_new_str_from_cstr(buf);
            tuple->items[1] = mp_obj_new_int(addr_sin->sin_port);
            return tuple;
        }
        #endif
        #if defined(CONFIG_NET_IPV6)
        case NET_AF_INET6: {
            struct net_sockaddr_in6 *addr_sin = (struct net_sockaddr_in6 *)saddr_in;
            char buf[NET_IPV6_ADDR_LEN];
            if (net_addr_ntop(saddr_in->sa_family, &addr_sin->sin6_addr, buf, NET_IPV6_ADDR_LEN) == NULL) {
                mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to convert ip to string"));
            }
            mp_obj_tuple_t *tuple = mp_obj_new_tuple(4, NULL);
            tuple->items[0] = mp_obj_new_str_from_cstr(buf);
            tuple->items[1] = mp_obj_new_int(addr_sin->sin6_port);
            tuple->items[2] = mp_obj_new_int(0);
            tuple->items[3] = mp_obj_new_int(addr_sin->sin6_scope_id);
            return tuple;
        }
        #endif
        #if defined(NET_SOCKETS_CAN)
        case NET_AF_CAN: {
            struct net_sockaddr_can *addr_sin = (struct net_sockaddr_can *)saddr_in;
            mp_obj_tuple_t *tuple = mp_obj_new_tuple(1, NULL);
            tuple->items[0] = mp_obj_new_int(addr_sin->can_ifindex);
            return tuple;
        }
        #endif
        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%s is not supported"), socket_family_get_str(saddr_in->sa_family));
            return mp_const_none;
    }

    return mp_const_none;
}

static int socket_read_handler(socket_obj_t *self, void *buf, size_t *len, uint32_t flags,
    struct net_sockaddr *from, net_socklen_t *from_len) {

    if (self->state == SOCK_STATE_NEW) {
        return MP_ENOTCONN;
    }

    if (self->state == SOCK_STATE_PEER_CLOSED) {
        *len = 0;
        return 0;
    }

    struct zsock_pollfd fds = {
        .fd = self->ctx,
        .events = ZSOCK_POLLIN,
        .revents = 0,
    };

    /* If data is waiting, don't unlock GIL */
    int ret = zsock_poll(&fds, 1, 0);
    if (ret < 0) {
        return MP_EIO;
    }

    if (fds.revents == 0) {
        MP_THREAD_GIL_EXIT();
    }
    ssize_t recv_len = zsock_recvfrom(self->ctx, buf, *len, flags, from, from_len);
    if (fds.revents == 0) {
        MP_THREAD_GIL_ENTER();
    }

    if (recv_len < 0) {
        if (errno == ENOTCONN) {
            self->state = SOCK_STATE_PEER_CLOSED;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return MP_EWOULDBLOCK;
        }
        return errno;
    }

    if (recv_len == 0) {
        self->state = SOCK_STATE_PEER_CLOSED;
    }

    *len = recv_len;

    return 0;
}

// Methods

static void socket_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->ctx < 0) {
        mp_printf(print, "<socket NULL>");
    } else {
        const socket_proto_t proto = socket_proto_get(self->proto);
        mp_printf(print, "<socket %p fd=%d timeout=%d domain=%s type=%s proto=%s bound=%b>",
            self,
            self->ctx,
            self->timeout,
            socket_family_get_str(self->family),
            socket_type_get_str(self->type),
            proto.str,
            self->bound);
    }
}

static mp_obj_t socket_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_family, ARG_type, ARG_proto, ARG_blocking };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_family, MP_ARG_INT, {.u_int = NET_AF_INET} },
        { MP_QSTR_type, MP_ARG_INT, {.u_int = NET_SOCK_STREAM} },
        { MP_QSTR_proto, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_blocking, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    (void)socket_family_get_str(args[ARG_family].u_int);

    (void)socket_type_get_str(args[ARG_type].u_int);

    if (args[ARG_proto].u_int == -1) {
        args[ARG_proto].u_int = NET_IPPROTO_TCP;
        if (args[ARG_type].u_int != NET_SOCK_STREAM) {
            args[ARG_proto].u_int = NET_IPPROTO_UDP;
        }
    } else {
        (void)socket_proto_get(args[ARG_proto].u_int);
    }

    socket_obj_t *self = socket_new(NULL);

    self->ctx = zsock_socket(args[ARG_family].u_int, args[ARG_type].u_int, args[ARG_proto].u_int);
    if (self->ctx < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("could not create socket"));
    }

    if (!args[ARG_blocking].u_bool) {
        /* Function only for setting NONBLOCK */
        if (zsock_fcntl_impl(self->ctx, F_SETFL, O_NONBLOCK) != 0) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("could not set socket to nonblock"));
        }
        self->timeout = 0;
    } else {
        self->timeout = -1;
    }

    self->family = args[ARG_family].u_int;
    self->type = args[ARG_type].u_int;
    self->proto = args[ARG_proto].u_int;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    socket_check_closed(self);

    struct net_sockaddr saddr_in;
    socket_parse_addr(self, addr_in, &saddr_in);

    int ret = zsock_bind(self->ctx, &saddr_in, sizeof(saddr_in));
    if (ret < 0) {
        mp_raise_OSError(errno);
    }
    self->bound = true;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

static mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    socket_check_closed(self);

    struct net_sockaddr saddr_in;
    socket_parse_addr(self, addr_in, &saddr_in);

    MP_THREAD_GIL_EXIT();
    int ret = zsock_connect(self->ctx, &saddr_in, sizeof(saddr_in));
    MP_THREAD_GIL_ENTER();
    if (ret < 0) {
        if (self->timeout == 0 && errno == EINPROGRESS) {
            self->state = SOCK_STATE_CONNECTING;
            return mp_const_none;
        }
        mp_raise_OSError(errno);
    }

    self->state = SOCK_STATE_CONNECTED;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

static mp_obj_t socket_listen(size_t n_args, const mp_obj_t *args) {
    socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    socket_check_closed(self);

    if (!self->bound) {
        mp_raise_OSError(MP_EINVAL);
    }

    mp_int_t backlog = MICROPY_PY_SOCKET_LISTEN_BACKLOG_DEFAULT;
    if (n_args > 1) {
        backlog = mp_obj_get_int(args[1]);
        backlog = (backlog < 0) ? 0 : backlog;
    }

    int ret = zsock_listen(self->ctx, backlog);
    if (ret < 0) {
        mp_raise_OSError(errno);
    }

    self->state = SOCK_STATE_LISTENING;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_listen_obj, 1, 2, socket_listen);

static mp_obj_t socket_accept(mp_obj_t self_in) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    socket_check_closed(self);

    if (self->state != SOCK_STATE_LISTENING) {
        mp_raise_OSError(MP_EINVAL);
    }

    struct net_sockaddr saddr_in;
    socklen_t addrlen = sizeof(saddr_in);
    int ctx = zsock_accept(self->ctx, &saddr_in, &addrlen);
    if (ctx < 0) {
        mp_raise_OSError(errno);
    }

    socket_obj_t *socket = socket_new(self);
    socket->state = SOCK_STATE_ACCEPTED;
    socket->ctx = ctx;

    mp_obj_tuple_t *client = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    client->items[0] = MP_OBJ_FROM_PTR(socket);
    client->items[1] = socket_format_addr(&saddr_in);

    return MP_OBJ_FROM_PTR(client);
}
static MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

static mp_obj_t socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    socket_check_closed(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    ssize_t len = zsock_send(self->ctx, bufinfo.buf, bufinfo.len, 0);
    if (errno == ENOTCONN) {
        self->state = SOCK_STATE_PEER_CLOSED;
    }
    if (len < 0) {
        mp_raise_OSError(errno);
    }

    return mp_obj_new_int_from_uint(len);
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, socket_send);

static mp_obj_t socket_recv(size_t n_args, const mp_obj_t *args) {
    socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    socket_check_closed(self);

    size_t len = mp_obj_get_uint(args[1]);
    vstr_t vstr;
    // +1 to accommodate for trailing \0
    vstr_init_len(&vstr, len + 1);

    int flags = n_args > 2 ? mp_obj_get_int(args[2]) : 0;

    int ret = socket_read_handler(self, vstr.buf, &len, flags, NULL, NULL);
    if (ret != 0) {
        vstr_clear(&vstr);
        mp_raise_OSError(ret);
    } else if (len == 0) {
        vstr_clear(&vstr);
        return mp_const_empty_bytes;
    }

    vstr.len = len;
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_recv_obj, 2, 3, socket_recv);

static mp_obj_t socket_recvfrom(size_t n_args, const mp_obj_t *args) {
    socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    socket_check_closed(self);

    size_t len = mp_obj_get_int(args[1]);
    vstr_t vstr;
    // +1 to accommodate for trailing \0
    vstr_init_len(&vstr, len + 1);

    int flags = n_args > 2 ? mp_obj_get_int(args[2]) : 0;

    struct net_sockaddr saddr;
    socklen_t slen = sizeof(saddr);

    int ret = socket_read_handler(self, vstr.buf, &len, flags, &saddr, &slen);
    if (ret != 0) {
        vstr_clear(&vstr);
        mp_raise_OSError(ret);
    }

    mp_obj_t items[2];
    if (len == 0) {
        vstr_clear(&vstr);
        items[0] = mp_const_empty_bytes;
    } else {
        vstr.len = len;
        items[0] = mp_obj_new_bytes_from_vstr(&vstr);
    }
    items[1] = socket_format_addr(&saddr);
    return mp_obj_new_tuple(2, items);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_recvfrom_obj, 2, 3, socket_recvfrom);

static mp_obj_t socket_setsockopt(size_t n_args, const mp_obj_t *args) {
    socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    socket_check_closed(self);

    int level = mp_obj_get_int(args[1]);
    int opt = mp_obj_get_int(args[2]);

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
    } else if (mp_obj_is_callable(args[3])) {
        optval = args[3];
        optlen = sizeof(optval);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
        optval = bufinfo.buf;
        optlen = bufinfo.len;
    }

    int ret = zsock_setsockopt(self->ctx, level, opt, optval, optlen);
    if (ret < 0) {
        if (errno == EINVAL) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("opt invalid for this level"));
        } else if (errno == ENOPROTOOPT) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("opt invalid for this protocol"));
        }
        mp_raise_OSError(errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_setsockopt_obj, 4, 4, socket_setsockopt);

static mp_obj_t socket_getsockopt(size_t n_args, const mp_obj_t *args) {
    socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    socket_check_closed(self);

    int level = mp_obj_get_int(args[1]);
    int opt = mp_obj_get_int(args[2]);

    void *optval;
    net_socklen_t optlen;
    int val;

    if (n_args == 4) {
        optlen = mp_obj_get_uint(args[3]);
        optval = m_new(byte, optlen);
    } else {
        optval = &val;
        optlen = sizeof(val);
    }

    int ret = zsock_getsockopt(self->ctx, level, opt, optval, &optlen);
    if (ret < 0) {
        if (errno == EINVAL) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("opt invalid for this level"));
        } else if (errno == ENOPROTOOPT) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("opt invalid for this protocol"));
        }
        mp_raise_OSError(errno);
    }

    if (n_args == 4) {
        return mp_obj_new_bytearray_by_ref(optlen, optval);
    }
    return mp_obj_new_int(val);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_getsockopt_obj, 3, 4, socket_getsockopt);

static mp_obj_t socket_setblocking(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);

    if (!mp_obj_is_true(arg1)) {
        if (zsock_fcntl_impl(self->ctx, F_SETFL, O_NONBLOCK) != 0) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("could not set socket to nonblock"));
        }
        self->timeout = 0;
    } else {
        if (zsock_fcntl_impl(self->ctx, F_SETFL, 0) != 0) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("could not set socket to blocking"));
        }
        if (self->timeout == 0) {
            self->timeout = -1;
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_setblocking_obj, socket_setblocking);

static mp_obj_t socket_makefile(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return args[0];
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_makefile_obj, 1, 3, socket_makefile);

static const mp_rom_map_elem_t socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&socket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&socket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&socket_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom), MP_ROM_PTR(&socket_recvfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&socket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_getsockopt), MP_ROM_PTR(&socket_getsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socket_setblocking_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_makefile), MP_ROM_PTR(&socket_makefile_obj) },
};
static MP_DEFINE_CONST_DICT(socket_locals_dict, socket_locals_dict_table);

/* Stream Protocol */

static mp_uint_t sock_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->ctx < 0) {
        // already closed
        *errcode = EBADF;
        return MP_STREAM_ERROR;
    }

    ssize_t len = zsock_send(self->ctx, buf, size, 0);
    if (len == -1) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }

    return len;
}

static mp_uint_t sock_read(mp_obj_t self_in, void *buf, mp_uint_t max_len, int *errcode) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->ctx < 0) {
        // already closed
        *errcode = EBADF;
        return MP_STREAM_ERROR;
    }

    size_t len = max_len;
    int ret = socket_read_handler(self, buf, &len, 0, NULL, NULL);
    if (ret != 0) {
        *errcode = ret;
        return MP_STREAM_ERROR;
    }

    return len;
}

static mp_uint_t sock_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
        case MP_STREAM_CLOSE:
            if (self->ctx >= 0) {
                int ret = zsock_close(self->ctx);
                if (ret < 0) {
                    *errcode = errno;
                    return MP_STREAM_ERROR;
                }
                self->ctx = -1;
            }
            return 0;
        case MP_STREAM_POLL: {
            if (self->ctx == -1) {
                return MP_STREAM_POLL_NVAL;
            }

            struct zsock_pollfd fds = {
                .fd = self->ctx,
                /* MPY poll tags match zephyr's */
                .events = arg,
                .revents = 0,
            };

            int ret = zsock_poll(&fds, 1, 0);
            if (ret < 0) {
                *errcode = MP_EIO;
                return MP_STREAM_ERROR;
            }

            // New (unconnected) sockets are writable and have HUP set.
            if (self->state == SOCK_STATE_NEW) {
                fds.revents |= (arg & MP_STREAM_POLL_WR) | MP_STREAM_POLL_HUP;
            }

            return fds.revents;
        }
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

/* Ensure poll tags stay matching */
BUILD_ASSERT(MP_STREAM_POLL_RD == ZSOCK_POLLIN);
BUILD_ASSERT(MP_STREAM_POLL_WR == ZSOCK_POLLOUT);
BUILD_ASSERT(MP_STREAM_POLL_ERR == ZSOCK_POLLERR);
BUILD_ASSERT(MP_STREAM_POLL_HUP == ZSOCK_POLLHUP);
BUILD_ASSERT(MP_STREAM_POLL_NVAL == ZSOCK_POLLNVAL);

static const mp_stream_p_t socket_stream_p = {
    .read = sock_read,
    .write = sock_write,
    .ioctl = sock_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_socket_type,
    MP_QSTR_socket,
    MP_TYPE_FLAG_NONE,
    make_new, socket_make_new,
    print, socket_print,
    protocol, &socket_stream_p,
    locals_dict, &socket_locals_dict
    );

//
// getaddrinfo() implementation
//

static mp_obj_t mod_socket_getaddrinfo(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_host, ARG_port, ARG_af, ARG_type, ARG_proto, ARG_flags };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_host, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_port, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_af, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_type, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_proto, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flags, MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    struct zsock_addrinfo hints = { 0 };
    struct zsock_addrinfo *result = NULL;
    const char *host = mp_obj_str_get_str(args[ARG_host].u_obj);
    unsigned int port_int;
    /* Max port be uint16_t max, this safe */
    char port[10];
    int ret;

    if (args[ARG_af].u_int != -1) {
        hints.ai_family = args[ARG_af].u_int;
        (void)socket_family_get_str(hints.ai_family);
    }
    if (args[ARG_type].u_int != -1) {
        hints.ai_socktype = args[ARG_type].u_int;
        /* 0 = Any */
        if (hints.ai_socktype != 0) {
            (void)socket_type_get_str(hints.ai_socktype);
        }
    }
    if (args[ARG_proto].u_int != -1) {
        hints.ai_protocol = args[ARG_proto].u_int;
        (void)socket_proto_get(hints.ai_protocol);
    }
    if (args[ARG_flags].u_int != -1) {
        hints.ai_flags = args[ARG_flags].u_int;
    }

    /* Parse int so we can use hex and stuffs if we want */
    if (mp_obj_is_integer(args[ARG_port].u_obj)) {
        port_int = mp_obj_get_uint(args[ARG_port].u_obj);
    } else {
        size_t len;
        const char *buf = mp_obj_str_get_data(args[ARG_port].u_obj, &len);
        port_int = mp_obj_get_uint(mp_parse_num_integer(buf, len, 0, NULL));
    }
    /* Recompose to string */
    ret = snprintf(port, sizeof(port), "%u", port_int);
    if (ret < 0 || ret + 1 > sizeof(port)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid port number"));
    }

    if (host[0] == '\0') {
        // a host of "" is equivalent to the default/all-local IP address
        host = "0.0.0.0";
    }

    hints.ai_flags |= AI_CANONNAME;

    MP_THREAD_GIL_EXIT();
    int tries = DNS_RETRY_AGAIN;
    do {
        ret = zsock_getaddrinfo(host, port, &hints, &result);
        tries--;
    } while ((ret == DNS_EAI_AGAIN || ret == DNS_EAI_CANCELED) && tries > 0);
    MP_THREAD_GIL_ENTER();

    if (ret == DNS_EAI_SYSTEM) {
        mp_raise_OSError(errno);
    } else if (ret != 0) {
        mp_raise_OSError(ret > 0 ? -ret : ret);
    }

    mp_obj_t ret_list = mp_obj_new_list(0, NULL);


    for (struct zsock_addrinfo *resi = result; resi; resi = resi->ai_next) {
        /* Replace port with port provided as input */
        socket_format_addr_correct_port(resi->ai_addr, port_int);
        mp_obj_t tobj[] = {
            mp_obj_new_int(resi->ai_family),
            mp_obj_new_int(resi->ai_socktype),
            mp_obj_new_int(resi->ai_protocol),
            mp_obj_new_str_from_cstr(resi->ai_canonname),
            socket_format_addr(resi->ai_addr),
        };
        mp_obj_list_append(ret_list, mp_obj_new_tuple(ARRAY_SIZE(tobj), tobj));
    }

    zsock_freeaddrinfo(result);

    return ret_list;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mod_socket_getaddrinfo_obj, 0, mod_socket_getaddrinfo);

#if defined(CONFIG_NET_BUF_POOL_USAGE)

#include <zephyr/net/net_pkt.h>

static mp_obj_t mod_socket_pkt_get_info(void) {
    struct k_mem_slab *rx, *tx;
    struct net_buf_pool *rx_data, *tx_data;
    net_pkt_get_info(&rx, &tx, &rx_data, &tx_data);
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(4, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(k_mem_slab_num_free_get(rx));
    t->items[1] = MP_OBJ_NEW_SMALL_INT(k_mem_slab_num_free_get(tx));
    t->items[2] = MP_OBJ_NEW_SMALL_INT(rx_data->avail_count);
    t->items[3] = MP_OBJ_NEW_SMALL_INT(tx_data->avail_count);
    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_0(mod_socket_pkt_get_info_obj, mod_socket_pkt_get_info);

#endif

/* Self-document features */

static const mp_rom_obj_tuple_t mod_socket_af_available_obj = {
    .base = {&mp_type_tuple},
    /* lol */
    .len = 1 IF_ENABLED(CONFIG_NET_IPV4, (+1)) IF_ENABLED(CONFIG_NET_IPV6, (+1))
        IF_ENABLED(CONFIG_NET_SOCKETS_PACKET, (+1)) IF_ENABLED(CONFIG_NET_SOCKETS_CAN, (+1))
        IF_ENABLED(CONFIG_NET_SOCKETS_NET_MGMT, (+1)),
    .items = {
        MP_ROM_INT(NET_AF_LOCAL),
        #if defined(CONFIG_NET_IPV4)
        MP_ROM_INT(NET_AF_INET),
        #endif
        #if defined(CONFIG_NET_IPV6)
        MP_ROM_INT(NET_AF_INET6),
        #endif
        #if defined(CONFIG_NET_SOCKETS_PACKET)
        MP_ROM_INT(NET_AF_PACKET),
        #endif
        #if defined(CONFIG_NET_SOCKETS_CAN)
        MP_ROM_INT(NET_AF_CAN),
        #endif
        #if defined(CONFIG_NET_SOCKETS_NET_MGMT)
        MP_ROM_INT(NET_AF_NET_MGMT),
        #endif
    },
};

static const mp_rom_obj_tuple_t mod_socket_proto_available_obj = {
    .base = {&mp_type_tuple},
    .len = 1 IF_ENABLED(CONFIG_NET_TCP, (+1)) IF_ENABLED(CONFIG_NET_UDP, (+1))
        IF_ENABLED(CONFIG_NET_IPV4, (+2)) IF_ENABLED(CONFIG_NET_IPV6, (+2))
        IF_ENABLED(CONFIG_NET_IPV4_IGMP, (+1)) IF_ENABLED(CONFIG_NET_L2_IPIP, (+1))
        IF_ENABLED(CONFIG_NET_SOCKETS_PACKET, (+1)) IF_ENABLED(CONFIG_NET_SOCKETS_SOCKOPT_TLS, (+4))
        IF_ENABLED(CONFIG_NET_SOCKETS_ENABLE_DTLS, (+2)) IF_ENABLED(CONFIG_QUIC, (+1)),
    .items = {
        MP_ROM_INT(NET_IPPROTO_RAW),
        #if defined(CONFIG_NET_TCP)
        MP_ROM_INT(NET_IPPROTO_TCP),
        #endif
        #if defined(CONFIG_NET_UDP)
        MP_ROM_INT(NET_IPPROTO_UDP),
        #endif
        #if defined(CONFIG_NET_IPV4)
        MP_ROM_INT(NET_IPPROTO_IP),
        MP_ROM_INT(NET_IPPROTO_ICMP),
        #endif
        #if defined(CONFIG_NET_IPV6)
        MP_ROM_INT(NET_IPPROTO_IPV6),
        MP_ROM_INT(NET_IPPROTO_ICMPV6),
        #endif
        #if defined(CONFIG_NET_IPV4_IGMP)
        MP_ROM_INT(NET_IPPROTO_IGMP),
        #endif
        #if defined(CONFIG_NET_L2_IPIP)
        MP_ROM_INT(NET_IPPROTO_IPIP),
        #endif
        #if defined(CONFIG_NET_SOCKETS_PACKET)
        MP_ROM_INT(NET_IPPROTO_ETH_P_ALL),
        #endif
        #if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
        MP_ROM_INT(NET_IPPROTO_TLS_1_0),
        MP_ROM_INT(NET_IPPROTO_TLS_1_1),
        MP_ROM_INT(NET_IPPROTO_TLS_1_2),
        MP_ROM_INT(NET_IPPROTO_TLS_1_3),
        #endif
        #if defined(CONFIG_NET_SOCKETS_ENABLE_DTLS)
        MP_ROM_INT(NET_IPPROTO_DTLS_1_0),
        MP_ROM_INT(NET_IPPROTO_DTLS_1_2),
        #endif
        #if defined(CONFIG_QUIC)
        MP_ROM_INT(NET_IPPROTO_QUIC),
        #endif
    },
};

static mp_obj_t mod_socket_print_available(void) {
    mp_obj_t tobj[2];
    mp_obj_t tobj_af[mod_socket_af_available_obj.len];
    mp_obj_t tobj_proto[mod_socket_proto_available_obj.len];

    for (size_t i = 0; i < mod_socket_af_available_obj.len; i++) {
        tobj_af[i] = mp_obj_new_str_from_cstr(
            socket_family_get_str(mp_obj_get_int(mod_socket_af_available_obj.items[i])));
    }

    for (size_t i = 0; i < mod_socket_proto_available_obj.len; i++) {
        tobj_proto[i] = mp_obj_new_str_from_cstr(
            socket_proto_get(mp_obj_get_int(mod_socket_proto_available_obj.items[i])).str);
    }

    tobj[0] = mp_obj_new_tuple(ARRAY_SIZE(tobj_af), tobj_af);
    tobj[1] = mp_obj_new_tuple(ARRAY_SIZE(tobj_proto), tobj_proto);

    return mp_obj_new_tuple(ARRAY_SIZE(tobj), tobj);
}
static MP_DEFINE_CONST_FUN_OBJ_0(mod_socket_print_available_obj, mod_socket_print_available);

/* Sockopts for SOCKET level */
static const mp_rom_map_elem_t mp_module_socket_globals_SOL_SOCKET_table[] = {
    { MP_ROM_QSTR(MP_QSTR_DEBUG), MP_ROM_INT(ZSOCK_SO_DEBUG) },
    { MP_ROM_QSTR(MP_QSTR_REUSEADDR), MP_ROM_INT(ZSOCK_SO_REUSEADDR) },
    { MP_ROM_QSTR(MP_QSTR_TYPE), MP_ROM_INT(ZSOCK_SO_TYPE) },
    { MP_ROM_QSTR(MP_QSTR_ERROR), MP_ROM_INT(ZSOCK_SO_ERROR) },
    { MP_ROM_QSTR(MP_QSTR_DONTROUTE), MP_ROM_INT(ZSOCK_SO_DONTROUTE) },
    { MP_ROM_QSTR(MP_QSTR_BROADCAST), MP_ROM_INT(ZSOCK_SO_BROADCAST) },
    { MP_ROM_QSTR(MP_QSTR_SNDBUF), MP_ROM_INT(ZSOCK_SO_SNDBUF) },
    { MP_ROM_QSTR(MP_QSTR_RCVBUF), MP_ROM_INT(ZSOCK_SO_RCVBUF) },
    { MP_ROM_QSTR(MP_QSTR_KEEPALIVE), MP_ROM_INT(ZSOCK_SO_KEEPALIVE) },
    { MP_ROM_QSTR(MP_QSTR_OOBINLINE), MP_ROM_INT(ZSOCK_SO_OOBINLINE) },
    { MP_ROM_QSTR(MP_QSTR_PRIORITY), MP_ROM_INT(ZSOCK_SO_PRIORITY) },
    { MP_ROM_QSTR(MP_QSTR_LINGER), MP_ROM_INT(ZSOCK_SO_LINGER) },
    { MP_ROM_QSTR(MP_QSTR_REUSEPORT), MP_ROM_INT(ZSOCK_SO_REUSEPORT) },
    { MP_ROM_QSTR(MP_QSTR_RCVLOWAT), MP_ROM_INT(ZSOCK_SO_RCVLOWAT) },
    { MP_ROM_QSTR(MP_QSTR_SNDLOWAT), MP_ROM_INT(ZSOCK_SO_SNDLOWAT) },
    { MP_ROM_QSTR(MP_QSTR_RCVTIMEO), MP_ROM_INT(ZSOCK_SO_RCVTIMEO) },
    { MP_ROM_QSTR(MP_QSTR_SNDTIMEO), MP_ROM_INT(ZSOCK_SO_SNDTIMEO) },
    { MP_ROM_QSTR(MP_QSTR_BINDTODEVICE), MP_ROM_INT(ZSOCK_SO_BINDTODEVICE) },
    { MP_ROM_QSTR(MP_QSTR_ACCEPTCONN), MP_ROM_INT(ZSOCK_SO_ACCEPTCONN) },
    { MP_ROM_QSTR(MP_QSTR_TIMESTAMPING), MP_ROM_INT(ZSOCK_SO_TIMESTAMPING) },
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL), MP_ROM_INT(ZSOCK_SO_PROTOCOL) },
    { MP_ROM_QSTR(MP_QSTR_DOMAIN), MP_ROM_INT(ZSOCK_SO_DOMAIN) },
    { MP_ROM_QSTR(MP_QSTR_SOCKS5), MP_ROM_INT(ZSOCK_SO_SOCKS5) },
    { MP_ROM_QSTR(MP_QSTR_TXTIME), MP_ROM_INT(ZSOCK_SO_TXTIME) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SOL_SOCKET, mp_module_socket_globals_SOL_SOCKET_table);

/* Sockopts for TCP level */
static const mp_rom_map_elem_t mp_module_socket_globals_SOL_TCP_table[] = {
    { MP_ROM_QSTR(MP_QSTR_NODELAY), MP_ROM_INT(ZSOCK_TCP_NODELAY) },
    { MP_ROM_QSTR(MP_QSTR_KEEPIDLE), MP_ROM_INT(ZSOCK_TCP_KEEPIDLE) },
    { MP_ROM_QSTR(MP_QSTR_KEEPINTVL), MP_ROM_INT(ZSOCK_TCP_KEEPINTVL) },
    { MP_ROM_QSTR(MP_QSTR_KEEPCNT), MP_ROM_INT(ZSOCK_TCP_KEEPCNT) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SOL_TCP, mp_module_socket_globals_SOL_TCP_table);

/* Sockopts for UDP level */
static const mp_rom_map_elem_t mp_module_socket_globals_SOL_UDP_table[] = {
    { MP_ROM_QSTR(MP_QSTR_OPT), MP_ROM_INT(ZSOCK_UDP_OPT) },
    { MP_ROM_QSTR(MP_QSTR_OPT_OCS), MP_ROM_INT(ZSOCK_UDP_OPT_OCS) },
    { MP_ROM_QSTR(MP_QSTR_OPT_APC), MP_ROM_INT(ZSOCK_UDP_OPT_APC) },
    { MP_ROM_QSTR(MP_QSTR_OPT_FRAG), MP_ROM_INT(ZSOCK_UDP_OPT_FRAG) },
    { MP_ROM_QSTR(MP_QSTR_OPT_MDS), MP_ROM_INT(ZSOCK_UDP_OPT_MDS) },
    { MP_ROM_QSTR(MP_QSTR_OPT_MRDS), MP_ROM_INT(ZSOCK_UDP_OPT_MRDS) },
    { MP_ROM_QSTR(MP_QSTR_OPT_REQ), MP_ROM_INT(ZSOCK_UDP_OPT_REQ) },
    { MP_ROM_QSTR(MP_QSTR_OPT_RES), MP_ROM_INT(ZSOCK_UDP_OPT_RES) },
    { MP_ROM_QSTR(MP_QSTR_OPT_TIME), MP_ROM_INT(ZSOCK_UDP_OPT_TIME) },
    { MP_ROM_QSTR(MP_QSTR_OPT_AUTH), MP_ROM_INT(ZSOCK_UDP_OPT_AUTH) },
    { MP_ROM_QSTR(MP_QSTR_OPT_EXP), MP_ROM_INT(ZSOCK_UDP_OPT_EXP) },
    { MP_ROM_QSTR(MP_QSTR_OPT_UCMP), MP_ROM_INT(ZSOCK_UDP_OPT_UCMP) },
    { MP_ROM_QSTR(MP_QSTR_OPT_UENC), MP_ROM_INT(ZSOCK_UDP_OPT_UENC) },
    { MP_ROM_QSTR(MP_QSTR_OPT_UEXP), MP_ROM_INT(ZSOCK_UDP_OPT_UEXP) },
    { MP_ROM_QSTR(MP_QSTR_OPT_DPLPMTUD), MP_ROM_INT(ZSOCK_UDP_OPT_DPLPMTUD) },
    { MP_ROM_QSTR(MP_QSTR_OPT_DPLPMTUD_APP_RESPOND), MP_ROM_INT(ZSOCK_UDP_OPT_DPLPMTUD_APP_RESPOND) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SOL_UDP, mp_module_socket_globals_SOL_UDP_table);

/* Sockopts for IP level */
static const mp_rom_map_elem_t mp_module_socket_globals_SOL_IP_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TOS), MP_ROM_INT(ZSOCK_IP_TOS) },
    { MP_ROM_QSTR(MP_QSTR_TTL), MP_ROM_INT(ZSOCK_IP_TTL) },
    { MP_ROM_QSTR(MP_QSTR_PKTINFO), MP_ROM_INT(ZSOCK_IP_PKTINFO) },
    { MP_ROM_QSTR(MP_QSTR_RECVTTL), MP_ROM_INT(ZSOCK_IP_RECVTTL) },
    { MP_ROM_QSTR(MP_QSTR_MTU), MP_ROM_INT(ZSOCK_IP_MTU) },
    { MP_ROM_QSTR(MP_QSTR_DONTFRAG), MP_ROM_INT(ZSOCK_IP_DONTFRAG) },
    { MP_ROM_QSTR(MP_QSTR_MULTICAST_IF), MP_ROM_INT(ZSOCK_IP_MULTICAST_IF) },
    { MP_ROM_QSTR(MP_QSTR_MULTICAST_TTL), MP_ROM_INT(ZSOCK_IP_MULTICAST_TTL) },
    { MP_ROM_QSTR(MP_QSTR_MULTICAST_LOOP), MP_ROM_INT(ZSOCK_IP_MULTICAST_LOOP) },
    { MP_ROM_QSTR(MP_QSTR_ADD_MEMBERSHIP), MP_ROM_INT(ZSOCK_IP_ADD_MEMBERSHIP) },
    { MP_ROM_QSTR(MP_QSTR_DROP_MEMBERSHIP), MP_ROM_INT(ZSOCK_IP_DROP_MEMBERSHIP) },
    { MP_ROM_QSTR(MP_QSTR_LOCAL_PORT_RANGE), MP_ROM_INT(ZSOCK_IP_LOCAL_PORT_RANGE) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SOL_IP, mp_module_socket_globals_SOL_IP_table);

/* Sockopts for IPV6 level */

static const mp_rom_map_elem_t mp_module_socket_globals_SO_IPV6_ADDR_PREFERENCES_table[] = {
    { MP_ROM_QSTR(MP_QSTR_MSK_PREFER_SRC_TMP), MP_ROM_INT(ZSOCK_IPV6_PREFER_SRC_TMP) },
    { MP_ROM_QSTR(MP_QSTR_MSK_PREFER_SRC_PUBLIC), MP_ROM_INT(ZSOCK_IPV6_PREFER_SRC_PUBLIC) },
    { MP_ROM_QSTR(MP_QSTR_MSK_PREFER_SRC_PUBTMP_DEFAULT), MP_ROM_INT(ZSOCK_IPV6_PREFER_SRC_PUBTMP_DEFAULT) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SO_IPV6_ADDR_PREFERENCES, mp_module_socket_globals_SO_IPV6_ADDR_PREFERENCES_table);

static const mp_rom_map_elem_t mp_module_socket_globals_SOL_IPV6_table[] = {
    { MP_ROM_QSTR(MP_QSTR_UNICAST_HOPS), MP_ROM_INT(ZSOCK_IPV6_UNICAST_HOPS) },
    { MP_ROM_QSTR(MP_QSTR_MULTICAST_IF), MP_ROM_INT(ZSOCK_IPV6_MULTICAST_IF) },
    { MP_ROM_QSTR(MP_QSTR_MULTICAST_HOPS), MP_ROM_INT(ZSOCK_IPV6_MULTICAST_HOPS) },
    { MP_ROM_QSTR(MP_QSTR_MULTICAST_LOOP), MP_ROM_INT(ZSOCK_IPV6_MULTICAST_LOOP) },
    { MP_ROM_QSTR(MP_QSTR_ADD_MEMBERSHIP), MP_ROM_INT(ZSOCK_IPV6_ADD_MEMBERSHIP) },
    { MP_ROM_QSTR(MP_QSTR_DROP_MEMBERSHIP), MP_ROM_INT(ZSOCK_IPV6_DROP_MEMBERSHIP) },
    { MP_ROM_QSTR(MP_QSTR_JOIN_GROUP), MP_ROM_INT(ZSOCK_IPV6_JOIN_GROUP) },
    { MP_ROM_QSTR(MP_QSTR_LEAVE_GROUP), MP_ROM_INT(ZSOCK_IPV6_LEAVE_GROUP) },
    { MP_ROM_QSTR(MP_QSTR_MTU), MP_ROM_INT(ZSOCK_IPV6_MTU) },
    { MP_ROM_QSTR(MP_QSTR_DONTFRAG), MP_ROM_INT(ZSOCK_IPV6_DONTFRAG) },
    { MP_ROM_QSTR(MP_QSTR_V6ONLY), MP_ROM_INT(ZSOCK_IPV6_V6ONLY) },
    { MP_ROM_QSTR(MP_QSTR_RECVPKTINFO), MP_ROM_INT(ZSOCK_IPV6_RECVPKTINFO) },
    { MP_ROM_QSTR(MP_QSTR_PKTINFO), MP_ROM_INT(ZSOCK_IPV6_PKTINFO) },
    { MP_ROM_QSTR(MP_QSTR_RECVHOPLIMIT), MP_ROM_INT(ZSOCK_IPV6_RECVHOPLIMIT) },
    { MP_ROM_QSTR(MP_QSTR_HOPLIMIT), MP_ROM_INT(ZSOCK_IPV6_HOPLIMIT) },
    { MP_ROM_QSTR(MP_QSTR_ADDR_PREFERENCES), MP_ROM_INT(ZSOCK_IPV6_ADDR_PREFERENCES) },
    { MP_ROM_QSTR(MP_QSTR_IPV6_TCLASS), MP_ROM_INT(ZSOCK_IPV6_TCLASS) },

    { MP_ROM_QSTR(MP_QSTR_ADDR_PREFERENCES_VALUES), MP_ROM_PTR(&mp_module_socket_globals_SO_IPV6_ADDR_PREFERENCES) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SOL_IPV6, mp_module_socket_globals_SOL_IPV6_table);

/* Sockopts for PACKET level */
static const mp_rom_map_elem_t mp_module_socket_globals_SOL_PACKET_table[] = {
    { MP_ROM_QSTR(MP_QSTR_ADD_MEMBERSHIP), MP_ROM_INT(ZSOCK_PACKET_ADD_MEMBERSHIP) },
    { MP_ROM_QSTR(MP_QSTR_DROP_MEMBERSHIP), MP_ROM_INT(ZSOCK_PACKET_DROP_MEMBERSHIP) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SOL_PACKET, mp_module_socket_globals_SOL_PACKET_table);

/* Sockopts for TLS level */

static const mp_rom_map_elem_t mp_module_socket_globals_SO_TLS_MAX_FRAGMENT_LENGTH_table[] = {
    { MP_ROM_QSTR(MP_QSTR_DEFAULT), MP_ROM_INT(ZSOCK_TLS_MFL_DEFAULT) },
    { MP_ROM_QSTR(MP_QSTR_DISABLED), MP_ROM_INT(ZSOCK_TLS_MFL_DISABLED) },
    { MP_ROM_QSTR(MP_QSTR_512), MP_ROM_INT(ZSOCK_TLS_MFL_512) },
    { MP_ROM_QSTR(MP_QSTR_1024), MP_ROM_INT(ZSOCK_TLS_MFL_1024) },
    { MP_ROM_QSTR(MP_QSTR_2048), MP_ROM_INT(ZSOCK_TLS_MFL_2048) },
    { MP_ROM_QSTR(MP_QSTR_4096), MP_ROM_INT(ZSOCK_TLS_MFL_4096) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SO_TLS_MAX_FRAGMENT_LENGTH, mp_module_socket_globals_SO_TLS_MAX_FRAGMENT_LENGTH_table);

static const mp_rom_map_elem_t mp_module_socket_globals_SO_TLS_PEER_VERIFY_table[] = {
    { MP_ROM_QSTR(MP_QSTR_NONE), MP_ROM_INT(ZSOCK_TLS_PEER_VERIFY_NONE) },
    { MP_ROM_QSTR(MP_QSTR_OPTIONAL), MP_ROM_INT(ZSOCK_TLS_PEER_VERIFY_OPTIONAL) },
    { MP_ROM_QSTR(MP_QSTR_REQUIRED), MP_ROM_INT(ZSOCK_TLS_PEER_VERIFY_REQUIRED) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SO_TLS_PEER_VERIFY, mp_module_socket_globals_SO_TLS_PEER_VERIFY_table);

static const mp_rom_map_elem_t mp_module_socket_globals_SO_TLS_DTLS_CID_STATUS_table[] = {
    { MP_ROM_QSTR(MP_QSTR_DISABLED), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_STATUS_DISABLED) },
    { MP_ROM_QSTR(MP_QSTR_DOWNLINK), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_STATUS_DOWNLINK) },
    { MP_ROM_QSTR(MP_QSTR_UPLINK), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_STATUS_UPLINK) },
    { MP_ROM_QSTR(MP_QSTR_BIDIRECTIONAL), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_STATUS_BIDIRECTIONAL) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SO_TLS_DTLS_CID_STATUS, mp_module_socket_globals_SO_TLS_DTLS_CID_STATUS_table);

static const mp_rom_map_elem_t mp_module_socket_globals_SO_TLS_DTLS_CID_table[] = {
    { MP_ROM_QSTR(MP_QSTR_DISABLED), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_DISABLED) },
    { MP_ROM_QSTR(MP_QSTR_SUPPORTED), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_SUPPORTED) },
    { MP_ROM_QSTR(MP_QSTR_ENABLED), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_ENABLED) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SO_TLS_DTLS_CID, mp_module_socket_globals_SO_TLS_DTLS_CID_table);

static const mp_rom_map_elem_t mp_module_socket_globals_SOL_TLS_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SEC_TAG_LIST), MP_ROM_INT(ZSOCK_TLS_SEC_TAG_LIST) },
    { MP_ROM_QSTR(MP_QSTR_HOSTNAME), MP_ROM_INT(ZSOCK_TLS_HOSTNAME) },
    { MP_ROM_QSTR(MP_QSTR_CIPHERSUITE_LIST), MP_ROM_INT(ZSOCK_TLS_CIPHERSUITE_LIST) },
    { MP_ROM_QSTR(MP_QSTR_CIPHERSUITE_USED), MP_ROM_INT(ZSOCK_TLS_CIPHERSUITE_USED) },
    { MP_ROM_QSTR(MP_QSTR_PEER_VERIFY), MP_ROM_INT(ZSOCK_TLS_PEER_VERIFY) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_ROLE), MP_ROM_INT(ZSOCK_TLS_DTLS_ROLE) },
    { MP_ROM_QSTR(MP_QSTR_ALPN_LIST), MP_ROM_INT(ZSOCK_TLS_ALPN_LIST) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_HANDSHAKE_TIMEOUT_MIN), MP_ROM_INT(ZSOCK_TLS_DTLS_HANDSHAKE_TIMEOUT_MIN) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_HANDSHAKE_TIMEOUT_MAX), MP_ROM_INT(ZSOCK_TLS_DTLS_HANDSHAKE_TIMEOUT_MAX) },
    { MP_ROM_QSTR(MP_QSTR_CERT_NOCOPY), MP_ROM_INT(ZSOCK_TLS_CERT_NOCOPY) },
    { MP_ROM_QSTR(MP_QSTR_NATIVE), MP_ROM_INT(ZSOCK_TLS_NATIVE) },
    { MP_ROM_QSTR(MP_QSTR_SESSION_CACHE), MP_ROM_INT(ZSOCK_TLS_SESSION_CACHE) },
    { MP_ROM_QSTR(MP_QSTR_SESSION_CACHE_PURGE), MP_ROM_INT(ZSOCK_TLS_SESSION_CACHE_PURGE) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_CID), MP_ROM_INT(ZSOCK_TLS_DTLS_CID) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_CID_STATUS), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_STATUS) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_CID_VALUE), MP_ROM_INT(ZSOCK_TLS_DTLS_CID_VALUE) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_PEER_CID_VALUE), MP_ROM_INT(ZSOCK_TLS_DTLS_PEER_CID_VALUE) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_HANDSHAKE_ON_CONNECT), MP_ROM_INT(ZSOCK_TLS_DTLS_HANDSHAKE_ON_CONNECT) },
    { MP_ROM_QSTR(MP_QSTR_CERT_VERIFY_RESULT), MP_ROM_INT(ZSOCK_TLS_CERT_VERIFY_RESULT) },
    { MP_ROM_QSTR(MP_QSTR_CERT_VERIFY_CALLBACK), MP_ROM_INT(ZSOCK_TLS_CERT_VERIFY_CALLBACK) },
    { MP_ROM_QSTR(MP_QSTR_MAX_FRAGMENT_LENGTH), MP_ROM_INT(ZSOCK_TLS_MAX_FRAGMENT_LENGTH) },

    { MP_ROM_QSTR(MP_QSTR_MAX_FRAGMENT_LENGTH_VALUES), MP_ROM_PTR(&mp_module_socket_globals_SO_TLS_MAX_FRAGMENT_LENGTH) },
    { MP_ROM_QSTR(MP_QSTR_PEER_VERIFY_VALUES), MP_ROM_PTR(&mp_module_socket_globals_SO_TLS_PEER_VERIFY) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_CID_STATUS_VALUES), MP_ROM_PTR(&mp_module_socket_globals_SO_TLS_DTLS_CID_STATUS) },
    { MP_ROM_QSTR(MP_QSTR_DTLS_CID_VALUES), MP_ROM_PTR(&mp_module_socket_globals_SO_TLS_DTLS_CID) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SOL_TLS, mp_module_socket_globals_SOL_TLS_table);

/* Sockopts for QUIC level */
static const mp_rom_map_elem_t mp_module_socket_globals_SOL_QUIC_table[] = {
    { MP_ROM_QSTR(MP_QSTR_STREAM_TYPE), MP_ROM_INT(ZSOCK_QUIC_SO_STREAM_TYPE) },
    { MP_ROM_QSTR(MP_QSTR_CERT_CHAIN_ADD), MP_ROM_INT(ZSOCK_QUIC_SO_CERT_CHAIN_ADD) },
    { MP_ROM_QSTR(MP_QSTR_CERT_CHAIN_DEL), MP_ROM_INT(ZSOCK_QUIC_SO_CERT_CHAIN_DEL) },
    { MP_ROM_QSTR(MP_QSTR_STOP_SENDING_CODE), MP_ROM_INT(ZSOCK_QUIC_SO_STOP_SENDING_CODE) },
    { MP_ROM_QSTR(MP_QSTR_SESSION_STATE), MP_ROM_INT(ZSOCK_QUIC_SO_SESSION_STATE) },
    { MP_ROM_QSTR(MP_QSTR_SESSION_TICKET_ENABLE), MP_ROM_INT(ZSOCK_QUIC_SO_SESSION_TICKET_ENABLE) },
    { MP_ROM_QSTR(MP_QSTR_MAX_EARLY_DATA_SIZE), MP_ROM_INT(ZSOCK_QUIC_SO_MAX_EARLY_DATA_SIZE) },
    { MP_ROM_QSTR(MP_QSTR_STREAM_EARLY_DATA), MP_ROM_INT(ZSOCK_QUIC_SO_STREAM_EARLY_DATA) },
};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals_SOL_QUIC, mp_module_socket_globals_SOL_QUIC_table);

static const mp_rom_map_elem_t mp_module_socket_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_socket) },
    /* Objects */
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&zephyr_socket_type) },

    /* Module functions */
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&mod_socket_getaddrinfo_obj) },
    #if defined(CONFIG_NET_BUF_POOL_USAGE)
    { MP_ROM_QSTR(MP_QSTR_pkt_get_info), MP_ROM_PTR(&mod_socket_pkt_get_info_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_print_available), MP_ROM_PTR(&mod_socket_print_available_obj) },

    /* Constants */
    { MP_ROM_QSTR(MP_QSTR_af_available), MP_ROM_PTR(&mod_socket_af_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_proto_available), MP_ROM_PTR(&mod_socket_proto_available_obj) },
    /* Address families */
    { MP_ROM_QSTR(MP_QSTR_AF_UNSPEC), MP_ROM_INT(NET_AF_UNSPEC) },
    { MP_ROM_QSTR(MP_QSTR_AF_LOCAL), MP_ROM_INT(NET_AF_LOCAL) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(NET_AF_INET) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET6), MP_ROM_INT(NET_AF_INET6) },
    { MP_ROM_QSTR(MP_QSTR_AF_PACKET), MP_ROM_INT(NET_AF_PACKET) },
    { MP_ROM_QSTR(MP_QSTR_AF_CAN), MP_ROM_INT(NET_AF_CAN) },
    { MP_ROM_QSTR(MP_QSTR_AF_NET_MGMT), MP_ROM_INT(NET_AF_NET_MGMT) },

    /* Socket types */
    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(NET_SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(NET_SOCK_DGRAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(NET_SOCK_RAW) },

    /* Socket protocols */
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IP), MP_ROM_INT(NET_IPPROTO_IP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_ICMP), MP_ROM_INT(NET_IPPROTO_ICMP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IGMP), MP_ROM_INT(NET_IPPROTO_IGMP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_ETH_P_ALL), MP_ROM_INT(NET_IPPROTO_ETH_P_ALL) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IPIP), MP_ROM_INT(NET_IPPROTO_IPIP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TCP), MP_ROM_INT(NET_IPPROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_UDP), MP_ROM_INT(NET_IPPROTO_UDP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IPV6), MP_ROM_INT(NET_IPPROTO_IPV6) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_ICMPV6), MP_ROM_INT(NET_IPPROTO_ICMPV6) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_RAW), MP_ROM_INT(NET_IPPROTO_RAW) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TLS_1_0), MP_ROM_INT(NET_IPPROTO_TLS_1_0) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TLS_1_1), MP_ROM_INT(NET_IPPROTO_TLS_1_1) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TLS_1_2), MP_ROM_INT(NET_IPPROTO_TLS_1_2) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TLS_1_3), MP_ROM_INT(NET_IPPROTO_TLS_1_3) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_DTLS_1_0), MP_ROM_INT(NET_IPPROTO_DTLS_1_0) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_DTLS_1_2), MP_ROM_INT(NET_IPPROTO_DTLS_1_2) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_QUIC), MP_ROM_INT(NET_IPPROTO_QUIC) },

    /* Sock Options */
    /* Sockopt level */
    { MP_ROM_QSTR(MP_QSTR_SOL_SOCKET), MP_ROM_INT(ZSOCK_SOL_SOCKET) },
    { MP_ROM_QSTR(MP_QSTR_SOL_TCP), MP_ROM_INT(NET_IPPROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_SOL_UDP), MP_ROM_INT(NET_IPPROTO_UDP) },
    { MP_ROM_QSTR(MP_QSTR_SOL_IP), MP_ROM_INT(NET_IPPROTO_IP) },
    { MP_ROM_QSTR(MP_QSTR_SOL_IPV6), MP_ROM_INT(NET_IPPROTO_IPV6) },
    { MP_ROM_QSTR(MP_QSTR_SOL_PACKET), MP_ROM_INT(ZSOCK_SOL_PACKET) },
    { MP_ROM_QSTR(MP_QSTR_SOL_TLS), MP_ROM_INT(ZSOCK_SOL_TLS) },
    { MP_ROM_QSTR(MP_QSTR_SOL_QUIC), MP_ROM_INT(ZSOCK_SOL_QUIC) },

    /* Sockopts */
    { MP_ROM_QSTR(MP_QSTR_SO_SOCKET), MP_ROM_PTR(&mp_module_socket_globals_SOL_SOCKET) },
    { MP_ROM_QSTR(MP_QSTR_SO_TCP), MP_ROM_PTR(&mp_module_socket_globals_SOL_TCP) },
    { MP_ROM_QSTR(MP_QSTR_SO_UDP), MP_ROM_PTR(&mp_module_socket_globals_SOL_UDP) },
    { MP_ROM_QSTR(MP_QSTR_SO_IP), MP_ROM_PTR(&mp_module_socket_globals_SOL_IP) },
    { MP_ROM_QSTR(MP_QSTR_SO_IPV6), MP_ROM_PTR(&mp_module_socket_globals_SOL_IPV6) },
    { MP_ROM_QSTR(MP_QSTR_SO_PACKET), MP_ROM_PTR(&mp_module_socket_globals_SOL_PACKET) },
    { MP_ROM_QSTR(MP_QSTR_SO_TLS), MP_ROM_PTR(&mp_module_socket_globals_SOL_TLS) },
    { MP_ROM_QSTR(MP_QSTR_SO_QUIC), MP_ROM_PTR(&mp_module_socket_globals_SOL_QUIC) },

    /* Sockopts for compatibility */
    { MP_ROM_QSTR(MP_QSTR_SO_REUSEADDR), MP_ROM_INT(ZSOCK_SO_REUSEADDR) },
    { MP_ROM_QSTR(MP_QSTR_SO_BROADCAST), MP_ROM_INT(ZSOCK_SO_BROADCAST) },
    { MP_ROM_QSTR(MP_QSTR_SO_KEEPALIVE), MP_ROM_INT(ZSOCK_SO_KEEPALIVE) },
    { MP_ROM_QSTR(MP_QSTR_SO_SNDTIMEO), MP_ROM_INT(ZSOCK_SO_SNDTIMEO) },
    { MP_ROM_QSTR(MP_QSTR_SO_RCVTIMEO), MP_ROM_INT(ZSOCK_SO_RCVTIMEO) },

};

static MP_DEFINE_CONST_DICT(mp_module_socket_globals, mp_module_socket_globals_table);

const mp_obj_module_t mp_module_socket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_socket_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_socket, mp_module_socket);

#endif // MICROPY_PY_SOCKET
