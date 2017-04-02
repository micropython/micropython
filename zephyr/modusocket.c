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

#include <stdio.h>
#include <zephyr.h>
#include <net/net_context.h>
#include <net/nbuf.h>

#if 0 // print debugging info
#define DEBUG_printf printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

typedef struct _socket_obj_t {
    mp_obj_base_t base;
    struct net_context *ctx;
    struct k_fifo recv_q;
    struct net_buf *cur_buf;

    #define STATE_NEW 0
    #define STATE_CONNECTING 1
    #define STATE_CONNECTED 2
    #define STATE_PEER_CLOSED 3
    int8_t state;
} socket_obj_t;

STATIC const mp_obj_type_t socket_type;

// Helper functions

#define RAISE_ERRNO(x) { int _err = x; if (_err < 0) mp_raise_OSError(-_err); }

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

// Copy data from Zephyr net_buf chain into linear buffer.
// We don't use net_nbuf_read(), because it's weird (e.g., we'd like to
// free processed data fragment ASAP, while net_nbuf_read() holds onto
// the whole fragment chain to do its deeds, and that's minor comparing
// to the fact that it copies data byte by byte).
static char *net_buf_gather(struct net_buf *buf, char *to, unsigned max_len) {
    struct net_buf *tmp = buf->frags;
    unsigned header_len = net_nbuf_appdata(buf) - tmp->data;
    net_buf_pull(tmp, header_len);

    while (tmp && max_len) {
        unsigned len = tmp->len;
        if (len > max_len) {
            len = max_len;
        }
        memcpy(to, tmp->data, len);
        to += len;
        max_len -= len;
        tmp = net_buf_frag_del(buf, tmp);
    }

    return to;
}

// Callback for incoming packets.
static void sock_received_cb(struct net_context *context, struct net_buf *net_buf, int status, void *user_data) {
    socket_obj_t *socket = (socket_obj_t*)user_data;
    DEBUG_printf("recv cb: context: %p, status: %d, buf: %p", context, status, net_buf);
    if (net_buf) {
        DEBUG_printf(" (sz=%d, l=%d), token: %p", net_buf->size, net_buf->len, net_nbuf_token(net_buf));
    }
    DEBUG_printf("\n");

    // if net_buf == NULL, EOF
    if (net_buf == NULL) {
        // TODO: k_fifo accessor for this?
        struct net_buf *last_buf = (struct net_buf*)sys_slist_peek_tail(&socket->recv_q.data_q);
        // We abuse "buf_sent" flag to store EOF flag
        net_nbuf_set_buf_sent(last_buf, true);
        DEBUG_printf("Set EOF flag on %p\n", last_buf);
        return;
    }

    // Make sure that "EOF flag" is not set
    net_nbuf_set_buf_sent(net_buf, false);

    // net_buf->frags will be overwritten by fifo, so save it
    net_nbuf_set_token(net_buf, net_buf->frags);
    k_fifo_put(&socket->recv_q, net_buf);
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

    socket_obj_t *socket = m_new_obj_with_finaliser(socket_obj_t);
    socket->base.type = type;
    k_fifo_init(&socket->recv_q);
    socket->cur_buf = NULL;
    socket->state = STATE_NEW;

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

    RAISE_ERRNO(net_context_get(family, socktype, proto, &socket->ctx));

    return socket;
}

STATIC mp_obj_t socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    socket_obj_t *socket = self_in;
    socket_check_closed(socket);

    struct sockaddr sockaddr;
    parse_inet_addr(socket, addr_in, &sockaddr);

    RAISE_ERRNO(net_context_bind(socket->ctx, &sockaddr, sizeof(sockaddr)));
    DEBUG_printf("Setting recv cb after bind\n");
    RAISE_ERRNO(net_context_recv(socket->ctx, sock_received_cb, K_NO_WAIT, socket));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

STATIC mp_obj_t socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    socket_obj_t *socket = self_in;
    socket_check_closed(socket);

    struct sockaddr sockaddr;
    parse_inet_addr(socket, addr_in, &sockaddr);

    RAISE_ERRNO(net_context_connect(socket->ctx, &sockaddr, sizeof(sockaddr), NULL, K_FOREVER, NULL));
    DEBUG_printf("Setting recv cb after connect()\n");
    RAISE_ERRNO(net_context_recv(socket->ctx, sock_received_cb, K_NO_WAIT, socket));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

STATIC mp_obj_t socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    socket_obj_t *socket = self_in;
    socket_check_closed(socket);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    struct net_buf *send_buf = net_nbuf_get_tx(socket->ctx, K_FOREVER);
    // TODO: Probably should limit how much data we send in one call still
    if (!net_nbuf_append(send_buf, bufinfo.len, bufinfo.buf, K_FOREVER)) {
        mp_raise_OSError(ENOSPC);
    }

    RAISE_ERRNO(net_context_send(send_buf, /*cb*/NULL, K_FOREVER, NULL, NULL));

    return mp_obj_new_int_from_uint(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, socket_send);

STATIC mp_obj_t socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    socket_obj_t *socket = self_in;
    socket_check_closed(socket);

    enum net_sock_type sock_type = net_context_get_type(socket->ctx);
    mp_int_t max_len = mp_obj_get_int(len_in);
    unsigned recv_len;
    vstr_t vstr;

    if (sock_type == SOCK_DGRAM) {

        struct net_buf *net_buf = k_fifo_get(&socket->recv_q, K_FOREVER);
        // Restore ->frags overwritten by fifo
        net_buf->frags = net_nbuf_token(net_buf);

        recv_len = net_nbuf_appdatalen(net_buf);
        DEBUG_printf("recv: net_buf=%p, appdatalen: %d\n", net_buf, recv_len);

        if (recv_len > max_len) {
            recv_len = max_len;
        }

        vstr_init_len(&vstr, recv_len);
        net_buf_gather(net_buf, vstr.buf, recv_len);
        net_nbuf_unref(net_buf);

    } else if (sock_type == SOCK_STREAM) {

        do {
            if (socket->state == STATE_PEER_CLOSED) {
                return mp_const_empty_bytes;
            }

            unsigned header_len = 0;
            if (socket->cur_buf == NULL) {
                DEBUG_printf("TCP recv: no cur_buf, getting\n");
                struct net_buf *net_buf = k_fifo_get(&socket->recv_q, K_FOREVER);
                // Restore ->frags overwritten by fifo
                net_buf->frags = net_nbuf_token(net_buf);

                header_len = net_nbuf_appdata(net_buf) - net_buf->frags->data;
                DEBUG_printf("TCP recv: new cur_buf: %p, hdr_len: %u\n", net_buf, header_len);
                socket->cur_buf = net_buf;
            }

            struct net_buf *frag = socket->cur_buf->frags;
            if (frag == NULL) {
                printf("net_buf has empty fragments on start!\n");
                assert(0);
            }

            net_buf_pull(frag, header_len);
            unsigned frag_len = frag->len;
            recv_len = frag_len;
            if (recv_len > max_len) {
                recv_len = max_len;
            }
            DEBUG_printf("%d data bytes in head frag, going to read %d\n", frag_len, recv_len);

            vstr_init_len(&vstr, recv_len);
            memcpy(vstr.buf, frag->data, recv_len);

            if (recv_len != frag_len) {
                net_buf_pull(frag, recv_len);
            } else {
                frag = net_buf_frag_del(socket->cur_buf, frag);
                if (frag == NULL) {
                    DEBUG_printf("Finished processing net_buf %p\n", socket->cur_buf);
                    // If "buf_sent" flag was set, it's last packet and we reached EOF
                    if (net_nbuf_buf_sent(socket->cur_buf)) {
                        socket->state = STATE_PEER_CLOSED;
                    }
                    net_nbuf_unref(socket->cur_buf);
                    socket->cur_buf = NULL;
                }
            }
        // Keep repeating while we're getting empty fragments
        // Zephyr IP stack appears to feed empty net_buf's with empty
        // frags for various TCP control packets.
        } while (recv_len == 0);

    } else {
        mp_not_implemented("");
    }

    mp_obj_t ret = mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recv_obj, socket_recv);

STATIC mp_obj_t socket_close(mp_obj_t self_in) {
    socket_obj_t *socket = self_in;
    if (socket->ctx != NULL) {
        RAISE_ERRNO(net_context_put(socket->ctx));
        socket->ctx = NULL;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_close_obj, socket_close);

STATIC const mp_map_elem_t socket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind), (mp_obj_t)&socket_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&socket_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&socket_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&socket_recv_obj },
};
STATIC MP_DEFINE_CONST_DICT(socket_locals_dict, socket_locals_dict_table);

STATIC const mp_obj_type_t socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .print = socket_print,
    .make_new = socket_make_new,
    //.protocol = &socket_stream_p,
    .locals_dict = (mp_obj_t)&socket_locals_dict,
};

STATIC const mp_map_elem_t mp_module_usocket_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_usocket) },
    // objects
    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&socket_type },
    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET), MP_OBJ_NEW_SMALL_INT(AF_INET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET6), MP_OBJ_NEW_SMALL_INT(AF_INET6) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_STREAM), MP_OBJ_NEW_SMALL_INT(SOCK_STREAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_DGRAM), MP_OBJ_NEW_SMALL_INT(SOCK_DGRAM) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_usocket_globals, mp_module_usocket_globals_table);

const mp_obj_module_t mp_module_usocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_usocket_globals,
};

#endif // MICROPY_PY_USOCKET
