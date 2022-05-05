/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 * and Mnemote Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017 Nick Moore @mnemote
 *
 * Based on extmod/modlwip.c
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Galen Hazelwood
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "py/runtime0.h"
#include "py/nlr.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"
#include "mdns.h"
#include "modnetwork.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/ip4.h"
#include "lwip/igmp.h"
#include "esp_log.h"

#define SOCKET_POLL_US (100000)
#define MDNS_QUERY_TIMEOUT_MS (5000)
#define MDNS_LOCAL_SUFFIX ".local"

enum {
    SOCKET_STATE_NEW,
    SOCKET_STATE_CONNECTED,
    SOCKET_STATE_PEER_CLOSED,
};

typedef struct _socket_obj_t {
    mp_obj_base_t base;
    int fd;
    uint8_t domain;
    uint8_t type;
    uint8_t proto;
    uint8_t state;
    unsigned int retries;
    #if MICROPY_PY_USOCKET_EVENTS
    mp_obj_t events_callback;
    struct _socket_obj_t *events_next;
    #endif
} socket_obj_t;

void _socket_settimeout(socket_obj_t *sock, uint64_t timeout_ms);

#if MICROPY_PY_USOCKET_EVENTS
// Support for callbacks on asynchronous socket events (when socket becomes readable)

// This divisor is used to reduce the load on the system, so it doesn't poll sockets too often
#define USOCKET_EVENTS_DIVISOR (8)

STATIC uint8_t usocket_events_divisor;
STATIC socket_obj_t *usocket_events_head;

void usocket_events_deinit(void) {
    usocket_events_head = NULL;
}

// Assumes the socket is not already in the linked list, and adds it
STATIC void usocket_events_add(socket_obj_t *sock) {
    sock->events_next = usocket_events_head;
    usocket_events_head = sock;
}

// Assumes the socket is already in the linked list, and removes it
STATIC void usocket_events_remove(socket_obj_t *sock) {
    for (socket_obj_t **s = &usocket_events_head;; s = &(*s)->events_next) {
        if (*s == sock) {
            *s = (*s)->events_next;
            return;
        }
    }
}

// Polls all registered sockets for readability and calls their callback if they are readable
void usocket_events_handler(void) {
    if (usocket_events_head == NULL) {
        return;
    }
    if (--usocket_events_divisor) {
        return;
    }
    usocket_events_divisor = USOCKET_EVENTS_DIVISOR;

    fd_set rfds;
    FD_ZERO(&rfds);
    int max_fd = 0;

    for (socket_obj_t *s = usocket_events_head; s != NULL; s = s->events_next) {
        FD_SET(s->fd, &rfds);
        max_fd = MAX(max_fd, s->fd);
    }

    // Poll the sockets
    struct timeval timeout = { .tv_sec = 0, .tv_usec = 0 };
    int r = select(max_fd + 1, &rfds, NULL, NULL, &timeout);
    if (r <= 0) {
        return;
    }

    // Call the callbacks
    for (socket_obj_t *s = usocket_events_head; s != NULL; s = s->events_next) {
        if (FD_ISSET(s->fd, &rfds)) {
            mp_call_function_1_protected(s->events_callback, s);
        }
    }
}

#endif // MICROPY_PY_USOCKET_EVENTS

static inline void check_for_exceptions(void) {
    mp_handle_pending(true);
}

// This function mimics lwip_getaddrinfo, with added support for mDNS queries
static int _socket_getaddrinfo3(const char *nodename, const char *servname,
    const struct addrinfo *hints, struct addrinfo **res) {

    #if MICROPY_HW_ENABLE_MDNS_QUERIES
    int nodename_len = strlen(nodename);
    const int local_len = sizeof(MDNS_LOCAL_SUFFIX) - 1;
    if (nodename_len > local_len
        && strcasecmp(nodename + nodename_len - local_len, MDNS_LOCAL_SUFFIX) == 0) {
        // mDNS query
        char nodename_no_local[nodename_len - local_len + 1];
        memcpy(nodename_no_local, nodename, nodename_len - local_len);
        nodename_no_local[nodename_len - local_len] = '\0';

        #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 1, 0)
        struct ip4_addr addr = {0};
        #else
        esp_ip4_addr_t addr = {0};
        #endif

        esp_err_t err = mdns_query_a(nodename_no_local, MDNS_QUERY_TIMEOUT_MS, &addr);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NOT_FOUND) {
                *res = NULL;
                return 0;
            }
            *res = NULL;
            return err;
        }

        struct addrinfo *ai = memp_malloc(MEMP_NETDB);
        if (ai == NULL) {
            *res = NULL;
            return EAI_MEMORY;
        }
        memset(ai, 0, sizeof(struct addrinfo) + sizeof(struct sockaddr_storage));

        struct sockaddr_in *sa = (struct sockaddr_in *)((uint8_t *)ai + sizeof(struct addrinfo));
        inet_addr_from_ip4addr(&sa->sin_addr, &addr);
        sa->sin_family = AF_INET;
        sa->sin_len = sizeof(struct sockaddr_in);
        sa->sin_port = lwip_htons((u16_t)atoi(servname));
        ai->ai_family = AF_INET;
        ai->ai_canonname = ((char *)sa + sizeof(struct sockaddr_storage));
        memcpy(ai->ai_canonname, nodename, nodename_len + 1);
        ai->ai_addrlen = sizeof(struct sockaddr_storage);
        ai->ai_addr = (struct sockaddr *)sa;

        *res = ai;
        return 0;
    }
    #endif

    // Normal query
    return lwip_getaddrinfo(nodename, servname, hints, res);
}

static int _socket_getaddrinfo2(const mp_obj_t host, const mp_obj_t portx, struct addrinfo **resp) {
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };

    mp_obj_t port = portx;
    if (mp_obj_is_small_int(port)) {
        // This is perverse, because lwip_getaddrinfo promptly converts it back to an int, but
        // that's the API we have to work with ...
        port = mp_obj_str_binary_op(MP_BINARY_OP_MODULO, mp_obj_new_str_via_qstr("%s", 2), port);
    }

    const char *host_str = mp_obj_str_get_str(host);
    const char *port_str = mp_obj_str_get_str(port);

    if (host_str[0] == '\0') {
        // a host of "" is equivalent to the default/all-local IP address
        host_str = "0.0.0.0";
    }

    MP_THREAD_GIL_EXIT();
    int res = _socket_getaddrinfo3(host_str, port_str, &hints, resp);
    MP_THREAD_GIL_ENTER();

    // Per docs: instead of raising gaierror getaddrinfo raises negative error number
    if (res != 0) {
        mp_raise_OSError(res > 0 ? -res : res);
    }
    // Somehow LwIP returns a resolution of 0.0.0.0 for failed lookups, traced it as far back
    // as netconn_gethostbyname_addrtype returning OK instead of error.
    if (*resp == NULL ||
        (strcmp(resp[0]->ai_canonname, "0.0.0.0") == 0 && strcmp(host_str, "0.0.0.0") != 0)) {
        mp_raise_OSError(-2); // name or service not known
    }

    return res;
}

STATIC void _socket_getaddrinfo(const mp_obj_t addrtuple, struct addrinfo **resp) {
    mp_obj_t *elem;
    mp_obj_get_array_fixed_n(addrtuple, 2, &elem);
    _socket_getaddrinfo2(elem[0], elem[1], resp);
}

STATIC mp_obj_t socket_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 3, false);

    socket_obj_t *sock = m_new_obj_with_finaliser(socket_obj_t);
    sock->base.type = type_in;
    sock->domain = AF_INET;
    sock->type = SOCK_STREAM;
    sock->proto = 0;
    if (n_args > 0) {
        sock->domain = mp_obj_get_int(args[0]);
        if (n_args > 1) {
            sock->type = mp_obj_get_int(args[1]);
            if (n_args > 2) {
                sock->proto = mp_obj_get_int(args[2]);
            }
        }
    }

    sock->state = sock->type == SOCK_STREAM ? SOCKET_STATE_NEW : SOCKET_STATE_CONNECTED;

    sock->fd = lwip_socket(sock->domain, sock->type, sock->proto);
    if (sock->fd < 0) {
        mp_raise_OSError(errno);
    }
    _socket_settimeout(sock, UINT64_MAX);

    return MP_OBJ_FROM_PTR(sock);
}

STATIC mp_obj_t socket_bind(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    struct addrinfo *res;
    _socket_getaddrinfo(arg1, &res);
    self->state = SOCKET_STATE_CONNECTED;
    int r = lwip_bind(self->fd, res->ai_addr, res->ai_addrlen);
    lwip_freeaddrinfo(res);
    if (r < 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_bind_obj, socket_bind);

// method socket.listen([backlog])
STATIC mp_obj_t socket_listen(size_t n_args, const mp_obj_t *args) {
    socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    int backlog = MICROPY_PY_USOCKET_LISTEN_BACKLOG_DEFAULT;
    if (n_args > 1) {
        backlog = mp_obj_get_int(args[1]);
        backlog = (backlog < 0) ? 0 : backlog;
    }

    self->state = SOCKET_STATE_CONNECTED;
    int r = lwip_listen(self->fd, backlog);
    if (r < 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_listen_obj, 1, 2, socket_listen);

STATIC mp_obj_t socket_accept(const mp_obj_t arg0) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);

    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);

    int new_fd = -1;
    for (int i = 0; i <= self->retries; i++) {
        MP_THREAD_GIL_EXIT();
        new_fd = lwip_accept(self->fd, &addr, &addr_len);
        MP_THREAD_GIL_ENTER();
        if (new_fd >= 0) {
            break;
        }
        if (errno != EAGAIN) {
            mp_raise_OSError(errno);
        }
        check_for_exceptions();
    }
    if (new_fd < 0) {
        if (self->retries == 0) {
            mp_raise_OSError(MP_EAGAIN);
        } else {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
    }

    // create new socket object
    socket_obj_t *sock = m_new_obj_with_finaliser(socket_obj_t);
    sock->base.type = self->base.type;
    sock->fd = new_fd;
    sock->domain = self->domain;
    sock->type = self->type;
    sock->proto = self->proto;
    sock->state = SOCKET_STATE_CONNECTED;
    _socket_settimeout(sock, UINT64_MAX);

    // make the return value
    uint8_t *ip = (uint8_t *)&((struct sockaddr_in *)&addr)->sin_addr;
    mp_uint_t port = lwip_ntohs(((struct sockaddr_in *)&addr)->sin_port);
    mp_obj_tuple_t *client = mp_obj_new_tuple(2, NULL);
    client->items[0] = sock;
    client->items[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);

    return client;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_accept_obj, socket_accept);

STATIC mp_obj_t socket_connect(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    struct addrinfo *res;
    _socket_getaddrinfo(arg1, &res);
    MP_THREAD_GIL_EXIT();
    self->state = SOCKET_STATE_CONNECTED;
    int r = lwip_connect(self->fd, res->ai_addr, res->ai_addrlen);
    MP_THREAD_GIL_ENTER();
    lwip_freeaddrinfo(res);
    if (r != 0) {
        mp_raise_OSError(errno);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_connect_obj, socket_connect);

STATIC mp_obj_t socket_setsockopt(size_t n_args, const mp_obj_t *args) {
    (void)n_args; // always 4
    socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    int opt = mp_obj_get_int(args[2]);

    switch (opt) {
        // level: SOL_SOCKET
        case SO_REUSEADDR: {
            int val = mp_obj_get_int(args[3]);
            int ret = lwip_setsockopt(self->fd, SOL_SOCKET, opt, &val, sizeof(int));
            if (ret != 0) {
                mp_raise_OSError(errno);
            }
            break;
        }

            #if MICROPY_PY_USOCKET_EVENTS
        // level: SOL_SOCKET
        // special "register callback" option
        case 20: {
            if (args[3] == mp_const_none) {
                if (self->events_callback != MP_OBJ_NULL) {
                    usocket_events_remove(self);
                    self->events_callback = MP_OBJ_NULL;
                }
            } else {
                if (self->events_callback == MP_OBJ_NULL) {
                    usocket_events_add(self);
                }
                self->events_callback = args[3];
            }
            break;
        }
            #endif

        // level: IPPROTO_IP
        case IP_ADD_MEMBERSHIP: {
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
            if (bufinfo.len != sizeof(ip4_addr_t) * 2) {
                mp_raise_ValueError(NULL);
            }

            // POSIX setsockopt has order: group addr, if addr, lwIP has it vice-versa
            err_t err = igmp_joingroup((const ip4_addr_t *)bufinfo.buf + 1, bufinfo.buf);
            if (err != ERR_OK) {
                mp_raise_OSError(-err);
            }
            break;
        }

        default:
            mp_printf(&mp_plat_print, "Warning: lwip.setsockopt() option not implemented\n");
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_setsockopt_obj, 4, 4, socket_setsockopt);

void _socket_settimeout(socket_obj_t *sock, uint64_t timeout_ms) {
    // Rather than waiting for the entire timeout specified, we wait sock->retries times
    // for SOCKET_POLL_US each, checking for a MicroPython interrupt between timeouts.
    // with SOCKET_POLL_MS == 100ms, sock->retries allows for timeouts up to 13 years.
    // if timeout_ms == UINT64_MAX, wait forever.
    sock->retries = (timeout_ms == UINT64_MAX) ? UINT_MAX : timeout_ms * 1000 / SOCKET_POLL_US;

    struct timeval timeout = {
        .tv_sec = 0,
        .tv_usec = timeout_ms ? SOCKET_POLL_US : 0
    };
    lwip_setsockopt(sock->fd, SOL_SOCKET, SO_SNDTIMEO, (const void *)&timeout, sizeof(timeout));
    lwip_setsockopt(sock->fd, SOL_SOCKET, SO_RCVTIMEO, (const void *)&timeout, sizeof(timeout));
    lwip_fcntl(sock->fd, F_SETFL, timeout_ms ? 0 : O_NONBLOCK);
}

STATIC mp_obj_t socket_settimeout(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    if (arg1 == mp_const_none) {
        _socket_settimeout(self, UINT64_MAX);
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT
        _socket_settimeout(self, (uint64_t)(mp_obj_get_float(arg1) * MICROPY_FLOAT_CONST(1000.0)));
        #else
        _socket_settimeout(self, mp_obj_get_int(arg1) * 1000);
        #endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_settimeout_obj, socket_settimeout);

STATIC mp_obj_t socket_setblocking(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    if (mp_obj_is_true(arg1)) {
        _socket_settimeout(self, UINT64_MAX);
    } else {
        _socket_settimeout(self, 0);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_setblocking_obj, socket_setblocking);

// XXX this can end up waiting a very long time if the content is dribbled in one character
// at a time, as the timeout resets each time a recvfrom succeeds ... this is probably not
// good behaviour.
STATIC mp_uint_t _socket_read_data(mp_obj_t self_in, void *buf, size_t size,
    struct sockaddr *from, socklen_t *from_len, int *errcode) {
    socket_obj_t *sock = MP_OBJ_TO_PTR(self_in);

    // A new socket cannot be read from.
    if (sock->state == SOCKET_STATE_NEW) {
        *errcode = MP_ENOTCONN;
        return MP_STREAM_ERROR;
    }

    // If the peer closed the connection then the lwIP socket API will only return "0" once
    // from lwip_recvfrom and then block on subsequent calls.  To emulate POSIX behaviour,
    // which continues to return "0" for each call on a closed socket, we set a flag when
    // the peer closed the socket.
    if (sock->state == SOCKET_STATE_PEER_CLOSED) {
        return 0;
    }

    // XXX Would be nicer to use RTC to handle timeouts
    for (int i = 0; i <= sock->retries; ++i) {
        // Poll the socket to see if it has waiting data and only release the GIL if it doesn't.
        // This ensures higher performance in the case of many small reads, eg for readline.
        bool release_gil;
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(sock->fd, &rfds);
            struct timeval timeout = { .tv_sec = 0, .tv_usec = 0 };
            int r = select(sock->fd + 1, &rfds, NULL, NULL, &timeout);
            release_gil = r != 1;
        }
        if (release_gil) {
            MP_THREAD_GIL_EXIT();
        }
        int r = lwip_recvfrom(sock->fd, buf, size, 0, from, from_len);
        if (release_gil) {
            MP_THREAD_GIL_ENTER();
        }
        if (r == 0) {
            sock->state = SOCKET_STATE_PEER_CLOSED;
        }
        if (r >= 0) {
            return r;
        }
        if (errno != EWOULDBLOCK) {
            *errcode = errno;
            return MP_STREAM_ERROR;
        }
        check_for_exceptions();
    }

    *errcode = sock->retries == 0 ? MP_EWOULDBLOCK : MP_ETIMEDOUT;
    return MP_STREAM_ERROR;
}

mp_obj_t _socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in,
    struct sockaddr *from, socklen_t *from_len) {
    size_t len = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, len);

    int errcode;
    mp_uint_t ret = _socket_read_data(self_in, vstr.buf, len, from, from_len, &errcode);
    if (ret == MP_STREAM_ERROR) {
        mp_raise_OSError(errcode);
    }

    vstr.len = ret;
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}

STATIC mp_obj_t socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    return _socket_recvfrom(self_in, len_in, NULL, NULL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recv_obj, socket_recv);

STATIC mp_obj_t socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    struct sockaddr from;
    socklen_t fromlen = sizeof(from);

    mp_obj_t tuple[2];
    tuple[0] = _socket_recvfrom(self_in, len_in, &from, &fromlen);

    uint8_t *ip = (uint8_t *)&((struct sockaddr_in *)&from)->sin_addr;
    mp_uint_t port = lwip_ntohs(((struct sockaddr_in *)&from)->sin_port);
    tuple[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_recvfrom_obj, socket_recvfrom);

int _socket_send(socket_obj_t *sock, const char *data, size_t datalen) {
    int sentlen = 0;
    for (int i = 0; i <= sock->retries && sentlen < datalen; i++) {
        MP_THREAD_GIL_EXIT();
        int r = lwip_write(sock->fd, data + sentlen, datalen - sentlen);
        MP_THREAD_GIL_ENTER();
        // lwip returns EINPROGRESS when trying to send right after a non-blocking connect
        if (r < 0 && errno != EWOULDBLOCK && errno != EINPROGRESS) {
            mp_raise_OSError(errno);
        }
        if (r > 0) {
            sentlen += r;
        }
        check_for_exceptions();
    }
    if (sentlen == 0) {
        mp_raise_OSError(sock->retries == 0 ? MP_EWOULDBLOCK : MP_ETIMEDOUT);
    }
    return sentlen;
}

STATIC mp_obj_t socket_send(const mp_obj_t arg0, const mp_obj_t arg1) {
    socket_obj_t *sock = MP_OBJ_TO_PTR(arg0);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg1, &bufinfo, MP_BUFFER_READ);
    int r = _socket_send(sock, bufinfo.buf, bufinfo.len);
    return mp_obj_new_int(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_send_obj, socket_send);

STATIC mp_obj_t socket_sendall(const mp_obj_t arg0, const mp_obj_t arg1) {
    // XXX behaviour when nonblocking (see extmod/modlwip.c)
    // XXX also timeout behaviour.
    socket_obj_t *sock = MP_OBJ_TO_PTR(arg0);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg1, &bufinfo, MP_BUFFER_READ);
    int r = _socket_send(sock, bufinfo.buf, bufinfo.len);
    if (r < bufinfo.len) {
        mp_raise_OSError(MP_ETIMEDOUT);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_sendall_obj, socket_sendall);

STATIC mp_obj_t socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // get the buffer to send
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    // create the destination address
    struct sockaddr_in to;
    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    to.sin_port = lwip_htons(netutils_parse_inet_addr(addr_in, (uint8_t *)&to.sin_addr, NETUTILS_BIG));

    // send the data
    for (int i = 0; i <= self->retries; i++) {
        MP_THREAD_GIL_EXIT();
        int ret = lwip_sendto(self->fd, bufinfo.buf, bufinfo.len, 0, (struct sockaddr *)&to, sizeof(to));
        MP_THREAD_GIL_ENTER();
        if (ret > 0) {
            return mp_obj_new_int_from_uint(ret);
        }
        if (ret == -1 && errno != EWOULDBLOCK) {
            mp_raise_OSError(errno);
        }
        check_for_exceptions();
    }
    mp_raise_OSError(MP_ETIMEDOUT);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(socket_sendto_obj, socket_sendto);

STATIC mp_obj_t socket_fileno(const mp_obj_t arg0) {
    socket_obj_t *self = MP_OBJ_TO_PTR(arg0);
    return mp_obj_new_int(self->fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_fileno_obj, socket_fileno);

STATIC mp_obj_t socket_makefile(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socket_makefile_obj, 1, 3, socket_makefile);

STATIC mp_uint_t socket_stream_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    return _socket_read_data(self_in, buf, size, NULL, NULL, errcode);
}

STATIC mp_uint_t socket_stream_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    socket_obj_t *sock = self_in;
    for (int i = 0; i <= sock->retries; i++) {
        MP_THREAD_GIL_EXIT();
        int r = lwip_write(sock->fd, buf, size);
        MP_THREAD_GIL_ENTER();
        if (r > 0) {
            return r;
        }
        // lwip returns MP_EINPROGRESS when trying to write right after a non-blocking connect
        if (r < 0 && errno != EWOULDBLOCK && errno != EINPROGRESS) {
            *errcode = errno;
            return MP_STREAM_ERROR;
        }
        check_for_exceptions();
    }
    *errcode = sock->retries == 0 ? MP_EWOULDBLOCK : MP_ETIMEDOUT;
    return MP_STREAM_ERROR;
}

STATIC mp_uint_t socket_stream_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    socket_obj_t *socket = self_in;
    if (request == MP_STREAM_POLL) {
        if (socket->fd == -1) {
            return MP_STREAM_POLL_NVAL;
        }

        fd_set rfds;
        FD_ZERO(&rfds);
        fd_set wfds;
        FD_ZERO(&wfds);
        fd_set efds;
        FD_ZERO(&efds);
        struct timeval timeout = { .tv_sec = 0, .tv_usec = 0 };
        if (arg & MP_STREAM_POLL_RD) {
            FD_SET(socket->fd, &rfds);
        }
        if (arg & MP_STREAM_POLL_WR) {
            FD_SET(socket->fd, &wfds);
        }
        if (arg & MP_STREAM_POLL_HUP) {
            FD_SET(socket->fd, &efds);
        }

        int r = select((socket->fd) + 1, &rfds, &wfds, &efds, &timeout);
        if (r < 0) {
            *errcode = MP_EIO;
            return MP_STREAM_ERROR;
        }

        mp_uint_t ret = 0;
        if (FD_ISSET(socket->fd, &rfds)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if (FD_ISSET(socket->fd, &wfds)) {
            ret |= MP_STREAM_POLL_WR;
        }
        if (FD_ISSET(socket->fd, &efds)) {
            ret |= MP_STREAM_POLL_HUP;
        }

        // New (unconnected) sockets are writable and have HUP set.
        if (socket->state == SOCKET_STATE_NEW) {
            ret |= (arg & MP_STREAM_POLL_WR) | MP_STREAM_POLL_HUP;
        }

        return ret;
    } else if (request == MP_STREAM_CLOSE) {
        if (socket->fd >= 0) {
            #if MICROPY_PY_USOCKET_EVENTS
            if (socket->events_callback != MP_OBJ_NULL) {
                usocket_events_remove(socket);
                socket->events_callback = MP_OBJ_NULL;
            }
            #endif
            int ret = lwip_close(socket->fd);
            if (ret != 0) {
                *errcode = errno;
                return MP_STREAM_ERROR;
            }
            socket->fd = -1;
        }
        return 0;
    }

    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}

STATIC const mp_rom_map_elem_t socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&socket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&socket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendall), MP_ROM_PTR(&socket_sendall_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendto), MP_ROM_PTR(&socket_sendto_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&socket_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom), MP_ROM_PTR(&socket_recvfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&socket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout), MP_ROM_PTR(&socket_settimeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_makefile), MP_ROM_PTR(&socket_makefile_obj) },
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&socket_fileno_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
};
STATIC MP_DEFINE_CONST_DICT(socket_locals_dict, socket_locals_dict_table);

STATIC const mp_stream_p_t socket_stream_p = {
    .read = socket_stream_read,
    .write = socket_stream_write,
    .ioctl = socket_stream_ioctl
};

STATIC const mp_obj_type_t socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .make_new = socket_make_new,
    .protocol = &socket_stream_p,
    .locals_dict = (mp_obj_t)&socket_locals_dict,
};

STATIC mp_obj_t esp_socket_getaddrinfo(size_t n_args, const mp_obj_t *args) {
    // TODO support additional args beyond the first two

    struct addrinfo *res = NULL;
    _socket_getaddrinfo2(args[0], args[1], &res);
    mp_obj_t ret_list = mp_obj_new_list(0, NULL);

    for (struct addrinfo *resi = res; resi; resi = resi->ai_next) {
        mp_obj_t addrinfo_objs[5] = {
            mp_obj_new_int(resi->ai_family),
            mp_obj_new_int(resi->ai_socktype),
            mp_obj_new_int(resi->ai_protocol),
            mp_obj_new_str(resi->ai_canonname, strlen(resi->ai_canonname)),
            mp_const_none
        };

        if (resi->ai_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in *)resi->ai_addr;
            // This looks odd, but it's really just a u32_t
            ip4_addr_t ip4_addr = { .addr = addr->sin_addr.s_addr };
            char buf[16];
            ip4addr_ntoa_r(&ip4_addr, buf, sizeof(buf));
            mp_obj_t inaddr_objs[2] = {
                mp_obj_new_str(buf, strlen(buf)),
                mp_obj_new_int(ntohs(addr->sin_port))
            };
            addrinfo_objs[4] = mp_obj_new_tuple(2, inaddr_objs);
        }
        mp_obj_list_append(ret_list, mp_obj_new_tuple(5, addrinfo_objs));
    }

    if (res) {
        lwip_freeaddrinfo(res);
    }
    return ret_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_socket_getaddrinfo_obj, 2, 6, esp_socket_getaddrinfo);

STATIC mp_obj_t esp_socket_initialize() {
    static int initialized = 0;
    if (!initialized) {
        ESP_LOGI("modsocket", "Initializing");
        tcpip_adapter_init();
        initialized = 1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_socket_initialize_obj, esp_socket_initialize);

STATIC const mp_rom_map_elem_t mp_module_socket_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usocket) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&esp_socket_initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&socket_type) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&esp_socket_getaddrinfo_obj) },

    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(AF_INET) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET6), MP_ROM_INT(AF_INET6) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(SOCK_DGRAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(SOCK_RAW) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TCP), MP_ROM_INT(IPPROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_UDP), MP_ROM_INT(IPPROTO_UDP) },
    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IP), MP_ROM_INT(IPPROTO_IP) },
    { MP_ROM_QSTR(MP_QSTR_SOL_SOCKET), MP_ROM_INT(SOL_SOCKET) },
    { MP_ROM_QSTR(MP_QSTR_SO_REUSEADDR), MP_ROM_INT(SO_REUSEADDR) },
    { MP_ROM_QSTR(MP_QSTR_IP_ADD_MEMBERSHIP), MP_ROM_INT(IP_ADD_MEMBERSHIP) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_socket_globals, mp_module_socket_globals_table);

const mp_obj_module_t mp_module_usocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_socket_globals,
};
