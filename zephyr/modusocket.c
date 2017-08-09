/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Linaro Limited
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
#ifdef MICROPY_PY_USOCKET

#include "py/runtime.h"
#include "py/stream.h"

#include <stdio.h>
#include <zephyr.h>
// Zephyr's generated version header
#include <version.h>
#include <net/net_context.h>
#include <net/net_pkt.h>
#include <net/dns_resolve.h>
#ifdef CONFIG_NET_SOCKETS
#include <net/socket.h>
#endif

#define DEBUG_PRINT 0
#if DEBUG_PRINT // print debugging info
#define DEBUG_printf printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

typedef struct _socket_obj_t {
    mp_obj_base_t base;
    struct net_context *ctx;
    #ifndef CONFIG_NET_SOCKETS
    union {
        struct k_fifo recv_q;
        struct k_fifo accept_q;
    };
    #endif

    #define STATE_NEW 0
    #define STATE_CONNECTING 1
    #define STATE_CONNECTED 2
    #define STATE_PEER_CLOSED 3
    int8_t state;
} socket_obj_t;

STATIC const mp_obj_type_t socket_type;

#ifdef CONFIG_NET_SOCKETS
#define SOCK_FIELD(ptr, field) ((ptr)->ctx->field)
#else
#define SOCK_FIELD(ptr, field) ((ptr)->field)
#endif

// k_fifo extended API

static inline void *_k_fifo_peek_head(struct k_fifo *fifo)
{
#if KERNEL_VERSION_NUMBER < 0x010763 /* 1.7.99 */
    return sys_slist_peek_head(&fifo->data_q);
#else
    return sys_slist_peek_head(&fifo->_queue.data_q);
#endif
}

static inline void *_k_fifo_peek_tail(struct k_fifo *fifo)
{
#if KERNEL_VERSION_NUMBER < 0x010763 /* 1.7.99 */
    return sys_slist_peek_tail(&fifo->data_q);
#else
    return sys_slist_peek_tail(&fifo->_queue.data_q);
#endif
}

static inline void _k_fifo_wait_non_empty(struct k_fifo *fifo, int32_t timeout)
{
    struct k_poll_event events[] = {
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_FIFO_DATA_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, fifo),
    };

    k_poll(events, MP_ARRAY_SIZE(events), timeout);
    DEBUG_printf("poll res: %d\n", events[0].state);
}

// Helper functions

#define RAISE_ERRNO(x) { int _err = x; if (_err < 0) mp_raise_OSError(-_err); }
#define RAISE_SOCK_ERRNO(x) { if ((int)(x) == -1) mp_raise_OSError(errno); }

STATIC void socket_check_closed(socket_obj_t *socket) {
    if (socket->ctx == NULL) {
        // already closed
        mp_raise_OSError(EBADF);
    }
}

STATIC void parse_inet_addr(socket_obj_t *socket, mp_obj_t addr_in, struct sockaddr *sockaddr) {
    // We employ the fact that port and address offsets are the same for IPv4 & IPv6
    struct sockaddr_in *sockaddr_in = (struct sockaddr_in*)sockaddr;

    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
    sockaddr_in->sin_family = net_context_get_family(socket->ctx);
    RAISE_ERRNO(net_addr_pton(sockaddr_in->sin_family, mp_obj_str_get_str(addr_items[0]), &sockaddr_in->sin_addr));
    sockaddr_in->sin_port = htons(mp_obj_get_int(addr_items[1]));
}

STATIC mp_obj_t format_inet_addr(struct sockaddr *addr, mp_obj_t port) {
    // We employ the fact that port and address offsets are the same for IPv4 & IPv6
    struct sockaddr_in6 *sockaddr_in6 = (struct sockaddr_in6*)addr;
    char buf[40];
    net_addr_ntop(addr->family, &sockaddr_in6->sin6_addr, buf, sizeof(buf));
    mp_obj_tuple_t *tuple = mp_obj_new_tuple(addr->family == AF_INET ? 2 : 4, NULL);

    tuple->items[0] = mp_obj_new_str(buf, strlen(buf), false);
    // We employ the fact that port offset is the same for IPv4 & IPv6
    // not filled in
    //tuple->items[1] = mp_obj_new_int(ntohs(((struct sockaddr_in*)addr)->sin_port));
    tuple->items[1] = port;

    if (addr->family == AF_INET6) {
        tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0); // flow_info
        tuple->items[3] = MP_OBJ_NEW_SMALL_INT(sockaddr_in6->sin6_scope_id);
    }

    return MP_OBJ_FROM_PTR(tuple);
}

// Copy data from Zephyr net_buf chain into linear buffer.
// We don't use net_pkt_read(), because it's weird (e.g., we'd like to
// free processed data fragment ASAP, while net_pkt_read() holds onto
// the whole fragment chain to do its deeds, and that's minor comparing
// to the fact that it copies data byte by byte).
static char *net_pkt_gather(struct net_pkt *pkt, char *to, unsigned max_len) {
    struct net_buf *tmp = pkt->frags;

    while (tmp && max_len) {
        unsigned len = tmp->len;
        if (len > max_len) {
            len = max_len;
        }
        memcpy(to, tmp->data, len);
        to += len;
        max_len -= len;
        tmp = net_pkt_frag_del(pkt, NULL, tmp);
    }

    return to;
}

// Callback for incoming packets.
static void sock_received_cb(struct net_context *context, struct net_pkt *pkt, int status, void *user_data) {
    socket_obj_t *socket = (socket_obj_t*)user_data;
    DEBUG_printf("recv cb: context: %p, status: %d, pkt: %p", context, status, pkt);
    if (pkt) {
        DEBUG_printf(" (appdatalen=%d), token: %p", pkt->appdatalen, net_pkt_token(pkt));
    }
    DEBUG_printf("\n");
    #if DEBUG_PRINT > 1
    net_pkt_print_frags(pkt);
    #endif

    // if net_buf == NULL, EOF
    if (pkt == NULL) {
        struct net_pkt *last_pkt = _k_fifo_peek_tail(&SOCK_FIELD(socket, recv_q));
        if (last_pkt == NULL) {
            socket->state = STATE_PEER_CLOSED;
            k_fifo_cancel_wait(&SOCK_FIELD(socket, recv_q));
            DEBUG_printf("Marked socket %p as peer-closed\n", socket);
        } else {
            // We abuse "buf_sent" flag to store EOF flag
            net_pkt_set_sent(last_pkt, true);
            DEBUG_printf("Set EOF flag on %p\n", last_pkt);
        }
        return;
    }

    // Make sure that "EOF flag" is not set
    net_pkt_set_sent(pkt, false);

    // We don't care about packet header, so get rid of it asap
    unsigned header_len = net_pkt_appdata(pkt) - pkt->frags->data;
    net_buf_pull(pkt->frags, header_len);

    k_fifo_put(&SOCK_FIELD(socket, recv_q), pkt);
}

// Callback for incoming connections.
static void sock_accepted_cb(struct net_context *new_ctx, struct sockaddr *addr, socklen_t addrlen, int status, void *user_data) {
    socket_obj_t *socket = (socket_obj_t*)user_data;
    DEBUG_printf("accept cb: context: %p, status: %d, new ctx: %p\n", socket->ctx, status, new_ctx);
    DEBUG_printf("new_ctx ref_cnt: %d\n", new_ctx->refcount);

    k_fifo_put(&SOCK_FIELD(socket, accept_q), new_ctx);
}

socket_obj_t *socket_new(void) {
    socket_obj_t *socket = m_new_obj_with_finaliser(socket_obj_t);
    socket->base.type = (mp_obj_t)&socket_type;
    socket->state = STATE_NEW;
    return socket;
}

// Methods

STATIC void socket_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    socket_obj_t *self = self_in;
    if (self->ctx == NULL) {
        mp_printf(print, "<socket NULL>");
    } else {
        struct net_context *ctx = self->ctx;
        mp_printf(print, "<socket %p type=%d>", ctx, net_context_get_type(ctx));
    }
}

STATIC mp_obj_t socket_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, false);

    socket_obj_t *socket = socket_new();

    int family = AF_INET;
    int socktype = SOCK_STREAM;
    int proto = -1;

    if (n_args >= 1) {
        family = mp_obj_get_int(args[0]);
        if (n_args >= 2) {
            socktype = mp_obj_get_int(args[1]);
            if (n_args >= 3) {
                proto = mp_obj_get_int(args[2]);
            }
        }
    }

    if (proto == -1) {
        proto = IPPROTO_TCP;
        if (socktype != SOCK_STREAM) {
            proto = IPPROTO_UDP;
        }
    }

    #ifdef CONFIG_NET_SOCKETS
    socket->ctx = (void*)zsock_socket(family, socktype, proto);
    RAISE_SOCK_ERRNO(socket->ctx);
    #else
    RAISE_ERRNO(net_context_get(family, socktype, proto, &socket->ctx));
    k_fifo_init(&SOCK_FIELD(socket, recv_q));
    #endif

    return MP_OBJ_FROM_PTR(socket);
}

STATIC mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    socket_obj_t *socket = self_in;
    socket_check_closed(socket);

    struct sockaddr sockaddr;
    parse_inet_addr(socket, addr_in, &sockaddr);

    int res = zsock_bind(socket->ctx, &sockaddr, sizeof(sockaddr));
    RAISE_SOCK_ERRNO(res);

    // For DGRAM socket, we expect to receive packets after call to bind(),
    // but for STREAM socket, next expected operation is listen(), which
    // doesn't work if recv callback is set.
    if (net_context_get_type(socket->ctx) == SOCK_DGRAM) {
        DEBUG_printf("Setting recv cb after bind\n");
        RAISE_ERRNO(net_context_recv(socket->ctx, sock_received_cb, K_NO_WAIT, socket));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

STATIC mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    socket_obj_t *socket = self_in;
    socket_check_closed(socket);

    struct sockaddr sockaddr;
    parse_inet_addr(socket, addr_in, &sockaddr);

    int res = zsock_connect(socket->ctx, &sockaddr, sizeof(sockaddr));
    RAISE_SOCK_ERRNO(res);

    DEBUG_printf("Setting recv cb after connect()\n");
    RAISE_ERRNO(net_context_recv(socket->ctx, sock_received_cb, K_NO_WAIT, socket));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

STATIC mp_obj_t socket_listen(mp_obj_t self_in, mp_obj_t backlog_in) {
    socket_obj_t *socket = self_in;
    socket_check_closed(socket);

    mp_int_t backlog = mp_obj_get_int(backlog_in);
    int res = zsock_listen(socket->ctx, backlog);
    RAISE_SOCK_ERRNO(res);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_listen_obj, socket_listen);

STATIC mp_obj_t socket_accept(mp_obj_t self_in) {
    socket_obj_t *socket = self_in;
    socket_check_closed(socket);

    struct sockaddr sockaddr;
    socklen_t addrlen = sizeof(sockaddr);
    void *ctx = zsock_accept(socket->ctx, &sockaddr, &addrlen);

    socket_obj_t *socket2 = socket_new();
    socket2->ctx = ctx;
    DEBUG_printf("Setting recv cb after accept()\n");
    RAISE_ERRNO(net_context_recv(ctx, sock_received_cb, K_NO_WAIT, socket2));

    mp_obj_tuple_t *client = mp_obj_new_tuple(2, NULL);
    client->items[0] = MP_OBJ_FROM_PTR(socket2);
    // TODO
    client->items[1] = mp_const_none;

    return MP_OBJ_FROM_PTR(client);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

STATIC mp_uint_t sock_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    socket_obj_t *socket = self_in;
    if (socket->ctx == -1) {
        // already closed
        *errcode = EBADF;
        return MP_STREAM_ERROR;
    }

    ssize_t len = zsock_send(socket->ctx, buf, size, 0);
    if (len == -1) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }

    return len;
}

STATIC mp_obj_t socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    int err = 0;
    mp_uint_t len = sock_write(self_in, bufinfo.buf, bufinfo.len, &err);
    if (len == MP_STREAM_ERROR) {
        mp_raise_OSError(err);
    }
    return mp_obj_new_int_from_uint(len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, socket_send);

STATIC mp_uint_t sock_read(mp_obj_t self_in, void *buf, mp_uint_t max_len, int *errcode) {
    socket_obj_t *socket = self_in;
    if (socket->ctx == NULL) {
        // already closed
        *errcode = EBADF;
        return MP_STREAM_ERROR;
    }

    enum net_sock_type sock_type = net_context_get_type(socket->ctx);
    unsigned recv_len;

    if (sock_type == SOCK_DGRAM) {

        struct net_pkt *pkt = k_fifo_get(&SOCK_FIELD(socket, recv_q), K_FOREVER);

        recv_len = net_pkt_appdatalen(pkt);
        DEBUG_printf("recv: pkt=%p, appdatalen: %d\n", pkt, recv_len);

        if (recv_len > max_len) {
            recv_len = max_len;
        }

        net_pkt_gather(pkt, buf, recv_len);
        net_pkt_unref(pkt);

    } else if (sock_type == SOCK_STREAM) {

        do {

            if (socket->state == STATE_PEER_CLOSED) {
                return 0;
            }

            _k_fifo_wait_non_empty(&SOCK_FIELD(socket, recv_q), K_FOREVER);
            struct net_pkt *pkt = _k_fifo_peek_head(&SOCK_FIELD(socket, recv_q));
            if (pkt == NULL) {
                DEBUG_printf("TCP recv: NULL return from fifo\n");
                continue;
            }

            DEBUG_printf("TCP recv: cur_pkt: %p\n", pkt);

            struct net_buf *frag = pkt->frags;
            if (frag == NULL) {
                printf("net_pkt has empty fragments on start!\n");
                assert(0);
            }

            unsigned frag_len = frag->len;
            recv_len = frag_len;
            if (recv_len > max_len) {
                recv_len = max_len;
            }
            DEBUG_printf("%d data bytes in head frag, going to read %d\n", frag_len, recv_len);

            memcpy(buf, frag->data, recv_len);

            if (recv_len != frag_len) {
                net_buf_pull(frag, recv_len);
            } else {
                frag = net_pkt_frag_del(pkt, NULL, frag);
                if (frag == NULL) {
                    DEBUG_printf("Finished processing pkt %p\n", pkt);
                    // Drop head packet from queue
                    k_fifo_get(&SOCK_FIELD(socket, recv_q), K_NO_WAIT);

                    // If "sent" flag was set, it's last packet and we reached EOF
                    if (net_pkt_sent(pkt)) {
                        socket->state = STATE_PEER_CLOSED;
                    }
                    net_pkt_unref(pkt);
                }
            }
        // Keep repeating while we're getting empty fragments
        // Zephyr IP stack appears to have fed empty net_buf's with empty
        // frags for various TCP control packets - in previous versions.
        } while (recv_len == 0);
    }

    return recv_len;
}

STATIC mp_obj_t socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    mp_int_t max_len = mp_obj_get_int(len_in);
    vstr_t vstr;
    // +1 to accommodate for trailing \0
    vstr_init_len(&vstr, max_len + 1);

    int err;
    mp_uint_t len = sock_read(self_in, vstr.buf, max_len, &err);

    if (len == MP_STREAM_ERROR) {
        vstr_clear(&vstr);
        mp_raise_OSError(err);
    }

    if (len == 0) {
        vstr_clear(&vstr);
        return mp_const_empty_bytes;
    }

    vstr.len = len;
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recv_obj, socket_recv);

STATIC mp_obj_t socket_setsockopt(size_t n_args, const mp_obj_t *args) {
    (void)n_args; // always 4
    mp_warning("setsockopt() not implemented");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_setsockopt_obj, 4, 4, socket_setsockopt);

STATIC mp_obj_t socket_makefile(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_makefile_obj, 1, 3, socket_makefile);

STATIC mp_obj_t socket_close(mp_obj_t self_in) {
    socket_obj_t *socket = self_in;
    if (socket->ctx != -1) {
        int res = zsock_close(socket->ctx);
        RAISE_SOCK_ERRNO(res);
        socket->ctx = -1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_close_obj, socket_close);

STATIC const mp_map_elem_t socket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind), (mp_obj_t)&socket_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&socket_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listen), (mp_obj_t)&socket_listen_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accept), (mp_obj_t)&socket_accept_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&socket_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&socket_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setsockopt), (mp_obj_t)&socket_setsockopt_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), (mp_obj_t)&mp_stream_readinto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_makefile), (mp_obj_t)&socket_makefile_obj },
};
STATIC MP_DEFINE_CONST_DICT(socket_locals_dict, socket_locals_dict_table);

STATIC const mp_stream_p_t socket_stream_p = {
    .read = sock_read,
    .write = sock_write,
    //.ioctl = sock_ioctl,
};

STATIC const mp_obj_type_t socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .print = socket_print,
    .make_new = socket_make_new,
    .protocol = &socket_stream_p,
    .locals_dict = (mp_obj_t)&socket_locals_dict,
};

//
// getaddrinfo() implementation
//

typedef struct _getaddrinfo_state_t {
    mp_obj_t result;
    struct k_sem sem;
    mp_obj_t port;
    int status;
} getaddrinfo_state_t;

void dns_resolve_cb(enum dns_resolve_status status, struct dns_addrinfo *info, void *user_data) {
    getaddrinfo_state_t *state = user_data;
    DEBUG_printf("dns status: %d\n", status);

    if (info == NULL) {
        if (status == DNS_EAI_ALLDONE) {
            status = 0;
        }
        state->status = status;
        k_sem_give(&state->sem);
        return;
    }

    mp_obj_tuple_t *tuple = mp_obj_new_tuple(5, NULL);
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(info->ai_family);
    // info->ai_socktype not filled
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(SOCK_STREAM);
    // info->ai_protocol not filled
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(IPPROTO_TCP);
    tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    tuple->items[4] = format_inet_addr(&info->ai_addr, state->port);
    mp_obj_list_append(state->result, MP_OBJ_FROM_PTR(tuple));
}

STATIC mp_obj_t mod_getaddrinfo(size_t n_args, const mp_obj_t *args) {
    mp_obj_t host_in = args[0], port_in = args[1];
    const char *host = mp_obj_str_get_str(host_in);
    mp_int_t family = 0;
    if (n_args > 2) {
        family = mp_obj_get_int(args[2]);
    }

    getaddrinfo_state_t state;
    // Just validate that it's int
    (void)mp_obj_get_int(port_in);
    state.port = port_in;
    state.result = mp_obj_new_list(0, NULL);
    k_sem_init(&state.sem, 0, UINT_MAX);

    for (int i = 2; i--;) {
        int type = (family != AF_INET6 ? DNS_QUERY_TYPE_A : DNS_QUERY_TYPE_AAAA);
        RAISE_ERRNO(dns_get_addr_info(host, type, NULL, dns_resolve_cb, &state, 3000));
        k_sem_take(&state.sem, K_FOREVER);
        if (family != 0) {
            break;
        }
        family = AF_INET6;
    }

    // Raise error only if there's nothing to return, otherwise
    // it may be IPv4 vs IPv6 differences.
    mp_int_t len = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(state.result));
    if (state.status != 0 && len == 0) {
        mp_raise_OSError(state.status);
    }

    return state.result;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_getaddrinfo_obj, 2, 3, mod_getaddrinfo);


STATIC mp_obj_t pkt_get_info(void) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pkt_get_info_obj, pkt_get_info);

STATIC const mp_map_elem_t mp_module_usocket_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_usocket) },
    // objects
    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&socket_type },
    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET), MP_OBJ_NEW_SMALL_INT(AF_INET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET6), MP_OBJ_NEW_SMALL_INT(AF_INET6) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_STREAM), MP_OBJ_NEW_SMALL_INT(SOCK_STREAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_DGRAM), MP_OBJ_NEW_SMALL_INT(SOCK_DGRAM) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SOL_SOCKET), MP_OBJ_NEW_SMALL_INT(1) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SO_REUSEADDR), MP_OBJ_NEW_SMALL_INT(2) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_getaddrinfo), (mp_obj_t)&mod_getaddrinfo_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pkt_get_info), (mp_obj_t)&pkt_get_info_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_usocket_globals, mp_module_usocket_globals_table);

const mp_obj_module_t mp_module_usocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_usocket_globals,
};

#endif // MICROPY_PY_USOCKET
